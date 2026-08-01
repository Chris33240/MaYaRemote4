#include "command_ir_capture.h"
#include "PinDefinitionsAndMore.h"
#include "interfaces/hardwarePresenter.h"
#include "commandHandler.h"
#include "setup.h"
#include "helpers/helpers.h"
#include "timeout.h"
#include "commands_io.h"
#include "globals.h"

// ---- Capture des commandes ---------------
bool isCaptureMode;

/// @brief Initialise le bouton de capture IR.
void initCaptureButton()
{
    pinMode(GPIO_CAPTURE_BUTTON, INPUT); // Nous avons déjà une résistance de tirage externe de 10kOhms relié en pull-Up.
}

/// @brief Démarre le mode de capture IR et initialise le clignotement LED.
/// @note Un timeout est ajouté pour arrêter la capture automatiquement.
void captureCommand()
{
    startCapture();
    isCaptureMode = true;
    pGlobalStatusLED->begin();
    pGlobalStatusLED->startBlinking(BLINK_LED_CAPTURE_INTERVAL);
    Serial.println(F(">> Waiting for capture..."));
    TimeoutManager2::addTimeout("IR_CAPTURE", CAPTURE_TIMEOUT, []()
                                { callbackCaptureTimeoutExpired(); });
}

/// @brief Callback appelé lorsque le timeout de capture IR est expiré.
void callbackCaptureTimeoutExpired()
{
    // Capture Timout expired
    isCaptureMode = false;
    Serial.println(F("Capture Timeout expired!"));
    sendBleNotifyCode("103");
}

/// @brief Boucle à appeler régulièrement pour gérer l'état de capture IR.
/// @note Vérifie si une trame a été reçue et enregistre la commande capturée.
void irCapture_loop()
{
    if (isCaptureMode)
    {
        bool lastReceived = getReceivedCaptureState();
        if (lastReceived)
        {
            // Capture Done
            isCaptureMode = false;
            registerIrCapture();
        }
    }
    else
    {
        pGlobalStatusLED->stopBlinking(false);
        TimeoutManager2::removeTimeout("IR_CAPTURE");
    }
}

