/// @file timeout.cpp
/// @brief Gestionnaire de timeouts avec callbacks.
/// @details Permet d’ajouter, supprimer, vérifier et exécuter des timeouts asynchrones.
/// Chaque timeout est identifié par une clé unique (std::string).
/// Un mutex est utilisé pour protéger l’accès concurrent à la map `timeouts` et sa fonction callback.
#include "timeout.h"

/// @brief Structure représentant un timeout.
/// @details Contient le temps de départ, la durée et la fonction callback associée.
struct Timeout
{
    unsigned long startTime;
    unsigned long duration;
    std::function<void()> callback;
};

/// @brief Conteneur global des timeouts actifs.
std::map<std::string, Timeout> timeouts;
/// @brief Mutex protégeant l’accès concurrent à `timeouts` et sa fonction callback.
std::mutex mtx;

/// @brief Ajoute un nouveau timeout avec callback.
/// @param name Nom unique du timeout.
/// @param duration Durée du timeout en millisecondes.
/// @param callback Fonction à appeler lorsque le timeout expire.
void TimeoutManager2::addTimeout(const std::string &name, unsigned long duration, std::function<void()> callback)
{
    std::lock_guard<std::mutex> lock(mtx);
    timeouts[name] = {millis(), duration, std::move(callback)};
}

/// @brief Vérifie si un timeout est expiré.
/// @param name Nom du timeout.
/// @return `true` si le timeout est expiré, `false` sinon.
bool TimeoutManager2::isExpired(const std::string &name)
{
    auto it = timeouts.find(name);
    if (it != timeouts.end())
    {
        return (millis() - it->second.startTime) >= it->second.duration;
    }
    return false;
}

/// @brief Supprime un timeout.
/// @param name Nom du timeout.
void TimeoutManager2::removeTimeout(const std::string &name)
{
    timeouts.erase(name);
}

/// @brief Définit le temps de départ d’un timeout.
/// @param name Nom du timeout.
/// @param value Valeur du nouveau temps de départ (en millisecondes).
void TimeoutManager2::setStartTime(const std::string &name, unsigned long value)
{
    if (timeouts.find(name) != timeouts.end())
    {
        timeouts[name].startTime = value;
    }
}

/// @brief Met à jour les timeouts.
/// @details Supprime les expirés et exécute leur callback si défini.
void TimeoutManager2::update()
{
    // Note : Parfois durant un update la fonction callback n'existe plus,
    // les verrous entre les fonctions d'ajout et d'update permettent de proteger l'intégrité de l'objet Timeout.
    std::lock_guard<std::mutex> lock(mtx); // Verrouille le mutex pour protéger `timeouts`
    auto it = timeouts.begin();
    while (it != timeouts.end())
    {
        if (isExpired(it->first))
        {
            if (it->second.callback)
            {
                it->second.callback();
            }
            else
            {
                Serial.printf("[ERROR] Callback for timout %s does not exist.\r\n", it->first.c_str());
            }
            it = timeouts.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/// @brief Vérifie si un timeout existe.
/// @param name Nom du timeout.
/// @return `true` si le timeout existe, `false` sinon.
bool TimeoutManager2::exists(const std::string &name)
{
    return timeouts.find(name) != timeouts.end();
}

/// @brief Retourne le temps restant avant expiration d’un timeout.
/// @param name Nom du timeout.
/// @return Temps restant en millisecondes, ou `-1` si le timeout n’existe pas ou est déjà expiré.
long TimeoutManager2::getRemainingTime(const std::string &name)
{
    auto it = timeouts.find(name);
    if (it != timeouts.end())
    {
        unsigned long elapsed = millis() - it->second.startTime;
        if (elapsed < it->second.duration)
        {
            return it->second.duration - elapsed;
        }
    }
    return -1;
}