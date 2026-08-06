#include "system_infos_fill.h"
#include "hardware/system_infos_hal.h"

/// @brief Remplit la structure contenant les informations système.
void fillSystemInfos(systemInfos &sysInfos)
{
    //==========================================================
    // CPU
    //==========================================================

    sysInfos.chipModel   = HAL::getChipModel();

    if (HAL::hasChipRevision())
        sysInfos.chipRev = HAL::getChipRevision();
    else
        sysInfos.chipRev = 0;

    sysInfos.cpuDefFreq  = HAL::getDefaultCpuFrequencyMHz();
    sysInfos.cpuFreq     = HAL::getCurrentCpuFrequencyMHz();
    sysInfos.chipCores   = HAL::getChipCoresCount();

    //==========================================================
    // RAM
    //==========================================================

    sysInfos.freeHeap    = HAL::getFreeHeap();
    sysInfos.totalHeap   = HAL::getTotalHeap();
    sysInfos.minFreeHeap = HAL::getMinFreeHeap();
    sysInfos.maxAllocHeap = HAL::getMaxAllocHeap();
    sysInfos.stack       = HAL::getStackHighWaterMark();

    //==========================================================
    // PSRAM
    //==========================================================

    if (HAL::hasPsram())
    {
        sysInfos.psramSize    = HAL::getPsramSize();
        sysInfos.freePsram    = HAL::getFreePsram();
        sysInfos.minFreePsram = HAL::getMinFreePsram();
        sysInfos.maxAllocPsram = HAL::getLargestFreePsramBlock();
    }
    else
    {
        sysInfos.psramSize     = 0;
        sysInfos.freePsram     = 0;
        sysInfos.minFreePsram  = 0;
        sysInfos.maxAllocPsram = 0;
    }

    //==========================================================
    // Flash
    //==========================================================

    sysInfos.flashSize       = HAL::getFlashSize();
    sysInfos.flashSpeed      = HAL::getFlashSpeed();
    sysInfos.flashSectorSize = HAL::getFlashSectorSize();

    if (HAL::hasFlashMode())
        sysInfos.flashMode = HAL::getFlashMode();
    else
        sysInfos.flashMode = 0;

    //==========================================================
    // Firmware
    //==========================================================

    sysInfos.sketchSize      = HAL::getSketchSize();
    sysInfos.freeSketchSpace = HAL::getFreeSketchSpace();

    //==========================================================
    // Filesystem
    //==========================================================

    if (HAL::hasFilesystem())
    {
        fsMount_HAL();
        sysInfos.fsUsed  = HAL::getFilesystemUsedBytes();
        sysInfos.fsTotal = HAL::getFilesystemTotalBytes();
        fsUnMount_HAL();
    }
    else
    {
        sysInfos.fsUsed  = 0;
        sysInfos.fsTotal = 0;
    }
}