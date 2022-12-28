

// Action to control save of settings ...
#pragma once

#include "flxCore.h"

// Define a clock interface -- really just want secs from unix epoch
class spIClock
{
  public:
    virtual uint32_t epoch(void) = 0;
    virtual void set(uint32_t) = 0;
};

#ifdef ESP32

#include <time.h>
class spClockESP32 : public spIClock
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

class _spClock : public spActionType<_spClock>
{

  public:
    // spClock is a singleton
    static _spClock &get(void)
    {

        static _spClock instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _spClock(_spClock const &) = delete;
    void operator=(_spClock const &) = delete;

    uint32_t epoch();

    uint32_t now();

    void setDefaultClock(spIClock *clock);

    void setClockReference(spIClock *clock);

    bool loop(void);

    bool initialize(void);

  private:
    _spClock();

    void updateClock();

    spIClock *_defaultClock;
    spIClock *_referenceClock;

    uint32_t _lastRefCheck;

    bool _bInitialized;
};
extern _spClock &spClock;