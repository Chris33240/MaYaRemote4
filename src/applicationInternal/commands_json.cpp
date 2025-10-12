/// @file commands_json.cpp
/// @brief Sérialisation et désérialisation des commandes JSON pour BootLoader et fichiers de commandes individuels.
#include "commands_json.h"
#include <helpers/helpers.h>

// ------------- Serialization/Deserialization de Commands_json_bootloader ---------------

/* exemple:
  "IR_MCE_POWER": {
  "commandHandler": "3",
  "requestType": "WRITE",
  "attributs": "",
  "commandPayloads": [
      "1001",
      "0xC800F040CLL",
      "36",
      "0x8000",
      "true"
  ]
  },
*/

/// @brief Désérialise une chaîne JSON au format du bootloader en map de commandes.
/// @param input Chaîne JSON à désérialiser.
/// @param loadingRawData Indique si l'on charge les données brutes.
/// @return Map associant le nom de la commande à ses données.
std::map<std::string, commandData2> deserializeCommands(String input)
{
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error)
  {
    Serial.print(F("ERREUR : Fail to deserialize: "));
    Serial.println(error.c_str());
    throw std::runtime_error(("ERREUR : Fail to deserialize: " + input).c_str());
  }
  std::map<std::string, commandData2> map = from_json_BootLoader(doc);
  return map;
}

/// @brief Transforme un JsonDocument au format du bootloader en map de commandes.
/// @param doc Document JSON à traiter.
/// @param loadingRawData Indique si l'on charge les données brutes.
/// @return Map de commandes.
std::map<std::string, commandData2> from_json_BootLoader(const JsonDocument &doc)
{
  std::map<std::string, commandData2> commandMap;

  for (JsonPairConst pair : doc.as<JsonObjectConst>())
  {
    // Construire un sous-document contenant uniquement la paire actuelle
    // StaticJsonDocument<512> subDoc; // Taille ajustée pour une paire clé-valeur
    JsonDocument subDoc;
    subDoc[pair.key()] = pair.value();

    // Utiliser from_json1 pour traiter cette paire
    auto command = from_json_Command(subDoc);

    // Ajouter le résultat à la map
    commandMap[command.first] = command.second;
  }

  return commandMap;
}

// ------------- Serialization/Deserialization des fichiers de commandData ---------------

/*
Exemple de structure de fichiers de commande json :

Nom du fichier : “IR_2_0xC800F040C.cde”
{
    "IR_2_0xC800F040C": {
    "commandHandler": "3",
    "requestType": "WRITE",
    "attributs": "w",
    "commandPayloads": [
        "2",
        "false",
        "0xC800F040C",
        "36",
        "67",
        "false",
        "2700, 850, 450, 450, 450, 450, 450, 850, 450, 900, 1350, 850, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 400, 450, 450, 450, 450, 900, 450, 450, 450, 450, 400, 450, 900, 450, 450, 450, 450, 450, 450, 450, 450, 900, 850, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 900, 450, 400, 950, 400, 450, 450",
        "",
        "0x0C, 0x04, 0x0F, 0x80, 0x0C",
        "0xC800F04",
        "0xC",
        "0xC800F040C"
    ]}
}

Nom du fichier : “IR_MCE_POWER.cde”
{
    "IR_MCE_POWER": {
    "commandHandler": "3",
    "requestType": "WRITE",
    "commandPayloads": [
        "901",
        "0xC800F040CLL",
        "36",
        "0x8000",
        "true"
  ]}
}

*/

/// @brief Sérialise une commande en chaîne JSON.
/// @param commandName Nom de la commande.
/// @param commandData Données de la commande.
/// @param JsonPretty Indique si la sortie doit être formatée joliment.
/// @return Chaîne JSON représentant la commande.
String serializeCommand(const std::string &commandName, const commandData2 &commandData, bool JsonPretty)
{
  String output;
  JsonDocument doc;
  to_json_Command(doc, commandName, commandData);
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

/// @brief Remplit un JsonDocument à partir d'une commande.
/// @param doc Document JSON à remplir.
/// @param commandName Nom de la commande.
/// @param commandData Données de la commande.
void to_json_Command(JsonDocument &doc, const std::string &commandName, const commandData2 &commandData)
{
  auto root = doc.to<JsonObject>();
  auto commandObj = root[commandName].to<JsonObject>();

  commandObj["requestType"] = commandData.requestType;
  commandObj["attributs"] = commandData.attributs;
  commandObj["commandHandler"] = commandData.commandHandler;
  auto payloads = commandObj["commandPayloads"].to<JsonArray>();
  for (const auto &payload : commandData.commandPayloads)
  {
    payloads.add(payload);
  }
}

/// @brief Désérialise une chaîne JSON en une paire nom-commande.
/// @param input Chaîne JSON à désérialiser.
/// @param loadingRawData Indique si l'on charge les données brutes.
/// @return Paire contenant le nom de la commande et ses données.
std::pair<std::string, commandData2> deserializeCommand(String input)
{
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error)
  {
    Serial.print(F("ERREUR : Fail to deserialize: "));
    Serial.println(error.c_str());
    throw std::runtime_error(("ERREUR : Fail to deserialize: " + input).c_str());
  }
  std::pair<std::string, commandData2> pair = from_json_Command(doc);
  return pair;
}

/// @brief Transforme un JsonDocument en paire nom-commande.
/// @param doc Document JSON à traiter.
/// @param loadingRawData Indique si l'on charge les données brutes.
/// @return Paire contenant le nom de la commande et ses données.
std::pair<std::string, commandData2> from_json_Command(const JsonDocument &doc)
{
  auto root = doc.as<JsonObjectConst>();
  std::string commandName;
  commandData2 data;

  // Trouver le nom de la commande (la clé du premier objet)
  for (JsonPairConst kv : root)
  {
    commandName = kv.key().c_str();
    break; // On ne prend que la première clé
  }

  auto commandObj = root[commandName].as<JsonObjectConst>();

  data.requestType = commandObj["requestType"].as<std::string>();
  data.attributs = commandObj["attributs"].as<std::string>();
  data.commandHandler = static_cast<commandHandlers>(commandObj["commandHandler"].as<int>());
  auto payloads = commandObj["commandPayloads"].as<JsonArrayConst>();

  for (size_t i = 0; i < payloads.size(); ++i)
  {
    std::string payload = payloads[i].as<std::string>();
    data.commandPayloads.push_back(payload);
  }

  return std::make_pair(commandName, data);
}

// --------------------------------------------------------------------------------------
/// @brief Désérialise une chaîne JSON pour récupérer seulement les clés.
/// @param input Chaîne JSON à désérialiser.
/// @return Ensemble contenant uniquement les noms des commandes.
std::set<std::string> deserializeKeys(String input)
{
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error)
  {
    Serial.print(F("ERREUR : Fail to deserialize: "));
    Serial.println(error.c_str());
    throw std::runtime_error(("ERREUR : Fail to deserialize: " + String(error.c_str())).c_str());
  }
  std::set<std::string> set = from_json_Keys(doc);
  return set;
}

/// @brief Transforme un JsonDocument en liste de clés de commandes.
/// @param doc Document JSON à traiter.
/// @return Liste des clés de commandes.
std::set<std::string> from_json_Keys(const JsonDocument &doc)
{
  std::set<std::string> commandSet;

  for (JsonPairConst pair : doc.as<JsonObjectConst>())
  {
    commandSet.insert(pair.key().c_str());
  }

  return commandSet;
}
