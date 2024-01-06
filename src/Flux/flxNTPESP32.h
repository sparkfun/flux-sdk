/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#ifdef ESP32

#include "flxFlux.h"
#include "flxNetwork.h"

#include "flxClock.h"

#define kNTPServerAddress1 "pool.ntp.org"
#define kNTPServerAddress2 "time.nist.gov"

// Use the NTP client on the ESP32 to sync the onboard clock

class flxNTPESP32 : public flxActionType<flxNTPESP32>, public flxIClock
{
  private:
    void set_isEnabled(bool bEnabled);
    bool get_isEnabled(void);

    // Event callback
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected);

  public:
    flxNTPESP32() : _isEnabled{true}, _theNetwork{nullptr}, _startupDelay{0}
    {
        flxRegister(enabled, "Enabled", "Enable or Disable the NTP Client");

        // NTP servers
        flxRegister(ntpServerOne, "NTP Server One", "Primary NTP server");
        flxRegister(ntpServerTwo, "NTP Server Two", "Secondary NTP server");

        setName("NTP Client", "NTP Time Sync Client");

        flux.add(this);
    };

    bool start(void);
    void stop(void);

    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        flxRegisterEventCB(flxEvent::kOnConnectionChange, this, &flxNTPESP32::onConnectionChange);
    }
    void setStartupDelay(uint delay)
    {
        _startupDelay = delay;
    }
    uint startupDelay(void)
    {
        return _startupDelay;
    }

    // timing/clock upating interface methods
    // Note - the NTP updates run in the background for the ESP32

    uint get_epoch(void);
    void set_epoch(const uint &)
    {
    }
    bool valid_epoch(void);

    // Properties

    // Enabled/Disabled
    flxPropertyRWBool<flxNTPESP32, &flxNTPESP32::get_isEnabled, &flxNTPESP32::set_isEnabled> enabled;

    // NTP servers
    flxPropertyString<flxNTPESP32> ntpServerOne = {kNTPServerAddress1};
    flxPropertyString<flxNTPESP32> ntpServerTwo = {kNTPServerAddress2};

  private:
    bool _isEnabled;

    flxNetwork *_theNetwork;

    uint _startupDelay;
};

#endif