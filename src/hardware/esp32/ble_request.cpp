#include "ble_request.h"

void BleRequest::request()
{
    requestPending = true;
}

bool BleRequest::isRequested() const
{
    return requestPending;
}

void BleRequest::setPacket(const std::string &newPacket)
{
    currentPacket = newPacket;
    packetReady = true;
    requestPending = false;
}

bool BleRequest::isReady() const
{
    return packetReady;
}

const std::string &BleRequest::getPacket() const
{
    return currentPacket;
}

void BleRequest::consume()
{
    packetReady = false;
}

void BleRequest::reset()
{
    requestPending = false;
    packetReady = false;
    currentPacket.clear();
}