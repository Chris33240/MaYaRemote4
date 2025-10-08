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

  if (payloads.size() >= 12)
  {
    // Création d'un itérateur pour parcourir la liste
    auto it = payloads.begin();
    // Extraction et conversion des éléments de la liste

    // Protocol
    std::string protocol = *it++;
    // IRprotocols2 protocol = static_cast<IRprotocols2>(std::stoi(*it++));
    // Serial.println(protocol.c_str());

    // Repeat
    bool repeat = helpers::IsTrue(*it++);

    // Code
    std::string dataStr = *it++;
    // https://cplusplus.com/reference/string/stoull/
    std::string::size_type sz = 0; // alias of size_t
    const uint64_t data = std::stoull(dataStr, &sz, 0);

    // Registered command with free Key
    std::string key = "IR_" + protocol + "_" + dataStr; // Ex: "RC6_0x21354";
    int x = 0;
    int &ref = x;
    std::string freeKey = getFreeKey(key, x);
    commandData2 commandData = makeCommandData2(IR, "WRITE", "w", payloads);
    register_command2(freeKey, commandData);

    //  Save commands
#if (ENABLED_IO_FILESYSTEM && ENABLED_IO_FILESYSTEM_SAVING)
    saveCommand(freeKey, commandData);
#endif

    // Send message to Gui
    pGlobalStatusLED->startBlinkingForDuration(BLINK_LED_CAPTURE_DONE_INTERVAL, BLINK_LED_CAPTURE_DONE_DURATION);
    Serial.println(F(">> Capture Successfull !"));
    // sendBleNotifyCode("102");
    // delay(500); 2 notifications sucessives ne fonctionnent pas malgrés le delay.
    sendBleNotifyNewCommandCaptured(freeKey);
  }
}
