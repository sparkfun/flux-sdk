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

// Implements WiFi specific interfaces and an action to set password via
// an external device/source (NFC for example)

#include "flxFlux.h"
#include <string>

// Define interfaces for WifiDevice and WiFiCredentialSource

// WiFi Device - control interface
class flxIWiFiDevice
{
  public:
    virtual bool connect(void) = 0;
    virtual void disconnect(void) = 0;
    virtual bool isConnected() = 0;
    virtual void setSSID(std::string &) = 0;
    virtual void setPassword(std::string &) = 0;
};

class flxIWiFiCredentialSource
{
  public:
    virtual std::string getSSID() = 0;
    virtual std::string getPassword() = 0;
    virtual flxSignalVoid &getUpdateEvent(void) = 0;
};

class flxSetWifiCredentials : public flxActionType<flxSetWifiCredentials>
{
public:
    flxSetWifiCredentials() : _source{nullptr}, _targetDevice{nullptr}
    {
        flxRegister(enabled, "Enabled", "Enable updating WiFi credentials from an external device.");

        flux.add(this);
    };

    void setWiFiDevice(flxIWiFiDevice *theDevice)
    {
        if (theDevice)
            _targetDevice = theDevice;
    }

    void onNewCredentials(void)
    {
        std::string password = _source->getPassword();
        std::string ssid = _source->getSSID();

        // Actual data?
        if (password.length() > 0 && ssid.length() > 0)
        {
            flxLog_I(F("New WiFi Credentials detected - updating to network: %s"), ssid.c_str());
            if (_targetDevice->isConnected())
                _targetDevice->disconnect();
            _targetDevice->setSSID(ssid);
            _targetDevice->setPassword(password);

            if (_targetDevice->connect())
                flxLog_I(F("Connected to %s"), ssid.c_str());
            else
                flxLog_E(F("Unable to connect to %s"), ssid.c_str());
        }
    }
    void setCredentialSource(flxIWiFiCredentialSource *theSource)
    {
        if (!theSource)
            return;

        _source = theSource;

        flxSignalVoid &theEvent = theSource->getUpdateEvent();

        // Register with the event.
        theEvent.call(this, &flxSetWifiCredentials::onNewCredentials);
    }

    flxPropertyBool<flxSetWifiCredentials> enabled = {true};

  private:
    flxIWiFiCredentialSource *_source;
    flxIWiFiDevice *_targetDevice;
};