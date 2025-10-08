#include <ArduinoJson.h>
#include "commandHandler.h"

std::map<std::string, commandData2> deserializeBootLoader(String input);
std::map<std::string, commandData2> from_json_BootLoader(const JsonDocument &doc);

String serializeCommand(const std::string &commandName, const commandData2 &commandData, bool JsonPretty);
void to_json_Command(JsonDocument &doc, const std::string &commandName, const commandData2 &commandData);

std::pair<std::string, commandData2> deserializeCommand(String input);
std::pair<std::string, commandData2> from_json_Command(const JsonDocument &doc);

std::set<std::string> deserializeKeys(String input);
std::set<std::string> from_json_Keys(const JsonDocument &doc);
