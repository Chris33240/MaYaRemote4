#include "commandHandler.h"
// #include <sstream>
// #include <algorithm>
// #include <stdexcept>
// #include "applicationInternal/scenes/sceneHandler.h"
#include "interfaces/hardwarePresenter.h"
#include "helpers/omote_log.h"
// #include <HardwareSerial.h>
// #include "devices/misc/device_specialCommands.h"
//  show WiFi status
// #include "applicationInternal/gui/guiBase.h"
//  show received IR and MQTT messages
// #include "guis/gui_irReceiver.h"

#include <thread>
#include "globals.h"
#include "setup.h"
#include "system_info.h"
#include "commands_json_bootloader.h"
#include "commands_json.h"
#include "command_ir_capture.h"
#include "commands_io.h"
#include "helpers/helpers.h"

/*
uint16_t COMMAND_UNKNOWN;

uint16_t KEYBOARD_DUMMY_UP                  ; //"Keyboard_dummy_up"
uint16_t KEYBOARD_DUMMY_DOWN                ; //"Keyboard_dummy_down"
uint16_t KEYBOARD_DUMMY_RIGHT               ; //"Keyboard_dummy_right"
uint16_t KEYBOARD_DUMMY_LEFT                ; //"Keyboard_dummy_left"
uint16_t KEYBOARD_DUMMY_SELECT              ; //"Keyboard_dummy_select"
uint16_t KEYBOARD_DUMMY_SENDSTRING          ; //"Keyboard_dummy_sendstring"
uint16_t KEYBOARD_DUMMY_BACK                ; //"Keyboard_dummy_back"
uint16_t KEYBOARD_DUMMY_HOME                ; //"Keyboard_dummy_home"
uint16_t KEYBOARD_DUMMY_MENU                ; //"Keyboard_dummy_menu"
uint16_t KEYBOARD_DUMMY_SCAN_PREVIOUS_TRACK ; //"Keyboard_dummy_scan_previous_track"
uint16_t KEYBOARD_DUMMY_REWIND_LONG         ; //"Keyboard_dummy_rewind_long"
uint16_t KEYBOARD_DUMMY_REWIND              ; //"Keyboard_dummy_rewind"
uint16_t KEYBOARD_DUMMY_PLAYPAUSE           ; //"Keyboard_dummy_playpause"
uint16_t KEYBOARD_DUMMY_FASTFORWARD         ; //"Keyboard_dummy_fastforward"
uint16_t KEYBOARD_DUMMY_FASTFORWARD_LONG    ; //"Keyboard_dummy_fastforward_long"
uint16_t KEYBOARD_DUMMY_SCAN_NEXT_TRACK     ; //"Keyboard_dummy_scan_next_track"
uint16_t KEYBOARD_DUMMY_MUTE                ; //"Keyboard_dummy_mute"
uint16_t KEYBOARD_DUMMY_VOLUME_INCREMENT    ; //"Keyboard_dummy_volume_increment"
uint16_t KEYBOARD_DUMMY_VOLUME_DECREMENT    ; //"Keyboard_dummy_volume_decrement"

uint16_t KEYBOARD_UP                        ; //PPCAT(KEYBOARD_PREFIX, UP)
uint16_t KEYBOARD_DOWN                      ; //PPCAT(KEYBOARD_PREFIX, DOWN)
uint16_t KEYBOARD_RIGHT                     ; //PPCAT(KEYBOARD_PREFIX, RIGHT)
uint16_t KEYBOARD_LEFT                      ; //PPCAT(KEYBOARD_PREFIX, LEFT)
uint16_t KEYBOARD_SELECT                    ; //PPCAT(KEYBOARD_PREFIX, SELECT)
uint16_t KEYBOARD_SENDSTRING                ; //PPCAT(KEYBOARD_PREFIX, SENDSTRING)
uint16_t KEYBOARD_BACK                      ; //PPCAT(KEYBOARD_PREFIX, BACK)
uint16_t KEYBOARD_HOME                      ; //PPCAT(KEYBOARD_PREFIX, HOME)
uint16_t KEYBOARD_MENU                      ; //PPCAT(KEYBOARD_PREFIX, MENU)
uint16_t KEYBOARD_SCAN_PREVIOUS_TRACK       ; //PPCAT(KEYBOARD_PREFIX, SCAN_PREVIOUS_TRACK)
uint16_t KEYBOARD_REWIND_LONG               ; //PPCAT(KEYBOARD_PREFIX, REWIND_LONG)
uint16_t KEYBOARD_REWIND                    ; //PPCAT(KEYBOARD_PREFIX, REWIND)
uint16_t KEYBOARD_PLAYPAUSE                 ; //PPCAT(KEYBOARD_PREFIX, PLAYPAUSE)
uint16_t KEYBOARD_FASTFORWARD               ; //PPCAT(KEYBOARD_PREFIX, FASTFORWARD)
uint16_t KEYBOARD_FASTFORWARD_LONG          ; //PPCAT(KEYBOARD_PREFIX, FASTFORWARD_LONG)
uint16_t KEYBOARD_SCAN_NEXT_TRACK           ; //PPCAT(KEYBOARD_PREFIX, SCAN_NEXT_TRACK)
uint16_t KEYBOARD_MUTE                      ; //PPCAT(KEYBOARD_PREFIX, MUTE)
uint16_t KEYBOARD_VOLUME_INCREMENT          ; //PPCAT(KEYBOARD_PREFIX, VOLUME_INCREMENT)
uint16_t KEYBOARD_VOLUME_DECREMENT          ; //PPCAT(KEYBOARD_PREFIX, VOLUME_DECREMENT)

std::map<uint16_t, commandData> commands;

uint16_t uniqueCommandID = 0;

// we don't yet have a command id
void register_command(uint16_t *command, commandData aCommandData) {
  *command = uniqueCommandID;
  uniqueCommandID++;

  commands[*command] = aCommandData;
}
// we already have a command id. Only used by BLE keyboard
void register_command_withID(uint16_t command, commandData aCommandData) {
  commands[command] = aCommandData;
}
// only get a unique ID. used by KEYBOARD_DUMMY, COMMAND_UNKNOWN and BLE keyboard
void get_uniqueCommandID(uint16_t *command) {
  *command = uniqueCommandID;
  uniqueCommandID++;
}

void register_keyboardCommands() {
  get_uniqueCommandID(&KEYBOARD_DUMMY_UP                  );
  get_uniqueCommandID(&KEYBOARD_DUMMY_DOWN                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_RIGHT               );
  get_uniqueCommandID(&KEYBOARD_DUMMY_LEFT                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_SELECT              );
  get_uniqueCommandID(&KEYBOARD_DUMMY_SENDSTRING          );
  get_uniqueCommandID(&KEYBOARD_DUMMY_BACK                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_HOME                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_MENU                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_SCAN_PREVIOUS_TRACK );
  get_uniqueCommandID(&KEYBOARD_DUMMY_REWIND_LONG         );
  get_uniqueCommandID(&KEYBOARD_DUMMY_REWIND              );
  get_uniqueCommandID(&KEYBOARD_DUMMY_PLAYPAUSE           );
  get_uniqueCommandID(&KEYBOARD_DUMMY_FASTFORWARD         );
  get_uniqueCommandID(&KEYBOARD_DUMMY_FASTFORWARD_LONG    );
  get_uniqueCommandID(&KEYBOARD_DUMMY_SCAN_NEXT_TRACK     );
  get_uniqueCommandID(&KEYBOARD_DUMMY_MUTE                );
  get_uniqueCommandID(&KEYBOARD_DUMMY_VOLUME_INCREMENT    );
  get_uniqueCommandID(&KEYBOARD_DUMMY_VOLUME_DECREMENT    );

#if (ENABLE_KEYBOARD_BLE == 1)
  KEYBOARD_UP                  = KEYBOARD_BLE_UP;
  KEYBOARD_DOWN                = KEYBOARD_BLE_DOWN;
  KEYBOARD_RIGHT               = KEYBOARD_BLE_RIGHT;
  KEYBOARD_LEFT                = KEYBOARD_BLE_LEFT;
  KEYBOARD_SELECT              = KEYBOARD_BLE_SELECT;
  KEYBOARD_SENDSTRING          = KEYBOARD_BLE_SENDSTRING;
  KEYBOARD_BACK                = KEYBOARD_BLE_BACK;
  KEYBOARD_HOME                = KEYBOARD_BLE_HOME;
  KEYBOARD_MENU                = KEYBOARD_BLE_MENU;
  KEYBOARD_SCAN_PREVIOUS_TRACK = KEYBOARD_BLE_SCAN_PREVIOUS_TRACK;
  KEYBOARD_REWIND_LONG         = KEYBOARD_BLE_REWIND_LONG;
  KEYBOARD_REWIND              = KEYBOARD_BLE_REWIND;
  KEYBOARD_PLAYPAUSE           = KEYBOARD_BLE_PLAYPAUSE;
  KEYBOARD_FASTFORWARD         = KEYBOARD_BLE_FASTFORWARD;
  KEYBOARD_FASTFORWARD_LONG    = KEYBOARD_BLE_FASTFORWARD_LONG;
  KEYBOARD_SCAN_NEXT_TRACK     = KEYBOARD_BLE_SCAN_NEXT_TRACK;
  KEYBOARD_MUTE                = KEYBOARD_BLE_MUTE;
  KEYBOARD_VOLUME_INCREMENT    = KEYBOARD_BLE_VOLUME_INCREMENT;
  KEYBOARD_VOLUME_DECREMENT    = KEYBOARD_BLE_VOLUME_DECREMENT;
#elif (ENABLE_KEYBOARD_MQTT == 1)
  KEYBOARD_UP                  = KEYBOARD_MQTT_UP;
  KEYBOARD_DOWN                = KEYBOARD_MQTT_DOWN;
  KEYBOARD_RIGHT               = KEYBOARD_MQTT_RIGHT;
  KEYBOARD_LEFT                = KEYBOARD_MQTT_LEFT;
  KEYBOARD_SELECT              = KEYBOARD_MQTT_SELECT;
  KEYBOARD_SENDSTRING          = KEYBOARD_MQTT_SENDSTRING;
  KEYBOARD_BACK                = KEYBOARD_MQTT_BACK;
  KEYBOARD_HOME                = KEYBOARD_MQTT_HOME;
  KEYBOARD_MENU                = KEYBOARD_MQTT_MENU;
  KEYBOARD_SCAN_PREVIOUS_TRACK = KEYBOARD_MQTT_SCAN_PREVIOUS_TRACK;
  KEYBOARD_REWIND_LONG         = KEYBOARD_MQTT_REWIND_LONG;
  KEYBOARD_REWIND              = KEYBOARD_MQTT_REWIND;
  KEYBOARD_PLAYPAUSE           = KEYBOARD_MQTT_PLAYPAUSE;
  KEYBOARD_FASTFORWARD         = KEYBOARD_MQTT_FASTFORWARD;
  KEYBOARD_FASTFORWARD_LONG    = KEYBOARD_MQTT_FASTFORWARD_LONG;
  KEYBOARD_SCAN_NEXT_TRACK     = KEYBOARD_MQTT_SCAN_NEXT_TRACK;
  KEYBOARD_MUTE                = KEYBOARD_MQTT_MUTE;
  KEYBOARD_VOLUME_INCREMENT    = KEYBOARD_MQTT_VOLUME_INCREMENT;
  KEYBOARD_VOLUME_DECREMENT    = KEYBOARD_MQTT_VOLUME_DECREMENT;
#else
  // Of course keyboard commands will not work if neither BLE nor MQTT keyboard is enabled, but at least code will compile.
  // But you have to change keys.cpp, gui_numpad.cpp and commandHandler.cpp where keyboard commands are used so that a command can be executed successfully.
  // Search for "executeCommand(Key" to find them.
  KEYBOARD_UP                  = KEYBOARD_DUMMY_UP;
  KEYBOARD_DOWN                = KEYBOARD_DUMMY_DOWN;
  KEYBOARD_RIGHT               = KEYBOARD_DUMMY_RIGHT;
  KEYBOARD_LEFT                = KEYBOARD_DUMMY_LEFT;
  KEYBOARD_SELECT              = KEYBOARD_DUMMY_SELECT;
  KEYBOARD_SENDSTRING          = KEYBOARD_DUMMY_SENDSTRING;
  KEYBOARD_BACK                = KEYBOARD_DUMMY_BACK;
  KEYBOARD_HOME                = KEYBOARD_DUMMY_HOME;
  KEYBOARD_MENU                = KEYBOARD_DUMMY_MENU;
  KEYBOARD_SCAN_PREVIOUS_TRACK = KEYBOARD_DUMMY_SCAN_PREVIOUS_TRACK;
  KEYBOARD_REWIND_LONG         = KEYBOARD_DUMMY_REWIND_LONG;
  KEYBOARD_REWIND              = KEYBOARD_DUMMY_REWIND;
  KEYBOARD_PLAYPAUSE           = KEYBOARD_DUMMY_PLAYPAUSE;
  KEYBOARD_FASTFORWARD         = KEYBOARD_DUMMY_FASTFORWARD;
  KEYBOARD_FASTFORWARD_LONG    = KEYBOARD_DUMMY_FASTFORWARD_LONG;
  KEYBOARD_SCAN_NEXT_TRACK     = KEYBOARD_DUMMY_SCAN_NEXT_TRACK;
  KEYBOARD_MUTE                = KEYBOARD_DUMMY_MUTE;
  KEYBOARD_VOLUME_INCREMENT    = KEYBOARD_DUMMY_VOLUME_INCREMENT;
  KEYBOARD_VOLUME_DECREMENT    = KEYBOARD_DUMMY_VOLUME_DECREMENT;
#endif

}

commandData makeCommandData(commandHandlers a, std::list<std::string> b) {
  commandData c = {a, b};
  return c;
}
*/

