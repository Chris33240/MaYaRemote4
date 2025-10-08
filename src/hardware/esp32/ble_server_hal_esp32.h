#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h> // Permet la notification UUID 0x2902.
#include <list>

#define BLE_DEVICE_NAME "MaYa_Remote_BLE" // BLE server name

void init_ble_server_HAL();
void addDescriptor(BLECharacteristic *pCharacteristic, const char *uuid, const char *valeur);
void start_ble_server_HAL();
void stop_ble_server_HAL();
void start_ble_advertising_HAL(BLEAdvertising *advertising);
void stop_ble_advertising_HAL(BLEAdvertising *advertising);
void start_ble_service_HAL(BLEService *service);
void stop_ble_service_HAL(BLEService *service);
void ble_server_loop_HAL();
bool isDeviceConnected_HAL();

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pMyServer);
    void onDisconnect(BLEServer *pMyServer);
};

void sendBleNotifyCode_HAL(const std::string &code);
void sendBleNotifyNewCommandCaptured_HAL(std::string command);
void sendBleNotify_HAL(std::string message);
void sendBleNotify_HAL(BLECharacteristic *pCharacteristic, const std::string &message);