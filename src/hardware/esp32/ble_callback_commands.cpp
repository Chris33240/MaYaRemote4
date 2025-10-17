/// @file ble_callback_commands.cpp
/// @brief Callbacks pour les caractéristiques Commands du serveur BLE ESP32.
#include "ble_callback_commands.h"
#include "globals.h"
#include "system_info.h"
#include "helpers/helpers.h"
#include "ble_uuid.h"
#include "ble_server_hal_esp32.h"
#include "infrared_receiver_hal_esp32.h"
#include "applicationInternal/tasksManager.h"
#include "applicationInternal/list_commands_handler.h"
#include <applicationInternal/list_commandsData_handler.h>
//#include <applicationInternal/micro_miniz.h>
//#include <applicationInternal/gZip.h>

/// Gestionnaire de listes de commandes pour le serveur BLE
ListCommandsHandler listCommandsHandler;
/// Gestionnaire de listes des données de commandes pour le serveur BLE
ListCommandsDataHandler listCommandsDataHandler;
/// Gestionnaire de paquets pour le serveur BLE
PacketsHandler packetsCommand;

// ------------------- Callbacks Commands ---------------------------
/*
TODO : méthodologie d'envoi de plusieurs paquets de données via la méthode onRead() :
1- Le client envoie une requette Read sur la caractéristique LIST_COMMANDS
2- Le serveur envoi un premier paquet 'Header' qui contient le nombre de paquets à transmettre.
3- Le client connaissant le nombres de paquets, envoie une requette Read pour chaque paquet à recevoir.
4- Le client attends que tout les paquets soit recus (timeout) et reconstitue le message originale.
*/
/// @brief Callback appelé lorsqu'un client effectue une lecture sur une caractéristique Commands
/// @param pCharacteristic Pointeur vers la caractéristique lue
void MyCallbacks::onRead(BLECharacteristic *pCharacteristic)
{
  // Obtenez l'UUID de la caractéristique
  const std::string &uuid = pCharacteristic->getUUID().toString();
  Serial.print(F("[BLE-onRead] BLE Characteristic uuid: "));
  Serial.println(uuid.c_str());

  /// Lecture du nombre de commandes
  if (uuid == CHARACTERISTIC_COMMANDS_COUNT_UUID)
  {
    // std::size_t count = getCommandsCount();
    std::size_t count = getCommandsCountKeys();
    Serial.print(F("[BLE-OnRead] CommandsCount: "));
    Serial.println(count);
    pCharacteristic->setValue(count);

    /// Lecture de la liste des commandes
  }
  else if (uuid == CHARACTERISTIC_LIST_COMMANDS_UUID)
  {
    // const std::map<std::string, commandData2>& commands = getCommands2();
    /*
    Structure du message en réponse à une requette READ pour la commande LIST_COMMANDS_R :
    HEADER:4
    {"command": "LIST_COMMANDS", "requestType": "READ", "commandHandler": "0"},
    HEADER:4
    {"command": "IR_CAPTURE", "requestType": "WRITE", "commandHandler": "3"},
    HEADER:4
    {"command": "IR_MCE_POWER", "requestType": "WRITE", "commandHandler": "3"},
    HEADER:4
    {"command": "IR_SONY_POWER", "requestType": "WRITE", "commandHandler": "3"}
    HEADER:5
    {"command": "IR_RC6_0x0800040F", "requestType": "WRITE", "commandHandler": "3"}
    */

    std::string str = listCommandsHandler.readCommandKeys();

    Serial.print(F("[BLE-onRead] listCommands packet: "));
    Serial.println(str.c_str());
    pCharacteristic->setValue(str);
  }
    else if (uuid == CHARACTERISTIC_LIST_COMMANDS_DATA_UUID)
  {
    std::string str = listCommandsDataHandler.readCommandsDataKeys();

    Serial.print(F("[BLE-onRead] listCommandsData packet: "));
    Serial.println(str.c_str());
    pCharacteristic->setValue(str);
  }
  else if (uuid == CHARACTERISTIC_LAST_CAPTURE_UUID)
  {
    PacketsHandler &packetsHandler = getBLELastCapturePacketsHandler_HAL();
    //   packetsHandler.setOnTimeoutCallback([]() {
    //   Serial.println("[Timeout] expired BLE Read Packet (LastCapture)");
    //   sendBleNotifyCode_HAL("104");
    // });
    std::string str = packetsHandler.getPacket();
    Serial.print(F("[BLE-onRead] lastCommand paquet: "));
    Serial.println(str.c_str());
    pCharacteristic->setValue(str);
  }
}

/*
Please note that heavy data preparation processing is carried out outside of the callback
in a call stack different from that of BLE so as not to overload the stack memory.
And the sending of data must be done inside the callback which means that the data
must already be ready before the callback is called.
*/

