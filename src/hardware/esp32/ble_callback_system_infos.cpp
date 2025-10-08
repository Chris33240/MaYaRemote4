/// @file ble_callback_system_infos.cpp
/// @brief Callbacks pour les caractéristiques System Infos du serveur BLE ESP32.
#include "ble_callback_system_infos.h"
#include <HardwareSerial.h>
#include <Esp.h>
#include "ble_uuid.h"
#include "interfaces/hardwarePresenter.h"

// ------------------- Callbacks System Infos ---------------------------
/// @brief Callback appelé lorsqu'un client effectue une lecture sur une caractéristique System Infos
/// @param pCharacteristic Pointeur vers la caractéristique lue
void MyCallbacksSystemInfos::onRead(BLECharacteristic *pCharacteristic)
{

    // Obtenez l'UUID de la caractéristique
    const std::string &uuid = pCharacteristic->getUUID().toString();
    Serial.print(F("[BLE-onRead] BLE Characteristic uuid: "));
    Serial.println(uuid.c_str());

    if (uuid == CHARACTERISTIC_FREE_HEAP_UUID)
    {
        uint32_t freeHeap = ESP.getFreeHeap();
        Serial.print(F("[BLE-OnRead] Free heap: "));
        Serial.println(freeHeap);
        pCharacteristic->setValue(freeHeap);
    }
    else if (uuid == CHARACTERISTIC_HWATER_MARK_UUID)
    {
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        Serial.print(F("[BLE-OnRead] High water mark: "));
        Serial.println(stackHighWaterMark);
        pCharacteristic->setValue(stackHighWaterMark);
    }
    else if (uuid == CHARACTERISTIC_USED_BYTES_UUID)
    {
        fsMount2();
        unsigned int usedBytes = getFsUsedBytes();
        fsUnMount2();
        Serial.print(F("[BLE-OnRead] Used bytes: "));
        Serial.println(usedBytes);
        pCharacteristic->setValue(usedBytes);
    }
    else if (uuid == CHARACTERISTIC_TOTAL_BYTES_UUID)
    {
        fsMount2();
        unsigned int totalBytes = getFsTotalBytes();
        fsUnMount2();
        Serial.print(F("[BLE-OnRead] Total bytes: "));
        Serial.println(totalBytes);
        pCharacteristic->setValue(totalBytes);
    }
}