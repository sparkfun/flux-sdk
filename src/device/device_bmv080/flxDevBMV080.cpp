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
 *  flxDevBMV080.h
 *
 *  Spark Device object for the VEML7700 device.
 *
 *
 *
 */

#include "flxDevBMV080.h"

// Define our class static variables - allocs storage for them

#define kBMV080AddressDefault SF_BMV080_DEFAULT_ADDRESS

uint8_t flxDevBMV080::defaultDeviceAddress[] = {kBMV080AddressDefault, kSparkDeviceAddressNull};

const uint16_t kDutyCycleDefault = 20;
//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBMV080);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBMV080::flxDevBMV080()
    : _obstructedEnabled{true}, _operatingMode{SF_BMV080_MODE_CONTINUOUS}, _dutyCycle{kDutyCycleDefault}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("BMV080 Particulate Matter Sensor");

    // Register parameters
    flxRegister(enableObstructed, "Obstruction Detection", "Enable or disable obstruction detection");

    flxRegister(operatingMode, "Operating Mode", "Continuous or Duty Cycle");
    flxRegister(dutyCycle, "Duty Cycle", "The duty cycle (secs) when in duty cycle mode");

    // Register read-write properties
    flxRegister(PM10, "PM10", "The PM10 concentration in micrograms per cubic meter (µg/m³)");
    flxRegister(PM25, "PM25", "The measurement sensitivity");
    flxRegister(PM1, "PM1", "The measurement persistence");
    flxRegister(obstructed, "Obstructed", "Is the sensor obstructed?");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBMV080::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // The BMV080 just has support for PING detection at the default address
    return i2cDriver.ping(address);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBMV080::onInitialize(TwoWire &wirePort)
{

    if (SparkFunBMV080::begin(address(), wirePort) == false)
    {
        flxLog_D(F("BMV080: Failed to initialize device at address 0x%02X"), address());
        return false;
    }

    if (SparkFunBMV080::init() == false)
    {
        flxLog_D(F("BMV080: Failed to initialize device"));
        return false;
    }

    // Set the obstruction detection state. And force the change
    _set_enable_obstructed(_obstructedEnabled, true);

    // set a duty cycle - do before setting the MODE.
    if (_operatingMode == SF_BMV080_MODE_DUTY_CYCLE)
    {
        if (!SparkFunBMV080::setDutyCyclingPeriod(_dutyCycle))
            flxLog_W(F("BMV080: Failed to set duty cycle: %d seconds"), _dutyCycle);
    }
    // op mode
    if (SparkFunBMV080::setMode(_operatingMode) == false)
    {
        flxLog_E(F("BMV080: Failed to set operating mode: %d"), _operatingMode);
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------
// execute()
//
bool flxDevBMV080::execute(void)
{
    return SparkFunBMV080::readSensor();
}

//----------------------------------------------------------------------------------------------------------
// Property Getters and Setters
//-----------------------------------------------------------------------------------------------------------
//
// get_enable_obstructed()
//
// Returns the current state of the enable obstructed property.
bool flxDevBMV080::get_enable_obstructed(void)
{
    return _obstructedEnabled;
}

void flxDevBMV080::_set_enable_obstructed(bool enable, bool force)
{
    if (_obstructedEnabled == enable && isInitialized() && !force)
        return;

    _obstructedEnabled = enable;

    // Has this device been setup yet an/or should we force the change?
    // Why have "force"? - This method manages the IsObstructed output parameter,
    // and we want to leverage this before the device's isInitialized flag is set.
    if (!isInitialized() && !force)
        return;

    // Set the obstruction detection state
    if (!SparkFunBMV080::setDoObstructionDetection(enable))
        flxLog_W(F("BMV080: Failed to set obstruction detection state: %s"), enable ? "Enabled" : "Disabled");

    // Set if the output variable is disabled or not
    obstructed.setEnabled(enable);
}

// For the property setter
void flxDevBMV080::set_enable_obstructed(bool enable)
{
    // relay
    _set_enable_obstructed(enable, false);
}

// Operating Mode - continuous or duty cycle
uint8_t flxDevBMV080::get_operating_mode(void)
{
    return _operatingMode;
}
void flxDevBMV080::set_operating_mode(uint8_t mode)
{
    if (mode == _operatingMode && isInitialized())
        return;

    _operatingMode = mode;

    // Has this device been setup yet?
    if (!isInitialized())
        return;

    // Set the operating mode
    if (!SparkFunBMV080::setMode(mode))
        flxLog_E(F("BMV080: Failed to set operating mode: %d"), mode);
}

// duty cycle
uint16_t flxDevBMV080::get_duty_cycle(void)
{
    return _dutyCycle;
}
void flxDevBMV080::set_duty_cycle(uint16_t dutyCycle)
{
    if (dutyCycle == _dutyCycle && isInitialized())
        return;

    _dutyCycle = dutyCycle;
    // Has this device been setup yet?
    if (!isInitialized())
        return;

    if (!SparkFunBMV080::setDutyCyclingPeriod(_dutyCycle))
        flxLog_E(F("BMV080: Failed to set duty cycle: %d seconds"), _dutyCycle);
}