/*
std::string convertStringListToString(std::list<std::string> listOfStrings)
{
  std::string result;
  for (const auto &word : listOfStrings)
  {
    result += word + ",";
  }
  return result;
}
*/

/*
void executeCommandWithData(uint16_t command, commandData commandData, std::string additionalPayload = "") {
  switch (commandData.commandHandler) {
    case IR: {
      omote_log_v("  generic IR, payloads %s\r\n", convertStringListToString(commandData.commandPayloads).c_str());

      // we received a comma separated list of strings
      // the first string is the IR protocol, the second is the payload to be sent
      std::list<std::string>::iterator it = commandData.commandPayloads.begin();
      // get protocol and erase first element in list
      std::string protocol = *it;
      it = commandData.commandPayloads.erase(it);
      omote_log_v("  protocol %s, payload %s\r\n", protocol.c_str(), convertStringListToString(commandData.commandPayloads).c_str());

      sendIRcode((IRprotocols)std::stoi(protocol), commandData.commandPayloads, additionalPayload);
      break;
    }

    #if (ENABLE_WIFI_AND_MQTT == 1)
    case MQTT: {
      auto current = commandData.commandPayloads.begin();
      std::string topic = *current;
      std::string payload;
      if (additionalPayload == "") {
        current = std::next(current, 1);
        payload = *current;
      } else {
        payload = additionalPayload;
      }
      omote_log_d("execute: will send MQTT, topic '%s', payload '%s'\r\n", topic.c_str(), payload.c_str());
      publishMQTTMessage(topic.c_str(), payload.c_str());
      break;
    }
    #endif

    #if (ENABLE_KEYBOARD_BLE == 1)
    case BLE_KEYBOARD: {
      // the real command for the BLE keyboard is the first element in payload
      auto current = commandData.commandPayloads.begin();
      uint16_t command = std::stoi(*current);
      std::string payload = "";
      if (additionalPayload != "") {
        payload = additionalPayload;
      }
      omote_log_d("execute: will send BLE keyboard command, command '%u', payload '%s'\r\n", command, payload.c_str());
      keyboard_ble_executeCommand(command, payload);
      break;
    }
    #endif

    // case SCENE: {
    //   // let the sceneHandler do the scene stuff
    //   omote_log_d("execute: will send scene command to the sceneHandler\r\n");
    //   handleScene(command, commandData, additionalPayload);
    //   break;
    // }

    // case GUI: {
    //   // let the sceneHandler find and show the gui
    //   omote_log_d("execute: will send gui command to the sceneHandler\r\n");
    //   handleGUI(command, commandData, additionalPayload);
    //   break;
    // }

    // case SPECIAL: {
    //   if (command == MY_SPECIAL_COMMAND) {
    //     // do your special command here
    //     omote_log_d("execute: could execute a special command here, if you define one\r\n");

    //   }
    //   break;
    // }

  }
}

void executeCommand(uint16_t command, std::string additionalPayload) {
  try {
    if (commands.count(command) > 0) {
      omote_log_d("command: will execute command '%u' with additionalPayload '%s'\r\n", command, additionalPayload.c_str());
      executeCommandWithData(command, commands.at(command), additionalPayload);
    } else {
      omote_log_w("command: command '%u' not found\r\n", command);
    }
  }
  catch (const std::out_of_range& oor) {
    omote_log_e("executeCommand: internal error, command not registered\r\n");
  }
}
*/

