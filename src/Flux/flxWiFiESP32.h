/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
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
#include "flxWiFi.h"

// WiFi client for EsP32 boards

class flxWiFiESP32 : public flxActionType<flxWiFiESP32>, public flxNetwork, public flxIWiFiDevice
{
  private:
    void set_isEnabled(bool bEnabled);
    bool get_isEnabled(void);

  public:
    flxWiFiESP32() : _wasConnected{false}, _isEnabled{true}
    {

        flxRegister(enabled, "Enabled", "Enable or Disable the WiFi Network connection");

        flxRegister(SSID, "Network Name", "The SSID of the WiFi network");
        flxRegister(password, "Password", "The Password to connect to the WiFi network");

        flxRegister(alt1_SSID, "Network 2 Name", "Alternative network 2 SSID");
        flxRegister(alt1_password, "Network 2 Password", "Alternative network 2 Password");

        flxRegister(alt2_SSID, "Network 3 Name", "Alternative network 3 SSID");
        flxRegister(alt2_password, "Network 3 Password", "Alternative network 3 Password");

        flxRegister(alt3_SSID, "Network 4 Name", "Alternative network 4 SSID");
        flxRegister(alt3_password, "Network 4 Password", "Alternative network 4 Password");

        setName("WiFi Network", "WiFi network connection for the system");

        flux.add(this);
    };

    // Properties
    flxPropertyString<flxWiFiESP32> SSID;
    flxPropertySecureString<flxWiFiESP32> password;

    flxPropertyString<flxWiFiESP32> alt1_SSID;
    flxPropertySecureString<flxWiFiESP32> alt1_password;

    flxPropertyString<flxWiFiESP32> alt2_SSID;
    flxPropertySecureString<flxWiFiESP32> alt2_password;

    flxPropertyString<flxWiFiESP32> alt3_SSID;
    flxPropertySecureString<flxWiFiESP32> alt3_password;

    flxPropertyRWBool<flxWiFiESP32, &flxWiFiESP32::get_isEnabled, &flxWiFiESP32::set_isEnabled> enabled;

    bool loop(void);

    bool connect(void);
    void disconnect(void);

    bool isConnected();

    bool initialize(void);

    void setSSID(std::string &theSSID)
    {
        SSID = theSSID;
    }

    void setPassword(std::string &thePassword)
    {
        password = thePassword;
    }

    const char *connectedSSID(void);

  private:
    // flag used to help with connection changes.
    bool _wasConnected;
    bool _isEnabled;
};

#endif