#include <Arduino.h>

#ifndef SYSTEMINFOFILLSHANDLER_H
#define SYSTEMINFOSFILLHANDLER_H

struct systemInfos
{
    String chipModel;
    uint32_t chipRev;
    uint32_t cpuDefFreq;
    uint32_t cpuFreq;
    uint32_t chipCores;

    uint32_t freeHeap;
    uint32_t totalHeap;
    uint32_t minFreeHeap;
    uint32_t maxAllocHeap;
    uint32_t stack;

    uint32_t psramSize;
    uint32_t freePsram;
    uint32_t minFreePsram;
    uint32_t maxAllocPsram;

    uint32_t flashSize;
    uint32_t flashSpeed;
    uint32_t flashSectorSize;
    uint32_t flashMode;

    //uint32_t sketchSize;
    //uint32_t freeSketchSpace;

    uint32_t fsUsed;
    uint32_t fsTotal;

};

void fillSystemInfos(systemInfos &sysInfos);

#endif