void receiveNewIRmessage_cb(std::string message)
{
  // showNewIRmessage(message);
  Serial.println("TODO : Implement showNewIRmessage");
  Serial.println((">> GUI received Message : " + message).c_str());
}

/*
#if (ENABLE_WIFI_AND_MQTT == 1)
void receiveWiFiConnected_cb(bool connected) {
  // show status in header
  showWiFiConnected(connected);

  if (connected) {
    // Here you could add sending a MQTT message. This message could be recognized by your home automation software.
    // When receiving this message, your home automation software could send the states of the smart home devices known to OMOTE.
    // With that, OMOTE could show on startup the correct status of the smart home devices.
    //
    // Remark: in your home automation software, maybe add a short delay (e.g. 100-200 ms) between receiving this message and sending out the status of the smart home devices.
    // WiFi connection could be already available, but MQTT connection could be not completely ready. Just try what works for you.

    // executeCommand(TRIGGER_UPDATE_OF_OMOTE_SMART_HOME_DEVICES);

  }
}
void receiveMQTTmessage_cb(std::string topic, std::string payload) {
  showMQTTmessage(topic, payload);
}

#endif
*/

// ___________________________________________________
// ************* Fonctions en test *******************
// ___________________________________________________

std::map<std::string, commandData2> commands2; // Contient seulement les commandes du BootLoader (avec commandData), utiliser commands2Keys pour reduire la taille mémoire.
std::set<std::string> commands2Keys;           // Contient seulement les clés des fichiers de commandes (sans commandData), mais diminue un peu les performances.

