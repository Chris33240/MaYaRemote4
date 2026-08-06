#pragma once

#include <string>

/// @brief Déporte les traitements coûteux des callbacks BLE vers la boucle principale.
///
/// Réduit l'utilisation de la pile du thread BLE (BTC_TASK) et évite les
/// stack overflows lors des traitements lourds (JSON, système de fichiers, etc.).
class BleRequest
{
public:

    /// @brief Demande la génération d'un nouveau paquet.
    void request();

    /// @brief Indique qu'un nouveau paquet est demandé.
    bool isRequested() const;

    /// @brief Stocke le paquet généré.
    /// @param newPacket Paquet à rendre disponible.
    void setPacket(const std::string &newPacket);

    /// @brief Indique si un paquet est disponible.
    bool isReady() const;

    /// @brief Retourne le paquet courant.
    const std::string &getPacket() const;

    /// @brief Indique que le paquet a été consommé.
    void consume();

    /// @brief Réinitialise complètement l'état.
    void reset();

private:

    std::string currentPacket;

    volatile bool requestPending = false;
    volatile bool packetReady = false;
};