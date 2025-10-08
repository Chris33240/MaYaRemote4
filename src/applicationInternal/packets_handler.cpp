/// @file packets_handler.cpp
/// @brief Implémentation de la classe PacketsHandler pour la gestion des paquets BLE.
/// @details Cette classe permet de découper un message en paquets (avec en-tête),
/// de reconstruire un message à partir de paquets reçus, et de gérer les timeouts associés.
#include <cmath>
#include "packets_handler.h"
#include "timeout.h"
#include "interfaces/hardwarePresenter.h"

unsigned int PacketsHandler::instanceCounter = 0;

/// @brief Constructeur de PacketsHandler.
/// Initialise les compteurs et assigne un ID unique à l’instance.
PacketsHandler::PacketsHandler() : currentPacketIndex(0), isWaitingForNextPacket2(false)
{
    instanceId = ++instanceCounter;
}

/// @brief Récupère l’identifiant unique de l’instance.
/// @return Identifiant de l’instance.
unsigned int PacketsHandler::getInstanceId() const
{
    return instanceId;
}

// ------------ make Packets ----------------
/// @brief Crée des paquets à partir d’une chaîne de caractères.
/// @param data Message à découper.
/// @param chunkSize Taille maximale d’un paquet.
void PacketsHandler::makePackets(const std::string &data, unsigned int chunkSize)
{
    std::vector<unsigned char> bytes = convertStringToByteArray(data);
    _chunkSize = chunkSize;
    unsigned int dataPacketCount = std::ceil(static_cast<double>(bytes.size()) / _chunkSize);
    packets.clear();
    reset();

    // Add header packet
    packets.push_back(createHeaderPacket(dataPacketCount, chunkSize));

    // Add data packets
    for (uint32_t time = 0; time < dataPacketCount; time++)
    {
        std::vector<unsigned char> packet(bytes.begin() + time * _chunkSize,
                                          bytes.begin() + std::min((time + 1) * _chunkSize, bytes.size()));
        packets.push_back(packet);
    }
}

// ------------ get Packets ----------------
/// @brief Récupère le prochain paquet sous forme de chaîne.
/// @return Paquet suivant sous forme de std::string.
std::string PacketsHandler::getPacket()
{
    std::string str;
    const static std::string timeoutKey = "GET_PACKET_" + std::to_string(instanceId);
    if (!isWaitingForNextPacket2)
    {
        // This is the first READ
    }

    if (!isWaitingForNextPacket2 && hasMorePackets())
    {
        // This is the first READ and PacketReader is ready
        isWaitingForNextPacket2 = true;
        // std::string timeoutKey = "GET_PACKET_" + std::to_string(instanceId);
        //  Serial.printf("[DEBUG] Add timeout: %s\r\n", timeoutKey.c_str());
        TimeoutManager2::addTimeout(timeoutKey, TIMEOUT_DURATION, [this]()
                                    { callbackReadPacketTimeout(); });
    }

    // has the message is too long for BLE, we split message in multiple Packets.
    // Each packet must be retrived by client with READ command before timeout.
    if (hasMorePackets())
    {
        // unsigned long currentTime = millis();
        // std::string timeoutKey = "GET_PACKET_" + std::to_string(instanceId);
        TimeoutManager2::setStartTime(timeoutKey, millis());
        str = getNextString();
        return str;
    }

    if (!hasMorePackets())
    {
        // No More packet
        reset();
        // isWaitingForNextPacket2 = false;
        // std::string timeoutKey = "GET_PACKET_" + std::to_string(instanceId);
        // Serial.printf("[DEBUG] Remove timeout on no more packets: %s\r\n", timeoutKey.c_str());
        //  Ne pas déplacer le removeTimeout dans le reset car ce dernier est appellé par le callbackReadPacketTimeout,
        //  hors on  ne peux pas supprimer un Timeout dans que son callback est en cours d'exécution.
        TimeoutManager2::removeTimeout(timeoutKey);
    }
    return str;
}

