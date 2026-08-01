
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
    #include <Esp.h>
    #include <esp_system.h>
    #include <esp_spi_flash.h>
    #include <esp_heap_caps.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
#elif defined(ARDUINO_ARCH_RP2040)
    #include <hardware/flash.h>
#endif
#include "filesystem_hal.h"

//==============================================================
// Hardware Abstraction Layer
//==============================================================

namespace HAL
{
    //==========================================================
    // CPU
    //==========================================================

    static String getChipModel()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getChipModel();
    #elif defined(ARDUINO_ARCH_RP2040)
        return "RP2040";
    #else
        return "Unknown";
    #endif
    }

    static bool hasChipRevision()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return true;
    #else
        return false;
    #endif
    }

    static uint32_t getChipRevision()
    {
        return ESP.getChipRevision();
    }

    /*
    static uint32_t getChipRevision()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getChipRevision();
    #else
        return 0;
    #endif
    }
    */

    static uint32_t getDefaultCpuFrequencyMHz()
    {
        return F_CPU / 1000000UL;
    }

    static uint32_t getCurrentCpuFrequencyMHz()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getCpuFreqMHz();
    #else
        return F_CPU / 1000000UL;
    #endif
    }

    static uint32_t getCpuCoreCount()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getChipCores();
    #elif defined(ARDUINO_ARCH_RP2040)
        return 2;
    #else
        return 1;
    #endif
    }

    //==========================================================
    // RAM
    //==========================================================

    static uint32_t getFreeHeap()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getFreeHeap();
    #else
        return 0;
    #endif
    }

    static uint32_t getTotalHeap()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getHeapSize();
    #else
        return 0;
    #endif
    }

    static uint32_t getMinFreeHeap()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getMinFreeHeap();
    #else
        return 0;
    #endif
    }

    static uint32_t getLargestFreeBlock()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
    #else
        return 0;
    #endif
    }

    static uint32_t getStackHighWaterMark()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return uxTaskGetStackHighWaterMark(nullptr);
    #else
        return 0;
    #endif
    }

    //==========================================================
    // PSRAM
    //==========================================================

    static bool hasPsram()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return psramFound();
    #else
        return false;
    #endif
    }

    static uint32_t getPsramSize()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return psramFound() ? ESP.getPsramSize() : 0;
    #else
        return 0;
    #endif
    }

    static uint32_t getFreePsram()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return psramFound() ? ESP.getFreePsram() : 0;
    #else
        return 0;
    #endif
    }

    //==========================================================
    // Flash
    //==========================================================

    static uint32_t getFlashSize()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getFlashChipSize();
    #elif defined(ARDUINO_ARCH_RP2040)
        return PICO_FLASH_SIZE_BYTES;
    #else
        return 0;
    #endif
    }

    static uint32_t getFlashSpeed()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getFlashChipSpeed();
    #else
        return 0;
    #endif
    }

    static uint32_t getFlashSectorSize()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return SPI_FLASH_SEC_SIZE;
    #elif defined(ARDUINO_ARCH_RP2040)
        return 4096;
    #else
        return 0;
    #endif
    }

    static bool hasFlashMode()
    {
    #if defined(CONFIG_IDF_TARGET_ESP32)
        return true;
    #else
        return false;
    #endif
    }

    static uint32_t getFlashMode()
    {
    #if defined(CONFIG_IDF_TARGET_ESP32)
        return ESP.getFlashChipMode();
    #else
        return 0;
    #endif
    }

    //==========================================================
    // Firmware
    //==========================================================

    static uint32_t getSketchSize()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getSketchSize();
    #else
        return 0;
    #endif
    }

    static uint32_t getFreeSketchSpace()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getFreeSketchSpace();
    #else
        return 0;
    #endif
    }

    //==========================================================
    // Filesystem
    //==========================================================

    static bool hasFilesystem()
    {
    #if ENABLED_IO_FILESYSTEM
        return true;
    #else
        return false;
    #endif
    }

    static uint32_t getFilesystemUsedBytes()
    {
        return getUsedBytes_HAL();
    }

    static uint32_t getFilesystemTotalBytes()
    {
        return getTotalBytes_HAL();
    }

    static uint32_t getFilesystemFreeBytes()
    {
        return getTotalBytes_HAL() - getUsedBytes_HAL();
    }

/*
    static uint32_t getFilesystemUsedBytes() A_CORRIGER
    {
    #if defined(ENABLED_IO_FILESYSTEM)
        return getUsedBytes_HAL();
    #else
        return 0;
    #endif
    }


    static uint32_t getFilesystemUsedBytes()
    {
    #if defined(ARDUINO_ARCH_ESP32)

        return getUsedBytes_HAL();

    #elif defined(ARDUINO_ARCH_RP2040)

        return getUsedBytes_HAL();

    #else

        return 0;

    #endif
    }
*/

}