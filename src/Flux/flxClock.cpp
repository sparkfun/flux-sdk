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

#define kNoClockName "No Clock"

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock()
    : _defaultClock{nullptr}, _refClock{nullptr}, _lastRefCheck{0}, _lastConnCheck{0}, _bInitialized{false},
      _nameRefClock{kNoClockName}
{
    // Set name and description
    setName("Time Sources", "Manage time reference sources");

    flux.add(this);

    flxRegister(referenceClock, "Reference Clock", "The current reference clock source");

    flxRegister(updateClockInterval, "Update Interval", "Main clock update interval in minutes. 0 = No update");

    flxRegister(useAlternativeClock, "Enable Clock Fallback",
                "Us a valid reference clock if the primary is not available");

    flxRegister(connectedClockInterval, "Dependant Interval",
                "Connected dependant clock update interval in minutes. 0 = No update");

    flxRegister(updateConnectedOnUpdate, "Update Connected", "Update connected clocks on main clock update");

    //
    referenceClock.setDataLimit(_refClockLimitSet);
    _refClockLimitSet.addItem("", kNoClockName);

#ifdef DEFAULT_DEFINED
    setDefaultClock(&defaultClock);
#endif
}

//----------------------------------------------------------------
flxIClock *_flxClock::findRefClockByName(const char *name)
{

    if (!name)
        return nullptr;

    for (auto it = _refNametoClock.begin(); it != _refNametoClock.end(); it++)
    {
        if (it->first.compare(name) == 0) // a hit!
            return it->second;
    }

    // if we are here, no match in our list
    return nullptr;
}
//----------------------------------------------------------------
void _flxClock::set_ref_clock(std::string selClock)
{

    // This can be called before our clock list is setup,  -- at startup.
    // So stash name, but not the null clock.

    _nameRefClock = selClock;

    _refClock = findRefClockByName(selClock.c_str());

}

//----------------------------------------------------------------
std::string _flxClock::get_ref_clock(void)
{
    return _nameRefClock;
}

//----------------------------------------------------------------
// Add a reference clock to the system
void _flxClock::addReferenceClock(flxIClock *clock, const char *name)
{

    if (!name)
        name = "Unknown clock";

    std::string stmp = name;
    _refNametoClock[stmp] = clock;

    // Add this to the current clock limit -- this creates a "list of available ref clocks
    _refClockLimitSet.addItem("", name);
}

//----------------------------------------------------------------
bool _flxClock::setReferenceClock(flxIClock *theClock, const char *name)
{
    if (!theClock || !name)
        return false;

    // In our current reference clock list?

    if (!findRefClockByName(name))
        addReferenceClock(theClock, name);

    _refClock = theClock;
    _nameRefClock = name;

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
        for (auto it = _refNametoClock.begin(); it != _refNametoClock.end(); it++)
        {
            if (it->second && it->second->valid_epoch()) // valid clock
            {
                theClock = it->second;
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