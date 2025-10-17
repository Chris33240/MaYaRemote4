/*
#include "list_commands_base.h"
#include "interfaces/hardwarePresenter.h"

ListCommandsBase::ListCommandsBase(onCurrentCommandCallback onCurrentCommand)
    : _onCurrentCommand(onCurrentCommand)
{
        // Initialise la variable local
    //this->_onCurrentCommand = onCurrentCommand;
}

std::string ListCommandsBase::read(const std::set<std::string>& commandsKeys)
{
    std::string str;

    if (!isInit)
    {
        isInit = true;
        isInitCommand = false;
        currentCommandIndex = 0;
        packets.setOnTimeoutCallback([this]() {
            reset();
            Serial.println("[Timeout] expired BLE Read Packet");
            sendBleNotifyCode("101");
        });
    }

    if (!isInitCommand && currentCommandIndex < commandsKeys.size())
    {
        auto command = std::next(commandsKeys.begin(), currentCommandIndex);
        const std::string& commandName = *command;

        std::string commandStr = _onCurrentCommand(commandName);

        packets.makePackets(commandStr.c_str());
        isInitCommand = true;
        currentCommandIndex++;
    }

    if (isInitCommand)
    {
        if (packets.hasMorePackets()) str = packets.getPacket();
        if (!packets.hasMorePackets()) isInitCommand = false;
    }

    if (currentCommandIndex > commandsKeys.size()) isInit = false;

    return str;
}

void ListCommandsBase::reset()
{
    isInit = false;
    isInitCommand = false;
    currentCommandIndex = 0;
}
*/
