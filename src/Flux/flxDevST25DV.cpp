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
 *  flxDevST25DV.cpp
 *
 *  Device object for the Qwiic ST25DV device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevST25DV.h"

/*
  DATA = 0x53,          // E2 = 0, E1 = 1
  SYSTEM = 0x57,        // E2 = 1, E1 = 1
  RF_SWITCH_OFF = 0x51, // E2 = 0, E1 = 0
  RF_SWITCH_ON = 0x55,  // E2 = 1, E1 = 0
*/
uint8_t flxDevST25DV::defaultDeviceAddress[] = {0x57, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevST25DV);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevST25DV::flxDevST25DV()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ST25DV Dynamic RFID Tag");

    // Register Properties
    flxRegister(ssid, "WiFi Record SSID", "The Service Set Identifier held in the WiFi record");
    flxRegister(password, "WiFi Record Key / Password", "The key (password) held in the WiFi record");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevST25DV::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the ST Product Code. Check it is 0x50 or 0x51
    // Note: these values are different to the ones in the ST25DVxxKC datasheet!
    // We found 0x51 by reading the device, and in this App Note:
    // https://www.st.com/resource/en/application_note/an5675-st25dv64kcdisco-firmware-documentation-stmicroelectronics.pdf
    uint8_t icRefReg[2] = { 0x00, 0x1D };
    if (!i2cDriver.write(address, icRefReg, 2))
        return false;
    uint8_t icRef = 0;
    if (i2cDriver.receiveResponse(address, &icRef, 1) != 1)
        return false;

    if ((icRef == 0x50) || (icRef == 0x51))
    {
        flxLog_I("ST25DV: isConnected icRef 0x%x", icRef);
        return true;
    }
    else
    {
        flxLog_E("ST25DV: isConnected unexpected icRef 0x%x", icRef);
        return false;
    }
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevST25DV::onInitialize(TwoWire &wirePort)
{

    bool rc = SFE_ST25DV64KC_NDEF::begin(wirePort);

    if (!rc)
        flxLog_E("ST25DV - begin failed");

    return rc;
}

// methods for the read-write properties
std::string flxDevST25DV::get_ssid()
{
    bool success = false;
    if (!_ssid)
    {
        success = SFE_ST25DV64KC_NDEF::readNDEFWiFi(_readSsid, MAX_SSID_PASSWORD_LEN, _readPassword, MAX_SSID_PASSWORD_LEN); // Read the first WiFi Record
    }
    if (success)
    {
        _password = true;
    }
    _ssid = false;

    std::string theString = _readSsid;

    return theString;
}
void flxDevST25DV::set_ssid(std::string newSsid)
{

}
std::string flxDevST25DV::get_password()
{
    bool success = false;
    if (!_password)
    {
        success = SFE_ST25DV64KC_NDEF::readNDEFWiFi(_readSsid, MAX_SSID_PASSWORD_LEN, _readPassword, MAX_SSID_PASSWORD_LEN); // Read the first WiFi Record
    }
    if (success)
    {
        _ssid = true;
    }
    _password = false;

    std::string theString = _readPassword;

    return theString;
}
void flxDevST25DV::set_password(std::string newPassword)
{
    
}

//----------------------------------------------------------------------------------------------------------
// Loop

bool flxDevST25DV::loop(void)
{
    bool result = false;

    static unsigned long lastMillis = millis() - 14000; // Read the tag after 1 second, first time around

    if (millis() > (lastMillis + 15000)) // Check tag every 15 seconds to avoid possible I2C/RF collisions
    {
        if (SFE_ST25DV64KC_NDEF::RFFieldDetected()) // Do a quick RF check first
        {
            lastMillis = millis() - 10000; // Wait another 5 seconds if an RF field was detected
        }
        else
        {
            lastMillis = millis(); // Update lastMillis
            bool success = SFE_ST25DV64KC_NDEF::readNDEFWiFi(_readSsid, MAX_SSID_PASSWORD_LEN, _readPassword, MAX_SSID_PASSWORD_LEN); // Read the first WiFi Record
            if (success && (strlen(_readSsid) > 0) && (strlen(_readPassword) > 0)
                && ((strcmp(_readSsid, _previousSsid) != 0) || (strcmp(_readPassword, _previousPassword) != 0))) // If either the SSID or Password has changed
            {
                strcpy(_previousPassword, _readPassword); // Update the previous SSID and Password
                strcpy(_previousSsid, _readSsid);
                new_WiFi_record.emit();
                result = true;
            }
        }
    }

    return result;
}
