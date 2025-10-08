#include <BLECharacteristic.h>

class MyCallbacks_UART : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic);
    void onWrite(BLECharacteristic *pCharacteristic);
};