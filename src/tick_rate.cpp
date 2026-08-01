#include "tick_rate.h"

/// @brief Dernier temps (en millisecondes) utilisé pour calculer le taux de ticks.
unsigned long lastTickTime = 0;
/// @brief Compteur de ticks depuis la dernière mise à jour.
unsigned long tickCount = 0;
/// @brief Taux de ticks calculé (en Hz, ticks par seconde).
float tickRate = 0.0f;
/// @brief Intervalle de mise à jour du taux (en millisecondes).
const unsigned long UPDATE_INTERVAL = 1000; // Intervalle de mise à jour en millisecondes

/// @brief Met à jour le taux de ticks.
///
/// Cette fonction :
/// - Incrémente le compteur de ticks
/// - Vérifie si l’intervalle d’une seconde est écoulé
/// - Calcule le taux de ticks par seconde
/// - Réinitialise le compteur
///
/// @note Doit être appelée régulièrement (par exemple dans `loop()`).
void updateTickRate()
{
    tickCount++;

    // Vérifie si l'intervalle de mise à jour est écoulé
    unsigned long currentTime = millis();
    if (currentTime - lastTickTime >= UPDATE_INTERVAL)
    {
        // Calcule le taux de ticks par seconde
        tickRate = (float)tickCount * 1000.0 / (float)(currentTime - lastTickTime);
        // Serial.println(tickCount);
        tickCount = 0;
        lastTickTime = currentTime;
    }
}

/// @brief Retourne le taux de ticks calculé.
/// @return Taux de ticks par seconde (Hz).
float getTickRate()
{
    return tickRate;
}