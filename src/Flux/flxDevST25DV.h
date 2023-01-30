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
 
/*
 *
 *  flxDevST25DV.h
 *
 *  Device object for the Qwiic ST25DV device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_ST25DV64KC_Arduino_Library.h"
#include "flxDevice.h"

// For WiFi updating of credentials
#include "flxWiFi.h"

// What is the name used to ID this device?
#define kST25DVDeviceName "ST25DV"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevST25DV : public flxDeviceI2CType<flxDevST25DV>, public SFE_ST25DV64KC_NDEF, public flxIWiFiCredentialSource
{

  public:
    flxDevST25DV();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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
    flxSignalVoid new_WiFi_record;

    bool loop(void);

    // Methods for the IWiFiCredentialSource interface ...

    std::string getSSID(void)
    {
        return get_ssid();
    }

    std::string getPassword(void)
    {
        return get_password();
    }

    flxSignalVoid & getUpdateEvent(void)
    {
        return new_WiFi_record;
    }

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
    flxPropertyRWString<flxDevST25DV, &flxDevST25DV::get_ssid, &flxDevST25DV::set_ssid> ssid;
    flxPropertyRWString<flxDevST25DV, &flxDevST25DV::get_password, &flxDevST25DV::set_password> password;
};
