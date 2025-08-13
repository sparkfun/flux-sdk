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

// note: Data sheet default is 30 seconds
static const uint16_t kDutyCycleDefault = 30;
static const uint16_t kIntegrationTimeDefault = 10; // Default integration time in seconds
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
    : _obstructedEnabled{true}, _operatingMode{SF_BMV080_MODE_CONTINUOUS}, _dutyCycle{kDutyCycleDefault},
      _vibrationFilterEnabled{false}, _integrationTime{kIntegrationTimeDefault}, _isRunning{false}, _updateCnt{0}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("BMV080 Particulate Matter Sensor");

    // Register parameters
    flxRegister(enableObstructed, "Obstruction Detection", "Enable or disable obstruction detection");

    flxRegister(operatingMode, "Operating Mode", "Continuous or Duty Cycle");
    flxRegister(dutyCycle, "Duty Cycle", "The duty cycle (secs) when in duty cycle mode");
    flxRegister(enableVibrationFilter, "Vibration Filter", "Enable or disable vibration filtering");
    flxRegister(integrationTime, "Integration Time", "The integration time in seconds");

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

    // update counter is 0
    _updateCnt = 0;
    // setting parameters is defered until after any properties are restore - once
    // the system is up and started any param changes seem to require a restart.

    return true;
}
void flxDevBMV080::stopSensor(void)
{
    // Stop the sensor
    if (_isRunning)
    {
        SparkFunBMV080::close();
        _isRunning = false; // Set the running flag to false
    }
}
//----------------------------------------------------------------------------------------------------------
void flxDevBMV080::startSensor(void)
{
    if (SparkFunBMV080::init() == false)
    {
        flxLog_D(F("BMV080: Failed to initialize device"));
        return;
    }
    // Set the obstruction detection state.
    if (!SparkFunBMV080::setDoObstructionDetection(_obstructedEnabled))
        flxLog_W(F("BMV080: Failed to set obstruction detection state: %d"), _obstructedEnabled);

    if (!SparkFunBMV080::setDoVibrationFiltering(_vibrationFilterEnabled))
        flxLog_E(F("BMV080: Failed to set vibration filtering: %d"), _vibrationFilterEnabled);

    // Set the integration time
    if (!SparkFunBMV080::setIntegrationTime((float)_integrationTime))
        flxLog_E(F("BMV080: Failed to set integration time: %d seconds"), _integrationTime);

    // set a duty cycle - do before setting the MODE.
    if (_operatingMode == SF_BMV080_MODE_DUTY_CYCLE)
    {
        if (!SparkFunBMV080::setDutyCyclingPeriod(_dutyCycle))
            flxLog_W(F("BMV080: Failed to set duty cycle: %d seconds"), _dutyCycle);
    }
    // op mode
    if (SparkFunBMV080::setMode(_operatingMode) == false)
        flxLog_E(F("BMV080: Failed to set operating mode: %d"), _operatingMode);

    _isRunning = true; // Set the running flag to true
}

void flxDevBMV080::restoreComplete(void)
{
    // Finalize the setup
    startSensor();
}
//----------------------------------------------------------------------------------------------------------
// execute()
//
bool flxDevBMV080::execute(void)
{
    return SparkFunBMV080::readSensor();
}

// used to manage updates -- if a parameter to the device changes, we need to re-init
void flxDevBMV080::beginUpdate(void)
{
    _updateCnt++;
}
void flxDevBMV080::endUpdate(void)
{
    if (_updateCnt == 1)
    {
        // If we are running, restart to take into account the updated settings.
        if (_isRunning)
        {
            stopSensor();
            startSensor();
        }
    }
    _updateCnt--;
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

void flxDevBMV080::set_enable_obstructed(bool enable)
{
    if (_obstructedEnabled == enable)
        return;

    beginUpdate();
    _obstructedEnabled = enable;

    // Set if the output variable is disabled or not
    obstructed.setEnabled(enable);

    endUpdate();
}

// Operating Mode - continuous or duty cycle
uint8_t flxDevBMV080::get_operating_mode(void)
{
    return _operatingMode;
}
void flxDevBMV080::set_operating_mode(uint8_t mode)
{
    if (mode == _operatingMode)
        return;

    beginUpdate();
    _operatingMode = mode;
    endUpdate();
}

// duty cycle
uint16_t flxDevBMV080::get_duty_cycle(void)
{
    return _dutyCycle;
}
void flxDevBMV080::set_duty_cycle(uint16_t dutyCycle)
{
    if (dutyCycle < kBMV080DutyCycleMin)
    {
        flxLog_W(F("BMV080: Duty cycle must be at least %d seconds - setting to minimum"), kBMV080DutyCycleMin);
        dutyCycle = kBMV080DutyCycleMin; // Ensure minimum duty cycle
    }

    if (dutyCycle == _dutyCycle || !isInitialized())
        return;

    beginUpdate();
    _dutyCycle = dutyCycle;

    // The duty cycle must be greater than the integration time by at least 2 seconds.
    // Check and adjust the integration time
    if (_integrationTime > _dutyCycle - 2)
    {
        uint16_t tmp = _dutyCycle - 2;
        flxLog_W(F("BMV080: Adjusting integration time to %d"), tmp);
        set_integration_time(tmp);
    }
    endUpdate();
}

// vibration filtering
void flxDevBMV080::set_enable_vibration_filter(bool enable)
{
    if (enable == _vibrationFilterEnabled)
        return;

    beginUpdate();
    _vibrationFilterEnabled = enable;

    endUpdate();
}

bool flxDevBMV080::get_enable_vibration_filter(void)
{
    return _vibrationFilterEnabled;
}

// vibration filtering
void flxDevBMV080::set_integration_time(uint16_t integrationTime)
{

    if (integrationTime == _integrationTime || !isInitialized())
        return;

    beginUpdate();
    _integrationTime = integrationTime;

    // The duty cycle must be greater than the integration time by at lest 2 seconds.
    // Check and adjust the duty cycle
    if (_dutyCycle < (_integrationTime + 2))
    {
        _dutyCycle = _integrationTime + 2;
        flxLog_W(F("BMV080: Adjusting duty cycle to %d seconds to be greater than integration time"), _dutyCycle);

        set_duty_cycle(_dutyCycle);
    }
    endUpdate();
}

uint16_t flxDevBMV080::get_integration_time(void)
{
    return _integrationTime;
}