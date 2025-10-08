#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <vector>
#include <sstream>

String boolToString(const bool value);
[[deprecated("Don't use this routine any more. Use the new one instead.")]]
String protocolToString(const decode_type_t protocol);
String hasACStateToString(const decode_type_t protocol);
String rawBufToString(const decode_results *const results);
std::vector<uint16_t> stringToRawBuf(const std::string &valuesStr);
std::vector<uint8_t> hexStringToByteArray(const std::string &input);
[[deprecated("Don't use this routine any more. Use the new one instead. 'helpers::convertToType<uint64_t>(strValue, 0)'")]]
uint64_t stringToUint64(const String &hexStr);
String ValueToStringIfNotHasState(const decode_results *const results);
String lengthToString(const decode_results *const results);
String ACStateCodeToString(const decode_results *const results);
String AddressToString(const uint32_t address);
String CommandToString(const uint32_t command);
String DataCodeToString(const uint64_t value);