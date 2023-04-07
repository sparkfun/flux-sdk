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

// No clock value
#define kNoClock -1

// A little hacky - maybe ...
#ifdef ESP32

#define DEFAULT_DEFINED
flxClockESP32 defaultClock;

#endif

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock()
    : _defaultClock{nullptr}, _refClock{nullptr}, _lastRefCheck{0}, _lastConnCheck{0}, _bInitialized{false},
      _iRefClock{kNoClock}
{
    // Set name and description
    setName("Epoch Clock", "Seconds since Unix Epoch");

    flux.add(this);

    flxRegister(referenceClock, "Reference Clock", "The current reference clock source");

    flxRegister(updateClockInterval, "Update Interval (Min)", "Main clock update interval in minutes. 0 = No update");

    flxRegister(useAlternativeClock, "Enable Clock Fallback",
                "Us a valid reference clock if the primary is not available");

    flxRegister(connectedClockInterval, "Dependant Interval (Min)",
                "Connected dependant clock update interval in minutes. 0 = No update");

    flxRegister(updateConnectedOnUpdate, "Update Connected", "Update connected clocks on main clock update");

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
int _flxClock::addReferenceClock(flxIClock *clock)
{

    int i = _referenceClocks.size();

    _referenceClocks.push_back(clock);

    // Add this to the current clock limit -- this creates a "list of available ref clocks"
    _refClockLimitSet.addItem(((flxObject *)clock)->name(), _referenceClocks.size() - 1);

    return i; // position in the list of things
}

//----------------------------------------------------------------
bool _flxClock::setReferenceClock(flxIClock *theClock)
{

    if (!theClock)
        return false;

    // In our current reference clock list?

    int iclock = 0;
    for (flxIClock *aRefClock : _referenceClocks)
    {
        if (aRefClock == theClock)
            break;

        iclock++;
    }
    // clock not in the reference list?
    if (iclock == _referenceClocks.size())
        iclock = addReferenceClock(theClock);

    // set this clock as the ref clock
    set_ref_clock(iclock);

    return true;
}
//----------------------------------------------------------------
int _flxClock::addConnectedClock(flxIClock *clock)
{
    int i = _connectedClocks.size();

    _connectedClocks.push_back(clock);

    return i;
}

//----------------------------------------------------------------
uint32_t _flxClock::epoch()
{
    if (_defaultClock)
        return _defaultClock->get_epoch();

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
// Connected clocks

void _flxClock::updateConnectedClocks(void)
{
    uint32_t epoch = _defaultClock->get_epoch();

    if (!epoch)
        return;

    for (flxIClock *aClock : _connectedClocks)
        aClock->set_epoch(epoch);

    _lastConnCheck = epoch;
}
//----------------------------------------------------------------
void _flxClock::updateClock()
{

    if (!_bInitialized || !_defaultClock)
        return;

    flxIClock *theClock = _refClock;

    // if we don't have a ref clock, or if the clock is invalid AND
    // the use alternative clock option set, find another clock!
    if ((!theClock || !theClock->valid_epoch()) && useAlternativeClock())
    {
        //  find a clock with a valid epoch
        for (flxIClock *aClock : _referenceClocks)
        {
            if (aClock->valid_epoch())
            {
                theClock = aClock;
                break;
            }
        }
    }

    // do we have a clock with a valid epoch value?
    if (theClock && theClock->valid_epoch())
    {
        uint32_t epoch = theClock->get_epoch();
        if (epoch)
        {
            _defaultClock->set_epoch(epoch);

            // update our connected clocks?
            if (updateConnectedOnUpdate())
                updateConnectedClocks();
        }
    }

    // mark this check time
    _lastRefCheck = epoch();
}
//----------------------------------------------------------------
bool _flxClock::initialize(void)
{
    _bInitialized = true;
    updateClock();
    return true;
}

//----------------------------------------------------------------
// framwork loop
bool _flxClock::loop(void)
{

    bool retval = false;

    if (!_bInitialized)
        return false;

    // Time to refresh?
    if (updateClockInterval() > 0 && epoch() - _lastRefCheck > updateClockInterval() * 60)
    {
        updateClock();
        retval = true;
    }

    if (connectedClockInterval() > 0 && epoch() - _lastConnCheck > connectedClockInterval() * 60)
    {
        updateConnectedClocks();
        retval = true;
    }

    return retval;
}