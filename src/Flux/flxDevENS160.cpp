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
 *  flxDevENS160.cpp
 *
 *  Spark Device object for the ENS160  Qwiic device.
 */

#include "Arduino.h"

#include "flxCoreEvent.h"
#include "flxDevENS160.h"

// device addresses for our device interface -- using macros from qwiic/arduino library
uint8_t flxDevENS160::defaultDeviceAddress[] = {ENS160_ADDRESS_HIGH, ENS160_ADDRESS_LOW, kSparkDeviceAddressNull};

///
/// @brief Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevENS160);

//----------------------------------------------------------------------------------------------------------
/// @brief Constructor
///
flxDevENS160::flxDevENS160() : _opMode{SFE_ENS160_STANDARD}, _tempCComp{nullptr}, _rhComp{nullptr}
{

    setName(getDeviceName(), "ScioSense ENS160 Indoor Air Quality Sensor");

    // Register properties
    flxRegister(operatingMode, "Operating Mode", "The Sensor Operating Mode");
    flxRegister(enableCompensation, "Enable Compensation", "Compensation from external device if connected");
    flxRegister(tempComp, "Temperature Compensation", "Manually set the compensation value");
    flxRegister(humidityComp, "Humidity Compensation", "Manually set the compensation value");
    flxRegister(updatePeriodSecs, "Update Period", "Compensation from input device update period (secs)");

    // Register output params
    flxRegister(val_AQI, "AQI", "Air Quality Index");
    flxRegister(val_TVOC, "TVOC", "Total Volatile Organic Compound");
    flxRegister(val_ETOH, "ETOH", "Ethanol Concentration");
    flxRegister(val_ECO2, "eCO2", "Equivalent CO2");
    flxRegister(val_TempC, "Temp Comp", "The current temperature compensation value (C)");
    flxRegister(val_RH, "Humidity Comp", "The current relative humidity compensation value");

    // setup our job -
    _theJob.setup(name(), _updatePeriod * 1000, this, &flxDevENS160::updateParams);
    if (_updatePeriod > 0)
        flxAddJobToQueue(_theJob);
}

//----------------------------------------------------------------------------------------------------------
/// @brief  Static method called to determine if device is connected
///
/// @param  i2cDriver - Framework i2c bus driver
/// @param  address - The address to check
///
/// @return true if the device is connected
///
bool flxDevENS160::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t tempVal[2] = {0};

    bool retVal = i2cDriver.readRegisterRegion(address, SFE_ENS160_PART_ID, tempVal, 2);

    if (!retVal)
        return false;

    uint16_t chipID = tempVal[0] | (tempVal[1] << 8);

    return (chipID == ENS160_DEVICE_ID);
}

//----------------------------------------------------------------------------------------------------------
///
/// @brief Called during the startup/initialization of the driver (after the constructor is called).
///
/// @param wirePort - The Arduino wire port for the I2C bus
///
/// @return true on success
///
bool flxDevENS160::onInitialize(TwoWire &wirePort)
{

    if (!SparkFun_ENS160::begin(address()))
        return false;

    SparkFun_ENS160::setOperatingMode(SFE_ENS160_RESET);

    delay(100); //

    SparkFun_ENS160::setOperatingMode(operatingMode());

    return true;
}

//---------------------------------------------------------------------------
// props
//---------------------------------------------------------------------------

///
/// @brief Operating mode property getter
///
/// @return - The value of the operating mode of the device.
uint8_t flxDevENS160::get_operating_mode(void)
{
    // is the up? If not, send our cached value
    if (!isInitialized())
        return _opMode;

    int8_t mode = SparkFun_ENS160::getOperatingMode();

    // if in error - the value is negative -- if this is the case, send standard
    return (mode > 0 ? mode : SFE_ENS160_STANDARD);
}

//---------------------------------------------------------------------------
///
/// @brief Operating Mode setter
///
/// @param newMode - the new mode to set the device to
///
void flxDevENS160::set_operating_mode(uint8_t newMode)
{
    if (isInitialized())
        SparkFun_ENS160::setOperatingMode(newMode);
    else
        _opMode = newMode;
}

