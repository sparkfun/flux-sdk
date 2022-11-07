

#pragma once

#ifdef ESP32

#include "spSpark.h"
#include "spNetwork.h"


// Use the NTP client on the ESP32 to sync the onboard clock

class spNTPESP32 : public spActionType<spNTPESP32>
{
private:
    void set_isEnabled(bool bEnabled);
    bool get_isEnabled(void);

    // Event callback 
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected);

public:
    spNTPESP32() : _isEnabled{true}, _theNetwork{nullptr}, _startupDelay{0}
    {
        spRegister(enabled, "Enabled", "Enable or Disable the NTP Client");

        spRegister(gmtOffsetMinutes, "GMT Offset", "GMT offset in minutes for timezone settings");
        spRegister(daylightOffsetMinutes, "Daylight Offset", "Daylight Savings offset in minutes for timezone settings");        

        setName("NTP Client", "NTP Time Synch Client");

        spark.add(this);
    };

    bool start(void);
    void stop(void);

    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(spSignalBool &theEvent)
    {
        // Regisgter to get notified on connection changes
        theEvent.call(this, &spNTPESP32::onConnectionChange);
    }

    void setNetwork(spNetwork *theNetwork)
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
    spPropertyRWBool<spNTPESP32, &spNTPESP32::get_isEnabled, &spNTPESP32::set_isEnabled> enabled;

    // Time offsets
    spPropertyUint<spNTPESP32>       gmtOffsetMinutes = {0};
    spPropertyUint<spNTPESP32>       daylightOffsetMinutes = {0};

private:
	bool  _isEnabled;

    spNetwork *_theNetwork;

    uint _startupDelay;
};

#endif