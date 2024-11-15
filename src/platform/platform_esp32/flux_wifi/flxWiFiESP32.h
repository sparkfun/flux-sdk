/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#ifdef ESP32

#include "flxCoreJobs.h"
#include "flxFlux.h"
#include "flxNetwork.h"
#include "flxWiFi.h"

// ESP wifi header
#include "WiFi.h"

const uint8_t kWiFiLevelPoor = 0;
const uint8_t kWiFiLevelFair = 1;
const uint8_t kWiFiLevelGood = 2;
const uint8_t kWiFiLevelExcellent = 3;

const uint16_t kWiFiUpdateHandlerTimeMS = 1500;
// WiFi client for EsP32 boards

class flxWiFiESP32 : public flxActionType<flxWiFiESP32>, public flxNetwork, public flxIWiFiDevice
{
  private:
    void set_isEnabled(bool bEnabled);
    bool get_isEnabled(void);

  public:
    flxWiFiESP32() : _wasConnected{false}, _isEnabled{true}, _delayedStartup{false}
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

        _theJob.setup("WiFi", kWiFiUpdateHandlerTimeMS, this, &flxWiFiESP32::jobHandlerCB);
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

    String connectedSSID(void);

    IPAddress localIP(void)
    {
        return WiFi.localIP();
    }

    int8_t RSSI(void)
    {
        return WiFi.RSSI();
    }

    uint rating(void);

    // used to prevent auto connect when init called - rare case
    void setDelayedStartup(bool bDelay = true)
    {
        _delayedStartup = bDelay;
    }

  private:
    void jobHandlerCB(void);

    // flag used to help with connection changes.
    bool _wasConnected;
    bool _isEnabled;
    bool _delayedStartup;

    flxJob _theJob;
};

#endif