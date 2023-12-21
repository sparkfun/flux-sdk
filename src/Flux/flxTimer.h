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

#include "flxCoreJobs.h"
#include "flxFlux.h"

class flxTimer : public flxActionType<flxTimer>
{

    uint get_interval(void)
    {
        return _timerJob.period();
    }

    void set_interval(uint in_interval)
    {
        _timerJob.setPeriod(in_interval);
    }

    void onTimer(void)
    {
        on_interval.emit();
    }

    // for our timing
    unsigned long _lastLogTime = 0;

    flxJob _timerJob;

  public:
    flxTimer(int start = 500) : _lastLogTime(0)
    {

        flxRegister(interval, "Interval", "Timer interval in milliseconds");

        // interval = start;

        flux.add(this);

        setName("Timer", "A reoccurring timer");

        _timerJob.setPeriod(start);
        _timerJob.setName("Timer");
    };

    bool initialize(void)
    {
        _timerJob.setHandler(this, &flxTimer::onTimer);
        flxJobQueue.addJob(_timerJob);

        return true;
    }

    // Our output event
    flxSignalVoid on_interval;

    // Property  - interval for the timer -- and set a default value of 10 seconds
    flxPropertyRWUint<flxTimer, &flxTimer::get_interval, &flxTimer::set_interval> interval = {10000};
};