// --- Enregistrement des commmandes capturées -----------------
/// @brief Enregistre la commande IR capturée dans le système et le fichier.
/// @note Génère une clé unique pour la commande, sauvegarde et notifie le GUI.
void registerIrCapture()
{
    std::list<std::string> payloads = getPayloads();
    Serial.println(F(">> Received Ir Capture from IrReceiver : "));
    helpers::printPayloadsContent(payloads);

    // Message recu par le recepteur IR :
    // Protocol  : DAIKIN
    // Code      : 0x11DA2700C51000E711DA270042971A0511DA2700004930003F000006600000C1800071 (280 Bits)
    // Mesg Desc.: Power: On, Mode: 4 (Heat), Temp: 24C, Fan: 1 (Low), Powerful: Off, Quiet: Off, Sensor: Off, Mould: Off, Comfort: Off, Swing(H): Off, Swing(V): On, Clock: 11:03, Day: 3 (Tue), On Timer: Off, Off Timer: Off, Weekly Timer: Off
    // uint16_t rawData[583] = {448, 406,  458, 402,  464, 408,  454, 410,  432, 434,  458, 25088,  3486, 1710,  454, 1276,  430, 434,  458, 408,  454, 412,  456, 1274,  456, 408,  456, 412,  454, 410,  430, 434,  430, 1302,  454, 410,  430, 1306,  450, 1276,  452, 412,  454, 1278,  452, 1278,  456, 1276,  454, 1276,  454, 1276,  452, 414,  452, 410,  454, 1278,  454, 412,  452, 412,  452, 412,  454, 412,  452, 412,  452, 414,  452, 414,  452, 412,  452, 414,  450, 414,  452, 1280,  452, 414,  450, 1280,  452, 416,  450, 412,  450, 416,  448, 1282,  450, 1280,  450, 438,  428, 438,  428, 438,  428, 438,  426, 1304,  428, 438,  426, 440,  426, 438,  426, 438,  426, 440,  424, 440,  426, 440,  426, 440,  424, 440,  426, 440,  424, 440,  424, 1306,  424, 1308,  422, 1308,  422, 442,  422, 442,  420, 1310,  420, 1310,  420, 1312,  420, 35516,  3452, 1744,  420, 1310,  420, 446,  420, 444,  420, 446,  420, 1310,  420, 444,  420, 444,  420, 446,  420, 444,  420, 1312,  420, 444,  420, 1310,  420, 1310,  420, 444,  422, 1310,  422, 1310,  420, 1310,  420, 1312,  422, 1310,  420, 444,  420, 444,  420, 1310,  420, 444,  420, 446,  420, 444,  420, 444,  422, 444,  396, 468,  420, 446,  420, 444,  420, 444,  420, 444,  422, 444,  420, 1312,  420, 444,  420, 444,  420, 444,  420, 444,  420, 1310,  420, 446,  420, 1310,  422, 1310,  420, 1310,  420, 446,  420, 1310,  420, 446,  420, 444,  420, 1310,  420, 446,  420, 1310,  420, 446,  420, 1310,  420, 1312,  420, 446,  420, 446,  420, 446,  420, 1312,  420, 446,  420, 1310,
    // 422, 444,  420, 446,  418, 446,  420, 444,  420, 446,  418, 35518,  3450, 1744,  420, 1312,  394, 470,  420, 446,  420, 444,  420, 1310,  420, 444,  422, 444,  420, 444,
    // 418, 446,  420, 1312,  420, 444,  420, 1310,  420, 1310,  420, 446,  420, 1310,  420, 1312,  420, 1310,  420, 1312,  420, 1310,  420, 444,  420, 446,  420, 1310,  396, 470,  420, 444,  420, 446,  418, 446,  420, 446,  418, 446,  420, 444,  420, 446,  420, 446,  418, 446,  420, 444,  420, 444,  420, 446,  418, 446,  418, 446,  418, 446,
    // 418, 446,  396, 470,  418, 1312,  418, 446,  420, 446,  420, 1310,  396, 470,  418, 446,  420, 1312,  418, 446,  418, 446,  420, 446,  418, 446,  418, 448,  418, 1312,  420, 1312,  418, 446,  418, 446,  418, 446,  420, 446,  420, 446,  418, 448,  418, 446,  418, 448,  418, 446,  420, 446,  418, 1312,  420, 1312,  418, 1312,  418, 1312,  420, 1310,  418, 1312,  394, 472,  420, 446,  418, 448,  392, 470,  394, 496,  394, 446,  392, 496,  394, 446,  418, 446,  420, 446,  418, 446,  394, 496,  394, 470,  380, 462,  418, 472,  368, 496,  368, 496,  394, 472,  370, 496,  384, 1346,  394, 1336,  370, 470,  396, 496,  370, 496,  368, 496,  368, 496,  394, 470,  394, 448,  416, 474,  394, 446,  394, 496,  368, 1338,  394, 1362,  394, 472,  368, 496,  370, 496,  370, 470,  394, 496,  370, 496,  370, 470,  394, 496,  394, 472,  394, 446,  394, 496,
    // 368, 496,  368, 496,  370, 496,  368, 496,  368, 496,  392, 450,  394, 1362,  368, 496,  368, 496,  370, 496,  368, 496,  370, 472,  394, 1362,  368, 1362,  394, 472,  370, 496,  368, 484,  382, 496,  370, 494,  370, 496,  370, 496,  368, 1362,  394, 472,  370, 496,  368, 496,  368, 496,  370, 496,  368, 498,  368, 496,  370, 496,  370,
    // 1362,  370, 496,  368, 496,  368, 496,  368, 1362,  370, 1360,  370, 1362,  392, 474,  368};  // DAIKIN
    // uint8_t state[35] = {0x11, 0xDA, 0x27, 0x00, 0xC5, 0x10, 0x00, 0xE7, 0x11, 0xDA, 0x27, 0x00, 0x42, 0x97, 0x1A, 0x05, 0x11, 0xDA, 0x27, 0x00, 0x00, 0x49, 0x30, 0x00, 0x3F, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0xC1, 0x80, 0x00, 0x71};

    // Ex for DAIKIN ON/OFF:
    // print payload content:
    //-16
    //-false
    //-0x11DA2700C51000E711DA270042681AD611DA2700004830003F000006600000C1800070
    //-280
    //-583
    //-true
    //-452, 404,  432, 434,  460, 404,  458, 408,  432, 432,  432, 25114,  3464, 1730,  456, 1274,  458, 408,  458, 406,  432, 432,  458, 1274,  458, 406,  458, 408,  460,
    // 404,  456, 408,  456, 1276,  458, 406,  458, 1272,  434, 1298,  458, 406,  460, 1272,  458, 1272,  458, 1274,  458, 1272,  458, 1274,  456, 408,  458, 408,  456, 1274,  458, 408,  456, 406,  458, 408,  458, 406,  458, 408,  454, 412,  454, 410,  454, 410,  432, 432,  456, 410,  458, 1272,  454, 410,  456, 1276,  458, 406,  456, 410,  454, 410,  456, 1276,  456, 1276,  458, 408,  458, 408,  458, 408,  456, 410,  456, 1276,  456, 408,  456, 410,  454, 408,  456, 410,  456, 410,  454, 410,  456, 410,  456, 412,  454, 410,  452, 412,  454, 412,  456, 1276,  454, 1276,  454, 1276,  430, 436,  454, 410,  428, 1302,  454, 1278,  454, 1276,  454, 35482,  3484, 1710,  454, 1278,
    // 454, 412,  454, 410,  452, 410,  456, 1278,  454, 410,  454, 410,  456, 408,  454, 412,  452, 1276,  456, 410,  456, 1276,  454, 1276,  454, 410,  454, 1278,  452, 1278,
    //  456, 1276,  452, 1278,  452, 1278,  452, 414,  454, 412,  452, 1280,  452, 412,  452, 412,  454, 412,  450, 414,  452, 414,  452, 412,  452, 412,  452, 414,  452, 412,
    // 452, 414,  452, 414,  452, 1278,  454, 412,  452, 414,  450, 416,  450, 414,  452, 1280,  452, 436,  430, 414,  450, 412,  452, 414,  452, 1280,  450, 416,  450, 1280,  450, 1280,  450, 416,  450, 416,  448, 1304,  428, 436,  428, 1282,  448, 1304,  428, 414,  450, 438,  428, 414,  452, 436,  428, 1302,  428, 1304,  426, 420,  446, 1304,
    //  428, 438,  428, 1304,  426, 1304,  426, 35510,  3458, 1738,  424, 1306,  426, 438,  426, 438,  426, 440,  426, 1304,  426, 440,  426, 438,  426, 440,  426, 440,  426, 1306,  424, 440,  424, 1306,  424, 1306,  426, 440,  424, 1306,  426, 1306,  426, 1306,  424, 1308,  424, 1306,  424, 440,  424, 442,  422, 1308,  424, 442,  422, 442,  422, 442,  422, 442,  422, 444,  422, 444,  396, 468,  422, 444,  420, 444,  420, 444,  420, 444,  420, 446,  420, 444,  420, 444,  420, 446,  420, 444,  420, 446,  420, 444,  420, 444,  420, 444,  422, 444,  420, 1312,  420, 446,  420, 446,  420, 1310,  420, 444,  422, 444,  420, 444,  420, 444,  420, 444,  420, 1312,  420, 1310,  420, 444,  420, 446,  420, 446,  420, 446,  420, 444,  420, 446,  420, 444,  420, 444,  420, 444,  420, 444,  420, 1310,  420, 1310,  420, 1312,  420, 1310,  420, 1310,  422, 1310,  420, 446,  420, 444,  420, 446,  420, 444,  420, 444,  420, 446,  420, 444,  420, 446,  418, 446,  420, 444,  420, 446,  418, 446,  420, 444,  420, 446,  418, 446,  420, 446,  420, 446,  420, 444,  422, 444,  420, 1310,  422, 1310,  420, 446,  418, 446,  420, 444,  420, 444,  420, 446,  420, 446,  418, 446,  420, 444,  420, 444,  420,
    // 444,  420, 1310,  420, 1312,  420, 446,  420, 446,  420, 446,  420, 446,  418, 446,  420, 446,  420, 446,  420, 446,  420, 446,  420, 444,  420, 446,  420, 446,  420, 446,  418, 446,  418, 446,  418, 446,  420, 446,  418, 1312,  418, 446,  420, 446,  420, 446,  420, 446,  420, 446,  418, 1312,  420, 1312,  420, 444,  420, 446,  418, 446,
    //  420, 446,  418, 446,  420, 446,  418, 446,  418, 1312,  420, 446,  418, 446,  418, 446,  420, 446,  420, 446,  420, 446,  420, 446,  418, 446,  418, 446,  420, 446,  418, 446,  418, 446,  420, 1312,  418, 1312,  418, 1314,  418, 446,  418
    //-
    //-0x11, 0xDA, 0x27, 0x00, 0xC5, 0x10, 0x00, 0xE7, 0x11, 0xDA, 0x27, 0x00, 0x42, 0x68, 0x1A, 0xD6, 0x11, 0xDA, 0x27, 0x00, 0x00, 0x48, 0x30, 0x00, 0x3F, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0xC1, 0x80, 0x00, 0x70
    //-0x27DA11
    //-0xD61A6842
    //-0xE70010C50027DA11

    if (payloads.size() >= 12)
    {
        Serial.println(F("[DEBUG]    begin register command: "));
        // Création d'un itérateur pour parcourir la liste
        auto it = payloads.begin();
        // Extraction et conversion des éléments de la liste
        Serial.println("[DEBUG] Try to get protocol:");

        // Protocol
        std::string protocol = *it;
        // IRprotocols2 protocol = static_cast<IRprotocols2>(std::stoi(*it++));
        Serial.printf("[DEBUG] Protocol: %s\n", protocol.c_str());

        it++;
        // Repeat
        bool repeat = helpers::IsTrue(*it);
        Serial.printf("[DEBUG] repeat: %s\n", repeat ? "true" : "false");

        it++;
        // --- value or state[] to simple hexadecimal ---
        // Ex: 0xC800F040C
        Serial.println("[DEBUG] Try to get value ou state[]:");
        std::string strValue = helpers::noNullOrBlank(&(*it));
        // data1Str;
        uint64_t data1; // value or state[] ???
        data1 = (strValue).empty() ? 0 : helpers::convertToType<uint64_t>(strValue, 0);
        data1 = data1 ? data1 : 0x0; // data
        Serial.printf("[DEBUG] value or state[] to simple hexadecimal: %s (%" PRIu64 ")\r\n", strValue.c_str(), data1);

        it++; // --- nBits ---
        it++; // --- Lenght ---
        it++; // --- HasACState ---
        it++; // --- Raw bugffer ---
        it++; // --- value if the decode_type doesn't have an A/C state. ---
        it++; // --- Decoded ACState codes to hexa ---
        it++; // --- Decoded Adress part to hexa ---
        it++; // --- Decoded Command part to hexa ---
        it++; // --- Decoded Value to hexa ---
        // Ex: 0xC800F040CLL
        Serial.println("[DEBUG] Try to get code:");
        strValue = helpers::noNullOrBlank(&(*it));
        std::string dataStr = strValue;
        // Serial.printf("[DEBUG] dataStr: %s\r\n", strValue.c_str());
        //  https://cplusplus.com/reference/string/stoull/
        // std::string::size_type sz = 0;   // alias of size_t
        // data = (strValue).empty() ? 0 : std::stoull(strValue, &sz, 0);
        uint64_t data = (strValue).empty() ? 0 : helpers::convertToType<uint64_t>(strValue, 0);
        data = data ? data : 0x0; // data
        Serial.printf("[DEBUG] code: %s (%" PRIu64 ")\r\n", strValue.c_str(), data);

        // Serial.println("[DEBUG] Try to get code:");
        //  Code
        // std::string dataStr = *it++;
        // Serial.printf("[DEBUG] code: %s\n", dataStr);
        //  https://cplusplus.com/reference/string/stoull/
        // std::string::size_type sz = 0; // alias of size_t
        // const uint64_t data = std::stoull(dataStr, &sz, 0);

        // Registered command with free Key
        // std::string key = "IR_" + protocol + "_" + dataStr; // Ex: "IR_2_0xC800F040C" Ex DAIKIN: 0xE70010C50027DA11
        std::string key = dataStr; // we need to reduced size name to 20 chars max !
        commandData2 commandData = makeCommandData2(IR, "WRITE", "w", payloads);
        // std::string freeKey = registerCommand(key, commandData, true, false);
        std::string freeKey = registerCommand2(key, commandData, false, true, false);

        //  Save commands
// #if (ENABLED_IO_FILESYSTEM && ENABLED_IO_FILESYSTEM_SAVING)
//     saveCommand(freeKey, commandData);
// #endif

        // Send message to Gui
        pGlobalStatusLED->startBlinkingForDuration(BLINK_LED_CAPTURE_DONE_INTERVAL, BLINK_LED_CAPTURE_DONE_DURATION);
        Serial.println(F(">> Capture Successfull !"));
        // sendBleNotifyCode("102");
        // delay(500); 2 notifications sucessives ne fonctionnent pas malgrés le delay.
        sendBleNotifyNewCommandCaptured(freeKey);
    }
}
