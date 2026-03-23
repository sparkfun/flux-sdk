/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
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

const uint32_t kClockMinutesToMS = 60000;

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

_flxClock::_flxClock()
    : _systemClock{nullptr}, _refClock{nullptr}, _refCheck{0}, _connCheck{0}, _bInitialized{false}, _bSysTimeSet{false},
      _nameRefClock{kNoClockName}
{
    // Set name and description
    setName("Time Setup", "Manage time configuration and reference sources");

    // flux.add(this);
    flux_add(this);
}
bool _flxClock::initialize(void)
{
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
    _refClockLimitSet.addItem(kNoClockName, kNoClockName);

#ifdef DEFAULT_DEFINED
    setSystemClock(&systemClock);
#endif

    // Setup our clock check jobs
    _jobRefCheck.setup("clock refchk", _refCheck * kClockMinutesToMS, this, &_flxClock::checkRefClock);
    if (_refCheck > 0)
        flxAddJobToQueue(_jobRefCheck);
    _jobConnCheck.setup("clock conchk", _connCheck * kClockMinutesToMS, this, &_flxClock::checkConnClock);
    if (_connCheck > 0)
        flxAddJobToQueue(_jobConnCheck);

    // setup time zone for the system....
    if (!_tzStorage.empty() && _systemClock)
        _systemClock->set_timezone(_tzStorage.c_str());

    _bInitialized = true;
    updateClock();

    // if the system time wasn't set at initialize - reset the update timer to one
    // minute.
    //
    // Only do this if we are updating our clock from reference

    if (!_bSysTimeSet && _refCheck > 0)
    {
        // Set the check to 1 minute - the value of our min to ms conversion constant
        _jobRefCheck.setPeriod(kClockMinutesToMS);
        flxUpdateJobInQueue(_jobRefCheck);
    }
    return true;
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
void _flxClock::set_ref_interval(uint32_t val)
{
    if (val == _refCheck)
        return;

    _refCheck = val;

    if (val == 0)
        flxRemoveJobFromQueue(_jobRefCheck);
    else if (_bSysTimeSet)
    {
        // We only update the update job value if the system clock has been set. Otherwise
        // leave the job as is -- rapid check intervals
        //
        // Once the system time is synced, the update period of the job is reset to the user value

        // val is in minutes, period is MS
        _jobRefCheck.setPeriod(val * kClockMinutesToMS);
        flxUpdateJobInQueue(_jobRefCheck);
    }
}
uint32_t _flxClock::get_ref_interval(void)
{
    return _refCheck;
}

void _flxClock::set_conn_interval(uint32_t val)
{
    if (val == _connCheck)
        return;

    _connCheck = val;

    if (val == 0)
        flxRemoveJobFromQueue(_jobConnCheck);
    else
    {
        // val is in minutes, period is MS
        _jobConnCheck.setPeriod(val * kClockMinutesToMS);
        flxUpdateJobInQueue(_jobConnCheck);
    }
}
uint32_t _flxClock::get_conn_interval(void)
{
    return _connCheck;
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
    _refClockLimitSet.addItem(name, name);
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
}
//----------------------------------------------------------------
// Method called to make sure reference update job timeout is correct
//
// At startup, if a ref clock is not valid, a rapid time check period takes place,
// Once we have a valid reference clock, the update value is then reset to the pref value
//
void _flxClock::resetReferenceUpdate(void)
{

    if (_refCheck * kClockMinutesToMS != _jobRefCheck.period())
    {
        // get the current preference value
        uint32_t val = get_ref_interval();

        // set value to 0 - to get past the value equal check at beginning of set method.
        set_ref_interval(0);

        // now set values.
        set_ref_interval(val);
    }
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

            // if first time setting the system time value:
            //   - flag that is has been set
            //   - make sure update interval is correct
            if (!_bSysTimeSet)
            {
                _bSysTimeSet = true;
                resetReferenceUpdate();
            }

            // we have successfully set system time.
            // update our connected clocks?
            if (updateConnectedOnUpdate())
                updateConnectedClocks();
        }
    }
}

//----------------------------------------------------------------
void _flxClock::checkRefClock(void)
{
    if (!_bInitialized)
        return;

    updateClock();
}

void _flxClock::checkConnClock(void)
{
    if (!_bInitialized)
        return;

    updateConnectedClocks();
}
