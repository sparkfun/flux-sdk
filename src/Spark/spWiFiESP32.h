

#pragma once

#ifdef ESP32

#include "spSpark.h"
#include "spNetwork.h"
#include "spWiFi.h"

// WiFi client for EsP32 boards

class spWiFiESP32 : public flxActionType<spWiFiESP32>, public spNetwork, public spIWiFiDevice
{
private:

	void set_isEnabled(bool bEnabled);
	bool get_isEnabled(void);


public:
    spWiFiESP32() : _wasConnected{false}, _isEnabled{true}
    {

        spRegister(SSID, "Network Name", "The SSID of the WiFi network");
        spRegister(password, "Password", "The Password to connect to the WiFi network");
        spRegister(enabled, "Enabled", "Enable or Disable the WiFi Network connection");


        setName("WiFi Network", "WiFi network connection for the system");

        spark.add(this);
    };

    // Properties 
    flxPropertyString<spWiFiESP32>      	SSID;
    flxPropertySecureString<spWiFiESP32>		password;
    flxPropertyRWBool<spWiFiESP32, &spWiFiESP32::get_isEnabled, &spWiFiESP32::set_isEnabled> enabled;

    bool loop(void);

    bool connect(void);
    void disconnect(void);

    bool isConnected();

    bool initialize(void);

    void setSSID(std::string & theSSID)
    {
        SSID = theSSID;
    }

    void setPassword(std::string & thePassword)
    {
        password = thePassword;
    }
private:

	// flag used to help with connection changes. 
	bool  _wasConnected;
	bool  _isEnabled;
};

#endif