/// @brief Vérifie s’il reste encore des paquets à traiter.
/// @return true si des paquets restent disponibles, false sinon.
bool PacketsHandler::hasMorePackets()
{
    return currentPacketIndex < packets.size();
}

/// @brief Convertit une chaîne en tableau d’octets.
/// @param str Chaîne d’entrée.
/// @return Vecteur d’octets correspondant.
std::vector<unsigned char> PacketsHandler::convertStringToByteArray(const std::string &str)
{
    return std::vector<unsigned char>(str.begin(), str.end());
}

/// @brief Convertit un tableau d’octets en chaîne.
/// @param bytes Vecteur d’octets en entrée.
/// @return Chaîne résultante.
std::string PacketsHandler::byteArrayToString(const std::vector<unsigned char> &bytes)
{
    return std::string(bytes.begin(), bytes.end());
}

/// @brief Crée un paquet d’en-tête contenant le nombre et la taille des paquets.
/// @param packetCount Nombre total de paquets.
/// @param chunkSize Taille maximale d’un paquet.
/// @return Paquet d’en-tête sous forme de vecteur d’octets.
std::vector<unsigned char> PacketsHandler::createHeaderPacket(unsigned int packetCount, unsigned int chunkSize)
{
    //std::string header = "HEADER:" + std::to_string(packetCount);
    std::string header = "HEADER:" + std::to_string(packetCount) + ";" + std::to_string(chunkSize);
    return convertStringToByteArray(header);
}

/// @brief Récupère le prochain paquet converti en chaîne.
/// @return Paquet converti en std::string.
std::string PacketsHandler::getNextString()
{
    return byteArrayToString(getNextPacket());
}

/// @brief Récupère le prochain paquet brut.
/// @return Vecteur d’octets correspondant au paquet suivant.
std::vector<unsigned char> PacketsHandler::getNextPacket()
{
    std::vector<unsigned char> packet;
    if (currentPacketIndex < packets.size())
    {
        packet = packets[currentPacketIndex];
        currentPacketIndex++;
    }
    else
    {
        // No more packet
    }
    return packet;
}

/// @brief Vérifie si un paquet est un en-tête.
/// @param packet Paquet à vérifier.
/// @return true si le paquet est un en-tête, false sinon.
bool PacketsHandler::isHeaderPacket(const std::vector<unsigned char> &packet)
{
    std::string packetStr = byteArrayToString(packet);
    return isHeaderPacket(packetStr);
}

/// @brief Vérifie si un paquet est un en-tête.
/// @param packetStr Paquet à vérifier.
/// @return true si le paquet est un en-tête, false sinon.
bool PacketsHandler::isHeaderPacket(const std::string &packetStr)
{
    return packetStr.substr(0, 7) == "HEADER:";
}

HeaderData PacketsHandler::getHeaderData(const std::string& headerStr)
{
    HeaderData headerData;
    headerData.packetsCount = getPacketCountFromHeader2(headerStr);
    headerData.chunkSize = getChunkSizeFromHeader(headerStr);
    return headerData;
}

unsigned int PacketsHandler::getPacketCountFromHeader2(const std::string& headerStr) {
    // suppose format correct et HEADER: présent
    size_t colonPos = headerStr.find(':');
    size_t semicolonPos = headerStr.find(';');
    return std::stoi(headerStr.substr(colonPos + 1, semicolonPos - colonPos - 1));
}

unsigned int PacketsHandler::getChunkSizeFromHeader(const std::string& headerStr) {
    size_t semicolonPos = headerStr.find(';');
    return std::stoi(headerStr.substr(semicolonPos + 1));
}

/*
/// @brief Extrait le nombre de paquets attendu depuis un paquet d’en-tête.
/// @param headerPacket Paquet d’en-tête.
/// @return Nombre total de paquets attendus.
uint16_t PacketsHandler::getPacketCountFromHeader(const std::vector<unsigned char> &headerPacket)
{
    std::string headerStr = byteArrayToString(headerPacket);
    uint16_t colonPos = headerStr.find(':');
    if (colonPos != std::string::npos)
    {
        return std::stoul(headerStr.substr(colonPos + 1));
    }
    return 0;
}
*/

