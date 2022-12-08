



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
    spLog_I(F("Starting NTP..."));
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
    configTzTime(timeZone().c_str(), ntpServerOne().c_str(), ntpServerTwo().c_str());

    // wait for time to sync if a delay is set. 
    // The indication that NTP is working is time moves to a current year
    if (_startupDelay)
    {
        uint32_t start = millis();
        uint32_t secs;
        uint32_t inc=0;

        struct tm *tm_now;
        time_t now;

        spLog_I_(F("Waiting on NTP startup [%u secs]..."), _startupDelay);


        while (true)
        // while (millis() - start < _startupDelay*1000)
        {
            secs = (millis() - start)/1000;
            if (secs >= _startupDelay)
                break;
            time(&now);
            tm_now = localtime(&now);
            if (tm_now && tm_now->tm_year > (2020 - 1900))
                break; // synched
            if (secs > inc)
            {
                spLog_N_(F("."));
                inc++;
            }
        }
        delay(100);
    }
    bool enabled = sntp_enabled();

    if (enabled)
        spLog_N(F("enabled"));
    else
        spLog_N(F("not enabled"));

    return enabled;
}

void spNTPESP32::stop(void)
{
    // If the sntp system is up, shut it down
    if(sntp_enabled())
        sntp_stop();
}
    

#endif