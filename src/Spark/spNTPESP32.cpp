



#ifdef ESP32

#include "spNTPESP32.h"

#include "time.h"
#include "lwip/apps/sntp.h"

#define kNTPServerAddress "pool.ntp.org"
//----------------------------------------------------------------
// Enabled Property setter/getters
void spNTPESP32::set_isEnabled(bool bEnabled)
{
    // Any changes?
    if (_isEnabled == bEnabled)
        return;  

    // changing state ...
    // TODO - Should this control connection state?

    _isEnabled = bEnabled;

    if ( _isEnabled)
        (void)start();
    else 
        stop();
}

//----------------------------------------------------------------
bool spNTPESP32::get_isEnabled(void)
{
    return _isEnabled;
}

// Event callback 
//----------------------------------------------------------------------------
void spNTPESP32::onConnectionChange(bool bConnected)
{
    // Are we enabled ...
    if (!_isEnabled)
        return; 

    if (bConnected)
        start();
    else
        stop();
}

bool spNTPESP32::start(void)
{
    // already running?
    if(sntp_enabled())
        return true;

    // do we have a network and is it connected
    if ( !_theNetwork || !_theNetwork->isConnected())
    {
        spLog_W(F("NTP Client - Unable to start, no network connection"));
        return false;
    }
    
    // Leverage ESP32 Arduino setup
    configTime(gmtOffsetMinutes()*60, daylightOffsetMinutes()*60, kNTPServerAddress);

    return sntp_enabled();
}

void spNTPESP32::stop(void)
{
    // If the sntp system is up, shut it down
    if(sntp_enabled())
        sntp_stop();
}
    

#endif