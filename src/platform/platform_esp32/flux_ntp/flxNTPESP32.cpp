/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#ifdef ESP32

#include "flxNTPESP32.h"

#include "esp_netif.h"
#include "lwip/apps/sntp.h"
#include "time.h"

//----------------------------------------------------------------
// Enabled Property setter/getters
void flxNTPESP32::set_isEnabled(bool bEnabled)
{
    // Any changes?
    if (_isEnabled == bEnabled)
        return;

    // changing state ...
    // TODO - Should this control connection state?

    _isEnabled = bEnabled;

    if (_isEnabled)
        (void)start();
    else
        stop();
}

//----------------------------------------------------------------
bool flxNTPESP32::get_isEnabled(void)
{
    return _isEnabled;
}

// Event callback
//---------------------------------------------------------------
void flxNTPESP32::onConnectionChange(bool bConnected)
{
    // Are we enabled ...
    if (!_isEnabled)
        return;

    if (bConnected)
        start();
    else
        stop();
}
//----------------------------------------------------------
bool flxNTPESP32::start(void)
{
    // already running?
    if (sntp_enabled())
        return true;

    // do we have a network and is it connected
    if (!_theNetwork || !_theNetwork->isConnected())
    {
        flxLogM_W(kMsgErrInitialization, name(), "network unavailable");
        return false;
    }

    // KDB - Nov 2023
    //
    // We were using the helper method - configTZTime() in the Arduino ESP port, but
    // it set the Timezone and the ntp servers (and got everything going). But for Flux,
    // the timezone is handled by the framework Clock object -- which would cause
    // a conflict -- either this object could grab the TZ from the clock, which connects
    // the two object together - not good design. Or just setup the NTP system, but skip
    // the Time zone work . This is what is done here.
    //
    // The following are from the implementation of configTzTime() - minus the TZ setup.

    esp_netif_init();
    if (sntp_enabled())
    {
        sntp_stop();
    }
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)ntpServerOne().c_str());
    sntp_setservername(1, (char *)ntpServerTwo().c_str());
    sntp_init();

    // end configTZtime() inline

    // wait for time to sync if a delay is set.
    // The indication that NTP is working is time moves to a current year
    if (_startupDelay)
    {
        uint32_t start = millis();
        uint32_t secs;
        uint32_t inc = 0;

        struct tm *tm_now;
        time_t now;

        flxLog_I_(F("NTP startup [%u secs]..."), _startupDelay);

        while (true)
        // while (millis() - start < _startupDelay*1000)
        {
            secs = (millis() - start) / 1000;
            if (secs >= _startupDelay)
                break;
            time(&now);
            tm_now = localtime(&now);
            if (tm_now && tm_now->tm_year > (2020 - 1900))
                break; // synched
            if (secs > inc)
            {
                flxLog_N_(F("."));
                inc++;
            }
        }
        delay(100);
    }
    bool enabled = sntp_enabled();

    if (enabled)
        flxLog_N(F("enabled"));
    else
        flxLog_N(F("not enabled"));

    return enabled;
}

//----------------------------------------------------------
void flxNTPESP32::stop(void)
{
    // If the sntp system is up, shut it down
    if (sntp_enabled())
        sntp_stop();
}
//----------------------------------------------------------
// Clock interface things
//----------------------------------------------------------
uint32_t flxNTPESP32::get_epoch(void)
{

    time_t now;
    time(&now);
    return now;
}

bool flxNTPESP32::valid_epoch(void)
{
    return _isEnabled && sntp_enabled();
}

#endif