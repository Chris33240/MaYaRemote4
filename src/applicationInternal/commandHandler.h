#pragma once

#include <string>
#include <list>
#include <map>
#include <set>
// #include "devices/keyboard/device_keyboard_mqtt/device_keyboard_mqtt.h"
// #include "devices/keyboard/device_keyboard_ble/device_keyboard_ble.h"
#include "packets_handler.h"
#include <FS.h>

extern uint16_t COMMAND_UNKNOWN;

/*
  Depending on which keyboard is enabled (BLE or MQTT), we define KEYBOARD_UP, KEYBOARD_DOWN and so on.
  These defines are used in keys.cpp, gui*.cpp and commandHandler.cpp
  Example:
  If BLE  is enabled, then KEYBOARD_UP will be the same as KEYBOARD_BLE_UP
  If MQTT is enabled, then KEYBOARD_UP will be the same as KEYBOARD_MQTT_UP
  If none of them is enabled, then KEYBOARD_UP will be the same as KEYBOARD_UP_DUMMY
  Doing so you can switch between the keyboards without changing the UI code (keys.cpp, gui*.cpp and commandHandler.cpp)
  If you need something different than this behaviour, then you can change the code in 'register_keyboardCommands()'
  or you can of course change keys.cpp, gui*.cpp and commandHandler.cpp so that they directly use KEYBOARD_BLE_UP or KEYBOARD_MQTT_UP etc.
*/

extern uint16_t KEYBOARD_DUMMY_UP;
extern uint16_t KEYBOARD_DUMMY_DOWN;
extern uint16_t KEYBOARD_DUMMY_RIGHT;
extern uint16_t KEYBOARD_DUMMY_LEFT;
extern uint16_t KEYBOARD_DUMMY_SELECT;
extern uint16_t KEYBOARD_DUMMY_SENDSTRING;
extern uint16_t KEYBOARD_DUMMY_BACK;
extern uint16_t KEYBOARD_DUMMY_HOME;
extern uint16_t KEYBOARD_DUMMY_MENU;
extern uint16_t KEYBOARD_DUMMY_SCAN_PREVIOUS_TRACK;
extern uint16_t KEYBOARD_DUMMY_REWIND_LONG;
extern uint16_t KEYBOARD_DUMMY_REWIND;
extern uint16_t KEYBOARD_DUMMY_PLAYPAUSE;
extern uint16_t KEYBOARD_DUMMY_FASTFORWARD;
extern uint16_t KEYBOARD_DUMMY_FASTFORWARD_LONG;
extern uint16_t KEYBOARD_DUMMY_SCAN_NEXT_TRACK;
extern uint16_t KEYBOARD_DUMMY_MUTE;
extern uint16_t KEYBOARD_DUMMY_VOLUME_INCREMENT;
extern uint16_t KEYBOARD_DUMMY_VOLUME_DECREMENT;

#if (ENABLE_KEYBOARD_BLE == 1)
#define KEYBOARD_PREFIX KEYBOARD_BLE_
#elif (ENABLE_KEYBOARD_MQTT == 1)
#define KEYBOARD_PREFIX KEYBOARD_MQTT_
#else
// Of course keyboard commands will not work if neither BLE nor MQTT keyboard is enabled, but at least code will compile.
// But you have to change keys.cpp, gui_numpad.cpp and commandHandler.cpp where keyboard commands are used so that a command can be executed successfully.
// Search for "executeCommand(Key" to find them.
#define KEYBOARD_PREFIX KEYBOARD_DUMMY_
#endif

extern uint16_t KEYBOARD_UP;
extern uint16_t KEYBOARD_DOWN;
extern uint16_t KEYBOARD_RIGHT;
extern uint16_t KEYBOARD_LEFT;
extern uint16_t KEYBOARD_SELECT;
extern uint16_t KEYBOARD_SENDSTRING;
extern uint16_t KEYBOARD_BACK;
extern uint16_t KEYBOARD_HOME;
extern uint16_t KEYBOARD_MENU;
extern uint16_t KEYBOARD_SCAN_PREVIOUS_TRACK;
extern uint16_t KEYBOARD_REWIND_LONG;
extern uint16_t KEYBOARD_REWIND;
extern uint16_t KEYBOARD_PLAYPAUSE;
extern uint16_t KEYBOARD_FASTFORWARD;
extern uint16_t KEYBOARD_FASTFORWARD_LONG;
extern uint16_t KEYBOARD_SCAN_NEXT_TRACK;
extern uint16_t KEYBOARD_MUTE;
extern uint16_t KEYBOARD_VOLUME_INCREMENT;
extern uint16_t KEYBOARD_VOLUME_DECREMENT;

