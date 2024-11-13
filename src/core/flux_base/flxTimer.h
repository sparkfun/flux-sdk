/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include <Arduino.h>

#include "flxCoreJobs.h"
#include "flxFlux.h"

class flxTimer : public flxActionType<flxTimer>
{

    uint32_t get_interval(void)
    {
        return _timerJob.period();
    }

    void set_interval(uint32_t in_interval)
    {
        if (_timerJob.period() == in_interval)
            return;

        _timerJob.setPeriod(in_interval);
        // update the job queue
        flxUpdateJobInQueue(_timerJob);
    }

    void onTimer(void)
    {
        on_interval.emit();
    }

    // for our timing
    unsigned long _lastLogTime = 0;
    uint32_t _initialInterval = 0;

    flxJob _timerJob;

  public:
    flxTimer(uint32_t start = 15000) : _lastLogTime(0), _initialInterval(start)
    {
        // Add this to the list of actions in the framework
        flux_add(this);
        setName("Timer", "A reoccurring timer");
    };

    bool initialize(void)
    {
        // Register the interval property - do here not in ctor due to uncertainty of global object creation order

        flxRegister(interval, "Interval", "Timer interval in milliseconds");
        // setup the job used to trigger the timer (note - we enable "job compression" for this )
        _timerJob.setup(name(), _initialInterval, this, &flxTimer::onTimer, false);
        // Add the job to the job queue
        flxAddJobToQueue(_timerJob);

        return true;
    }

    // Our output event
    flxSignalVoid on_interval;

    // Property  - interval for the timer -- and set a default value of 10 seconds
    flxPropertyRWUInt32<flxTimer, &flxTimer::get_interval, &flxTimer::set_interval> interval = {10000};
};
