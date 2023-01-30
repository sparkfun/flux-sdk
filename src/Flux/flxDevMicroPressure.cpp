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
 *  flxDevMicroPressure.cpp
 *
 *  Spark Device object for the MicroPressure device.
 * 
 * 
 */

#include "Arduino.h"

#include "flxDevMicroPressure.h"

// The default address is named DEFAULT_ADDRESS... Let's not use that...
uint8_t flxDevMicroPressure::defaultDeviceAddress[] = { 0x18, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevMicroPressure);

flxDevMicroPressure::flxDevMicroPressure()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("MPR series Micro-Pressure sensor from Honeywell");

    // Register output params
    flxRegister(pressure_PSI, "Pressure (PSI)", "Atmospheric pressure in Pounds per Square Inch");
    flxRegister(pressure_Pa, "Pressure (Pa)", "Atmospheric pressure in Pascals");
    flxRegister(pressure_kPa, "Pressure (kPa)", "Atmospheric pressure in kilo-Pascals");
    flxRegister(pressure_torr, "Pressure (torr)", "Atmospheric pressure in torr");
    flxRegister(pressure_inHg, "Pressure (inHg)", "Atmospheric pressure in inches of mercury");
    flxRegister(pressure_atm, "Pressure (atm)", "Atmospheric pressure in atmospheres");
    flxRegister(pressure_bar, "Pressure (bar)", "Atmospheric pressure in bar");
}

// Function to encapsulate the ops needed to get values from the sensor.
float flxDevMicroPressure::read_pressure_PSI()
{
    return SparkFun_MicroPressure::readPressure();
}
float flxDevMicroPressure::read_pressure_Pa()
{
    return SparkFun_MicroPressure::readPressure(PA);
}
float flxDevMicroPressure::read_pressure_kPa()
{
    return SparkFun_MicroPressure::readPressure(KPA);
}
float flxDevMicroPressure::read_pressure_torr()
{
    return SparkFun_MicroPressure::readPressure(TORR);
}
float flxDevMicroPressure::read_pressure_inHg()
{
    return SparkFun_MicroPressure::readPressure(INHG);
}
float flxDevMicroPressure::read_pressure_atm()
{
    return SparkFun_MicroPressure::readPressure(ATM);
}
float flxDevMicroPressure::read_pressure_bar()
{
    return SparkFun_MicroPressure::readPressure(BAR);
}

// Static method used to determine if this device is connected

bool flxDevMicroPressure::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the sensor status
    uint8_t status = 0;
    int response = i2cDriver.receiveResponse(address, &status, 1);

    return ((response == 1) && ((status & 0x5A) == 0x40));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMicroPressure::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = SparkFun_MicroPressure::begin(address(), wirePort);

    if (!result)
        flxLog_E("MicroPressure - begin failed");

    return result;
}