// ----------- Getters ------------
/// @brief Récupère la map des commandes BootLoader.
/// @return Référence constante vers la map <nom de commande, données>.
const std::map<std::string, commandData2> &getCommands2()
{
  return commands2;
}

/// @brief Récupère le nombre total de commandes BootLoader.
/// @return Nombre de commandes.
const std::size_t getCommandsCount()
{
  return commands2.size();
}

/// @brief Récupère le set des clés des fichiers de commandes.
/// @return Référence constante vers le set de noms de commandes.
const std::set<std::string> &getCommands2Keys()
{
  return commands2Keys;
}

/// @brief Récupère le nombre total de clés de commandes.
/// @return Nombre de clés.
const std::size_t getCommandsCountKeys()
{
  return commands2Keys.size();
}

// --- Initialisation des commandes ---------
/// @brief Initialise les commandes du système et du BootLoader.
void init_commands()
{
  int errorCount = 0;
  // updateCommandFiles(errorCount, "/", 0);
  commands2 = deserializeCommands(commands_json_bootloader);
  commands2Keys = deserializeKeys(commands_json_bootloader);
  register_device_test();

#if (ENABLED_IO_FILESYSTEM)
  updateCommandsFromFiles(commands2, errorCount); // Update les données du bootloader
  loadCommandsKeysFromFiles(commands2Keys, errorCount, "/", 0); // enregistre les clés contenues dans les fichiers de commande.
  if (!enableMemoryReduction)
    loadCommandFiles(commands2, errorCount, "/", 0); // Charge les fichiers de commandes en mémoire.
#endif

  Serial.print(F(">> Commmands initialized with "));
  Serial.print(errorCount);
  Serial.println(F(" error(s)"));

  printSerializedCommandsKeys(commands2Keys);
  printCommandsCountKeys(commands2Keys);
}

