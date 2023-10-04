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
 *  flxDevTEMPLATE.cpp // REPLACE ME //
 *
 *  Spark Device object for the TEMPLATE device. // REPLACE ME //
 *
 *
 *
 */

#include <Arduino.h>

#include "flxDevTEMPLATE.h" /* REPLACE ME */

#define kTEMPLATEDEVAddressDefault TEMPLATEDEV_ADDRESS /* REPLACE ME */
#define kTEMPLATEDEVAddressSecondary TEMPLATEDEV_SECONDARY_ADDRESS /* OPTIONAL: REPLACE ME */

#define kTemplateID 0x00 /* OPTIONAL: REPLACE ME */

// Define our class static variables - allocs storage for them
uint8_t flxDevTEMPLATE::defaultDeviceAddress[] = {kTEMPLATEDEVAddressDefault, kTEMPLATEDEVAddressSecondary, kSparkDeviceAddressNull}; /* REPLACE ME */

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevTEMPLATE /* REPLACE ME */);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevTEMPLATE::flxDevTEMPLATE() /* REPLACE ME */
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "TEMPLATE DEVICE DESCRIPTION" /* REPLACE ME */);

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties and parameters

    // Register Properties
    flxRegister(deviceProperty1 /* REPLACE ME */, "PROPERTY1 NAME" /* REPLACE ME */, "PROPERTY1 DESCRIPTION" /* REPLACE ME */);
    flxRegister(deviceProperty2 /* REPLACE ME */, "PROPERTY2 NAME" /* REPLACE ME */, "PROPERTY2 DESCRIPTION" /* REPLACE ME */);

    // Register Output Parameters
    flxRegister(senseValue1 /* REPLACE ME */, "SENSE1 NAME" /* REPLACE ME */, "SENSE1 DESCRIPTION" /* REPLACE ME */);

    // Register Input Parameters
    flxRegister(compensationValue1 /* REPLACE ME */, "COMPENSATION1 NAME" /* REPLACE ME */, "COMPENSATION1 DESCRIPTION" /* REPLACE ME */);
}

//-----------------------------------------------------------------------------
// Static method used to determine if device is connected before creating the 
// object (if creating dynamically)
bool flxDevTEMPLATE/* REPLACE ME */::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;
    
    // TODO: Add in a way to verify that you're talking to the right device, such as verifying serial number, product ID, etc.
    uint8_t readValue = 0;

    return (readValue == kTemplateID); /* REPLACE ME */
}

//-----------------------------------------------------------------------------
// onInitialize
//
// Called during the startup/initialization of the driver (after the 
// constructor is called).
//
// Place to initialize the underlying device library/driver.
bool flxDevTEMPLATE/* REPLACE ME */::onInitialize(TwoWire &wirePort)
{
    // TODO: Any kind of initialization and verification that it's up and running, etc. 
    return TEMPLATECLASS::begin(wirePort); /* REPLACE ME */
}

/* Read methods for parameters */

uint flxDevTEMPLATE/* REPLACE ME */::read_SENSORVALUE1() /* REPLACE ME */
{
    // TODO: Get that good stuff from the sensor
}

/* Write methods for parameters */

void flxDevTEMPLATE/* REPLACE ME */::write_COMPENSATIONVALUE1(const uint16_t &value /* REPLACE ME */) /* REPLACE ME */
{
    // TODO: Write some value to the sensor
}

/* Getter methods for properties */

bool flxDevTEMPLATE/* REPLACE ME */::get_DEVICEPROPERTY1() /* REPLACE ME */
{
    // TODO: Get some state/status from the sensor and return it
}

uint flxDevTEMPLATE/* REPLACE ME */::get_DEVICEPROPERTY2() /* REPLACE ME */
{
    // TODO: Get some value from the sensor and return it
}

/* Setter methods for properties */

void flxDevTEMPLATE/* REPLACE ME */::set_DEVICEPROPERTY1(bool someBool) /* REPLACE ME */
{
    // TODO: Set some state/status based on someBool
}

void flxDevTEMPLATE/* REPLACE ME */::set_DEVICEPROPERTY2(uint someUint) /* REPLACE ME */
{
    // TODO: Set some value based on someUint
}
