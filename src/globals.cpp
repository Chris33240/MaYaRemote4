/// @file globals.cpp
/// @brief Définition et initialisation des variables globales.
/// @details Ce fichier contient l'implémentation des variables globales
/// utilisées dans le projet, notamment pour la gestion de la mémoire.
#include "globals.h"
#include "setup.h"

/// @brief Active ou désactive la réduction de mémoire.
/// @details Sa valeur initiale est définie par la macro `ENABLED_MEMORY_REDUCTION`.
bool enableMemoryReduction = ENABLED_MEMORY_REDUCTION;