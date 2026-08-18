/*
  Mainly based on IRrecvDumpV2 from IRremoteESP8266
  Slightly changed to run with OMOTE hardware.
*/

/*
 * IRremoteESP8266: IRrecvDumpV2 - dump details of IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input kRecvPin.
 *
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 * Copyright 2017-2019 David Conran
 *
 * Example circuit diagram:
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 *
 * Changes:
 *   Version 1.2 October, 2020
 *     - Enable easy setting of the decoding tolerance value.
 *   Version 1.0 October, 2019
 *     - Internationalisation (i18n) support.
 *     - Stop displaying the legacy raw timing info.
 *   Version 0.5 June, 2019
 *     - Move A/C description to IRac.cpp.
 *   Version 0.4 July, 2018
 *     - Minor improvements and more A/C unit support.
 *   Version 0.3 November, 2017
 *     - Support for A/C decoding for some protocols.
 *   Version 0.2 April, 2017
 *     - Decode from a copy of the data so we can start capturing faster thus
 *       reduce the likelihood of miscaptures.
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 */

// #include <Arduino.h>
// #include <string>
#include <list>
// #include <assert.h>
// #include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
// #include <IRtext.h>
// #include <IRutils.h>

#include "infrared_receiver_hal_esp32.h"
#include "infrared_utils.h"
#include "PinDefinitionsAndMore.h"
#include "applicationInternal/message_data.h"
#include "interfaces/hardwarePresenter.h"
#include "helpers/helpers.h"
#include "helpers/omote_log.h"

// uint8_t IR_RX_GPIO  = IR_RECEIVE_PIN; // IR receiver input
// uint8_t IR_VCC_GPIO = IR_RECEIVER_POWER; // IR receiver power

bool irReceiverEnabled = false;
bool isCodeReceived = false;

tAnnounceNewIRmessage_cb thisAnnounceNewIRmessage_cb = NULL;
void set_announceNewIRmessage_cb_HAL(tAnnounceNewIRmessage_cb pAnnounceNewIRmessage_cb)
{
    thisAnnounceNewIRmessage_cb = pAnnounceNewIRmessage_cb;
}

// decode_results getResults() {
//   return results;
// }

// The Serial connection baud rate.
// i.e. Status message will be sent to the PC at this baud rate.
// Try to avoid slow speeds like 9600, as you will miss messages and
// cause other problems. 115200 (or faster) is recommended.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/decoder, let us use a larger
// than normal buffer so we can handle Air Conditioner remote codes.
const uint16_t kCaptureBufferSize = 1024;

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
// This parameter is an interesting trade-off. The longer the timeout, the more
// complex a message it can capture. e.g. Some device protocols will send
// multiple message packets in quick succession, like Air Conditioner remotes.
// Air Coniditioner protocols often have a considerable gap (20-40+ms) between
// packets.
// The downside of a large timeout value is a lot of less complex protocols
// send multiple messages when the remote's button is held down. The gap between
// them is often also around 20+ms. This can result in the raw data be 2-3+
// times larger than needed as it has captured 2-3+ messages in a single
// capture. Setting a low timeout value can resolve this.
// So, choosing the best kTimeout value for your use particular case is
// quite nuanced. Good luck and happy hunting.
// NOTE: Don't exceed kMaxTimeoutMs. Typically 130ms.
#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else  // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif // DECODE_AC
// Alternatives:
// const uint8_t kTimeout = 90;
// Suits messages with big gaps like XMP-1 & some aircon units, but can
// accidentally swallow repeated messages in the rawData[] output.
//
// const uint8_t kTimeout = kMaxTimeoutMs;
// This will set it to our currently allowed maximum.
// Values this high are problematic because it is roughly the typical boundary
// where most messages repeat.
// e.g. It will stop decoding a message and start sending it to serial at
//      precisely the time when the next message is likely to be transmitted,
//      and may miss it.

// Set the smallest sized "UNKNOWN" message packets we actually care about.
// This value helps reduce the false-positive detection rate of IR background
// noise as real messages. The chances of background IR noise getting detected
// as a message increases with the length of the kTimeout value. (See above)
// The downside of setting this message too large is you can miss some valid
// short messages for protocols that this library doesn't yet decode.
//
// Set higher if you get lots of random short UNKNOWN messages when nothing
// should be sending a message.
// Set lower if you are sure your setup is working, but it doesn't see messages
// from your device. (e.g. Other IR remotes work.)
// NOTE: Set this value very high to effectively turn off UNKNOWN detection.
const uint16_t kMinUnknownSize = 12;

// How much percentage lee way do we give to incoming signals in order to match
// it?
// e.g. +/- 25% (default) to an expected value of 500 would mean matching a
//      value between 375 & 625 inclusive.
// Note: Default is 25(%). Going to a value >= 50(%) will cause some protocols
//       to no longer match correctly. In normal situations you probably do not
//       need to adjust this value. Typically that's when the library detects
//       your remote's message some of the time, but not all of the time.
const uint8_t kTolerancePercentage = kTolerance; // kTolerance is normally 25%

