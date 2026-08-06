#include "ble_loop_list_commands.h"
#include "applicationInternal/list_commands_handler.h"

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