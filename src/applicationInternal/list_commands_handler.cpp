/// @file list_commands_handler.cpp
/// @brief Gestion de l'envoi séquentiel des commandes via BLE, avec découpage en paquets.
#include "list_commands_handler.h"
#include "interfaces/hardwarePresenter.h"
#include "commands_io.h"
#include "commands_json.h"
#include "helpers/omote_log.h"

/// @brief Constructeur de ListCommandsHandler.
ListCommandsHandler::ListCommandsHandler() : currentCommandIndex(0), isInitCommand(false)
{
}

/// @brief Crée les paquets, attache un callback de Timeout et retourne chacun des paquets de maniere successive jusqu'a épuisement.
/// @return Une chaîne correspondant au prochain paquet de la commande sérialisée.
std::string ListCommandsHandler::readCommandKeys()
{
    return readCommandKeys(getCommands2Keys());
}

/// @brief Crée les paquets, attache un callback de Timeout et retourne chacun des paquets de maniere successive jusqu'a épuisement.
/// @param commandsKeys Liste des clés de commandes.
/// @return Une chaîne correspondant au prochain paquet de la commande sérialisée.
std::string ListCommandsHandler::readCommandKeys(const std::set<std::string> &commandsKeys)
{
    std::string str;

    if (!isInit)
    {
        isInit = true;
        isInitCommand = false;
        currentCommandIndex = 0;
        packets.setOnTimeoutCallback([this]()
                                     {
        reset();
        Serial.println("[Timeout] expired BLE Read Packet (ListCommands)");
        sendBleNotifyCode("101"); });
    }

    if (!isInitCommand && currentCommandIndex < commandsKeys.size())
    {
        auto command = std::next(commandsKeys.begin(), currentCommandIndex);

        const std::string &commandName = *command;
        commandData2 data;
        //std::string status;
        //omote_log_v("DEBUG: we are here 2\r\n");
        //omote_log_v_mem();
        if (findCommandData(commandName, data))
        {
            //status = "OK";
        }
        else
        {
            data = makeCommandData2(SPECIAL, "", "", {}); // Empty data not used
            //status = "ERROR_READING_FILE";
            Serial.printf("[DEBUG] Failed to read serialized commands %u/%u: '%s'.\r\n", currentCommandIndex + 1, commandsKeys.size(), commandName.c_str());
        }

        //const std::string &commandStr = serialize2(commandName, data);
        const String &commandStr = serializeCommandWithStatus(commandName, data, false);
        packets.makePackets(commandStr.c_str());

        Serial.printf("[DEBUG] serialized commands %u/%u: '%s'.\r\n", currentCommandIndex + 1, commandsKeys.size(), commandStr.c_str());

        isInitCommand = true;
        currentCommandIndex++;
    }

    if (isInitCommand)
    {
        if (packets.hasMorePackets())
        {
            str = packets.getPacket();
        }
        if (!packets.hasMorePackets())
        {
            isInitCommand = false;
        }
    }

    if (currentCommandIndex > commandsKeys.size())
    {
        isInit = false;
    }

    return str;
}

/// @brief Réinitialise l'état interne du gestionnaire de commandes.
void ListCommandsHandler::reset()
{
    isInit = false;
    isInitCommand = false;
    currentCommandIndex = 0;
}