// Legacy (No longer supported!)
//
// Change to `true` if you miss/need the old "Raw Timing[]" display.
#define LEGACY_TIMING_INFO false
// ==================== end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(GPIO_IR_RECEIVER, kCaptureBufferSize, kTimeout, true);
decode_results results; // Somewhere to store the results
// std::list<std::string> payloads; // Store payloads from results;
// PacketWriter bleLastCapturePacketsWriter;
PacketsHandler bleLastCapturePacketsHandler;

// This section of code runs only once at start-up.
void start_infraredReceiver_HAL()
{
    pinMode(GPIO_IR_RECEIVER, INPUT);
    pinMode(GPIO_POWER, OUTPUT);
    digitalWrite(GPIO_POWER, HIGH); // Turn on IR receiver
    Serial.print(F("IR Receiver Power Turn ON on pin : "));
    Serial.println(GPIO_POWER);
    Serial.print(F("Ready to receive IR signals on pin : "));
    Serial.println(GPIO_IR_RECEIVER);
    // Perform a low level sanity checks that the compiler performs bit field
    // packing as we expect and Endianness is as we expect.
    assert(irutils::lowLevelSanityCheck() == 0);

    Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", GPIO_IR_RECEIVER);
#if DECODE_HASH
    // Ignore messages with less than minimum on or off pulses.
    irrecv.setUnknownThreshold(kMinUnknownSize);
#endif                                         // DECODE_HASH
    irrecv.setTolerance(kTolerancePercentage); // Override the default tolerance.
    irrecv.enableIRIn();                       // Start the receiver

    bleLastCapturePacketsHandler.setOnTimeoutCallback([]()
                                                      {
                                                        callbackReadPacketsTimeout();
                                                      });
}

void shutdown_infraredReceiver_HAL()
{
    irrecv.disableIRIn();
    digitalWrite(GPIO_POWER, LOW); // IR Receiver off
}

// The repeating section of the code
void infraredReceiver_loop_HAL()
{
    // Check if the IR code has been received.
    if (irrecv.decode(&results))
    {
        isCodeReceived = true;
        // Display a crude timestamp.
        uint32_t now = millis();
        Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
        // Check if we got an IR message that was to big for our capture buffer.
        if (results.overflow)
            Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);
        // Display the library version the message was captured with.
        Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_STR "\n");
        // Display the tolerance percentage if it has been change from the default.
        if (kTolerancePercentage != kTolerance)
            Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);
        // Display the basic output of what we found.
        Serial.print(resultToHumanReadableBasic(&results));
        // Display any extra A/C info if we have it.
        String description = IRAcUtils::resultAcToString(&results);
        if (description.length())
            Serial.println(D_STR_MESGDESC ": " + description);
        yield(); // Feed the WDT as the text output can take a while to print.
#if LEGACY_TIMING_INFO
        // Output legacy RAW timing info of the result.
        Serial.println(resultToTimingInfo(&results));
        yield(); // Feed the WDT (again)
#endif           // LEGACY_TIMING_INFO
        // Output the results as source code
        Serial.println(resultToSourceCode(&results));
        Serial.println(); // Blank line between entries

        String message = "";
        message += typeToString((&results)->decode_type, (&results)->repeat);
        message += " ";
        message += resultToHexidecimal(&results);
        if (thisAnnounceNewIRmessage_cb != NULL)
        {
            thisAnnounceNewIRmessage_cb(std::string(message.c_str()));
        }

        // makeLastCapturePackets();
        makeLastCapturePacketsHandler();

        yield(); // Feed the WDT (again)

        // if message repeats infinitely, you can try one of these two workarounds. Don't know why this is needed.
        // irrecv.resume();
        // another workaround could be:
        // irrecv.disableIRIn();
        // irrecv.enableIRIn();
    }
}

bool get_irReceiverEnabled_HAL()
{
    return irReceiverEnabled;
}
void set_irReceiverEnabled_HAL(bool aIrReceiverEnabled)
{
    irReceiverEnabled = aIrReceiverEnabled;
}

// ------------- Fonctions en tests ---------------------------------
/// @brief Réinitialise l’état pour une capture IR.
/// Met le flag global `isCodeReceived` à false.
void startCapture_HAL()
{
    isCodeReceived = false;
}

/// @brief Vérifie si un code IR a été reçu.
/// @return true si un code a été capturé, false sinon.
bool getReceiveCaptureState_HAL()
{
    return isCodeReceived;
}

/// @brief Récupère les données de la dernière capture IR au format payloads.
/// @return Liste de chaînes contenant les données capturées.
std::list<std::string> getPayloads_HAL()
{
    return getPayloads(&results);
}

