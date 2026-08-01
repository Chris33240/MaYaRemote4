#ifndef BLELISTCOMMANDSMANAGER_H
#define BLELISTCOMMANDSMANAGER_H
#include <string>

extern std::string currentPacket;
extern volatile bool packetReady;
extern volatile bool requestNewPacket;

void processListCommandsLoop();

#endif // BLELISTCOMMANDSMANAGER_H