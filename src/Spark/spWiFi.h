

#pragma once

// Implements WiFi specific interfaces and an action to set password via
// an external device/source (NFC for example)

#include "spSpark.h"
#include <string>

// Define interfaces for WifiDevice and WiFiCredentialSource

// WiFi Device - control interface
class spIWiFiDevice
{
  public:
    virtual bool connect(void) = 0;
    virtual void disconnect(void) = 0;
    virtual bool isConnected() = 0;
    virtual void setSSID(std::string &) = 0;
    virtual void setPassword(std::string &) = 0;
};

class spIWiFiCredentialSource
{
  public:
    virtual std::string getSSID() = 0;
    virtual std::string getPassword() = 0;
    virtual spSignalVoid &getUpdateEvent(void) = 0;
};

class spSetWifiCredentials : public spActionType<spSetWifiCredentials>
{
public:
    spSetWifiCredentials() : _source{nullptr}, _targetDevice{nullptr}
    {
        spRegister(enabled, "Enabled", "Enable updating WiFi credentials from an external device.");

        spark.add(this);
    };

    void setWiFiDevice(spIWiFiDevice *theDevice)
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
            spLog_I(F("Updating WiFi Credentials. Network: %s"), ssid.c_str());
            if (_targetDevice->isConnected())
                _targetDevice->disconnect();
            _targetDevice->setSSID(ssid);
            _targetDevice->setPassword(password);

            if (_targetDevice->connect())
                spLog_I(F("Connected to %s"), ssid.c_str());
            else
                spLog_E(F("Unable to connect to %s"), ssid.c_str());
        }
    }
    void setCredentialSource(spIWiFiCredentialSource *theSource)
    {
        if (!theSource)
            return;

        _source = theSource;

        spSignalVoid &theEvent = theSource->getUpdateEvent();

        // Register with the event.
        theEvent.call(this, &spSetWifiCredentials::onNewCredentials);
    }

    spPropertyBool<spSetWifiCredentials> enabled = {true};

  private:
    spIWiFiCredentialSource *_source;
    spIWiFiDevice *_targetDevice;
};