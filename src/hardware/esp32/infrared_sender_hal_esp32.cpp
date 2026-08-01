#include "infrared_sender_hal_esp32.h"
// #include <Arduino.h>
#include <sstream>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PinDefinitionsAndMore.h>
#include "helpers/helpers.h"
#include "hardware/esp32/infrared_protocols.h"
#include "infrared_sender_RC6_toggle.h"
// #include <applicationInternal/commands_json.h>
#include <applicationInternal/commands_tasks_json.h>
#include "infrared_utils.h"
#include <helpers/omote_log.h>
#include <ir_Daikin.h>
// #include "system_info.h"

// uint8_t IR_LED_GPIO = IR_SEND_PIN; // IR LED output

IRsend IrSender(GPIO_IR_LED, GPIO_IR_LED_INVERTED);

/// Initialise le module d’envoi infrarouge (HAL).
///
/// Configure la broche IR en sortie, applique la polarité (inversée ou non),
/// et démarre l’émetteur.
///
void init_infraredSender_HAL(void)
{
    // IR Pin Definition
    pinMode(GPIO_IR_LED, OUTPUT);
    if (GPIO_IR_LED_INVERTED)
        digitalWrite(GPIO_IR_LED, HIGH); // Init IR_SEND_PIN with HIGH (LED Off) if inverted = true

    IrSender.begin();
    // Serial.println("Ready to send IR signals at pin : " + String(IR_SEND_PIN));
    Serial.print(F("Ready to send IR signals on pin : "));
    Serial.println(GPIO_IR_LED);
}

// IR protocols
[[deprecated("Original code from Omote. Use the new one instead. 'IRprotocols2'")]]
enum IRprotocols {
    IR_PROTOCOL_GC = 0,
    IR_PROTOCOL_NEC = 1,
    IR_PROTOCOL_SAMSUNG = 2,
    IR_PROTOCOL_SONY = 3,
    IR_PROTOCOL_RC5 = 4,
    IR_PROTOCOL_DENON = 5,
    IR_PROTOCOL_SAMSUNG36 = 6
};

[[deprecated("Original code from Omote. Use the new one instead. 'sendIRcode_HAL2'")]]
void sendIRcode_HAL(int protocol, std::list<std::string> commandPayloads, std::string additionalPayload)
{
    switch (protocol)
    {
    case IR_PROTOCOL_GC:
    {
        auto current = commandPayloads.begin();
        std::string arrayStr = *current;
        // first create array of needed size
        std::string::difference_type size = std::count(arrayStr.begin(), arrayStr.end(), ',');
        size += 1;
        uint16_t *buf = new uint16_t[size];
        // now get comma separated values and fill array
        int pos = 0;
        std::stringstream ss(arrayStr);
        while (ss.good())
        {
            std::string dataStr;
            std::getline(ss, dataStr, ',');
            // https://cplusplus.com/reference/string/stoull/
            std::string::size_type sz = 0; // alias of size_t
            const uint64_t data = std::stoull(dataStr, &sz, 0);
            // Serial.printf("  next string data %s (%" PRIu64 ")\r\n", dataStr.c_str(), data);
            buf[pos] = data;
            pos += 1;
        }
        Serial.printf("execute: will send IR GC, array size %d\r\n", size);
        IrSender.sendGC(buf, size);
        delete[] buf;
        break;
    }

    case IR_PROTOCOL_NEC:
    {
        auto current = commandPayloads.begin();
        std::string dataStr = *current;
        // https://cplusplus.com/reference/string/stoull/
        std::string::size_type sz = 0; // alias of size_t
        const uint64_t data = std::stoull(dataStr, &sz, 0);
        Serial.printf("execute: will send IR NEC, data %s (%" PRIu64 ")\r\n", dataStr.c_str(), data);
        IrSender.sendNEC(data);
        break;
    }

    case IR_PROTOCOL_SAMSUNG:
    {
        auto current = commandPayloads.begin();
        std::string dataStr = *current;
        // https://cplusplus.com/reference/string/stoull/
        std::string::size_type sz = 0; // alias of size_t
        const uint64_t data = std::stoull(dataStr, &sz, 0);
        Serial.printf("execute: will send IR SAMSUNG, data %s (%" PRIu64 ")\r\n", dataStr.c_str(), data);
        IrSender.sendSAMSUNG(data);
        break;
    }

    case IR_PROTOCOL_SONY:
    {
        std::string::size_type sz = 0; // alias of size_t
        uint64_t data;
        if (commandPayloads.empty() && (additionalPayload == ""))
        {
            Serial.printf("execute: cannot send IR SONY, because both data and payload are empty\r\n");
        }
        else
        {
            if (additionalPayload != "")
            {
                data = std::stoull(additionalPayload, &sz, 0);
            }
            else
            {
                auto current = commandPayloads.begin();
                data = std::stoull(*current, &sz, 0);
            }
            Serial.printf("execute: will send IR SONY 15 bit, data (%" PRIu64 ")\r\n", data);
            IrSender.sendSony(data, 15);
        }
        break;
    }

    case IR_PROTOCOL_RC5:
    {
        std::string::size_type sz = 0; // alias of size_t
        uint64_t data;
        if (commandPayloads.empty() && (additionalPayload == ""))
        {
            Serial.printf("execute: cannot send IR RC5, because both data and payload are empty\r\n");
        }
        else
        {
            if (additionalPayload != "")
            {
                data = std::stoull(additionalPayload, &sz, 0);
            }
            else
            {
                auto current = commandPayloads.begin();
                data = std::stoull(*current, &sz, 0);
            }
            Serial.printf("execute: will send IR RC5, data (%" PRIu64 ")\r\n", data);
            IrSender.sendRC5(IrSender.encodeRC5X(0x00, data));
        }
        break;
    }

    case IR_PROTOCOL_DENON:
    {
        std::string::size_type sz = 0; // alias of size_t
        uint64_t data;
        if (commandPayloads.empty() && (additionalPayload == ""))
        {
            Serial.printf("execute: cannot send IR DENON 48 bit, because both data and payload are empty\r\n");
        }
        else
        {
            if (additionalPayload != "")
            {
                data = std::stoull(additionalPayload, &sz, 0);
            }
            else
            {
                auto current = commandPayloads.begin();
                data = std::stoull(*current, &sz, 0);
            }
            Serial.printf("execute: will send IR DENON 48 bit, data (%" PRIu64 ")\r\n", data);
            IrSender.sendDenon(data, 48);
        }
        break;
    }

    case IR_PROTOCOL_SAMSUNG36:
    {
        auto current = commandPayloads.begin();
        std::string dataStr = *current;
        // https://cplusplus.com/reference/string/stoull/
        std::string::size_type sz = 0; // alias of size_t
        const uint64_t data = std::stoull(dataStr, &sz, 0);
        Serial.printf("execute: will send IR SAMSUNG36, data %s (%" PRIu64 ")\r\n", dataStr.c_str(), data);
        IrSender.sendSamsung36(data);
        break;
    }
    }
}

