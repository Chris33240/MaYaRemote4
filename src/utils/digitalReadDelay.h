#ifndef DIGITAL_READ_DELAY_H
#define DIGITAL_READ_DELAY_H

#include <Arduino.h>

// Déclaration de la fonction
void digitalReadDelay(uint8_t pin, unsigned long delayMs, void (*callback)());

#endif // DIGITAL_READ_DELAY_H