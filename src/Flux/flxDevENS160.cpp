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
 *  Spark Device object for the CCS811 Qwiic device.
 */

#include "Arduino.h"

#include "flxDevENS160.h"

uint8_t flxDevENS160::defaultDeviceAddress[] = {ENS160_ADDRESS_HIGH, ENS160_ADDRESS_LOW, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevENS160);

flxDevENS160::flxDevENS160() : _opMode{SFE_ENS160_STANDARD}, _tempCComp{nullptr}, _rhComp{nullptr}, _lastCompCheck{0}
{

    setName(getDeviceName(), "ScioSense ENS160 Indoor Air Quality Sensor");

    flxRegister(operatingMode, "Operating Mode", "The Sensor Operating Mode");
    flxRegister(enableCompensation, "Enable Compensation", "Enable Temp and Humidity compensation if available");
    flxRegister(updatePeriodSecs, "Update Period", "The compensation update period in seconds");

    // Register output params
    flxRegister(val_AQI, "AQI", "Air Quality Index");
    flxRegister(val_TVOC, "TVOC", "Total Volatile Organic Compound");
    flxRegister(val_ETOH, "ETOH", "Ethanol Concentration");
    flxRegister(val_ECO2, "eCO2", "Equivalent CO2");
    flxRegister(val_TempC, "Temperature Compensation", "The current temperature compensation value (C)");
    flxRegister(val_RH, "Humidity Compensation", "The current relative humidity value");
    ;
}

// Static method used to determine if this device is connected

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
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
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
// Operating mode
uint8_t flxDevENS160::get_operating_mode(void)
{
    if (!isInitialized())
        return _opMode;

    int8_t mode = SparkFun_ENS160::getOperatingMode();

    return ( mode > 0 ? mode : SFE_ENS160_STANDARD);
}

//---------------------------------------------------------------------------
void flxDevENS160::set_operating_mode(uint8_t newMode)
{
    if (isInitialized())
        SparkFun_ENS160::setOperatingMode(newMode);
    else
        _opMode = newMode;
}

// methodd to set a parameter to use for temp compensation
void flxDevENS160::setTemperatureCompParameter(flxParameterOutScalar& compParam)
{
    _tempCComp = compParam.accessor();
}

// Relitive humidity comp
void flxDevENS160::setHumidityCompParameter(flxParameterOutScalar& compParam)
{
    _rhComp = compParam.accessor();
}


bool flxDevENS160::loop(void)
{
    // Time to update our comp values?

    if (!isInitialized() || !enableCompensation() || (_rhComp == nullptr && _tempCComp == nullptr))
        return false;

    // we need a time element here ..

    if (millis() - _lastCompCheck > updatePeriodSecs() * 1000 )
    {
        float value;
        if (_rhComp != nullptr)
        {
            value = _rhComp->getFloat();
            SparkFun_ENS160::setRHCompensationFloat(value);
        }

        if (_tempCComp != nullptr)
        {
            value = _tempCComp->getFloat();
            SparkFun_ENS160::setTempCompensationCelsius(value);
        }

        _lastCompCheck = millis();
    }

    return false;
}

//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------
// read_AQI()

uint8_t flxDevENS160::read_AQI(void)
{
    return SparkFun_ENS160::getAQI();
}

//---------------------------------------------------------------------------
// read_TVOC()

uint16_t flxDevENS160::read_TVOC(void)
{
    return SparkFun_ENS160::getTVOC();
}

//---------------------------------------------------------------------------
// read_ETOH()

uint16_t flxDevENS160::read_ETOH(void)
{
    return SparkFun_ENS160::getETOH();
}

//---------------------------------------------------------------------------
// read_ECO2()

uint16_t flxDevENS160::read_ECO2(void)
{
    return SparkFun_ENS160::getECO2();
}

//---------------------------------------------------------------------------
// read_TempC()

float flxDevENS160::read_TempC(void)
{
    return SparkFun_ENS160::getTempCelsius();
}

//---------------------------------------------------------------------------
// read_RH()

float flxDevENS160::read_RH(void)
{
    return SparkFun_ENS160::getRH();
}
