#include <Arduino.h>

#ifndef LISTCOMMANDSHANDLERDATA_H
#define LISTCOMMANDSHANDLERDATA_H

#include <map>
#include <string>
#include "packets_handler.h"
#include "commandHandler.h"

class ListCommandsDataHandler
{
public:
    ListCommandsDataHandler();
    std::string readCommandsDataKeys();
    std::string readCommandsDataKeys(const std::set<std::string> &commandsKeys);

private:
    bool isInit;
    std::map<std::string, commandData2>::iterator it;
    bool isInitCommand;
    size_t currentCommandIndex;
    PacketsHandler packets;
    void reset();
};

#endif