/// @brief Réinitialise l’état interne du gestionnaire de paquets (lecture).
void PacketsHandler::reset()
{
    isWaitingForNextPacket2 = false;
    currentPacketIndex = 0;
}

/// @brief Callback exécuté en cas de timeout lors de la lecture des paquets.
void PacketsHandler::callbackReadPacketTimeout()
{
    if (onTimeoutCallback)
    {
        onTimeoutCallback();
    }
    reset();
}

/// @brief Définit une fonction callback à exécuter en cas de timeout.
/// @param callback Fonction à exécuter.
void PacketsHandler::setOnTimeoutCallback(std::function<void()> callback)
{
    onTimeoutCallback = std::move(callback);
}

// ------------ set Packets ----------------
// This function is performed from the Bluetooth service which uses a lot of RAM memory (stack).
// Please, avoid large local variable allocation inside this function.

/// @brief Ajoute un paquet reçu et reconstruit le message si complet.
/// @param texte Chaîne représentant un paquet reçu.
void PacketsHandler::setPacket(const std::string &texte)
{
    const static std::string timeoutKey = "SET_PACKET_" + std::to_string(instanceId);
    if (!isWaitingForPackets)
    {
        // Premier paquet, vérifier s'il s'agit de l'en-tête
        //if (texte.substr(0, 7) == "HEADER:")
        if (isHeaderPacket(texte))
        {
            //expectedPacketCount = std::stoul(texte.substr(7));
            headerData = getHeaderData(texte);
            // Serial.printf("Expected packets count: '%u'\r\n", expectedPacketCount);
            isWaitingForPackets = true;
            receivedPacketCount = 0;
            reconstructedMessage.clear();
            // std::string timeoutKey = "SET_PACKET_" + std::to_string(instanceId);
            // Serial.printf("[DEBUG] Add timeout: %s\r\n", timeoutKey.c_str());
            TimeoutManager2::addTimeout(timeoutKey, TIMEOUT_DURATION, [this]()
                                        { callbackReadPacketTimeout2(); });
            return;
        }
        else
        {
            // Erreur : le premier paquet devrait être l'en-tête
            reset2();
            return;
        }
    }
    else
    {
        TimeoutManager2::setStartTime(timeoutKey, millis());
    };

    // Ajouter le contenu du paquet au message reconstruit
    reconstructedMessage += texte;
    receivedPacketCount++;
    // Serial.printf("Received packets count: '%u'\r\n", receivedPacketCount);

    // Vérifier si tous les paquets ont été reçus
    //if (receivedPacketCount == expectedPacketCount)
    if (receivedPacketCount == headerData.packetsCount)
    {
        if (onMessageCompleteCallback)
        {
            onMessageCompleteCallback(reconstructedMessage);
        }
        reset2();
    }
}

/// @brief Définit une fonction callback à exécuter quand le message complet est reconstruit.
/// @param callback Fonction prenant en paramètre le message complet.
void PacketsHandler::setOnMessageCompleteCallback(std::function<void(const std::string &)> callback)
{
    onMessageCompleteCallback = std::move(callback);
}

/// @brief Callback exécuté en cas de timeout lors de la reconstruction du message.
void PacketsHandler::callbackReadPacketTimeout2()
{
    // Serial.println("[DEBUG] Timeout");
    if (onTimeoutCallback)
    {
        onTimeoutCallback();
    }
    reset2();
}

/// @brief Réinitialise l’état interne du gestionnaire de paquets (réception).
void PacketsHandler::reset2()
{
    isWaitingForPackets = false;
    receivedPacketCount = 0;
    //expectedPacketCount = 0;
    // Aprés une erreur, un timeout ou la reception de tous les paquets, nous n'avons plus besoin de HeaderData
    headerData = HeaderData{}; // (Ex: packetsCount = 0)
    reconstructedMessage.clear();
}
