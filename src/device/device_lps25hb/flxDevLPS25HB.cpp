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
 *  flxDevLPS25HB.cpp
 *
 *  Spark Device object for the LPS25HB device.
 *
 *
 */

#include "Arduino.h"

#include "flxDevLPS25HB.h"

uint8_t flxDevLPS25HB::defaultDeviceAddress[] = {LPS25HB_I2C_ADDR_DEF, LPS25HB_I2C_ADDR_ALT, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevLPS25HB);

flxDevLPS25HB::flxDevLPS25HB()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("LPS25HB Temperature and Pressure sensor");

    // Register output params
    flxRegister(temperatureC, "Temperature (C)", "The temperature in degrees C", kParamValueTempC);
    flxRegister(pressurehPa, "Pressure (hPa)", "The atmospheric pressure in hPa", kParamValuePressure_F);
}

// Function to encapsulate the ops needed to get values from the sensor.
float flxDevLPS25HB::read_temperature_c()
{
    return LPS25HB::getTemperature_degC();
}
float flxDevLPS25HB::read_pressure_hpa()
{
    return LPS25HB::getPressure_hPa();
}

// Static method used to determine if this device is connected

bool flxDevLPS25HB::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = i2cDriver.readRegister(address, LPS25HB_REG_WHO_AM_I);

    return (chipID == LPS25HB_DEVID);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevLPS25HB::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    bool result = LPS25HB::begin(wirePort, address());

    if (!result)
        flxLog_E("LPS25HB - begin failed");

    return result;
}
