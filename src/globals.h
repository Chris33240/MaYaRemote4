#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "utils/led_blinker.h"
#include "PinDefinitionsAndMore.h"

// Variables globales
// extern int maVariableGlobale = 42;
// extern const char* maConstanteGlobale;

// Déclaration globale de l'objet LED
extern LedBlinker *pGlobalStatusLED;

// inline bool enableMemoryReduction = true;
extern bool enableMemoryReduction;

#endif // CONFIG_H