/// @brief Affiche toutes les commandes avec leurs données sur le Serial.
void printSerializedCommands(const std::map<std::string, commandData2> &commands)
{
  Serial.println(F(""));
  Serial.println(F("all commands is listed below :"));
  for (const auto &pair : commands)
  {
    Serial.print(F("    -"));
    Serial.print(pair.first.c_str());
    commandData2 data = pair.second;
    Serial.print(F(" | "));
    Serial.print(data.requestType.c_str());
    Serial.print(F(" | "));
    Serial.println(data.commandHandler);
  }
}

/// @brief Affiche le nombre de commandes.
void printCommandsCount(const std::map<std::string, commandData2> &commands)
{
  Serial.println(F("---------------------------------"));
  Serial.print(F("nombre de commandes: "));
  Serial.println(commands.size());
  Serial.println(F("---------------------------------"));
}

/// @brief Affiche toutes les clés de commandes avec leurs données.
void printSerializedCommandsKeys(const std::set<std::string> &commandskeys)
{
  Serial.println(F(""));
  Serial.println(F("all commands Keys is listed below :"));
  for (const auto &key : commandskeys)
  {
    std::string commandName = key;
    try
    {
      std::pair<std::string, commandData2> pair = loadCommand(commandName);
      Serial.print(F("    -"));
      Serial.print(pair.first.c_str());
      commandData2 data = pair.second;
      Serial.print(F(" | "));
      Serial.print(data.requestType.c_str());
      Serial.print(F(" | "));
      Serial.println(data.commandHandler);
    }
    catch (const std::exception &e)
    {
      Serial.print(F("File not found or corrupted: "));
      Serial.println(commandName.c_str());
    }
  }
}

