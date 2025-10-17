#include <ArduinoJson.h>
#include "commandHandler.h"

std::map<std::string, commandData2> deserializeCommands(String input);
std::map<std::string, commandData2> from_json_BootLoader(const JsonDocument &doc);

//String serializeCommand(const std::string &commandName, const commandData2 &commandData, bool JsonPretty, bool withStatus = false);
//void to_json_Command(JsonDocument &doc, const std::string &commandName, const commandData2 &commandData, bool withStatus = false);

String serializeCommandWithStatus(const std::string &commandName, const commandData2 &commandData, bool JsonPretty = false);
String serializeCommandWithPayloads(const std::string &commandName, const commandData2 &commandData, bool JsonPretty = false);
String serializeCommandWithStatusAndPayloads(const std::string &commandName, const commandData2 &commandData, bool JsonPretty = false);
void to_json_CommandBase(JsonObject &commandObj, const commandData2 &commandData);
void to_json_CommandWithStatus(JsonObject &commandObj, const commandData2 &commandData);
void to_json_CommandWithPayloads(JsonObject &commandObj, const commandData2 &commandData);

std::pair<std::string, commandData2> deserializeCommand(String input);
std::pair<std::string, commandData2> from_json_Command(const JsonDocument &doc);

std::set<std::string> deserializeKeys(String input);
std::set<std::string> from_json_Keys(const JsonDocument &doc);
