#include <Arduino.h>

#ifndef LISTCOMMANDSHANDLER_H
#define LISTCOMMANDSHANDLER_H

#include <map>
#include <string>
#include "packets_handler.h"
#include "commandHandler.h"

class ListCommandsHandler
{
public:
    ListCommandsHandler();
    [[deprecated("Don't use this routine any more. Use the new one instead. 'readCommand2Keys()'")]]
    std::string readCommand2(const std::map<std::string, commandData2> &commands);
    std::string readCommand2Keys(const std::set<std::string> &commandsKeys);

private:
    bool isInit;
    std::map<std::string, commandData2>::iterator it;
    bool isInitCommand;
    size_t currentCommandIndex;
    PacketsHandler packets;
    void reset();
};

#endif