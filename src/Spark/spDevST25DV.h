/*
 *
 *  spDevST25DV.h
 *
 *  Device object for the Qwiic ST25DV device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_ST25DV64KC_Arduino_Library.h"
#include "spDevice.h"

// What is the name used to ID this device?
#define kST25DVDeviceName "st25dv"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevST25DV : public spDeviceType<spDevST25DV>, public SFE_ST25DV64KC_NDEF
{

  public:
    spDevST25DV();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kST25DVDeviceName;
    };
    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];
    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // Events
    // Our output event
    spSignalVoid new_WiFi_record;

    bool loop(void);

  private:
    #define MAX_SSID_PASSWORD_LEN 64

    char _readSsid[MAX_SSID_PASSWORD_LEN] = {'\0'};
    char _readPassword[MAX_SSID_PASSWORD_LEN] = {'\0'};
    char _previousSsid[MAX_SSID_PASSWORD_LEN] = {'\0'};
    char _previousPassword[MAX_SSID_PASSWORD_LEN] = {'\0'};

    // Flags to prevent readNDEFWiFi being called multiple times
    bool _ssid = false;
    bool _password = false;

    // Flags to prevent writeNDEFWiFi being called multiple times
    bool _ssidW = false;
    bool _passwordW = false;

    // methods for our read-write properties
    std::string get_ssid();
    void set_ssid(std::string);
    std::string get_password();
    void set_password(std::string);

  public:
    spPropertyRWString<spDevST25DV, &spDevST25DV::get_ssid, &spDevST25DV::set_ssid> ssid;
    spPropertyRWString<spDevST25DV, &spDevST25DV::get_password, &spDevST25DV::set_password> password;
};
