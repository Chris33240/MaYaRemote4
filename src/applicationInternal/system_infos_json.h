#include <Arduino.h>
#include <ArduinoJson.h>
#include "system_infos_fill.h"

String serializeSystemInfos(const systemInfos &systemInfos, bool JsonPretty);
void to_json(JsonObject &root, const systemInfos &systemInfos);