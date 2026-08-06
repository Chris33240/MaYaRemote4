/// @file list_commands_handler.cpp
/// @brief Gestion de l'envoi séquentiel de systemInfos via BLE, avec découpage en paquets.
#include "system_infos_handler.h"
#include <interfaces/hardwarePresenter.h>
#include "system_infos_json.h"
#include "helpers/omote_log.h"

/// @brief Constructeur de SystemInfosHandler.
SystemInfosHandler::SystemInfosHandler() : isInit(false),
      isInitCommand(false)
{
}

/// @brief Crée les paquets, attache un callback de Timeout et retourne chacun des paquets de maniere successive jusqu'a épuisement.
/// @return Une chaîne correspondant au prochain paquet de la commande sérialisée.
std::string SystemInfosHandler::readSystemInfos()
{
    std::string str;

    if (!isInit)
    {
        omote_log_d("1-isInit");
        isInit = true;
        isInitCommand = false;
        
        packets.setOnTimeoutCallback([this]()
        {
            omote_log_d("2-setTimeout");
            reset();
            Serial.println("[Timeout] expired BLE Read Packet (SystemInfos)");
            sendBleNotifyCode("101");
        });
    }

    if (!isInitCommand)
    {
        omote_log_d("3-fillInfos");
        systemInfos sysInfos;
        fillSystemInfos(sysInfos);
        omote_log_d("4-serializeInfos");
        String systemInfosStr = serializeSystemInfos(sysInfos, false);
        omote_log_d("5-makePacketsInfos");
        packets.makePackets(systemInfosStr.c_str());

        Serial.printf("[DEBUG] Serialized SystemInfos: '%s'.\r\n",
                      systemInfosStr.c_str());

        isInitCommand = true;
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

    if (!isInitCommand)
    {
        isInit = false;
    }

    return str;
}

/// @brief Réinitialise l'état interne du gestionnaire.
void SystemInfosHandler::reset()
{
    isInit = false;
    isInitCommand = false;
}