#include <Arduino.h>
#include <map>
#include <string>
#include <mutex>

class TimeoutManager2
{
private:
    struct Timeout;

public:
    static void addTimeout(const std::string &name, unsigned long duration, std::function<void()> callback);
    static bool isExpired(const std::string &name);
    static void removeTimeout(const std::string &name);
    static void setStartTime(const std::string &name, unsigned long value);
    static void update();
    static bool exists(const std::string &name);
    static long getRemainingTime(const std::string &name);
};