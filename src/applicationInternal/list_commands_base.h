#pragma once

#include <Arduino.h>
#include <map>
#include <string>
#include "packets_handler.h"
#include "commandHandler.h"

class ListCommandsBase
{
public:
    using onCurrentCommandCallback = std::function<std::string(const std::string&)>;

    explicit ListCommandsBase(onCurrentCommandCallback onCurrentCommand);

    std::string read(const std::set<std::string>& commandsKeys);
    void reset();

private:
    bool isInit = false;
    bool isInitCommand = false;
    size_t currentCommandIndex = 0;

    PacketsHandler packets;   // Adapter si le vrai type est différent
    onCurrentCommandCallback _onCurrentCommand;
};