// ---------- Fonctions en tests ------------------------------
namespace nsIR
{
    /// Envoie un code IR avec support de paramètres additionnels.
    ///
    /// Cette fonction utilise une liste de payloads standardisés (12 champs)
    /// et un payload additionnel (ex: fréquence, toggleMask, répétition).
    ///
    /// @param commandPayloads Liste des payloads (résultats décodés).
    /// @param additionalPayload Paramètres additionnels sérialisés.
    void sendIRcode_HAL2(std::list<std::string> commandPayloads, std::string additionalPayload)
    {
        // IRremoteESP8266 protocols returns results object witch is converted in Payloads with 12 entries. Entries order is the same for all of this protocols.
        // Specific protocols like SONY-R2 or MCE_T is working with Payloads of respectitively 3 and 4 entries.
        // additionalPayload support optionnal parameters like frequency, tooglemask, repeat...
        // and it is sending by BLE client to allow settings to be completed or replaced remotely.

        if (commandPayloads.empty() && (additionalPayload == ""))
        {
            Serial.printf("IR codes cannot be send, because both data and payload are empty\r\n");
            return;
        }

        // ---------------------------------------------------------------------------------------
        // commandPayloads: Initialise les variables à partir de la liste de chaines de caractères
        // les chaines vide sont interprétées comme des valeurs 0, false....etc
        // ---------------------------------------------------------------------------------------
        // const nsIR::IRprotocols2 enumProtocol = static_cast<nsIR::IRprotocols2>(protocol);
        int protocol;
        bool isRepeat;
        std::string data1Str;
        // uint64_t data1; // value or state[] ???
        uint16_t nbits;
        uint16_t length;
        bool hasAcState;
        std::vector<uint16_t> rawBuf; // uint16_t rawBuffer[];
        uint64_t data2;               //--- value if the decode_type doesn't have an A/C state. --- ???
        std::vector<uint8_t> dataAC;  // Decoded ACState codes to hexa ???
        uint32_t address;
        uint32_t command;
        std::string dataStr;
        uint64_t data; // decoded data

        if (commandPayloads.size() >= 12)
        {
            std::string strValue;
            auto it = commandPayloads.begin();
            // --- protocol ---
            strValue = helpers::noNullOrBlank(&(*it));
            omote_log_v("  generic IR, protocol: %s, payload: %s\r\n", strValue.c_str(), helpers::convertStringListToString(commandPayloads).c_str());
            protocol = (strValue).empty() ? 0 : helpers::convertToType<int>(strValue);
            protocol = protocol ? protocol : 0;
            Serial.printf("[DEBUG] protocol: %s (%u)\r\n", strValue.c_str(), protocol);

            it++;
            // --- isRepeat ---
            isRepeat = helpers::IsTrue(*it);
            Serial.printf("[DEBUG] isRepeat: %d\r\n", isRepeat);

            it++;
            // --- value or state[] to simple hexadecimal ---
            // Ex: 0xC800F040C, Ex for DAIKIN ON/OFF : 0x11DA2700C51000E711DA270042871AF511DA2700004930003F000006600000C1800071 (280bits)
            strValue = helpers::noNullOrBlank(&(*it));
            data1Str = strValue;
            // data1 = (strValue).empty() ? 0 : helpers::convertToType<uint64_t>(strValue, 0); // La conversion de strValue > 64 bits (AC) ne fonctionne pas.
            // data1 = data1 ? data1 : 0x0; // data
            // Serial.printf("[DEBUG] value or state[] to simple hexadecimal: %s (%" PRIu64 ")\r\n", strValue.c_str(), data1);
            Serial.printf("[DEBUG] value or state[] to simple hexadecimal: %s\r\n", strValue.c_str());

            it++;
            // --- nBits ---
            strValue = helpers::noNullOrBlank(&(*it));
            nbits = (strValue).empty() ? 0 : helpers::convertToType<uint16_t>(strValue);
            // nbits = nbits? nbits : 15; // nbits
            Serial.printf("[DEBUG] nbits: %s (%hu)\r\n", strValue.c_str(), nbits);

            it++;
            // --- Lenght ---
            length = (*it).empty() ? 0 : std::stoi(*it);
            Serial.printf("[DEBUG] length: %u\r\n", length);

            it++;
            // --- HasACState ---
            hasAcState = helpers::IsTrue(*it);
            Serial.printf("[DEBUG] hasAcState: %d\r\n", hasAcState);

            it++;
            // --- Raw bugffer ---
            strValue = helpers::noNullOrBlank(&(*it));
            Serial.printf("[DEBUG] rawBuffer: %s\r\n", strValue.c_str());
            rawBuf = stringToRawBuf(strValue);

            it++;
            // --- value if the decode_type doesn't have an A/C state. ---
            // Ex: 0xC800F040C
            strValue = helpers::noNullOrBlank(&(*it));
            data2 = (strValue).empty() ? 0 : helpers::convertToType<uint64_t>(strValue, 0);
            data2 = data2 ? data2 : 0x0;
            Serial.printf("[DEBUG] value without A/C state: %s (%" PRIu64 ")\r\n", strValue.c_str(), data2);

            it++;
            // --- Decoded ACState codes to hexa ---
            strValue = helpers::noNullOrBlank(&(*it));
            Serial.printf("[DEBUG] Decoded ACState: %s\r\n", strValue.c_str());
            dataAC = hexStringToByteArray(strValue);

            it++;
            // --- Decoded Adress part to hexa ---
            strValue = helpers::noNullOrBlank(&(*it));
            address = (strValue).empty() ? 0 : helpers::convertToType<uint32_t>(strValue, 0);
            address = address ? address : 0x0;
            Serial.printf("[DEBUG] address: %s (%" PRIu32 ")\r\n", strValue.c_str(), address);

            it++;
            // --- Decoded Command part to hexa ---
            strValue = helpers::noNullOrBlank(&(*it));
            command = (strValue).empty() ? 0 : helpers::convertToType<uint32_t>(strValue, 0);
            command = command ? command : 0x0;
            Serial.printf("[DEBUG] command: %s (%" PRIu32 ")\r\n", strValue.c_str(), command);

            it++;
            // --- Decoded Value to hexa ---
            // Ex: 0xC800F040CLL Ex DAIKIN: 0xE70010C50027DA11
            strValue = helpers::noNullOrBlank(&(*it));
            dataStr = strValue;
            // Serial.printf("[DEBUG] dataStr: %s\r\n", strValue.c_str());
            //  https://cplusplus.com/reference/string/stoull/
            // std::string::size_type sz = 0;   // alias of size_t
            // data = (strValue).empty() ? 0 : std::stoull(strValue, &sz, 0);
            data = (strValue).empty() ? 0 : helpers::convertToType<uint64_t>(strValue, 0);
            data = data ? data : 0x0; // data
            Serial.printf("[DEBUG] code: %s (%" PRIu64 ")\r\n", strValue.c_str(), data);

            Serial.printf("Some paraméters for commmandPayload, protocol: '%u' %hu bits, data: %s (%" PRIu64 ")\r\n", protocol, nbits, dataStr.c_str(), data);
        }

        // -----------------------------------------------------------------------------------------
        // additionalPayload: Initialise les variables à partir de la liste de chaines de caractères
        // les chaines vide sont remplacées par des valeurs par défaut.
        // -----------------------------------------------------------------------------------------
        Payload payload;
        unsigned short frequency;
        uint32_t toggleMask;
        uint16_t repeat;
        bool raw;

        if (!additionalPayload.empty())
        {
            Serial.printf("Deserialized AdditionnalPayload: '%s'\r\n", additionalPayload.c_str());
            payload = deserializeAdditionnalPayload2(additionalPayload.c_str());

            payload.frequency = helpers::noNullOrBlank(&payload.frequency);
            frequency = (payload.frequency).empty() ? 0 : helpers::convertToType<unsigned short>(payload.frequency);
            frequency = frequency ? frequency : 36;
            Serial.printf("[DEBUG] frequency: %s (%hu)\r\n", payload.frequency.c_str(), frequency);

            payload.toggleMask = helpers::noNullOrBlank(&payload.toggleMask);
            toggleMask = (payload.toggleMask).empty() ? 0 : helpers::convertToType<uint32_t>(payload.toggleMask, 16);
            toggleMask = toggleMask ? toggleMask : 0x0;
            Serial.printf("[DEBUG] toggleMask: %s (%u)\r\n", payload.toggleMask.c_str(), toggleMask);

            payload.repeat = helpers::noNullOrBlank(&payload.repeat);
            repeat = (payload.repeat).empty() ? 0 : helpers::convertToType<uint16_t>(payload.repeat);
            repeat = repeat ? repeat : 0;
            Serial.printf("[DEBUG] repeat: %s (%hu)\r\n", payload.repeat.c_str(), repeat);

            payload.raw = helpers::noNullOrBlank(&payload.raw);
            raw = (payload.raw).empty() ? 0 : helpers::IsTrue(payload.raw);
            raw = raw ? raw : false;
            Serial.printf("[DEBUG] raw: %s (%s)\r\n", payload.raw.c_str(), raw ? "true" : "false");
        }

        // See also raw protocol
        if (raw)
        {
            Serial.printf("Sending RAW rawBuf '%s', frequency '%hu'\r\n", payload.raw.c_str(), frequency);
            IrSender.sendRaw(rawBuf.data(), rawBuf.size(), frequency);
            return;
        }

        switch (protocol)
        {
        case nsIR::UNKNOWN:
        {

            break;
        }

        case nsIR::RC5:
        {
            // IrSender.sendRC5(IrSender.encodeRC5X(0x00, data));
            Serial.printf("Sending RC5 data '%s'\r\n", dataStr.c_str());
            IrSender.sendRC5(data);
            break;
        }

        case nsIR::RC6:
        {
            // frequency.empty()? frequency : "36";
            // frequency = frequency.empty()? "36" : frequency;
            // nbits = nbits? nbits : 36;
            // if (!toggleMask.empty()) {
            if (toggleMask > 0)
            {
                // int toggle_bit_mask = std::stoi(toggleMask, nullptr, 16);
                Serial.printf("Sending RC6 data '%s' with bits '%hu', toggleMask: '%s' (%" PRIu64 ")\r\n", dataStr.c_str(), nbits, payload.toggleMask.c_str(), toggleMask);
                sendRC6Toggle(IrSender, data, nbits, toggleMask);
            }
            else
            {
                // Serial.println("[DEBUG] Sending RC6");
                Serial.printf("Sending RC6 data '%s' with bits: '%hu'\r\n", dataStr.c_str(), nbits);
                IrSender.sendRC6(data, nbits);
            }
            break;
        }

        case nsIR::NEC:
        {
            Serial.printf("Sending NEC data '%s'\r\n", dataStr.c_str());
            IrSender.sendNEC(data);
            break;
        }

        case nsIR::SONY:
        {
            // repeat.empty()? repeat : "1";
            // repeat = repeat.empty()? "1" : repeat;
            // nbits = nbits? nbits : 15;
            Serial.printf("Sending SONY data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSony(data, nbits, repeat);
            break;
        }

        case nsIR::PANASONIC:
        {
            // Serial.printf("Sending PANASONIC data '%s' with address '%u', nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), address, nbits, repeat);
            // IrSender.sendPanasonic(address, data, nbits, repeat);
            Serial.printf("Sending PANASONIC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendPanasonic64(data, nbits, repeat);
            // IrSender.sendPanasonicAC32(data, nbits, repeat);
            // IrSender.sendPanasonicAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::JVC:
        {
            Serial.printf("Sending JVC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendJVC(data, nbits, repeat);
            break;
        }

        case nsIR::SAMSUNG:
        {
            Serial.printf("Sending SAMSUNG data '%s'\r\n", dataStr.c_str());
            IrSender.sendSAMSUNG(data);
            break;
        }

        case nsIR::WHYNTER:
        {
            Serial.printf("Sending WHYNTER data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendWhynter(data, nbits, repeat);
            break;
        }

        case nsIR::AIWA_RC_T501:
        {
            Serial.printf("Sending AIWA_RC_T501 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendAiwaRCT501(data, nbits, repeat);
            break;
        }

        case nsIR::LG:
        {
            Serial.printf("Sending LG data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendLG(data, nbits, repeat);
            IrSender.sendLG2(data, nbits, repeat);
            break;
        }

        case nsIR::SANYO:
        {
            Serial.printf("Sending SANYO data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendSanyoAc152(data, dataAC.size(), repeat);
            // IrSender.sendSanyoAc88(data, dataAC.size(), repeat);
            // IrSender.sendSanyoAc(data, dataAC.size(), repeat);
            IrSender.sendSanyoLC7461(data, nbits, repeat); // See also SanyoLC7461
            break;
        }

        case nsIR::MITSUBISHI:
        {
            Serial.printf("Sending MITSUBISHI data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendMitsubishi112(dataAC.data() dataAC.size(), repeat);
            // IrSender.sendMitsubishi136(dataAC.data() dataAC.size(), repeat);
            // IrSender.sendMitsubishi2(data, nbits, repeat);
            IrSender.sendMitsubishi(data, nbits, repeat);
            // IrSender.sendMitsubishiAC(dataAC.data() dataAC.size(), repeat);
            // IrSender.sendMitsubishiHeavy152(dataAC.data() dataAC.size(), repeat);
            // IrSender.sendMitsubishiHeavy88(dataAC.data() dataAC.size(), repeat);
            break;
        }

        case nsIR::DISH:
        {
            Serial.printf("Sending DISH data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendDISH(data, nbits, repeat);
            break;
        }

        case nsIR::SHARP:
        {
            Serial.printf("Sending SHARP data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSharp(address, command, nbits, repeat);
            // IrSender.sendSharpAc(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendSharpRaw(data, nbits, repeat);
            break;
        }

        case nsIR::COOLIX:
        {
            Serial.printf("Sending COOLIX data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendCoolix48(data, nbits, repeat);
            IrSender.sendCOOLIX(data, nbits, repeat);
            break;
        }

        case nsIR::DAIKIN:
        {
            Serial.printf("Sending DAIKIN data '%s' with nbits '%hu', repeat: '%hu'\r\n", data1Str.c_str(), nbits, repeat);
            // IrSender.sendDaikin128(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin152(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin160(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin176(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin200(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin216(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin2(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendDaikin312(dataAC.data(), dataAC.size(), repeat);
            ////////////////////////////IrSender.sendDaikin64(data, nbits, repeat);

            // ta trame longue (avec ou sans le préfixe "0x")
            // const std::string longHex = "0x11DA2700C51000E711DA270042681AD611DA2700004830003F000006600000C1800070";
            // data1Str = longHex;

            std::vector<uint8_t> bytes;
            if (!helpers::hexStringToBytes(data1Str, bytes))
            {
                Serial.println(F("Erreur: hex invalide"));
                return;
            }

            Serial.printf("Bytes len = %u\n", (unsigned)bytes.size());
            Serial.print("Octets: ");
            for (auto b : bytes)
            {
                Serial.printf("%02X ", b);
            }
            Serial.println();

            // Création de l'objet Daikin
            IRDaikinESP ac(GPIO_IR_LED);
            ac.begin(); // initialise l'envoi IR (timings, PWM, ...)

            // 1) Méthode recommandée : injecter l'état brut dans l'objet Daikin,
            // puis appeler send() pour que la lib construise et envoie header+long frame.
            ac.setRaw(bytes.data(), static_cast<uint16_t>(bytes.size()));
            Serial.println("Etat interne mis à jour via setRaw(...). Envoi...");
            ac.send(); // envoie header + trame longue

            Serial.println("Envoi termine (setRaw -> send).");

            // IrSender.sendDaikin(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DENON:
        {
            // nbits = nbits? nbits : 48;
            Serial.printf("Sending DENON data '%s' with nbits '%u'\r\n", dataStr.c_str(), nbits);
            IrSender.sendDenon(data, nbits);
            break;
        }

        case nsIR::KELVINATOR:
        {
            Serial.printf("Sending KELVINATOR data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendKelvinator(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SHERWOOD:
        {
            Serial.printf("Sending SHERWOOD data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSherwood(data, nbits, repeat);
            break;
        }

        case nsIR::MITSUBISHI_AC:
        {
            Serial.printf("Sending MITSUBISHI_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            // IrSender.sendMitsubishi112(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendMitsubishi136(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendMitsubishi2(data, nbits, repeat);
            IrSender.sendMitsubishiAC(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendMitsubishiHeavy152(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendMitsubishiHeavy88(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::RCMM:
        {
            Serial.printf("Sending RCMM data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendRCMM(data, nbits, repeat);
            break;
        }

        case nsIR::SANYO_LC7461:
        {
            Serial.printf("Sending SANYO_LC7461 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSanyoLC7461(data, nbits, repeat);
            break;
        }

        case nsIR::RC5X:
        {
            Serial.printf("Sending RC5X data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendRC5(data, nbits, repeat);
            break;
        }

        case nsIR::GREE:
        {
            Serial.printf("Sending GREE data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendGree(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::PRONTO:
        {
            // Serial.printf("Sending PRONTO data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendPronto(dataAC.data(), len, repeat);
            break;
        }

        case nsIR::NEC_LIKE:
        {
            Serial.printf("Sending NEC_LIKE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendNEC(data, nbits, repeat);
            break;
        }

        case nsIR::ARGO:
        {
            Serial.printf("Sending ARGO data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            // IrSender.sendArgo(dataAC.data(), dataAC.size(), repeat, sendFooter);
            IrSender.sendArgo(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::TROTEC:
        {
            Serial.printf("Sending TROTEC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            // IrSender.sendTrotec3550(dataAC.data(), nbyes, repeat);
            IrSender.sendTrotec(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::NIKAI:
        {
            Serial.printf("Sending NIKAI data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendNikai(data, nbits, repeat);
            break;
        }

        // See raw
        case nsIR::RAW:
        {
            Serial.printf("Sending RAW rawBuf '%s', frequency '%hu'\r\n", payload.raw.c_str(), frequency);
            IrSender.sendRaw(rawBuf.data(), rawBuf.size(), frequency);
            break;
        }

        case nsIR::GLOBALCACHE:
        {
            // Serial.printf("Sending GLOBALCACHE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendGC(dataAC.data(), len);
            break;
        }

        case nsIR::TOSHIBA_AC:
        {
            Serial.printf("Sending TOSHIBA_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendToshibaAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::FUJITSU_AC:
        {
            Serial.printf("Sending FUJITSU_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendFujitsuAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MIDEA:
        {
            Serial.printf("Sending MIDEA data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendMidea24(data, nbits, repeat);
            IrSender.sendMidea(data, nbits, repeat);
            break;
        }

        case nsIR::MAGIQUEST:
        {
            Serial.printf("Sending MAGIQUEST data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMagiQuest(data, nbits, repeat);
            break;
        }

        case nsIR::LASERTAG:
        {
            Serial.printf("Sending LASERTAG data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendLasertag(data, nbits, repeat);
            break;
        }

        case nsIR::CARRIER_AC:
        {
            Serial.printf("Sending CARRIER_AC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            // IrSender.sendCarrierAC128(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendCarrierAC40(data, nbits, repeat);
            // IrSender.sendCarrierAC64(data, nbits, repeat);
            // IrSender.sendCarrierAC84(dataAC.data(), dataAC.size(), repeat);
            IrSender.sendCarrierAC(data, nbits, repeat);
            break;
        }

        case nsIR::HAIER_AC:
        {
            Serial.printf("Sending HAIER_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            // IrSender.sendHaierAC160(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHaierAC176(dataAC.data(), dataAC.size(), repeat);
            IrSender.sendHaierAC(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHaierACYRW02(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MITSUBISHI2:
        {
            Serial.printf("Sending MITSUBISHI2 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMitsubishi2(data, nbits, repeat);
            break;
        }

        case nsIR::HITACHI_AC:
        {
            Serial.printf("Sending HITACHI_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            // IrSender.sendHitachiAC1(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAc264(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAc296(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAC2(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAc344(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAc3(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendHitachiAc424(dataAC.data(), dataAC.size(), repeat);
            IrSender.sendHitachiAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::HITACHI_AC1:
        {
            Serial.printf("Sending HITACHI_AC1 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::HITACHI_AC2:
        {
            Serial.printf("Sending HITACHI_AC2 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAC2(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::GICABLE:
        {
            Serial.printf("Sending GICABLE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendGICable(data, nbits, repeat);
            break;
        }

        case nsIR::HAIER_AC_YRW02:
        {
            Serial.printf("Sending HAIER_AC_YRW02 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHaierACYRW02(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::WHIRLPOOL_AC:
        {
            Serial.printf("Sending WHIRLPOOL_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendWhirlpoolAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SAMSUNG_AC:
        {
            Serial.printf("Sending SAMSUNG_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendSamsungAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::LUTRON:
        {
            Serial.printf("Sending LUTRON data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendLutron(data, nbits, repeat);
            break;
        }

        case nsIR::ELECTRA_AC:
        {
            Serial.printf("Sending ELECTRA_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendElectraAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::PANASONIC_AC:
        {
            Serial.printf("Sending PANASONIC_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendPanasonicAC(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::PIONEER:
        {
            Serial.printf("Sending PIONEER data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendPioneer(data, nbits, repeat);
            break;
        }

        case nsIR::LG2:
        {
            Serial.printf("Sending LG2 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendLG2(data, nbits, repeat);
            break;
        }

        case nsIR::MWM:
        {
            Serial.printf("Sending MWM data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMWM(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN2:
        {
            Serial.printf("Sending DAIKIN2 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin2(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::VESTEL_AC:
        {
            Serial.printf("Sending VESTEL_AC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendVestelAc(data, nbits, repeat);
            break;
        }

        case nsIR::TECO:
        {
            Serial.printf("Sending TECO data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendTeco(data, nbits, repeat);
            break;
        }

        case nsIR::SAMSUNG36:
        {
            Serial.printf("Sending SAMSUNG36 data '%s'\r\n", dataStr.c_str());
            IrSender.sendSamsung36(data);
            break;
        }

        case nsIR::TCL112AC:
        {
            Serial.printf("Sending TCL112AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendTcl112Ac(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendTcl96Ac(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::LEGOPF:
        {
            Serial.printf("Sending LEGOPF data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendLegoPf(data, nbits, repeat);
            break;
        }

        case nsIR::MITSUBISHI_HEAVY_88:
        {
            Serial.printf("Sending MITSUBISHI_HEAVY_88 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMitsubishiHeavy88(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MITSUBISHI_HEAVY_152:
        {
            Serial.printf("Sending MITSUBISHI_HEAVY_152 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMitsubishiHeavy152(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN216:
        {
            Serial.printf("Sending DAIKIN216 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin216(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SHARP_AC:
        {
            Serial.printf("Sending SHARP_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendSharpAc(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::GOODWEATHER:
        {
            Serial.printf("Sending GOODWEATHER data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendGoodweather(data, nbits, repeat);
            break;
        }

        case nsIR::INAX:
        {
            Serial.printf("Sending INAX data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendInax(data, nbits, repeat);
            break;
        }

        case nsIR::DAIKIN160:
        {
            Serial.printf("Sending DAIKIN160 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin160(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::NEOCLIMA:
        {
            Serial.printf("Sending NEOCLIMA data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendNeoclima(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN176:
        {
            Serial.printf("Sending DAIKIN176 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin176(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN128:
        {
            Serial.printf("Sending DAIKIN128 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin128(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::AMCOR:
        {
            Serial.printf("Sending AMCOR data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendAmcor(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN152:
        {
            Serial.printf("Sending DAIKIN152 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin152(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MITSUBISHI136:
        {
            Serial.printf("Sending MITSUBISHI136 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMitsubishi136(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MITSUBISHI112:
        {
            Serial.printf("Sending MITSUBISHI112 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMitsubishi112(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::HITACHI_AC424:
        {
            Serial.printf("Sending HITACHI_AC424 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAc424(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SONY_38K:
        {
            Serial.printf("Sending SONY_38K data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSony38(data, nbits, repeat);
            break;
        }

        case nsIR::EPSON:
        {
            Serial.printf("Sending EPSON data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendEpson(data, nbits, repeat);
            break;
        }

        case nsIR::SYMPHONY:
        {
            Serial.printf("Sending SYMPHONY data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendSymphony(data, nbits, repeat);
            break;
        }

        case nsIR::HITACHI_AC3:
        {
            Serial.printf("Sending HITACHI_AC3 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAc3(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN64:
        {
            Serial.printf("Sending DAIKIN64 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendDaikin64(data, nbits, repeat);
            break;
        }

        case nsIR::AIRWELL:
        {
            Serial.printf("Sending AIRWELL data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendAirwell(data, nbits, repeat);
            break;
        }

        case nsIR::DELONGHI_AC:
        {
            Serial.printf("Sending DELONGHI_AC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendDelonghiAc(data, nbits, repeat);
            break;
        }

        case nsIR::DOSHISHA:
        {
            Serial.printf("Sending DOSHISHA data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendDoshisha(data, nbits, repeat);
            break;
        }

        case nsIR::MULTIBRACKETS:
        {
            Serial.printf("Sending MULTIBRACKETS data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMultibrackets(data, nbits, repeat);
            break;
        }

        case nsIR::CARRIER_AC40:
        {
            Serial.printf("Sending CARRIER_AC40 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendCarrierAC40(data, nbits, repeat);
            break;
        }

        case nsIR::CARRIER_AC64:
        {
            Serial.printf("Sending CARRIER_AC64 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendCarrierAC64(data, nbits, repeat);
            break;
        }

        case nsIR::HITACHI_AC344:
        {
            Serial.printf("Sending HITACHI_AC344 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAc344(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::CORONA_AC:
        {
            Serial.printf("Sending CORONA_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendCoronaAc(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::MIDEA24:
        {
            Serial.printf("Sending MIDEA24 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMidea24(data, nbits, repeat);
            break;
        }

        case nsIR::ZEPEAL:
        {
            Serial.printf("Sending ZEPEAL data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendZepeal(data, nbits, repeat);
            break;
        }

        case nsIR::SANYO_AC:
        {
            Serial.printf("Sending SANYO_AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendSanyoAc(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::VOLTAS:
        {
            Serial.printf("Sending VOLTAS data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendVoltas(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::METZ:
        {
            Serial.printf("Sending METZ data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMetz(data, nbits, repeat);
            break;
        }

        case nsIR::TRANSCOLD:
        {
            Serial.printf("Sending TRANSCOLD data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendTranscold(data, nbits, repeat);
            break;
        }

        case nsIR::TECHNIBEL_AC:
        {
            Serial.printf("Sending TECHNIBEL_AC data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendTechnibelAc(data, nbits, repeat);
            break;
        }

        case nsIR::MIRAGE:
        {
            Serial.printf("Sending MIRAGE data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendMirage(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::ELITESCREENS:
        {
            Serial.printf("Sending ELITESCREENS data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendElitescreens(data, nbits, repeat);
            break;
        }

        case nsIR::PANASONIC_AC32:
        {
            Serial.printf("Sending PANASONIC_AC32 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendPanasonicAC32(data, nbits, repeat);
            break;
        }

        case nsIR::MILESTAG2:
        {
            Serial.printf("Sending MILESTAG2 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendMilestag2(data, nbits, repeat);
            break;
        }

        case nsIR::ECOCLIM:
        {
            Serial.printf("Sending ECOCLIM data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendEcoclim(data, nbits, repeat);
            break;
        }

        case nsIR::XMP:
        {
            Serial.printf("Sending XMP data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendXmp(data, nbits, repeat);
            break;
        }

        case nsIR::TRUMA:
        {
            Serial.printf("Sending TRUMA data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendTruma(data, nbits, repeat);
            break;
        }

        case nsIR::HAIER_AC176:
        {
            Serial.printf("Sending HAIER_AC176 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHaierAC176(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::TEKNOPOINT:
        {
            Serial.printf("Sending TEKNOPOINT data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendTeknopoint(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::KELON:
        {
            Serial.printf("Sending KELON data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendKelon(data, nbits, repeat);
            // IrSender.sendKelon168(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::TROTEC_3550:
        {
            Serial.printf("Sending TROTEC_3550 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendTrotec3550(dataAC.data(), dataAC.size(), repeat);
            // IrSender.sendTrotec(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SANYO_AC88:
        {
            Serial.printf("Sending SANYO_AC88 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendSanyoAc88(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::BOSE:
        {
            Serial.printf("Sending BOSE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendBose(data, nbits, repeat);
            break;
        }

        case nsIR::ARRIS:
        {
            Serial.printf("Sending ARRIS data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendArris(data, nbits, repeat);
            break;
        }

        case nsIR::RHOSS:
        {
            Serial.printf("Sending RHOSS data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendRhoss(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::AIRTON:
        {
            Serial.printf("Sending AIRTON data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendAirton(data, nbits, repeat);
            break;
        }

        case nsIR::COOLIX48:
        {
            Serial.printf("Sending COOLIX48 data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendCoolix48(data, nbits, repeat);
            break;
        }

        case nsIR::HITACHI_AC264:
        {
            Serial.printf("Sending HITACHI_AC264 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAc264(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::KELON168:
        {
            Serial.printf("Sending KELON168 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendKelon168(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::HITACHI_AC296:
        {
            Serial.printf("Sending HITACHI_AC296 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHitachiAc296(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN200:
        {
            Serial.printf("Sending DAIKIN200 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin200(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::HAIER_AC160:
        {
            Serial.printf("Sending HAIER_AC160 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendHaierAC160(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::CARRIER_AC128:
        {
            Serial.printf("Sending CARRIER_AC128 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendCarrierAC128(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::TOTO:
        {
            Serial.printf("Sending TOTO data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendToto(data, nbits, repeat);
            break;
        }

        case nsIR::CLIMABUTLER:
        {
            Serial.printf("Sending CLIMABUTLER data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendClimaButler(data, nbits, repeat);
            break;
        }

        case nsIR::TCL96AC:
        {
            Serial.printf("Sending TCL96AC data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendTcl96Ac(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::BOSCH144:
        {
            Serial.printf("Sending BOSCH144 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendBosch144(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::SANYO_AC152:
        {
            Serial.printf("Sending SANYO_AC152 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendSanyoAc152(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::DAIKIN312:
        {
            Serial.printf("Sending DAIKIN312 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendDaikin312(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::GORENJE:
        {
            Serial.printf("Sending GORENJE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendGorenje(data, nbits, repeat);
            break;
        }

        case nsIR::WOWWEE:
        {
            Serial.printf("Sending WOWWEE data '%s' with nbits '%hu', repeat: '%hu'\r\n", dataStr.c_str(), nbits, repeat);
            IrSender.sendWowwee(data, nbits, repeat);
            break;
        }

        case nsIR::CARRIER_AC84:
        {
            Serial.printf("Sending CARRIER_AC84 data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendCarrierAC84(dataAC.data(), dataAC.size(), repeat);
            break;
        }

        case nsIR::YORK:
        {
            Serial.printf("Sending YORK data '%s' with repeat: '%hu'\r\n", dataStr.c_str(), repeat);
            IrSender.sendYork(dataAC.data(), dataAC.size(), repeat);
            break;
        }

            /*
            // Now not standard IRremote8266 protocol
            case nsIR::SONY_R2:
            {
              sendSonyR2_HAL(commandPayloads);
              break;
            }

            case nsIR::MCE_T:
            {
              sendMceT_HAL(commandPayloads);
              break;
            }
            */
        }
    }

    void sendSonyR2_HAL(const std::list<std::string> &commandPayloads)
    {
        if (commandPayloads.size() >= 4)
        {
            // Serial.println("Send SONY xxxxxx");
            //  Création d'un itérateur pour parcourir la liste
            auto it = commandPayloads.begin();
            // Extraction et conversion des éléments de la liste
            // auto current = commandPayloads.begin();
            // std::string dataStr = *current;

            // Protocol:

            it++;
            std::string dataStr = *it++;
            // https://cplusplus.com/reference/string/stoull/
            std::string::size_type sz = 0; // alias of size_t
            const uint64_t data = std::stoull(dataStr, &sz, 0);

            // IRprotocols2 protocol = static_cast<IRprotocols2>(std::stoi(*it++));
            // int frequency = 36;
            // int nbits = std::stoi(*it++);
            uint16_t nbits = std::stoi(*it++);
            // Serial.printf("nbits: '%u'\r\n", nbits);
            // int toggle = 0;
            // int repeat = std::stoi(*it++);
            uint16_t repeat = std::stoi(*it++);
            // int hexValue = hexStringToInt(*it++);
            // int intValue1 = std::stoi(*it++);
            // int intValue2 = std::stoi(*it++);
            Serial.printf("Sending specific protocol SONY_R2 %hu sended with data: '%s' (%" PRIu64 ")\r\n", nbits, dataStr.c_str(), data);
            Serial.printf("Additionnal parameters: repeat: '%hu'.\r\n", repeat);
            IrSender.sendSony(data, nbits, repeat);
        }
    }

    void sendMceT_HAL(const std::list<std::string> &commandPayloads)
    {
        if (commandPayloads.size() >= 5)
        {
            // Création d'un itérateur pour parcourir la liste
            auto it = commandPayloads.begin();
            // Extraction et conversion des éléments de la liste
            // Protocol:

            it++;
            // Code : Ex: 0xC800F040CLL
            std::string dataStr = *it++;
            // https://cplusplus.com/reference/string/stoull/
            std::string::size_type sz = 0; // alias of size_t
            const uint64_t data = std::stoull(dataStr, &sz, 0);
            // Serial.printf("execute: will send IR MCE, data %s (%" PRIu64 ")\r\n", dataStr.c_str(), data);
            // IRprotocols2 protocol = static_cast<IRprotocols2>(std::stoi(*it++));

            // nbits : Ex: 36
            // int nbits = std::stoi(*it++);
            uint16_t nbits = std::stoi(*it++);

            // toggle_bit_mask = Ex: 0x8000;
            int toggle_bit_mask = std::stoi(*it++, nullptr, 16);
            // Serial.println("Mask : '" + String(toggle_bit_mask));

            // Send with Toggle or Send RC6
            Serial.printf("Sending specific protocol MCE_T %hu sended with data: '%s' (%" PRIu64 ")\r\n", nbits, dataStr.c_str(), data);
            Serial.printf("Additionnal parameters: bits: %hu', toggleMask: '%s'\r\n", nbits);
            bool toggle = helpers::IsTrue(*it++);
            if (toggle)
            {
                // sendRC6Toggle(command, frequency, toggle_bit_mask);
                sendRC6Toggle(IrSender, data, nbits, toggle_bit_mask);
            }
            else
            {
                IrSender.sendRC6(data, nbits);
            }
        }
    }

} // Namespace
