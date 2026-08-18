#pragma once

#include <Arduino.h>
#include <map>
#include <string>
#include "timer.h"

class TimeManager
{
public:
    static void init();
    static void addTimer(const std::string &name, const Timer &timer);
    static void update();

    static void pause();
    static void resume();

    static void pauseTimer(const std::string &name);
    static void resumeTimer(const std::string &name);
    static void resetTimer(const std::string &name);

    static unsigned long getElapsed(const std::string &name);
    static unsigned long getRemaining(const std::string &name);
    static unsigned long getMinimumRemaining();
    //static uint32_t getMinimumRemainingSeconds();

private:
    static std::map<std::string, Timer> timers;
    static bool isPaused;
    static unsigned long lastUpdate;
    static unsigned long lastPrint;

    static void updateTimer(Timer &timer, unsigned long dt);
};