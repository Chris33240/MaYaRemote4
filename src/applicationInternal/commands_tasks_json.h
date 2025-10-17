#include <ArduinoJson.h>
#include "commandHandler.h"
#include "tasksManager.h"

Task2 deserializeTask2(const char *json);
Payload deserializeAdditionnalPayload2(const char *json);

String serializeAdditionnalPayload2(const Payload &payload, bool JsonPretty);
void to_json(JsonDocument &doc, const Payload &payload);