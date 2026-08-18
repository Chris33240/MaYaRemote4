#include "time_manager.h"

std::map<std::string, Timer> TimeManager::timers;
bool TimeManager::isPaused = false;         // Pause de tous les timers
unsigned long TimeManager::lastUpdate = 0;  // La derniere fois queTimeManager à été updaté
unsigned long TimeManager::lastPrint = 0;   // La derniere fois que le temps s'est affiché dans la console

void TimeManager::init()
{
    timers.clear();
    isPaused = false;

    lastUpdate = millis();
    lastPrint = lastUpdate;
}

void TimeManager::addTimer(const std::string &name, const Timer &timer)
{
    timers[name] = timer;
}

void TimeManager::update()
{
    unsigned long now = millis();
    unsigned long dt = now - lastUpdate;
    lastUpdate = now;

    if (!isPaused)
    {
        for (auto &entry : timers)
            updateTimer(entry.second, dt);
    }

    if (now - lastPrint >= 10000)
    {
        lastPrint = now;
        //lastPrint += 10000;

        for (const auto &entry : timers)
        {
            const Timer &timer = entry.second;

            Serial.printf(
                "[Timer] %s : %lu/%lu s, remaining=%lu s %s | ",
                entry.first.c_str(),
                timer.elapsed / 1000,
                timer.duration / 1000,
                (timer.duration - timer.elapsed) / 1000,
                timer.paused ? "(Pause)" : "");
        }
        Serial.printf("\r\n");
    }
}

void TimeManager::updateTimer(Timer &timer, unsigned long dt)
{
    if (timer.paused)
        return;

    timer.elapsed += dt;

    if (timer.elapsed >= timer.duration)
    {
        timer.elapsed = timer.duration;

        if (timer.onElapsed)
        {
            timer.paused = true;
            timer.onElapsed();
        }
    }
}

void TimeManager::pause()
{
    isPaused = true;
}

void TimeManager::resume()
{
    isPaused = false;
}

void TimeManager::pauseTimer(const std::string &name)
{
    auto it = timers.find(name);

    if (it != timers.end())
        it->second.paused = true;
}

void TimeManager::resumeTimer(const std::string &name)
{
    auto it = timers.find(name);

    if (it != timers.end())
        it->second.paused = false;
}

void TimeManager::resetTimer(const std::string &name)
{
    auto it = timers.find(name);

    if (it != timers.end())
        it->second.elapsed = 0;
}

unsigned long TimeManager::getElapsed(const std::string &name)
{
    auto it = timers.find(name);

    return it != timers.end() ? it->second.elapsed : 0;
}

unsigned long TimeManager::getRemaining(const std::string &name)
{
    auto it = timers.find(name);

    if (it == timers.end())
        return 0;

    return it->second.elapsed >= it->second.duration
               ? 0
               : it->second.duration - it->second.elapsed;
}

unsigned long TimeManager::getMinimumRemaining()
{
    unsigned long minimum = ULONG_MAX;

    for (const auto &entry : timers)
    {
        const Timer &timer = entry.second;

        if (timer.paused)
            continue;

        unsigned long remaining = timer.duration - timer.elapsed;

        if (remaining < minimum)
            minimum = remaining;
    }

    return minimum;
}

/*
uint32_t TimeManager::getMinimumRemainingSeconds()
{
    return static_cast<uint32_t>(getMinimumRemaining() / 1000);
}
*/
