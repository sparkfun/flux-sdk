
#pragma once

#include <Arduino.h>

#include "spSpark.h"

class flxTimer : public flxActionType<flxTimer>
{

    // for our timing
    unsigned long _lastLogTime = 0;

  public:
    flxTimer(int start = 500) : _lastLogTime(0)
    {

        spRegister(interval, "interval", "The ms time interval for the timer");
        interval = start;
        
        flux.add(this);

        setName("Timer", "A reoccurring timer");
    };

    // Our output event
    flxSignalVoid on_interval;

    // Property  - interval for the timer
    flxPropertyInt<flxTimer> interval;

    bool loop(void)
    {
        // Is it time to log - Delta since last log > Log Interval??
        if (millis() - _lastLogTime < interval)
            return false; // we didn't do anything

        // Reset our last logged time to now
        _lastLogTime = millis();

        // trigger the event
        on_interval.emit();

        return true;
    }
};
