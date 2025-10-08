/// @file commands_list_json.cpp
/// @brief Sérialisation des commandes individuelles pour la lecture via BLE.
#include "commands_list_json.h"

// ------------------ Serialiser Commands list -----------------------

// -----Json pour lecture du catalogues des commandes via la requette ble READ -------------

/// @brief Sérialise une commande en JSON pour la requête BLE.
/// @param commandName Nom de la commande.
/// @param commandData Données de la commande.
/// @param status Statut associé à la commande (ex: "INVALID_FILE").
/// @return Chaîne JSON représentant la commande avec son statut.
std::string serialize2(const std::string &commandName, const commandData2 &commandData, const std::string &status)
{
  std::string output;
  JsonDocument doc;
  to_json2(doc, commandName, commandData, status);
  serializeJson(doc, output);
  return output;
}

// Exemple : {"command": "IR_CAPTURE", "status": "INVALID_FILE", "requestType": "WRITE", "attributs": "w", "commandHandler": "3"}
/// @brief Remplit un JsonDocument à partir d'une commande et de son statut.
/// @param doc Document JSON à remplir.
/// @param commandName Nom de la commande.
/// @param commandData Données de la commande.
/// @param status Statut associé à la commande.
void to_json2(JsonDocument &doc, const std::string &commandName, const commandData2 &commandData, const std::string &status)
{
  // JsonDocument doc;
  auto root = doc.to<JsonObject>();

  root["command"] = commandName;
  root["status"] = status;
  root["requestType"] = commandData.requestType;
  root["attributs"] = commandData.attributs;
  root["commandHandler"] = commandData.commandHandler;
}