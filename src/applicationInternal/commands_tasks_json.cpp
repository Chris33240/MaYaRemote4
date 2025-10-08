/// @file commands_tasks_json.cpp
/// @brief Sérialisation et désérialisation des tâches (Task2) et des payloads additionnels.
#include "commands_tasks_json.h"

// ------------- Sérializer/Déserializer une tache json ---------------

/*
Exemple:
{
"taskType":"EXECUTE",
"commandName":"IR_4_0xA90_1",
"directData":{
    "protocol":"3",
    "data":"238"
    },
"payload":{
    "frequency":"36",
    "toggleMask":"0x8000",
    "repeat":"2"
    }
}
*/

/// @brief Désérialise un JSON en une structure Task2.
/// @param json Chaîne JSON représentant la tâche.
/// @return Structure Task2 remplie avec les données du JSON.
Task2 deserializeTask2(const char *json)
{
  JsonDocument doc;
  // Créez un document JSON
  // StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, json);

  Task2 task;

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return task;
  }

  /*
    // Créez un objet Task à partir du JSON désérialisé
    task.taskType = doc["taskType"].as<std::string>();
    //task.taskPayload.commandName = doc["taskPayload"]["commandName"].as<std::string>();
    task.commandName = doc["commandName"].as<std::string>();
    task.directData.protocolID = doc["directData"]["protocol"].as<std::string>();
    task.directData.dataCode = doc["directData"]["data"].as<std::string>();
    task.directData.bits = doc["directData"]["bits"].as<std::string>();
    task.payload.frequency = doc["payload"]["frequency"].as<std::string>();
    task.payload.toggleMask = doc["payload"]["toggleMask"].as<std::string>();
    task.payload.repeat = doc["payload"]["repeat"].as<std::string>();
  */

  task.taskType = doc["taskType"].isNull() ? "" : doc["taskType"].as<std::string>();
  task.commandName = doc["commandName"].isNull() ? "" : doc["commandName"].as<std::string>();
  task.directData.protocolID = doc["directData"]["protocol"].isNull() ? "" : doc["directData"]["protocol"].as<std::string>();
  task.directData.dataCode = doc["directData"]["data"].isNull() ? "" : doc["directData"]["data"].as<std::string>();
  task.directData.bits = doc["directData"]["bits"].isNull() ? "" : doc["directData"]["bits"].as<std::string>();
  task.payload.frequency = doc["payload"]["frequency"].isNull() ? "" : doc["payload"]["frequency"].as<std::string>();
  task.payload.toggleMask = doc["payload"]["toggleMask"].isNull() ? "" : doc["payload"]["toggleMask"].as<std::string>();
  task.payload.repeat = doc["payload"]["repeat"].isNull() ? "" : doc["payload"]["repeat"].as<std::string>();
  task.payload.raw = doc["payload"]["raw"].isNull() ? "" : doc["payload"]["raw"].as<std::string>();

  return task;
}

// ------------- Sérializer/Déserializer additionnalPayload ---------------

/*
Exemple:
{
"payload":{
    "frequency":"36",
    "toggleMask":"0x8000",
    "repeat":"2"
    }
}
*/

/// @brief Désérialise un JSON contenant uniquement les données de payload additionnel.
/// @param json Chaîne JSON représentant le payload.
/// @return Structure Payload remplie avec les données du JSON.
Payload deserializeAdditionnalPayload2(const char *json)
{
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, json);

  Payload payload;

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return payload;
  }

  payload.frequency = doc["payload"]["frequency"].as<std::string>();
  payload.toggleMask = doc["payload"]["toggleMask"].as<std::string>();
  payload.repeat = doc["payload"]["repeat"].as<std::string>();
  payload.raw = doc["payload"]["raw"].as<std::string>();

  return payload;
}

/// @brief Sérialise un payload additionnel en JSON.
/// @param payload Payload à sérialiser.
/// @param JsonPretty True pour un JSON indenté, false pour compact.
/// @return Chaîne JSON représentant le payload.
String serializeAdditionnalPayload2(const Payload &payload, bool JsonPretty)
{
  String output;
  JsonDocument doc;

  to_json(doc, payload);

  if (JsonPretty)
  {
    serializeJsonPretty(doc, output);
  }
  else
  {
    serializeJson(doc, output);
  }
  return output;
}

/// @brief Remplit un JsonDocument à partir d'un Payload.
/// @param doc Document JSON à remplir.
/// @param payload Payload contenant les informations à sérialiser.
void to_json(JsonDocument &doc, const Payload &payload)
{
  auto root = doc.to<JsonObject>();
  auto payloads = root["payload"].to<JsonObject>();
  payloads["frequency"] = payload.frequency;
  payloads["toggleMask"] = payload.toggleMask;
  payloads["repeat"] = payload.repeat;
  payloads["raw"] = payload.raw;
}