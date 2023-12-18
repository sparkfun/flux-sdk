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
flxClockESP32 systemClock;

#endif

#define kNoClockName "No Clock"

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock()
    : _systemClock{nullptr}, _refClock{nullptr}, _lastRefCheck{0}, _lastConnCheck{0}, _bInitialized{false},
      _nameRefClock{kNoClockName}
{
    // Set name and description
    setName("Time Setup", "Manage time configuration and reference sources");

    flux.add(this);

    // Timezone
    flxRegister(timeZone, "The Time Zone", "Time zone setting string for the device");

    flxRegister(referenceClock, "Reference Clock", "The current reference clock source");

    flxRegister(updateClockInterval, "Update Interval", "Main clock update interval in minutes. 0 = No update");

    flxRegister(useAlternativeClock, "Enable Clock Fallback",
                "Use a valid reference clock if the primary is not available");

    flxRegister(connectedClockInterval, "Dependent Interval",
                "Connected dependent clock update interval in minutes. 0 = No update");

    flxRegister(updateConnectedOnUpdate, "Update Connected", "Update connected clocks when the main clock is updated");

    //
    referenceClock.setDataLimit(_refClockLimitSet);
    _refClockLimitSet.addItem("", kNoClockName);

#ifdef DEFAULT_DEFINED
    setSystemClock(&systemClock);
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
void _flxClock::set_timezone(std::string tz)
{
    if (tz.empty())
        return;

    _tzStorage = tz;
    if (_bInitialized && _systemClock)
        _systemClock->set_timezone(tz.c_str());
}
//----------------------------------------------------------------
std::string _flxClock::get_timezone(void)
{
    std::string stmp = "";
    // do we have a system clock?
    if (_systemClock)
    {
        char szBuffer[128];
        if (_systemClock->get_timezone(szBuffer, sizeof(szBuffer)) > 0)
            stmp = szBuffer;
    }
    else if (!_tzStorage.empty())
        stmp = _tzStorage;

    return stmp;
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
    if (_systemClock)
        return _systemClock->get_epoch();

    // We need something ...
    return millis() / 1000; // TODO - Revisit
}
//----------------------------------------------------------------
uint32_t _flxClock::now()
{
    return epoch();
}
//----------------------------------------------------------------
void _flxClock::setSystemClock(flxISystemClock *clock)
{
    if (clock)
        _systemClock = clock;
}

//----------------------------------------------------------------
// Connected clocks

void _flxClock::updateConnectedClocks(void)
{

    if (!_systemClock)
        return;

    uint32_t epoch = _systemClock->get_epoch();

    if (!epoch)
        return;

    for (flxIClock *aClock : _connectedClocks)
        aClock->set_epoch(epoch);

    _lastConnCheck = epoch;
}
//----------------------------------------------------------------
void _flxClock::updateClock()
{

    if (!_bInitialized || !_systemClock)
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
            _systemClock->set_epoch(epoch);

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
    // setup time zone for the system....
    if (!_tzStorage.empty() && _systemClock)
        _systemClock->set_timezone(_tzStorage.c_str());

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