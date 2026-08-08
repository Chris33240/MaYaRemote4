#include "system_infos.h"
#include "hardware/system_infos_hal.h"

/// @brief Affiche des informations système de base.
void printSystemInfos()
{
    Serial.println(F("===== System Information ====="));

    // ===== CPU =====
    Serial.printf("CPU Frequency (current)  : %u MHz\n", HAL::getCurrentCpuFrequencyMHz());
    //Serial.printf("Chip Revision            : %u\n", HAL::getChipRevision());
    if (HAL::hasChipRevision())
    {
        Serial.printf("Chip Revision         : %u\n", HAL::getChipRevision());
    }

    // ===== RAM =====
    Serial.printf("Free Heap                : %u bytes\n", HAL::getFreeHeap());
    Serial.printf("Total Heap               : %u bytes\n", HAL::getTotalHeap());
    Serial.printf("Stack High Water Mark    : %u words\n", HAL::getStackHighWaterMark());

    // ===== Flash =====
    Serial.printf("Flash Size               : %u MB\n", HAL::getFlashSize() / (1024 * 1024));
    Serial.printf("Flash Speed              : %u MHz\n", HAL::getFlashSpeed() / 1000000);

    if (HAL::hasFlashMode())
    {
        Serial.printf("Flash Mode               : %u\n", HAL::getFlashMode());
    }

    Serial.printf("Flash Sector Size        : %u bytes\n", HAL::getFlashSectorSize());

    // ===== Firmware =====
    Serial.printf("Sketch Size              : %u bytes\n", HAL::getSketchSize());
    Serial.printf("Free Sketch Space        : %u bytes\n", HAL::getFreeSketchSpace());

    // ===== Temp et Battery voltage =====

    if (HAL::hasChipTemperature())
    {
        Serial.printf("Chip Temperature         : %.2f °C\n", HAL::getChipTemperature());
    }
    
    if (HAL::hasBatteryMeasurement())
    {
        Serial.printf("Battery ADC              : %u\n", HAL::getBatteryAnalogValue());

        Serial.printf("Battery voltage          : %.3f V\n", HAL::getBatteryVoltage());
    }

    Serial.println();
}