/// @brief Affiche le nombre de clés de commandes.
void printCommandsCountKeys(const std::set<std::string> &commandsKeys)
{
  Serial.println(F("---------------------------------"));
  Serial.print(F("nombre de commandes: "));
  Serial.println(commandsKeys.size());
  Serial.println(F("---------------------------------"));
}

// Fonction inutilisé.
void printCommands(std::map<std::string, commandData2> commands)
{
  Serial.println(F("all commands is listed below :"));
  for (const auto &pair : commands)
  {
    std::string commandName = pair.first;
    Serial.print(F("commandName: "));
    Serial.println(commandName.c_str());
    commandData2 data = pair.second;
    // commandHandler:
    Serial.print(F("commandHandler: "));
    Serial.println(data.commandHandler);
    // commandPayloads
    for (const auto &payload : data.commandPayloads)
    {
      Serial.print(F(" - payload: "));
      Serial.println(payload.c_str());
    }
  }
};

/// @brief Crée un objet commandData2.
/// @param a CommandHandler.
/// @param d Type de requête WRITE/READ.
/// @param e Attributs.
/// @param b Payloads.
/// @return Objet commandData2 initialisé.
commandData2 makeCommandData2(commandHandlers a, std::string d, std::string e, std::list<std::string> b)
{
  commandData2 c = {true, a, d, e, "", b};
  return c;
}

// --- Enregistrement des commandes -------
/// @brief Enregistre des commandes de test directement depuis le code.
void register_device_test()
{
  registerCommand("IR_SONY_POWER", makeCommandData2(IR, "WRITE", "w", {std::to_string(nsIR::SONY), "0xA90", "12", "2"}), false, true);
  registerCommand("IR_MCE_POWER", makeCommandData2(IR, "WRITE", "w", {std::to_string(nsIR::RC6), "0xC800F040CLL", "36", "0x8000", "True"}), false, true);
}

/// @brief Enregistre une commande dans le système et retourne une clé unique.
/// @param key Nom de la commande (clé).
/// @param aCommandData Données de la commande.
/// @return retourne la clé unique
std::string registerCommand(std::string key, commandData2 aCommandData, bool saveToFS, bool forceMemory)
{
  // Get new key if key already exist
  key = getFreeKey(key);
  commands2Keys.insert(key);

  bool availableFS = false;
#if (ENABLED_IO_FILESYSTEM && ENABLED_IO_FILESYSTEM_SAVING)
  availableFS = true;
  if (saveToFS)
    saveCommand(key, aCommandData);
#else
  omote_log_w("Filesystem saving disabled");
#endif
bool isSaveToFS = availableFS && saveToFS;
bool isForceMemory = forceMemory || !isSaveToFS || (isSaveToFS && !enableMemoryReduction);
  if (isForceMemory)
  {
    commands2[key] = aCommandData;
  }

  Serial.print(F(">> Command registered with key: "));
  Serial.println(key.c_str());
  printCommandsCountKeys(commands2Keys);

  return key;
}

/// @brief Désenregistre et supprime une commande existante.
/// @warning cette fonction ne supprime pas le fichier.
/// @param commandName Nom de la commande à supprimer.
void unregisterCommand(std::string commandName)
{
  commands2.erase(commandName);
  commands2Keys.erase(commandName);
  deleteCommandFile(commands2, commandName);
}

/// @brief Génère une clé unique pour une commande.
/// @param command Nom de la commande.
/// @return Nom de commande unique.
std::string getFreeKey(std::string command)
{
  int x = 0;
  int &ref = x;
  return getFreeKey(command, commands2Keys, x);
}

/// @brief Génère une clé unique pour une commande en fonction du set de clés.
/// @param command Nom de la commande.
/// @param commandsKeys Set de noms de commandes.
/// @param id Identifiant pour gérer les doublons.
/// @return Nom de commande unique.
std::string getFreeKey(std::string command, std::set<std::string> &commandsKeys, int &id)
{
  std::string newCommand = command;
  if (id > 0)
  {
    newCommand += "_" + std::to_string(id);
  }

  if (commandsKeys.find(newCommand) != commandsKeys.end())
  {
    id++;
    return getFreeKey(command, commandsKeys, id);
  }
  return newCommand;
}

// --------- Execution des commandes ------------
/// @brief Exécute une commande non enregistrée.
/// @param commandData Données de la commande.
/// @param additionalPayload Payload supplémentaire optionnel.
void executeUnregisteredCommand(commandData2 commandData, std::string additionalPayload = "")
{
  try
  {
    executeCommandWithData2("", commandData, additionalPayload);
  }
  catch (...)
  {
    omote_log_e("executeDirectCommand: unknow internal error\r\n");
  }
}

