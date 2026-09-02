#include "ble_loop_list_commands.h"
#include "applicationInternal/list_commands_handler.h"
#include "applicationInternal/list_commandsData_handler.h"

// Ce code créer une loop destinée à alléger la stack bluetooth BTC_TASK
// Ainsi le traitement lourd est déporté sur la stack principal (main).

BleRequest listCommandsRequest;
static ListCommandsHandler listCommandsHandler;

void processListCommandsLoop()
{
    if (listCommandsRequest.isRequested())
    {
        listCommandsRequest.setPacket(
            listCommandsHandler.readCommandKeys());
    }
}

BleRequest listCommandsDataRequest;
static ListCommandsDataHandler listCommandsDataHandler;

void processListCommandsDataLoop()
{
    if (listCommandsDataRequest.isRequested())
    {
        listCommandsDataRequest.setPacket(
            listCommandsDataHandler.readCommandsDataKeys());
    }
}