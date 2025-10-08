#pragma once

extern uint8_t IR_RX_GPIO;
extern uint8_t IR_VCC_GPIO;

#include <Arduino.h>
#include "IRrecv.h"
#include "applicationInternal/packets_handler.h"

void start_infraredReceiver_HAL(void);
void shutdown_infraredReceiver_HAL(void);
void infraredReceiver_loop_HAL(void);

bool get_irReceiverEnabled_HAL();
void set_irReceiverEnabled_HAL(bool aIrReceiverEnabled);

typedef void (*tAnnounceNewIRmessage_cb)(std::string message);
void set_announceNewIRmessage_cb_HAL(tAnnounceNewIRmessage_cb pAnnounceNewIRmessage_cb);

// ---- Fonctions en test -----
void startCapture_HAL();
bool getReceiveCaptureState_HAL();

std::list<std::string> getPayloads_HAL();
std::list<std::string> getPayloads(const decode_results *const results);

void makeLastCapturePacketsHandler();
void callbackReadPacketsTimeout();
PacketsHandler &getBLELastCapturePacketsHandler_HAL();