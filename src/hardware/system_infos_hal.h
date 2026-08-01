
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
    #include <Esp.h>
    #include <esp_system.h>
    #include <esp_spi_flash.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
#endif

//==============================================================
// Hardware Abstraction Layer
//==============================================================

namespace HAL
{
    //==========================================================
    // CPU
    //==========================================================

    static uint32_t getDefaultCpuFrequencyMHz()
    {
        return F_CPU / 1000000UL;
    }

    static uint32_t getCurrentCpuFrequencyMHz()
    {
        return ESP.getCpuFreqMHz();
    }

    static uint32_t getChipRevision()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return ESP.getChipRevision();
    #else
        return 0;
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

    static uint32_t getStackHighWaterMark()
    {
    #if defined(ARDUINO_ARCH_ESP32)
        return uxTaskGetStackHighWaterMark(nullptr);
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
        //return RP2040.getFlashSize();
        //return PICO_FLASH_SIZE_BYTES;
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
}