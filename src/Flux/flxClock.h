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

// Action to control save of settings ...
#pragma once

#include "flxCore.h"

// Define a clock interface -- really just want secs from unix epoch
class flxIClock
{
  public:
    virtual uint32_t epoch(void) = 0;
    virtual void set(uint32_t) = 0;
};

#ifdef ESP32

#include <time.h>
class flxClockESP32 : public flxIClock
{
  public:
    uint32_t epoch(void)
    {
        time_t now;
        time(&now);
        return now;
    }

    void set(uint32_t refEpoch)
    {
        timeval epoch = {(time_t)refEpoch, 0};
        const timeval *tv = &epoch;
        timezone utc = {0, 0};
        const timezone *tz = &utc;
        settimeofday(tv, tz);
    }
};
#endif

class _flxClock : public flxActionType<_flxClock>
{

  public:
    // flxClock is a singleton
    static _flxClock &get(void)
    {

        static _flxClock instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _flxClock(_flxClock const &) = delete;
    void operator=(_flxClock const &) = delete;

    uint32_t epoch();

    uint32_t now();

    void setDefaultClock(flxIClock *clock);

    void setClockReference(flxIClock *clock);

    bool loop(void);

    bool initialize(void);

  private:
    _flxClock();

    void updateClock();

    flxIClock *_defaultClock;
    flxIClock *_referenceClock;

    uint32_t _lastRefCheck;

    bool _bInitialized;
};
extern _flxClock &flxClock;