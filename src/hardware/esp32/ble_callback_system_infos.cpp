/// @file ble_callback_system_infos.cpp
/// @brief Callbacks pour les caractéristiques System Infos du serveur BLE ESP32.
#include <HardwareSerial.h>
#include <Esp.h>
#include "ble_uuid.h"
#include "ble_callback_system_infos.h"
#include "interfaces/hardwarePresenter.h"
#include "hardware/system_infos_hal.h"
//#include "applicationInternal/system_infos_handler.h"
#include "ble_loop_system_infos.h"

/// Gestionnaire de listes de commandes pour le serveur BLE
//SystemInfosHandler systemInfosHandler;

// ------------------- Callbacks System Infos ---------------------------
/// @brief Callback appelé lorsqu'un client effectue une lecture sur une caractéristique System Infos
/// @param pCharacteristic Pointeur vers la caractéristique lue
void MyCallbacksSystemInfos::onRead(BLECharacteristic *pCharacteristic)
{

    // Obtenez l'UUID de la caractéristique
    const std::string &uuid = pCharacteristic->getUUID().toString();
    Serial.print(F("[BLE-onRead] BLE Characteristic uuid: "));
    Serial.println(uuid.c_str());

    if (uuid == CHARACTERISTIC_CPU_CHIP_MODEL_UUID)
    {
        String cpuChipModel = HAL::getChipModel();
        Serial.print(F("[BLE-OnRead] CPU Chip model: "));
        Serial.println(cpuChipModel);
        pCharacteristic->setValue(cpuChipModel.c_str());
    }
    if (uuid == CHARACTERISTIC_CPU_CHIP_REVISION_UUID)
    {
        uint32_t cpuChipRevision = HAL::getChipRevision();
        Serial.print(F("[BLE-OnRead] CPU Chip revision: "));
        Serial.println(cpuChipRevision);
        pCharacteristic->setValue(cpuChipRevision);
    }
    if (uuid == CHARACTERISTIC_CPU_FREQUENCY_UUID)
    {
        uint32_t cpuFrequency = HAL::getCurrentCpuFrequencyMHz();
        Serial.print(F("[BLE-OnRead] CPU Frequency: "));
        Serial.println(cpuFrequency);
        pCharacteristic->setValue(cpuFrequency);
    }
    if (uuid == CHARACTERISTIC_FREE_HEAP_UUID)
    {
        //uint32_t freeHeap = ESP.getFreeHeap();
        uint32_t freeHeap = HAL::getFreeHeap();
        Serial.print(F("[BLE-OnRead] Free heap: "));
        Serial.println(freeHeap);
        pCharacteristic->setValue(freeHeap);
    }
    else if (uuid == CHARACTERISTIC_TOTAL_HEAP_UUID)
    {
        //uint32_t totalHeap = ESP.getHeapSize();
        uint32_t totalHeap = HAL::getTotalHeap();
        Serial.print(F("[BLE-OnRead] Total heap: "));
        Serial.println(totalHeap);
        pCharacteristic->setValue(totalHeap);
    }
    else if (uuid == CHARACTERISTIC_HWATER_MARK_UUID)
    {
        //UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        uint32_t stackHighWaterMark = HAL::getStackHighWaterMark();
        Serial.print(F("[BLE-OnRead] High water mark: "));
        Serial.println(stackHighWaterMark);
        pCharacteristic->setValue(stackHighWaterMark);
    }
    else if (uuid == CHARACTERISTIC_MIN_FREE_HEAP_UUID)
    {
        uint32_t minFreeHeap = HAL::getMinFreeHeap();
        Serial.print(F("[BLE-OnRead] Min free heap: "));
        Serial.println(minFreeHeap);
        pCharacteristic->setValue(minFreeHeap);
    }
    else if (uuid == CHARACTERISTIC_MAX_ALLOC_HEAP_UUID)
    {
        uint32_t maxAllocHeap = HAL::getMaxAllocHeap();
        Serial.print(F("[BLE-OnRead] Max Alloc Heap: "));
        Serial.println(maxAllocHeap);
        pCharacteristic->setValue(maxAllocHeap);
    }
    else if (uuid == CHARACTERISTIC_USED_BYTES_UUID)
    {
        fsMount2();
        //unsigned int usedBytes = getFsUsedBytes();
        uint32_t usedBytes = HAL::getFilesystemUsedBytes();
        fsUnMount2();
        Serial.print(F("[BLE-OnRead] Used bytes: "));
        Serial.println(usedBytes);
        pCharacteristic->setValue(usedBytes);
    }
    else if (uuid == CHARACTERISTIC_TOTAL_BYTES_UUID)
    {
        fsMount2();
        //unsigned int totalBytes = getFsTotalBytes();
        uint32_t totalBytes = HAL::getFilesystemTotalBytes();
        fsUnMount2();
        Serial.print(F("[BLE-OnRead] Total bytes: "));
        Serial.println(totalBytes);
        pCharacteristic->setValue(totalBytes);
    }
        else if (uuid == CHARACTERISTIC_SYSTEM_INFOS_UUID)
    {
        /*
        std::string str = systemInfosHandler.readSystemInfos();

        Serial.print(F("[BLE-onRead] systemInfos packet: "));
        Serial.println(str.c_str());
        pCharacteristic->setValue(str);
        */

        if (!systemInfosRequest.isReady())
        {
            uint32_t start = millis();

            systemInfosRequest.request();

            while (!systemInfosRequest.isReady())
            {
                if (millis() - start > 2000)
                {
                    Serial.println("Timeout BLE_SystemInfos");
                    break;
                }

                delay(1);
            }
        }
        std::string str = systemInfosRequest.getPacket();
        Serial.print(F("[BLE-onRead] systemInfos packet: "));
        Serial.println(str.c_str());
        pCharacteristic->setValue(str);
        systemInfosRequest.consume();
    }
}