

#pragma once

#ifdef ESP32

#include "flxFlux.h"
#include "flxNetwork.h"


#define kNTPServerAddress1 "pool.ntp.org"
#define kNTPServerAddress2 "time.nist.gov"

#define kNTPTimeZoneSparkFun "MST7MDT,M3.2.0,M11.1.0"

// Use the NTP client on the ESP32 to sync the onboard clock

class flxNTPESP32 : public flxActionType<flxNTPESP32>
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
        flxRegister(ntpServerOne, "NTP Server One", "The primary NTP server to use");
        flxRegister(ntpServerTwo, "NTP Server Two", "The secondary NTP server to use");

        // Timezone
        flxRegister(timeZone, "The Time Zone", "Time zone setting string for the device");

        setName("NTP Client", "NTP Time Synch Client");

        flux.add(this);
    };

    bool start(void);
    void stop(void);

    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(flxSignalBool &theEvent)
    {
        // Regisgter to get notified on connection changes
        theEvent.call(this, &flxNTPESP32::onConnectionChange);
    }

    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    void setStartupDelay(uint delay)
    {
        _startupDelay=delay;
    }
    uint startupDelay(void)
    {
        return _startupDelay;
    }
    // Properties 

    // Enabled/Disabled
    flxPropertyRWBool<flxNTPESP32, &flxNTPESP32::get_isEnabled, &flxNTPESP32::set_isEnabled> enabled;

    // NTP servers
    flxPropertyString<flxNTPESP32>     ntpServerOne = {kNTPServerAddress1};
    flxPropertyString<flxNTPESP32>     ntpServerTwo = {kNTPServerAddress2};

    // TimeZone string
    flxPropertyString<flxNTPESP32>     timeZone = {kNTPTimeZoneSparkFun};

private:
	bool  _isEnabled;

    flxNetwork *_theNetwork;

    uint _startupDelay;
};

#endif