/// @brief Extrait et formate les informations d’un objet `decode_results`.
/// @param results Résultats du décodage IR.
/// @return Liste de chaînes contenant :
///   - protocole (id numérique),
///   - état de répétition,
///   - valeur ou state[] hexadécimale,
///   - nombre de bits,
///   - longueur,
///   - indication AC state,
///   - buffer brut,
///   - valeur si pas AC state,
///   - état AC en hexadécimal,
///   - adresse hexadécimale,
///   - commande hexadécimale,
///   - valeur hexadécimale.
std::list<std::string> getPayloads(const decode_results *const results)
{
    // Parcourir les données de results et les ajouter à commandPayloads
    std::list<std::string> payloads;
    String output = "";
    // Reserve some space for the string to reduce heap fragmentation.
    // "Protocol  : LONGEST_PROTOCOL_NAME (Repeat)\n"
    // "Code      : 0x (NNNN Bits)\n" = 70 chars
    output.reserve(2 * kStateSizeMax + 70); // Should cover most cases.

    // PART1 : ------- resultToHumanReadableBasic --------
    // Protocol (Return the Enum value)
    // output = protocolToString(results->decode_type); // Ex: RC6
    output = results->decode_type; // Ex: 2
    payloads.push_back(output.c_str());
    // Serial.print("DEBUG: valeur du protocol (decode_type): ");
    // Serial.println(output);

    // isRepeat
    output = boolToString(results->repeat); // Ex: false
    payloads.push_back(output.c_str());

    // value or state[] to simple hexadecimal
    output = resultToHexidecimal(results); // Ex: 0xC800F040C
    payloads.push_back(output.c_str());

    // Bits
    output = uint64ToString(results->bits); // Ex: 36
    payloads.push_back(output.c_str());

    // PART2 : ------- resultToSourceCode ---------------
    // Lenght
    output = lengthToString(results); // Ex: 67
    payloads.push_back(output.c_str());

    // HasACState
    output = hasACStateToString(results->decode_type); // Ex: false
    payloads.push_back(output.c_str());

    // Raw bugffer
    output = rawBufToString(results); // Ex: {2658, 884,  454, 438,  440, 450,  404, 936,  456, 878,  1368, 862,  478, 414,  458, 432,  470, 422,  444, 446,  456, 434,  460, 432,  454, 434,  468, 426,  422, 468,  452, 442,  900, 434,  456, 432,  462, 428,  420, 916,  458, 432,  482, 408,  442, 448,  454, 448,  866, 918,  444, 448,  464, 426,  442, 450,  462, 428,  454, 436,  910, 428,  472, 864,  480, 412,  418}
    // Serial.printf("[DEBUG] [getPayloads] rawBuffer: %s\r\n", output.c_str());
    std::string result = output.c_str();
    result = helpers::trimChar(result, '{');
    result = helpers::trimChar(result, '}');
    payloads.push_back(result.c_str());

    // Only return the value if the decode_type doesn't have an A/C state.
    output = ValueToStringIfNotHasState(results);
    payloads.push_back(output.c_str());

    /*
    // Decoded ACState codes to hexa
    output = ACStateCodeToString(results); // Ex: {0x0C, 0x04, 0x0F, 0x80, 0x0C}
    result = std::string(output.c_str());
    result = helpers::trimChar(result, '{');
    result = helpers::trimChar(result, '}');
    payloads.push_back(std::string(result.c_str()));
    */

    // Decoded ACState codes to hexa
    bool hasState = hasACState(results->decode_type);
    if (hasState)
    {
        output = ACStateCodeToString(results); // Appel identique à la lib
        std::string result = output.c_str();
        result = helpers::trimChar(result, '{');
        result = helpers::trimChar(result, '}');
        payloads.push_back(result.c_str());
    }
    else
    {
        // La lib ne produit rien dans ce cas, donc on ajoute une chaîne vide
        payloads.push_back("");
    }

    // Decoded Adress part to hexa
    output = AddressToString(results->address); // Ex: 0xC800F04
    payloads.push_back(output.c_str());

    // Decoded Command part to hexa
    output = CommandToString(results->command); // Ex: 0xC
    payloads.push_back(output.c_str());

    // Decoded Value to hexa
    output = DataCodeToString(results->value); // Ex: 0xC800F040C
    payloads.push_back(output.c_str());

    return payloads;
}

/// @brief Sérialise et prépare les paquets BLE de la dernière capture IR.
void makeLastCapturePacketsHandler()
{
    std::string str = serialize("Données de la derniere capture", getPayloads_HAL(), "");
    bleLastCapturePacketsHandler.makePackets(str.c_str());
}

/// @brief Callback utilisé pour envoyer une notification au client lorsque la lecture des paquets BLE expire (timeout).
void callbackReadPacketsTimeout()
{
    Serial.println("[Timeout] Read packets expired (LastCapture)");
    sendBleNotifyCode("104");
}

/// @brief Récupère l’instance du gestionnaire de paquets BLE pour la dernière capture.
/// @return Rétourne l’instance `bleLastCapturePacketsHandler`.
PacketsHandler &getBLELastCapturePacketsHandler_HAL()
{
    return bleLastCapturePacketsHandler;
}