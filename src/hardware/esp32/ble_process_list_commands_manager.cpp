#include "ble_process_list_commands_manager.h"
#include "applicationInternal/list_commands_handler.h"

std::string currentPacket;
volatile bool packetReady = false;
volatile bool requestNewPacket = false;
// Gestionnaire de listes de commandes pour le serveur BLE
ListCommandsHandler listCommandsHandler1;

void processListCommandsLoop()
{
    if (requestNewPacket)
    {
        currentPacket = listCommandsHandler1.readCommandKeys();
        packetReady = true;
        requestNewPacket = false;

    }
}