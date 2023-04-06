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

#include "flxClock.h"

#include "flxFlux.h"

// A little hacky - maybe ...
#ifdef ESP32

#define DEFAULT_DEFINED
flxClockESP32 defaultClock;

#endif

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock() : _defaultClock{nullptr}, _referenceClock{nullptr}, _lastRefCheck{0}, _bInitialized{false}
{
    // Set name and description
    setName("Epoch Clock", "Seconds since Unix Epoch");

    flux.add(this);

    flxRegister(referenceClock, "Reference Clock", "The current reference clock source");

    flxRegister(updateClockInterval, "Update Interval (Min)", "Main clock update interval in minutes. 0 = No update");

    // 
    referenceClock.setDataLimit(_refClockLimitSet);
    _refClockLimitSet.addItem("No Clock", -1);

#ifdef DEFAULT_DEFINED
    setDefaultClock(&defaultClock);
#endif
}

void _flxClock::addReferenceClock(flxIClock *clock)
{

    _referenceClocks.push_back(clock);

    // Add this to the current clock limit -- this creates a "list of available ref clocks"
    _refClockLimitSet.addItem(((flxObject*)clock)->name(), _referenceClocks.size()-1);
}
//----------------------------------------------------------------
uint32_t _flxClock::epoch()
{
    if (_defaultClock)
        return _defaultClock->epoch();

    // We need something ...
    return millis() / 1000; // TODO - Revisit
}
//----------------------------------------------------------------
uint32_t _flxClock::now()
{
    return epoch();
}
//----------------------------------------------------------------
void _flxClock::setDefaultClock(flxIClock *clock)
{
    if (clock)
        _defaultClock = clock;
}
//----------------------------------------------------------------
void _flxClock::setClockReference(flxIClock *clock)
{

    if (!clock)
        return;

    _referenceClock = clock;

    updateClock();
}
//----------------------------------------------------------------
void _flxClock::updateClock()
{

    // refresh our clock
    if (_bInitialized && _defaultClock && _referenceClock)
    {
        uint32_t epoch = _referenceClock->epoch();
        if (epoch)
            _defaultClock->set_epoch(epoch);
    }

    _lastRefCheck = epoch();
}
//----------------------------------------------------------------
bool _flxClock::initialize(void)
{
    _bInitialized = true;
    updateClock();
    return true;
}

bool _flxClock::loop(void)
{
    // Time to refresh?
    if (_bInitialized && updateClockInterval() > 0 &&
        epoch() - _lastRefCheck > updateClockInterval() * 60)
    {
        updateClock();
        return true;
    }
    return false;
}