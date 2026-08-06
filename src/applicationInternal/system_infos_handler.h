#include <Arduino.h>

#ifndef SYSTEMINFOSHANDLER_H
#define SYSTEMINFOSHANDLER_H

#include <map>
#include <string>
#include "packets_handler.h"

class SystemInfosHandler
{
public:
    SystemInfosHandler();
    std::string readSystemInfos();

private:
    bool isInit;
    bool isInitCommand;
    PacketsHandler packets;
    void reset();
};

#endif