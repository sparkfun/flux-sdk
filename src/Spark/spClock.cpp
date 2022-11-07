

#include "spClock.h"

#include "spSpark.h"

// A little hacky - maybe ...
#ifdef ESP32

#define DEFAULT_DEFINED
spClockESP32 defaultClock;

#endif

// Global object - for quick access to Settings system
_spClock &spClock = _spClock::get();

// Refresh period if we have a reference clock - time in secs

#define kClockReferenceRefresh 3600

_spClock::_spClock() : _defaultClock{nullptr}, _referenceClock{nullptr}, _lastRefCheck{0}, _bInitialized{false}
{
    // Set name and description
    setName("Epoch Clock", "Seconds since Unix Epoch");

    spark.add(this);

#ifdef DEFAULT_DEFINED
    setDefaultClock(&defaultClock);
#endif
}
//----------------------------------------------------------------
uint32_t _spClock::epoch()
{
    if (_defaultClock)
        return _defaultClock->epoch();

    // We need something ...
    return millis() / 1000; // TODO - Revisit
}
//----------------------------------------------------------------
uint32_t _spClock::now()
{
    return epoch();
}
//----------------------------------------------------------------
void _spClock::setDefaultClock(spIClock *clock)
{
    if (clock)
        _defaultClock = clock;
}
//----------------------------------------------------------------
void _spClock::setClockReference(spIClock *clock)
{

    if (!clock)
        return;

    _referenceClock = clock;

    updateClock();
}
//----------------------------------------------------------------
void _spClock::updateClock()
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
bool _spClock::initialize(void)
{
    _bInitialized = true;
    updateClock();
    return true;
}

bool _spClock::loop(void)
{
    // Time to refresh?
    if (_bInitialized && epoch() - _lastRefCheck > kClockReferenceRefresh)
    {
        updateClock();
        return true;
    }
    return false;
}