//---------------------------------------------------------------------------
///
/// @brief method to set a parameter to use for temp compensation
///
/// @param compParam - the input parameter to pull Temp compensation values from
///
void flxDevENS160::setTemperatureCompParameter(flxParameterOutScalar &compParam)
{
    // Get the actual input scalar parameter that we can call directly.
    _tempCComp = compParam.accessor();
}

//---------------------------------------------------------------------------
///
/// @brief method to set a parameter to use for relative humidity compensation
///
/// @param compParam - the input parameter to pull humidity compensation values from
///
void flxDevENS160::setHumidityCompParameter(flxParameterOutScalar &compParam)
{
    // Get the actual input scalar parameter that we can call directly.
    _rhComp = compParam.accessor();
}

//---------------------------------------------------------------------------
///
/// @brief loop method - system interface for the device object - called every loop iteration
/// @note  If the ENS160 has an input device set for temp and humidity compensation, the loop
///        method will pull in new values every N seconds and set these values in the sensor
///
/// @return - always false -- a true value cases the system to update UX/LED
///
void flxDevENS160::updateParams(void)
{
    // Time to update our comp values? - initialized? enabled? have input params?

    if (!isInitialized() || !enableCompensation() || (_rhComp == nullptr && _tempCComp == nullptr))
        return;

    float value;
    if (_rhComp != nullptr)
    {
        // get the Humidity value from the input device and set in our device
        value = _rhComp->getFloat();
        SparkFun_ENS160::setRHCompensationFloat(value);
    }

    if (_tempCComp != nullptr)
    {
        // get the Temperature value from the input device and set in our device
        value = _tempCComp->getFloat();
        SparkFun_ENS160::setTempCompensationCelsius(value);
    }
    // Send an activity event to the system
    if (_tempCComp != nullptr || _rhComp != nullptr)
        flxSendEvent(flxEvent::kOnSystemActivity);
}

void flxDevENS160::set_update_period(uint val)
{
    if (val == _updatePeriod)
        return;

    _updatePeriod = val;

    if (!val)
        return;

    _theJob.setPeriod(_updatePeriod * 1000);
    flxUpdateJobInQueue(_theJob);
}

uint flxDevENS160::get_update_period(void)
{
    return _updatePeriod;
}

//---------------------------------------------------------------------------
///
/// @brief - setter for temp compensation property. Helpful if not input device set
///
/// @param value - new value to set
///
void flxDevENS160::set_temp_comp(float value)
{
    // only set if the device is up and running
    if (isInitialized())
        SparkFun_ENS160::setTempCompensationCelsius(value);
}

//---------------------------------------------------------------------------
///
/// @brief - setter for humidity compensation property. Helpful if not input device set
///
/// @param value - new value to set
///
void flxDevENS160::set_humid_comp(float value)
{
    // only set if the device is up and running
    if (isInitialized())
        SparkFun_ENS160::setRHCompensationFloat(value);
}
//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// @brief Parameter read method  - AQI
/// @param  none
/// @return the current AQI value
///
uint8_t flxDevENS160::read_AQI(void)
{
    return SparkFun_ENS160::getAQI();
}

//---------------------------------------------------------------------------
/// @brief Parameter read method  - TVOC
/// @param  none
/// @return the current TVOC value
///
uint16_t flxDevENS160::read_TVOC(void)
{
    return SparkFun_ENS160::getTVOC();
}

//---------------------------------------------------------------------------
/// @brief Parameter read method  - Ethanol
/// @param  none
/// @return the current ETHO value
///
uint16_t flxDevENS160::read_ETOH(void)
{
    return SparkFun_ENS160::getETOH();
}

//---------------------------------------------------------------------------
/// @brief Parameter read method  - CO2
/// @param  none
/// @return the current CO2 value
///
uint16_t flxDevENS160::read_ECO2(void)
{
    return SparkFun_ENS160::getECO2();
}

//---------------------------------------------------------------------------
/// @brief Parameter read method  - Compensation Temperature (C)
/// @param  none
/// @return the current Temperature value
///
float flxDevENS160::read_TempC(void)
{
    return SparkFun_ENS160::getTempCelsius();
}

//---------------------------------------------------------------------------
/// @brief Parameter read method  - Compensation Humidity
/// @param  none
/// @return the current Humidity value
///
float flxDevENS160::read_RH(void)
{
    return SparkFun_ENS160::getRH();
}
