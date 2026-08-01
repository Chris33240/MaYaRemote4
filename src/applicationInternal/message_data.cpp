/// @file message_data.cpp
/// @brief Sérialisation et création des messages pour le système IR et BLE.
#include "message_data.h"

/// @brief Crée un objet messageData.
/// @param message Texte du message.
/// @param payloads Liste de chaînes représentant les payloads associés.
/// @param code Code optionnel pour le message.
/// @return Un objet messageData rempli avec les valeurs passées.
messageData makeMessageData(const std::string &message, const std::list<std::string> &payloads, const std::string &code)
{
    messageData messageData = {message, code, payloads};
    return messageData;
}

/*
Structure des messages pour la lecture READ de la derniere capture :
{
"Code":xxx,  Code optionnel pour traitement du message
"Message": "Nouveau code IR capturé : SONY_0x090A",
"Payloads": [
    "RC              -> Protocol
    "false           -> repeat
    "0xC800F040C",   -> result
    "36"             -> frequency
    ...etc
]
}
*/
/// @brief Sérialise un message avec ses payloads et code en JSON.
/// @param message Texte du message.
/// @param payloads Liste de chaînes représentant les payloads associés.
/// @param code Code optionnel pour le message.
/// @return Une chaîne contenant le JSON sérialisé du message.
std::string serialize(const std::string &message, const std::list<std::string> &payloads, const std::string &code)
{
    std::string output;
    // DynamicJsonDocument doc(1024); // Alloue sur le tas (heap)
    JsonDocument doc = to_json(message, payloads, code);

    // Sérialisation du JsonObject dans la String
    serializeJsonPretty(doc, output);
    return output;
}

/// @brief Sérialise un objet messageData en JSON.
/// @param messageData Objet messageData à sérialiser.
/// @return Une chaîne contenant le JSON sérialisé.
std::string serialize(const messageData &messageData)
{
    std::string output;
    JsonDocument doc = to_json(messageData);

    // Sérialisation du JsonObject dans la String
    serializeJsonPretty(doc, output);
    return output;
}

/// @brief Convertit un message, ses payloads et son code en un document JSON.
/// @param message Texte du message.
/// @param payloads Liste de chaînes représentant les payloads associés.
/// @param code Code optionnel pour le message.
/// @return Un JsonDocument représentant le message.
JsonDocument to_json(const std::string &message, const std::list<std::string> &payloads, const std::string &code)
{
    JsonDocument doc;
    auto root = doc.to<JsonObject>();

    // Sérialiser message
    root["message"] = message;

    // Sérialiser code
    root["code"] = code;

    // Sérialiser payloads
    auto payloads1 = root["payloads"].to<JsonArray>();
    for (const auto &payload : payloads)
    {
        payloads1.add(payload);
    }
    return doc;
}

/// @brief Convertit un objet messageData en un document JSON.
/// @param messageData Objet messageData à convertir.
/// @return Un JsonDocument représentant le message.
JsonDocument to_json(const messageData &messageData)
{
    JsonDocument doc;
    auto root = doc.to<JsonObject>();

    // Sérialiser message
    root["message"] = messageData.message;

    // Sérialiser code
    root["code"] = messageData.code;

    // Sérialiser payloads
    auto payloads = root["payloads"].to<JsonArray>();
    for (const auto &payload : messageData.payloads)
    {
        payloads.add(payload);
    }
    return doc;
}