/// @brief Callback appelé lorsqu'un client écrit sur une caractéristique Commands
/// @param pCharacteristic Pointeur vers la caractéristique écrite
void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
  std::string uuid = pCharacteristic->getUUID().toString();
  Serial.print(F("[BLE-onWrite] BLE Characteristic uuid: "));
  Serial.println(uuid.c_str());

  /// Réception d'une commande via CHARACTERISTIC_COMMAND2_UUID
  if (uuid == CHARACTERISTIC_COMMAND2_UUID)
  {
    std::string message = pCharacteristic->getValue();
    if (message.length() > 0)
    {
      Serial.print(F("[BLE-onWrite] Received Command: "));
      Serial.println(message.c_str());
      // PacketsHandler& packetsCommand = getPacketsCommand();

      // std::string jsonTask = R"({"taskType":"EXECUTE","commandName":"IR_4_0xA90_1","directData":{"protocol":"3","data":"238"},"addPayload":{"frequency":"36","toggleMask":"0x0","repeat":"2"}})";
      // TasksManager2::addTask(jsonTask);

      packetsCommand.setOnTimeoutCallback([]()
                                          {
                                            // Serial.println("[Timeout] expired BLE Write Packet (Command)");
                                            sendBleNotifyCode_HAL("105");
                                          });
      packetsCommand.setOnMessageCompleteCallback([](const std::string &jsonTask)
                                                  {
                                                    // printSystemInfo();
                                                    // Serial.printf("Completed received command: '%s'\r\n", message.c_str());
                                                    // Serial.println("DEBUG1");
                                                    //  Be carfull !!! Notify call inside this callback cause stack overflow !!!
                                                    //////sendBleNotifyCode_HAL("106");
                                                    // Message complet :
                                                    // std::string jsonTask = R"({"taskType":"EXECUTE","commandName":"IR_4_0xA90_1","directData":{"protocol":"3","data":"238"},"addPayload":{"frequency":"36","toggleMask":"0x0","repeat":"2"}})";
                                                    TasksManager2::addTask(jsonTask);
                                                  });
      // Received paquet : Packet header= "HEADER:7", packet1="..." , packet2="..." ...etc
      packetsCommand.setPacket(message.c_str());
      sendBleNotifyCode_HAL("107");
    }

    // ************* DEPRECATED ************
    /*
  } else if (uuid == CHARACTERISTIC_COMMAND_UUID) {
    std::string message = pCharacteristic->getValue();
    if (message.length() > 0) {
      Serial.print(F("[BLE-onWrite] Received Command: "));
      Serial.println(message.c_str());
      //PacketsHandler& packetsCommand = getPacketsCommand();

      //std::string jsonTask = R"({"taskType": "EXECUTE", "taskPayload": {"commandName": "IR_MCE_POWER", "frequency": "36", "toggleMask": "0x8000", "repeat": ""}})";
      //std::string jsonTask = R"({"taskType": "EXECUTE", "taskPayload": {"commandName": "IR_2_0xC800F040C", "frequency": "36", "toggleMask": "0x8000", "repeat": ""}})";
      //std::string jsonTask = R"({"taskType": "EXECUTE", "taskPayload": {"commandName": "IR_4_0xA90", "frequency": "", "toggleMask": "", "repeat": "2"}})";
      //TasksManager::addTask(jsonTask);

      packetsCommand.setOnTimeoutCallback([]() {
        //Serial.println("[Timeout] expired BLE Write Packet (Command)");
        sendBleNotifyCode_HAL("105");
      });
      packetsCommand.setOnMessageCompleteCallback([](const std::string& jsonTask) {
        //Serial.printf("Completed received command: '%s'\r\n", message.c_str());
        // Be carfull !!! Notify call inside this callback cause stack overflow !!!
        //////sendBleNotifyCode_HAL("106");
        // Message complet :
        //std::string jsonTask = R"({"taskType": "EXECUTE", "taskPayload": {"commandName": "IR_MCE_POWER", "frequency": "36", "toggleMask": "0x8000", "repeat": ""}})";
        TasksManager::addTask(jsonTask);
      });
      // Received paquet : Packet header= "HEADER:7", packet1="..." , packet2="..." ...etc
      packetsCommand.setPacket(message.c_str());
       sendBleNotifyCode_HAL("107");
    }
    */

    /// Exécution d'une commande via CHARACTERISTIC_EXECUTE_COMMAND_UUID
  }
  else if (uuid == CHARACTERISTIC_EXECUTE_COMMAND_UUID)
  {
    std::string message = pCharacteristic->getValue();
    if (message.length() > 0)
    {
      Serial.print(F("[BLE-onWrite] Received Execute Command: "));
      Serial.println(message.c_str());

      // std::string jsontask = R"({"taskType": "EXECUTE", "taskPayload": {"commandName": ")" + message + R"("}})";
      std::string jsonTask = R"({"taskType":"EXECUTE","commandName": ")" + message + R"("}})";
      TasksManager2::addTask(jsonTask);
    }

    /// Suppression d'une commande via CHARACTERISTIC_DELETE_COMMAND_UUID
  }
  else if (uuid == CHARACTERISTIC_DELETE_COMMAND_UUID)
  {
    std::string message = pCharacteristic->getValue();
    // Serial.printf("[DEBUG] Command: '%s' Length: '%u'\r\n", RxMessage.c_str(), RxMessage.length());
    if (message.length() > 0)
    {
      Serial.print(F("[BLE-onWrite] Received Delete Command: "));
      Serial.println(message.c_str());

      std::string jsonTask = R"({"taskType":"DELETE","commandName": ")" + message + R"("}})";
      TasksManager2::addTask(jsonTask);
    }
  }
}
