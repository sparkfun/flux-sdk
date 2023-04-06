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


#define kNoClock -1

// A little hacky - maybe ...
#ifdef ESP32

#define DEFAULT_DEFINED
flxClockESP32 defaultClock;

#endif

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock() : _defaultClock{nullptr}, _refClock{nullptr}, 
    _lastRefCheck{0}, _lastSyncCheck{0}, _bInitialized{false}, _iRefClock{kNoClock}
{
    // Set name and description
    setName("Epoch Clock", "Seconds since Unix Epoch");

    flux.add(this);

    flxRegister(referenceClock, "Reference Clock", "The current reference clock source");

    flxRegister(updateClockInterval, "Update Interval (Min)", "Main clock update interval in minutes. 0 = No update");

    flxRegister(syncClockInterval, "Dependant Interval (Min)", "Connected dependant clock update interval in minutes. 0 = No update");

    // 
    referenceClock.setDataLimit(_refClockLimitSet);
    _refClockLimitSet.addItem("No Clock", -1);

#ifdef DEFAULT_DEFINED
    setDefaultClock(&defaultClock);
#endif
}

//----------------------------------------------------------------
void _flxClock::set_ref_clock(int iclock)
{

    if (iclock == kNoClock)
        _refClock = nullptr;
    else
    {
        _refClock = _referenceClocks.at(iclock);
        updateClock();
    }
}

//----------------------------------------------------------------
int _flxClock::get_ref_clock(void)
{
    return _iRefClock;
}

//----------------------------------------------------------------
void _flxClock::addReferenceClock(flxIClock *clock)
{

    _referenceClocks.push_back(clock);

    // Add this to the current clock limit -- this creates a "list of available ref clocks"
    _refClockLimitSet.addItem(((flxObject*)clock)->name(), _referenceClocks.size()-1);
}

//----------------------------------------------------------------
void _flxClock::addSyncClock(flxIClock *clock)
{
    _syncClocks.push_back(clock);
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
// Sync clocks

void _flxClock::syncClocks(void)
{
    uint32_t epoch = _defaultClock->epoch();

    for (flxIClock *clock : _syncClocks)
        clock->set_epoch(epoch);

    _lastSyncCheck = epoch;
}
//----------------------------------------------------------------
void _flxClock::updateClock()
{

    // refresh our clock
    if (_bInitialized && _defaultClock && _refClock)
    {
        uint32_t epoch = _refClock->epoch();
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

    bool retval = false;

    if (!_bInitialized)
        return false;

    // Time to refresh?
    if (updateClockInterval() > 0 &&
        epoch() - _lastRefCheck > updateClockInterval() * 60)
    {
        updateClock();
        retval = true;
    }

    if ( syncClockInterval() > 0 &&
        epoch() - _lastSyncCheck > syncClockInterval() * 60)
    {
        syncClocks();
        retval = true;
    }

    return retval;
}