// Not needed anymore, but maybe again in future
// /*
//  * Concatenate preprocessor tokens A and B without expanding macro definitions
//  * (however, if invoked from a macro, macro arguments are expanded).
//  */
// #define PPCAT_NX(A, B) A ## B
// /*
//  * Concatenate preprocessor tokens A and B after macro-expanding them.
//  */
// #define PPCAT(A, B) PPCAT_NX(A, B)
//
// Test
// https://stackoverflow.com/questions/5256313/c-c-macro-string-concatenation
// #define STR(x) #x
// #define XSTR(x) STR(x)
// #pragma message "1 The value is: " XSTR(KEYBOARD_BLE_UP)
// #pragma message "2 The value is: " XSTR(KEYBOARD_MQTT_UP)
// #pragma message "3 The value is: " XSTR(KEYBOARD_UP)

enum commandHandlers
{
    SPECIAL = 0,
    SCENE = 1,
    GUI = 2,
    IR = 3,
#if (ENABLE_WIFI_AND_MQTT == 1)
    MQTT = 4,
#endif
#if (ENABLE_KEYBOARD_BLE == 1)
    BLE_KEYBOARD = 5,
#endif
};

struct commandData
{
    commandHandlers commandHandler;
    std::list<std::string> commandPayloads;
};

// we don't yet have a command id
void register_command(uint16_t *command, commandData aCommandData);
// we already have a command id. Only used by BLE keyboard
void register_command_withID(uint16_t command, commandData aCommandData);
// only get a unique ID. used by KEYBOARD_DUMMY, COMMAND_UNKNOWN and BLE keyboard
void get_uniqueCommandID(uint16_t *command);

void register_keyboardCommands();
commandData makeCommandData(commandHandlers a, std::list<std::string> b);
void executeCommand(uint16_t command, std::string additionalPayload = "");

void receiveNewIRmessage_cb(std::string message);
#if (ENABLE_WIFI_AND_MQTT == 1)
// used as callbacks from hardware
void receiveWiFiConnected_cb(bool connected);
void receiveMQTTmessage_cb(std::string topic, std::string payload);
#endif

// ************* Fonctions en test *******************
/*
namespace nsCH {
enum payloadTypes
{
  SPECIAL = 0,
  SCENE = 1,
  GUI = 2,
  IR = 3,
#if (ENABLE_WIFI_AND_MQTT == 1)
  MQTT = 4,
#endif
#if (ENABLE_KEYBOARD_BLE == 1)
  BLE_KEYBOARD = 5,
#endif
};
}
*/

struct commandData2
{
    bool hasData;
    commandHandlers commandHandler;
    std::string requestType;
    std::string attributs;
    std::string status1;
    std::list<std::string> commandPayloads;
};

// Getters
[[deprecated("Don't use this routine any more. Use the new one instead. 'getCommands2Keys()'")]]
const std::map<std::string, commandData2> &getCommands2();
[[deprecated("Don't use this routine any more. Use the new one instead. 'getCommandsCountKeys()'")]]
const std::size_t getCommandsCount();
const uint16_t getCommandsFilesCount();
const std::set<std::string> &getCommands2Keys();
const std::size_t getCommandsCountKeys();

// Init commands
void init_commands();

//[[deprecated("Don't use this routine any more. Use the new one instead. 'printMetaDatasCommandsKeys()'")]]
// void printSerializedCommands(const std::map<std::string, commandData2> &commands);
//[[deprecated("Don't use this routine any more. Use the new one instead. 'printSerializedCommandsCountKeys()'")]]
// void printCommandsCount(const std::map<std::string, commandData2> &commands);
void printMetaDatasCommandsKeys(const std::set<std::string> &commandskeys);
void printCommandsCount();
// void printCommandsFilesCount();
// void printCommandsCountKeys(const std::set<std::string> &commandsKeys);

//[[deprecated("Don't use this routine any more. Use the new one instead. 'printSerializedCommands()'")]]
// void printCommands(std::map<std::string, commandData2> commands);

// Register commands
commandData2 makeCommandData2(commandHandlers a, std::string d, std::string e, std::list<std::string> b);
void register_device_test();
[[deprecated("Don't use this routine any more. Use the new one instead. 'registerCommand2()'")]]
std::string registerCommand(std::string command, commandData2 aCommandData, bool saveToFS = false, bool forceMemory = false);
std::string registerCommand2(std::string key, commandData2 aCommandData, bool overwriteIfExists, bool saveToFS, bool forceMemory);
void unregisterCommand(std::string commandName);
std::string getFreeKey(std::string command);
std::string getFreeKey(std::string command, std::set<std::string> &commandsKeys, int &id);

// Execute commands
void executeUnregisteredCommand(commandData2 commandData, std::string additionalPayload);
bool findCommandData(const std::string &commandName, commandData2 &commandDataOut);
bool findCommandDataFiles(const std::string &commandName, commandData2 &commandDataOut);
void executeRegisteredCommand(std::string commandName, std::string additionalPayload = "");
void executeCommandWithData2(std::string commandName, commandData2 commandData, std::string additionalPayload);