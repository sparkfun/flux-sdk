

#pragma once

#ifdef ESP32

#include "spSpark.h"


// WiFi client for EsP32 boards

class spWiFiESP32 : public spActionType<spWiFiESP32>
{
private:

	void set_isEnabled(bool bEnabled);
	bool get_isEnabled(void);


public:
    spWiFiESP32() : _wasConnected{false}, _isEnabled{true}
    {

        spRegister(SSID, "SSID", "The SSID of the WiFi network");
        spRegister(password, "Password", "The Password to connect to the WiFi network");
        spRegister(enabled, "Enabled", "Enable or Disable the WiFi Network connection");


        setName("WiFi", "WiFi network connection");

        spark.add(this);
    };


    // Event - triggered on connection changes
    spSignalBool on_connectionChange;

    // Properties 
    spPropertyString<spWiFiESP32>      	SSID;
    spPropertyString<spWiFiESP32>		password;
    spPropertyRWBool<spWiFiESP32, &spWiFiESP32::get_isEnabled, &spWiFiESP32::set_isEnabled> enabled;

    bool loop(void);

    bool connect(void);
    void disconnect(void);

    bool isConnected();

    bool initialize(void);

private:

	// flag used to help with connection changes. 
	bool  _wasConnected;
	bool  _isEnabled;
};

#endif