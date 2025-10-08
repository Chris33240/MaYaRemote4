/// @file DigitalReadDelay.cpp
/// @brief Implémentation d'une lecture digitale avec délai.
/// @details Fournit une fonction utilitaire pour détecter un appui prolongé
/// sur un bouton en appliquant un délai avant de déclencher un callback.
#include "DigitalReadDelay.h"

/// @brief Lit l’état d’un bouton avec un délai avant exécution d’un callback.
/// @param pin Numéro de la broche à lire.
/// @param delayMs Durée en millisecondes que le bouton doit rester appuyé avant déclenchement.
/// @param callback Fonction à exécuter lorsque la condition est remplie.
/// @details 
/// - Si le bouton est pressé (LOW détecté), le compteur démarre.
/// - Si le bouton reste pressé pendant au moins `delayMs`, le callback est appelé.
/// - Évite les répétitions indésirables en réinitialisant le compteur après chaque déclenchement.
void digitalReadDelay(uint8_t pin, unsigned long delayMs, void (*callback)()) {
  static unsigned long pressStartTime = 0;
  static bool lastButtonPressed = false;

  // Lire l'état du bouton
  bool isButtonPressed = (digitalRead(pin) == LOW);

  if (isButtonPressed && !lastButtonPressed) {
    // Le bouton vient d'être pressé
    pressStartTime = millis(); // Enregistrer le temps actuel
  }

  if (isButtonPressed && (millis() - pressStartTime >= delayMs)) {
    // Vérifier si le délai est écoulé
    if (callback) callback(); // Appeler le callback
    pressStartTime = millis(); // Réinitialiser pour éviter des répétitions non désirées
  }

  lastButtonPressed = isButtonPressed; // Mettre à jour l'état précédent
}
