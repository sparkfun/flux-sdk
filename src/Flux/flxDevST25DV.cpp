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

const uint32_t kST25DVTimeOutPeriod = 15000;
const uint32_t kST25DVFieldDetectPeriod = 5000;
//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevST25DV);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevST25DV::flxDevST25DV() : _sPassword{""}, _sSSID{""}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ST25DV Dynamic RFID Tag");

    // Register Properties
    flxRegister(ssid, "WiFi Record SSID", "The SSID held in the WiFi record");
    flxRegister(password, "WiFi Record Key / Password", "The password held in the WiFi record");

    // The update job used for this device - start with a 1 second timeout.
    _theJob.setup("ST25DV Device", 1000, this, &flxDevST25DV::jobHandlerCB);
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
    uint8_t icRefReg[2] = {0x00, 0x1D};
    if (!i2cDriver.write(address, icRefReg, 2))
        return false;
    uint8_t icRef = 0;
    if (i2cDriver.receiveResponse(address, &icRef, 1) != 1)
        return false;

    if ((icRef == 0x50) || (icRef == 0x51))
    {
        // flxLog_I("ST25DV: isConnected icRef 0x%x", icRef);
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
        flxLogM_E(kMsgErrDeviceInit, name(), "start");
    else
        flxAddJobToQueue(_theJob); // start our timer job

    return rc;
}

//----------------------------------------------------------------------------------------------------------
bool flxDevST25DV::updateWiFiRecord(void)
{

    char szPassword[kRecordElementLength];
    char szSSID[kRecordElementLength];

    bool status = SFE_ST25DV64KC_NDEF::readNDEFWiFi(szSSID, kRecordElementLength, (char *)szPassword,
                                                    kRecordElementLength); // Read the first WiFi Record

    if (!status)
        return false;

    // Anything change?
    status = _sPassword.compare(szPassword) != 0 || _sSSID.compare(szSSID) != 0;

    if (status)
    {
        _sPassword = szPassword;
        _sSSID = szSSID;
    }
    return status;
}

//----------------------------------------------------------------------------------------------------------
// methods for the read-write properties
std::string flxDevST25DV::get_ssid()
{
    if (_sSSID.length() == 0)
    {
        // any updates available?
        updateWiFiRecord();
    }
    return _sSSID;
}
//----------------------------------------------------------------------------------------------------------
void flxDevST25DV::set_ssid(std::string newSsid)
{
}

//----------------------------------------------------------------------------------------------------------
std::string flxDevST25DV::get_password()
{

    if (_sPassword.length() == 0)
    {
        // Any updates available?
        updateWiFiRecord();
    }

    return _sPassword;
}
//----------------------------------------------------------------------------------------------------------
//
void flxDevST25DV::set_password(std::string newPassword)
{
}

//----------------------------------------------------------------------------------------------------------
// Timed job callback handler

void flxDevST25DV::jobHandlerCB(void)
{

    // update the jobs period - give the device 5 seconds to try to detect?
    if (SFE_ST25DV64KC_NDEF::RFFieldDetected()) // Do a quick RF check first
        _theJob.setPeriod(kST25DVFieldDetectPeriod);
    else
    {
        if (updateWiFiRecord())
            new_WiFi_record.emit();

        // tweak the period if needed - back to normal check level after startup
        if (_theJob.period() != kST25DVTimeOutPeriod)
            _theJob.setPeriod(kST25DVTimeOutPeriod);
    }
}
