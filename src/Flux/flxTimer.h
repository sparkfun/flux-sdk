/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
#pragma once

#include <Arduino.h>

#include "flxFlux.h"

class flxTimer : public flxActionType<flxTimer>
{

    // for our timing
    unsigned long _lastLogTime = 0;

  public:
    flxTimer(int start = 500) : _lastLogTime(0)
    {

        flxRegister(interval, "Interval", "The timer interval in milliseconds");
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
