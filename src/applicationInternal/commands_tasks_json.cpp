/// @file commands_tasks_json.cpp
/// @brief Sérialisation et désérialisation des tâches (Task2) et des payloads additionnels.
#include "commands_tasks_json.h"

// ------------- Sérializer/Déserializer une tache json ---------------

/*
Exemple:
{
"taskType": "EXECUTE",
"commandName": "IR_4_0xA90_1",
"directData": {
    "protocol": "3",
    "data": "238"
    },
"addPayload": {
    "frequency": "36",
    "toggleMask": "0x8000",
    "repeat": "2"
    "raw": "0"
    }
}
*/

/// @brief Désérialise un JSON en une structure Task2.
/// @param json Chaîne JSON représentant la tâche.
/// @return Structure Task2 remplie avec les données du JSON.
Task2 deserializeTask2(const char *json)
{
    JsonDocument doc;
    // StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, json);

    Task2 task;

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return task;
    }

    task.taskType = doc["taskType"].isNull() ? "" : doc["taskType"].as<std::string>();
    task.commandName = doc["commandName"].isNull() ? "" : doc["commandName"].as<std::string>();

    // ✅ DirectData activé seulement si le bloc existe
    if (!doc["directData"].isNull())
    {
        task.directData.hasData = true;
        task.directData.protocolID = doc["directData"]["protocol"].isNull() ? "" : doc["directData"]["protocol"].as<std::string>();
        task.directData.dataCode = doc["directData"]["data"].isNull() ? "" : doc["directData"]["data"].as<std::string>();
        task.directData.bits = doc["directData"]["bits"].isNull() ? "" : doc["directData"]["bits"].as<std::string>();
    }
    else
    {
        task.directData.hasData = false;
    }

    if (!doc["addPayload"].isNull())
    {
        task.additionalPayload.hasData = true;
        task.additionalPayload.frequency = doc["addPayload"]["frequency"].isNull() ? "" : doc["addPayload"]["frequency"].as<std::string>();
        task.additionalPayload.toggleMask = doc["addPayload"]["toggleMask"].isNull() ? "" : doc["addPayload"]["toggleMask"].as<std::string>();
        task.additionalPayload.repeat = doc["addPayload"]["repeat"].isNull() ? "" : doc["addPayload"]["repeat"].as<std::string>();
        task.additionalPayload.raw = doc["addPayload"]["raw"].isNull() ? "" : doc["addPayload"]["raw"].as<std::string>();
    }
    else
    {
        task.additionalPayload.hasData = false;
    }

    if (!doc["commandData"].isNull())
    {
        task.commandData.hasData = true;

        // On suppose qu’il n’y a qu’une seule clé dans commandData (par ex. "IR_4_0xA90_1")
        JsonObject commandDataObj = doc["commandData"].as<JsonObject>();

        if (!commandDataObj.isNull())
        {
            // On suppose qu’il n’y a qu’une seule clé dans commandData (par ex. "IR_4_0xA90_1")
            JsonObject inner = commandDataObj.begin()->value().as<JsonObject>();

            // task.commandData.commandHandler = static_cast<commandHandlers>(doc["commandData"]["commandHandler"].as<int>());
            task.commandData.commandHandler = inner["commandHandler"].isNull() ? static_cast<commandHandlers>(0) : static_cast<commandHandlers>(inner["commandHandler"].as<int>());
            task.commandData.requestType = inner["requestType"].isNull() ? "" : inner["requestType"].as<std::string>();
            task.commandData.attributs = inner["attributs"].isNull() ? "" : inner["attributs"].as<std::string>();
            // task.commandData.commandPayloads = inner["commandPayloads"].isNull() ? "" : doc["commandData"]["commandPayloads"].as<std::string>();
            if (!inner["commandPayloads"].isNull() && inner["commandPayloads"].is<JsonArrayConst>())
            {
                auto payloads = inner["commandPayloads"].as<JsonArrayConst>();
                for (size_t i = 0; i < payloads.size(); ++i)
                {
                    std::string payload = payloads[i].as<std::string>();
                    task.commandData.commandPayloads.push_back(payload);
                }
            }
        }
    }
    else
    {
        task.commandData.hasData = false;
    }

    return task;
}

// ------------- Sérializer/Déserializer additionnalPayload ---------------

/*
Exemple:
{
"addPayload":{
    "frequency":"36",
    "toggleMask":"0x8000",
    "repeat":"2",
    "raw": "0"
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

    payload.frequency = doc["addPayload"]["frequency"].as<std::string>();
    payload.toggleMask = doc["addPayload"]["toggleMask"].as<std::string>();
    payload.repeat = doc["addPayload"]["repeat"].as<std::string>();
    payload.raw = doc["addPayload"]["raw"].as<std::string>();

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
    auto payloads = root["addPayload"].to<JsonObject>();
    payloads["frequency"] = payload.frequency;
    payloads["toggleMask"] = payload.toggleMask;
    payloads["repeat"] = payload.repeat;
    payloads["raw"] = payload.raw;
}