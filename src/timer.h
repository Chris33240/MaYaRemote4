#pragma once

#include <string>

struct Timer
{
    unsigned long duration;
    unsigned long elapsed;
    bool paused;
    void (*onElapsed)();
};