/// @brief Recherche une commande en mémoire ou via loadCommand.
/// @return true si trouvée, false sinon. Remplit commandDataOut si succès.
bool findCommandData(const std::string &commandName, commandData2 &commandDataOut)
{
  // 1️⃣ Recherche en mémoire
  auto it = commands2.find(commandName);
  if (it != commands2.end())
  {
    commandDataOut = it->second;
    commandDataOut.status1 = "OK";
    return true;
  }

  // 2️⃣ Si activé, recherche sur disque
  if (enableMemoryReduction)
  {
    try
    {
      auto pair = loadCommand(commandName);
      commandDataOut = pair.second;
      commandDataOut.status1 = "OK";
      return true;
    }
    catch (...)
    {
      commandDataOut.status1 = "ERROR_READING_FILE";
      omote_log_e("command: failed to load command '%s' from storage\r\n", commandName.c_str());
    }
  }

  return false;
}

/// @brief Exécute une commande pré-enrgistrées par son nom.
/// @param commandName Nom de la commande.
/// @param additionalPayload Payload supplémentaire optionnel.
void executeRegisteredCommand(std::string commandName, std::string additionalPayload)
{
  commandData2 commandData;

  if (!findCommandData(commandName, commandData))
  {
    omote_log_w("command: command '%s' not found\r\n", commandName.c_str());
    return;
  }

  try
  {
    omote_log_d("command: will execute command '%s' with additionalPayload '%s'\r\n",
                commandName.c_str(), additionalPayload.c_str());
    executeCommandWithData2(commandName, commandData, additionalPayload);
  }
  catch (...)
  {
    omote_log_e("executeCommand: unknown internal error during execution\r\n");
  }
}

