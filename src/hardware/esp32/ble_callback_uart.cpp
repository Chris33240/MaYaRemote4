/// @file ble_callbacks_uart.cpp
/// @brief Callbacks pour la caractéristique UART du serveur BLE ESP32.

#include "ble_callback_uart.h"
#include <HardwareSerial.h>

// ------------------- Callbacks UART ---------------------------
/// @brief Callback appelé lorsqu'un client effectue une lecture sur la caractéristique UART
/// @param pCharacteristic Pointeur vers la caractéristique lue
void MyCallbacks_UART::onRead(BLECharacteristic *pCharacteristic)
{
  std::string message = "UART message";
  pCharacteristic->setValue(message);
  Serial.println(("[BLE-onRead] " + message).c_str());
}

/// @brief Callback appelé lorsqu'un client écrit sur la caractéristique UART
/// @param pCharacteristic Pointeur vers la caractéristique écrite
void MyCallbacks_UART::onWrite(BLECharacteristic *pCharacteristic)
{
  std::string message = pCharacteristic->getValue();
  if (message.length() > 0)
  {
    Serial.println(("[BLE-onWrite] " + message).c_str());
  }
}