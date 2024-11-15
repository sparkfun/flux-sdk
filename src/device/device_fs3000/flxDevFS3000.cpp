/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevFS3000.cpp
 *
 *  Spark Device object for the FS3000 device.
 *
 *
 */

#include "Arduino.h"

#include "flxDevFS3000.h"

uint8_t flxDevFS3000::defaultDeviceAddress[] = {FS3000_DEVICE_ADDRESS, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevFS3000);

flxDevFS3000::flxDevFS3000()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("FS3000 air velocity sensor");

    // Register output params
    flxRegister(flow_mps, "Flow (MPS)", "Flow (Metres Per Second)", kParamValueMPS);
    flxRegister(flow_mph, "Flow (MPH)", "Flow (Miles Per Hour)", kParamValueMPH);

    // Register property
    flxRegister(fs3000version, "FS3000 Version", "FS3000 Sensor Version");
}

// Methods to read the parameters
float flxDevFS3000::read_mps()
{
    return FS3000::readMetersPerSecond();
}
float flxDevFS3000::read_mph()
{
    return FS3000::readMilesPerHour();
}

// Static method used to determine if this device is connected

bool flxDevFS3000::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Initialize checksumData to something non-zero.
    // If checksumData is initialized to zero and the read fails, sum will also be zero and taken as valid.
    uint8_t checksumData[5] = {0xFF};
    if (i2cDriver.receiveResponse(address, checksumData, 5) != 5) // The FS3000 has no registers..
        return false;

    uint8_t sum = 0;
    for (uint8_t i = 0; i <= 4; i++)
    {
        sum += checksumData[i];
    }

    return (sum == 0x00);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevFS3000::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    bool result = FS3000::begin(wirePort);

    if (!result)
        flxLog_E("FS3000 - begin failed");

    if (result)
    {
        if (_fs3000_1005)
            FS3000::setRange(AIRFLOW_RANGE_7_MPS); // FS3000-1005
        else
            FS3000::setRange(AIRFLOW_RANGE_15_MPS); // FS3000-1015
    }

    return result;
}

// Read-write properties
uint8_t flxDevFS3000::get_fs3000_version()
{
    return (uint8_t)_fs3000_1005;
}
void flxDevFS3000::set_fs3000_version(uint8_t is1005)
{
    _fs3000_1005 = (bool)is1005;
    if (isInitialized())
    {
        if (_fs3000_1005)
            FS3000::setRange(AIRFLOW_RANGE_7_MPS); // FS3000-1005
        else
            FS3000::setRange(AIRFLOW_RANGE_15_MPS); // FS3000-1015
    }
}
