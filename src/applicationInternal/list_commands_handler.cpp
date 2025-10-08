/// @file list_commands_handler.cpp
/// @brief Gestion de l'envoi séquentiel des commandes via BLE, avec découpage en paquets.
#include "list_commands_handler.h"
#include "commands_list_json.h"
#include "interfaces/hardwarePresenter.h"
#include "commands_io.h"

/// @brief Constructeur de ListCommandsHandler.
ListCommandsHandler::ListCommandsHandler() : currentCommandIndex(0), isInitCommand(false)
{
}

/// @brief Crée les paquets, attache un callback de Timeout et retourne chacun des paquets de maniere successive jusqu'a épuisement.
/// @param commands Map des commandes.
/// @return Une chaîne correspondant au prochain paquet de la commande sérialisée.
std::string ListCommandsHandler::readCommand2(const std::map<std::string, commandData2> &commands)
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

    if (!isInitCommand && currentCommandIndex < commands.size())
    {
        auto command = std::next(commands.begin(), currentCommandIndex);

        std::string commandName = command->first;
        commandData2 data = command->second;

        std::string commandStr = serialize2(commandName, data, "OK");
        packets.makePackets(commandStr);

        Serial.printf("[DEBUG] serialized commands %u/%u: '%s'.\r\n", currentCommandIndex + 1, commands.size(), commandStr.c_str());

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

    if (currentCommandIndex > commands.size())
    {
        isInit = false;
    }

    return str;
}

/// @brief Crée les paquets, attache un callback de Timeout et retourne chacun des paquets de maniere successive jusqu'a épuisement.
/// @param commandsKeys Liste des clés de commandes.
/// @return Une chaîne correspondant au prochain paquet de la commande sérialisée.
std::string ListCommandsHandler::readCommand2Keys(const std::set<std::string> &commandsKeys)
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
        std::string status;
        try
        {
            const std::pair<std::string, commandData2> &pair = loadCommand(commandName);
            data = pair.second;
            status = "OK";
        }
        catch (const std::exception &e)
        {
            data = makeCommandData2(SPECIAL, "", "", {}); // Empty data not used
            status = "ERROR_READING_FILE";
            Serial.printf("[DEBUG] Failed to read serialized commands %u/%u: '%s'.\r\n", currentCommandIndex + 1, commandsKeys.size(), commandName.c_str());
        }

        const std::string &commandStr = serialize2(commandName, data, status);
        packets.makePackets(commandStr);

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