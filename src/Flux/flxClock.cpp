

#include "flxClock.h"

#include "spSpark.h"

// A little hacky - maybe ...
#ifdef ESP32

#define DEFAULT_DEFINED
flxClockESP32 defaultClock;

#endif

// Global object - for quick access to Settings system
_flxClock &flxClock = _flxClock::get();

// Refresh period if we have a reference clock - time in secs

#define kClockReferenceRefresh 3600

_flxClock::_flxClock() : _defaultClock{nullptr}, _referenceClock{nullptr}, _lastRefCheck{0}, _bInitialized{false}
{
    // Set name and description
    setName("Epoch Clock", "Seconds since Unix Epoch");

    flux.add(this);

#ifdef DEFAULT_DEFINED
    setDefaultClock(&defaultClock);
#endif
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
            _defaultClock->set(epoch);
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
    if (_bInitialized && epoch() - _lastRefCheck > kClockReferenceRefresh)
    {
        updateClock();
        return true;
    }
    return false;
}