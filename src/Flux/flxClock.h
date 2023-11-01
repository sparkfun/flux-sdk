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

#include <map>
#include <vector>

// default tz
#define kClockTimeZoneSparkFun "MST7MDT,M3.2.0,M11.1.0"

// Define a clock interface -- really just want secs from unix epoch
class flxIClock
{
  public:
    virtual uint get_epoch(void) = 0;
    virtual void set_epoch(const uint &) = 0;
    virtual bool valid_epoch(void) = 0;
};

// Define a System Clock interface -- a clock interface and some TimeZone magic

class flxISystemClock : public flxIClock
{
  public:
    virtual void set_timezone(const char *) = 0;
    virtual int get_timezone(char *, size_t) = 0;
};

#ifdef ESP32

#include <time.h>
class flxClockESP32 : public flxISystemClock
{
  public:
    uint get_epoch(void)
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

    bool valid_epoch(void)
    {
        // Determine if the on-board clock is valid by looking
        // at the current year

        struct tm *tm_now;
        time_t now;

        time(&now);
        tm_now = localtime(&now);

        return (tm_now && tm_now->tm_year > (2020 - 1900));
    }

    void set_timezone(const char *szTimeZone)
    {
        if (!szTimeZone)
            return;

        // setenv("TZ", szTimeZone, 1);
        // tzset();
    }

    int get_timezone(char *szTimeZone, size_t len)
    {
        if (!szTimeZone || !len)
            return 0;

        char *tz = getenv("TZ");

        if (!tz)
            return 0;

        return (int)strlcpy(szTimeZone, tz, len);
    }
};
#endif

class _flxClock : public flxActionType<_flxClock>
{

  private:
    // prop things
    void set_ref_clock(std::string name);
    std::string get_ref_clock(void);

    void set_timezone(std::string tz);
    std::string get_timezone(void);

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

    void setSystemClock(flxISystemClock *clock);

    bool setReferenceClock(flxIClock *clock, const char *name = nullptr);

    void addReferenceClock(flxIClock *clock, const char *name = nullptr);

    int addConnectedClock(flxIClock *clock);

    // Update the clock - will update from the reference clock
    void updateClock(void);

    void updateConnectedClocks(void);

    bool loop(void);

    bool initialize(void);

    flxPropertyRWString<_flxClock, &_flxClock::get_ref_clock, &_flxClock::set_ref_clock> referenceClock;

    flxPropertyUint<_flxClock> updateClockInterval = {60};

    // Use alternative clock if primary isn't available?
    flxPropertyBool<_flxClock> useAlternativeClock = {true};

    flxPropertyUint<_flxClock> connectedClockInterval = {60};
    flxPropertyBool<_flxClock> updateConnectedOnUpdate = {true};

    // TimeZone string
    flxPropertyRWString<_flxClock, &_flxClock::get_timezone, &_flxClock::set_timezone> timeZone = {
        kClockTimeZoneSparkFun};

  private:
    _flxClock();
    flxIClock *findRefClockByName(const char *name);

    // our system / runtime clock
    flxISystemClock *_systemClock;

    // The reference clock
    flxIClock *_refClock;

    uint32_t _lastRefCheck;

    uint32_t _lastConnCheck;

    bool _bInitialized;

    // Reference clocks
    std::map<std::string, flxIClock *> _refNametoClock; // map name for UX to index in

    // data limit for the above property
    flxDataLimitSetString _refClockLimitSet;

    // Connected clocks -- clocks that are synced with this time
    std::vector<flxIClock *> _connectedClocks;

    std::string _nameRefClock;

    std::string _tzStorage;
};
extern _flxClock &flxClock;