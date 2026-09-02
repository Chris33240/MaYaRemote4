#ifndef BLELISTCOMMANDSLOOP_H
#define BLELISTCOMMANDSLOOP_H
#include <string>
#include "ble_request.h"

extern BleRequest listCommandsRequest;
extern BleRequest listCommandsDataRequest;

void processListCommandsLoop();
void processListCommandsDataLoop();

#endif // BLELISTCOMMANDSLOOP_H