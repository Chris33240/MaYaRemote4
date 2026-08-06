#include "ble_loop_system_infos.h"
#include "applicationInternal/system_infos_handler.h"

BleRequest systemInfosRequest;
static SystemInfosHandler systemInfosHandler;

void processSystemInfosLoop()
{
    if (systemInfosRequest.isRequested())
    {
        systemInfosRequest.setPacket(
            systemInfosHandler.readSystemInfos());
    }
}