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

#include <vector>

// Define a clock interface -- really just want secs from unix epoch
class flxIClock
{
  public:
    virtual uint32_t epoch(void) = 0;
    virtual void set_epoch(const uint &) = 0;
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

    void set_epoch(const uint &refEpoch)
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

private:

  // prop things
  void set_ref_clock( int );
  int get_ref_clock(void);

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

    void addReferenceClock(flxIClock *clock);

    void addSyncClock(flxIClock *clock);

    bool loop(void);

    bool initialize(void);

    flxPropertyRWInt<_flxClock, &_flxClock::get_ref_clock, &_flxClock::set_ref_clock>  referenceClock;

    flxPropertyUint<_flxClock>  updateClockInterval={60};

    flxPropertyUint<_flxClock>  syncClockInterval={60};


  private:
    _flxClock();

    void updateClock(void);
    void syncClocks(void);

    flxIClock *_defaultClock;
    flxIClock *_refClock;

    uint32_t _lastRefCheck;

    uint32_t _lastSyncCheck;    

    bool _bInitialized;

    // Reference clocks
    std::vector<flxIClock*> _referenceClocks;

    // data limit for the above property 
    flxDataLimitSetInt  _refClockLimitSet;

    // Sync clocks -- clocks that are synced with this time
    std::vector<flxIClock*> _syncClocks;   

    int _iRefClock; 

};
extern _flxClock &flxClock;