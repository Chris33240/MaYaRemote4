#include <Arduino.h>
#include <ArduinoJson.h>
#include "commandHandler.h"

std::string serialize2(const std::string &commandName, const commandData2 &commandData, const std::string &status);
void to_json2(JsonDocument &doc, const std::string &commandName, const commandData2 &commandData, const std::string &status);