#include <string>
#include <list>
#include <ArduinoJson.h>

struct messageData
{
    std::string message;
    std::string code;
    std::list<std::string> payloads;
};

messageData makeMessageData(const std::string &message, const std::list<std::string> &payloads, const std::string &code);

std::string serialize(const std::string &message, const std::list<std::string> &payloads, const std::string &code);
std::string serialize(const messageData &messageData);
JsonDocument to_json(const std::string &message, const std::list<std::string> &payloads, const std::string &code);
JsonDocument to_json(const messageData &messageData);