/// @brief Exécute une commande par son nom, avec ses données et un payload supplémentaire.
/// @param commandName Nom de la commande.
/// @param commandData Données de la commande.
/// @param additionalPayload Payload supplémentaire optionnel.
void executeCommandWithData2(std::string commandName, commandData2 commandData, std::string additionalPayload = "")
{
  pGlobalStatusLED->startBlinkingForDuration(BLINK_LED_EXECUTE_INTERVAL, BLINK_LED_EXECUTE_DURATION);
  switch (commandData.commandHandler)
  {
  case IR:
  {
    if (commandName == "IR_SONY_POWER")
    {
      sendSonyR2(commandData.commandPayloads);
      break;
    }
    if (commandName == "IR_MCE_POWER")
    {
      sendMceT(commandData.commandPayloads);
      break;
    }

    sendIRcode2(commandData.commandPayloads, additionalPayload);
    break;
  }

#if (ENABLE_WIFI_AND_MQTT == 1)
  case MQTT:
  {
    auto current = commandData.commandPayloads.begin();
    std::string topic = *current;
    std::string payload;
    if (additionalPayload == "")
    {
      current = std::next(current, 1);
      payload = *current;
    }
    else
    {
      payload = additionalPayload;
    }
    omote_log_d("execute: will send MQTT, topic '%s', payload '%s'\r\n", topic.c_str(), payload.c_str());
    publishMQTTMessage(topic.c_str(), payload.c_str());
    break;
  }
#endif

#if (ENABLE_KEYBOARD_BLE == 1)
  case BLE_KEYBOARD:
  {
    // the real command for the BLE keyboard is the first element in payload
    auto current = commandData.commandPayloads.begin();
    uint16_t command = std::stoi(*current);
    std::string payload = "";
    if (additionalPayload != "")
    {
      payload = additionalPayload;
    }
    omote_log_d("execute: will send BLE keyboard command, command '%u', payload '%s'\r\n", command, payload.c_str());
    keyboard_ble_executeCommand(command, payload);
    break;
  }
#endif

    // case SCENE: {
    //   // let the sceneHandler do the scene stuff
    //   omote_log_d("execute: will send scene command to the sceneHandler\r\n");
    //   handleScene(command, commandData, additionalPayload);
    //   break;
    // }

    // case GUI: {
    //   // let the sceneHandler find and show the gui
    //   omote_log_d("execute: will send gui command to the sceneHandler\r\n");
    //   handleGUI(command, commandData, additionalPayload);
    //   break;
    // }

  case SPECIAL:
  {
    // if (command == MY_SPECIAL_COMMAND) {
    //   // do your special command here
    //   omote_log_d("execute: could execute a special command here, if you define one\r\n");

    // }
    if (commandName == "LIST_COMMANDS")
    {

      /*
      // Simuler un traiment long et vérifier si l'update de LedBlinker s'effectue.
      unsigned long lastTime = millis();
      for (int i=0; i<500; i++) {
        Serial.println(i);
        delay(10);
        vTaskDelay(10 / portTICK_PERIOD_MS); // Délai de 10 ms en mode multitâche
        yield();
        while (millis() - lastTime < 10) {
            yield(); // Continuer à permettre au système de fonctionner
        }
        lastTime = millis();
      }*/

      printSerializedCommandsKeys(commands2Keys);
      printCommandsCountKeys(commands2Keys);
    }
    else if (commandName == "TEST_NOTIFY")
    {
      sendBleNotify("0123456789ABCDEFGHIJ"); // Ok limited to 20 charactères.
    }
    else if (commandName == "LISTDIR_COMMANDS")
    {
#if (ENABLED_IO_FILESYSTEM)
      // commandData2 command = commands2["IR_MCE_POWER"];
      // String path = combinePath("","test1.txt");
      // saveCommand("test", command, path, SAVE_COMMAND_JSONPRETTY);

      try
      {
        // saveCommand2("BLEKB_5", commands2["IR_MCE_POWER"], combinePath("","BLEKB_5.cde"));
        // String jsonString = serialize("BLEKB_5",  commands2["BLEKB_5"], SAVE_COMMAND_JSONPRETTY);
        // saveCommand(jsonString, combinePath("","BLEKB_5.cde"));

        // deleteCommandFile2("BLEKB_5");
        // deleteCommandFile2(combinePath("","test1.cde"));
        // deleteCommand3(commands2, "BLEKB_5");
        fsMount2();
        // deleteFile2(combinePath("","BLEKB_5.cde"));
        // deleteFile2(combinePath("","DELETE_COMMANDS.cde"));
        // deleteFile2(combinePath("","FREE_HEAP.cde"));
        // deleteFile2(combinePath("","FS_TOTAL_BYTES.cde"));
        // deleteFile2(combinePath("","FS_USED_BYTES.cde"));
        // deleteFile2(combinePath("","GUI_3.cde"));
        // deleteFile2(combinePath("","HIGH_WATER_MARK.cde"));
        // deleteFile2(combinePath("","IR_2_0xC800F040C.cde"));
        // deleteFile2(combinePath("","IR_3_0xFFA25D.cde"));
        // deleteFile2(combinePath("","IR_3_0xFFA25D_1.cde"));
        // deleteFile2(combinePath("","IR_3_0xFFA25D_2.cde"));
        // deleteFile2(combinePath("","IR_CAPTURE.cde"));
        // deleteFile2(combinePath("","IR_MCE_POWER.cde"));
        // deleteFile2(combinePath("","IR_SONY_POWER.cde"));
        // deleteFile2(combinePath("","LISTDIR_COMMANDS.cde"));
        // deleteFile2(combinePath("","LIST_COMMANDS.cde"));
        // deleteFile2(combinePath("","LIST_COMMANDS_R.cde"));
        // deleteFile2(combinePath("","MQTT_4.cde"));
        // deleteFile2(combinePath("","SAVE_COMMANDS.cde"));
        // deleteFile2(combinePath("","SCENE_2.cde"));
        // deleteFile2(combinePath("","SPECIALE_1.cde"));
        // deleteFile2(combinePath("","TEST_NOTIFY.cde"));
        // deleteFile2(combinePath("","TICK_RATE.cde"));
        fsUnMount2();
      }
      catch (const std::exception &e)
      {
        // Nothing to do ???
      }

      // TODO: Exécuter les traiments longs dans un thread secondaire pour eviter le blocage de l'update de LedBlinker.
      // listDir2("/", 0);
      printFilesInfo("/", 0);
      fsMount2();
      unsigned int usedBytes = getFsUsedBytes();
      unsigned int totalBytes = getFsTotalBytes();
      fsUnMount2();

      Serial.printf("FileSystem : '%u' used Bytes / '%u' total Bytes\r\n", usedBytes, totalBytes);
#endif
    }
    else if (commandName == "DELETE_COMMANDS")
    {
      int errorCount = 0;
#if (ENABLED_IO_FILESYSTEM)
      deleteAllFiles(errorCount, "/", 0);
      Serial.print(F(">> All files deleted with "));
      Serial.print(errorCount);
      Serial.println(F(" error(s)"));
      init_commands();
#endif
    }
    else if (commandName == "IR_CAPTURE")
    {
      captureCommand();
    }

    break;
  }
  }
}