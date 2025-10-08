#ifndef LEDBLINKER_H
#define LEDBLINKER_H

#include <Arduino.h>

class LedBlinker {
private:
    int ledPin;
    unsigned long onTime;
    unsigned long offTime;
    unsigned long previousMillis;
    bool initialLedState;
    bool ledState;
    bool isBlinking;
    bool isDurationMode;
    unsigned long blinkDuration;
    unsigned long blinkStartTime;

public:
    LedBlinker(int pin, unsigned long on = 500, unsigned long off = 500);
    void begin(bool initialLedState = LOW);
    void reset();
    void update();
    void startBlinking(unsigned long time);
    void startBlinkingForDuration(unsigned long time, unsigned long duration);
    void stopBlinking(bool stopDurationMode);
    void setBlinkTimes(unsigned long on, unsigned long off);
    bool isLedOn() const;
};

#endif