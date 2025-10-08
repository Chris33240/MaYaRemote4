#include <BLECharacteristic.h>

class MyCallbacksSystemInfos : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic);
};