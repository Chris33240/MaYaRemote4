#ifndef PACKETSHANDLER_H
#define PACKETSHANDLER_H

#include <Arduino.h>
#include <vector>
#include <string>
#include <setup.h>

struct HeaderData
{
  unsigned int packetsCount;
  unsigned int chunkSize;
};

class PacketsHandler
{
public:
    PacketsHandler();
    // ------------ make Packets ----------------
    void makePackets(const std::string &data, unsigned int chunkSize = DEFAULT_CHUNK_SIZE);

    // ------------ get Packets ----------------
    std::string getPacket();
    bool hasMorePackets();
    // Callbacks: Système d'abonnement aux événements de timeout
    void setOnTimeoutCallback(std::function<void()> callback);


    // ------------ set Packets ----------------
    void setPacket(const std::string &Texte);
    // Callbacks: Système d'abonnement aux événements de message completé
    void setOnMessageCompleteCallback(std::function<void(const std::string &)> callback);

private:
    std::vector<std::vector<unsigned char>> packets;

    // ------------ get Packets ----------------
    //size_t instanceId;
    unsigned int instanceId;
    //static size_t instanceCounter;
    static unsigned int instanceCounter;
    unsigned int currentPacketIndex;
    bool isWaitingForNextPacket2;
    unsigned int _chunkSize;
    const unsigned long TIMEOUT_DURATION = 5000; // timeout en milliseconds
    //size_t getInstanceId() const;
    unsigned int getInstanceId() const;
    std::vector<unsigned char> convertStringToByteArray(const std::string &str);
    std::string byteArrayToString(const std::vector<unsigned char> &bytes);
    std::vector<unsigned char> createHeaderPacket(unsigned int packetCount, unsigned int chunkSize);

    std::string getNextString();
    std::vector<unsigned char> getNextPacket();
    bool isHeaderPacket(const std::vector<unsigned char> &packet);
    bool isHeaderPacket(const std::string &packetStr);
    HeaderData getHeaderData(const std::string& headerStr);
    unsigned int getPacketCountFromHeader2(const std::string& headerStr);
    unsigned int getChunkSizeFromHeader(const std::string& headerStr);
    //[[deprecated("Don't use this routine any more. Use the new one instead. 'getPacketCountFromHeader2()'")]]
    //uint16_t getPacketCountFromHeader(const std::vector<unsigned char> &headerPacket);
    void reset();

    // Callbacks
    std::function<void()> onTimeoutCallback;
    void callbackReadPacketTimeout();


    // ------------ set Packets ----------------
    bool isWaitingForPackets;
    std::string reconstructedMessage;
    //unsigned int expectedPacketCount;
    HeaderData headerData;
    unsigned int receivedPacketCount;

    // Callbacks
    std::function<void(const std::string &)> onMessageCompleteCallback;
    void callbackReadPacketTimeout2();
    void reset2();
};

#endif