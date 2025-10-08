/// @file led_blinker.cpp
/// @brief Contrôle du clignotement d'une LED avec durée et intervalle personnalisable.
#include "led_blinker.h"

/// @brief Constructeur de LedBlinker.
/// @param pin Pin de la LED.
/// @param on Durée d'allumage en millisecondes.
/// @param off Durée d'extinction en millisecondes.
LedBlinker::LedBlinker(int pin, unsigned long on, unsigned long off)
    : ledPin(pin), onTime(on), offTime(off), previousMillis(0), initialLedState(LOW), ledState(LOW),
        isBlinking(false), isDurationMode(false), blinkDuration(0), blinkStartTime(0) {
    pinMode(ledPin, OUTPUT);
}

/// @brief Initialise la LED et réinitialise son état.
/// @param initialLedState État initial de la LED (HIGH ou LOW).
void LedBlinker::begin(bool initialLedState) {
    reset();
    digitalWrite(ledPin, ledState);
}

/// @brief Réinitialise les variables internes de la LED.
void LedBlinker::reset() {
    ledState = initialLedState;
    previousMillis = 0;
}

/// @brief Met à jour l'état de la LED en fonction du temps écoulé.
void LedBlinker::update() {
    if (!isBlinking) return;
    // TODO: Exécuter les traiments longs dans un thread secondaire pour eviter le blocage de l'update de LedBlinker.
    //Serial.println("[DEBUG] Update LedBlinker");
    unsigned long currentMillis = millis();

    if (isDurationMode && (currentMillis - blinkStartTime >= blinkDuration)) {
        stopBlinking(true);
        return;
    }

    if ((ledState == HIGH) && (currentMillis - previousMillis >= onTime)) {
        ledState = LOW;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
    } else if ((ledState == LOW) && (currentMillis - previousMillis >= offTime)) {
        ledState = HIGH;
        previousMillis = currentMillis;
        digitalWrite(ledPin, ledState);
    }
}

/// @brief Démarre le clignotement de la LED avec un intervalle donné.
/// @param time Durée de chaque phase (allumée/éteinte) en millisecondes.
void LedBlinker::startBlinking(unsigned long time) {
    reset();
    setBlinkTimes(time, time);
    isBlinking = true;
    isDurationMode = false;
}

/// @brief Démarre le clignotement de la LED pour une durée totale spécifiée.
/// @param time Durée de chaque phase (allumée/éteinte) en millisecondes.
/// @param duration Durée totale du clignotement en millisecondes.
void LedBlinker::startBlinkingForDuration(unsigned long time, unsigned long duration) {
    reset();
    setBlinkTimes(time, time);
    isBlinking = true;
    isDurationMode = true;
    blinkDuration = duration;
    blinkStartTime = millis();
}

/// @brief Arrête le clignotement de la LED.
/// @param stopDurationMode Indique si le mode durée doit également être arrêté.
void LedBlinker::stopBlinking(bool stopDurationMode) {
    if(!isDurationMode | stopDurationMode) {
        isBlinking = false;
        isDurationMode = false;
        digitalWrite(ledPin, initialLedState);
    }
}

/// @brief Définit les durées d'allumage et d'extinction de la LED.
/// @param on Temps d'allumage en millisecondes.
/// @param off Temps d'extinction en millisecondes.
void LedBlinker::setBlinkTimes(unsigned long on, unsigned long off) {
    onTime = on;
    offTime = off;
}

/// @brief Vérifie si la LED est actuellement allumée.
/// @return true si la LED est allumée, false sinon.
bool LedBlinker::isLedOn() const {
    return ledState == HIGH;
}