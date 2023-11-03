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
 *  flxDevTMAG5273.cpp
 *
 *  Spark Device object for the STMicroelectronics TMAG5273
 *  low-power linear 3D Hall-effect sensor device. 
 *
 *
 */

#include <Arduino.h>
#include "SparkFun_TMAG5273_Arduino_Library.h"

#include "flxDevTMAG5273.h" 

#define kTMAG5273AddressDefault TMAG5273_I2C_ADDRESS_INITIAL

// Define our class static variables - allocs storage for them
uint8_t flxDevTMAG5273::defaultDeviceAddress[] = {kTMAG5273AddressDefault, kSparkDeviceAddressNull}; 

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevTMAG5273);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevTMAG5273::flxDevTMAG5273() 
{
    // Setup unique identifiers for this device and basic device object systems
    setName(kTMAG5273DeviceName, "Hall-Effect Sensor");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties and parameters

    // Register Properties
    flxRegister(xData, "X Mag Field (mT)", "X Magnetic Field (mT)");
    flxRegister(yData, "Y Mag Field (mT)", "Y Magnetic Field (mT)");
    flxRegister(zData, "Z Mag Field (mT)", "Z Magnetic Field (mT)");
    flxRegister(tempData, "Temperature (C)", "Temperature Value (C)");
    flxRegister(angleResult, "Angle Result (degree)", "Angle Result Value (degree)");
    flxRegister(magnitudeData, "Magnitude Result", "Magnitude Result");

    // Register Parameters
    flxRegister(crcEnable, "CRC Enable", "CRC Enable/Disable");
    flxRegister(convAvg, "Conversion Average", "Conversion Average");
    flxRegister(opMode, "Operating Mode", "Operating Mode");
    flxRegister(magEnable, "Magnetic Channels Enable", "Magnetic Channel Enable");
    flxRegister(sleeptime, "Sleeptime", "Sleeptime of Device");
    flxRegister(numThreshold, "Threshold Crossing Number", "Threshold Crossing Number");
    flxRegister(thresholdCross, "Direction of Threshold Crossing", "Direction of Threshold Crossing");
    flxRegister(channelAdjust "Channel Adjustment", "Channel Adjustment Selection");
    flxRegister(angleCalc, "Angle Calculation", "Axis Selection for Angle Calculation");
    flxRegister(xyRange, "XY Range Selection", "XY Range Selection");
    flxRegister(zRange, "Z Range Selection", "Z Range Selection");
    flxRegister(tempEnable, "Temperature Enable", "Enable Temperature Selection");
    flxRegister(intAsserted, "", ""); /*FIX*/
    flxRegister(intPinState, "Interrupt Routing", "Route Interrupt Location");
    flxRegister(intEnable, "", ""); /*FIX*/
    flxRegister(intPinEnable, "Interrupt Pin Enable", "Interrupt Pin Enable");
    flxRegister(i2cAddressEn, "Enable I2C Address Change", "Enable I2C Address Change");
    flxRegister(porEn, "POR Status", "POR Status/Clear");
    flxRegister(oscError, "Oscillator Error", "Oscillator Error Detection");
    flxRegister(intError, "Interrupt Error", "Interrupt Error Detection");

}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevTMAG5273::onInitialize(TwoWire &wirePort)
{
    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_i2cBus, address());

    bool result = true;

    // Give the I2C port provided by the user to the I2C bus class.
    _i2cBus.init(wirePort, true);

    // Initialize the system -
    result = this->TMAG5273_I2C::init();
    // Checks for error when beginning the function
    result &= begin(); 

    if (result)
        result = flxDevTMAG5273::onInitialize();
    else
        flxLog_E("TMAG5273 onInitialize: device did not begin");

    return result;
}

// GETTER methods for output params
float flxDevTMAG5273::read_Temperature()
{
    return getTemp();
}
float flxDevTMAG5273::read_XData()
{
    return getXData();
}
float flxDevTMAG5273::read_YData()
{
    return getYData();
}
float flxDevTMAG5273::read_ZData()
{
    return getZData();
}
float flxDevTMAG5273::read_AngleResult()
{
    return getAngleResult();
}
float flxDevTMAG5273::read_MagnitudeResult()
{
    return getMagnitudeResult();
}

// GETTER and SETTER methods for device properties
void flxDevTMAG5273::set_CRCMode(uint8_t crc)
{
    setCRCMode(crc);
}
uint8_t flxDevTMAG5273::getCRCMode()
{
    return getCRCMode();
}
void flxDevTMAG5273::set_MagTempCoefficient(uint8_t coef)
{
    setMagTemp(coef);
}
uint8_t flxDevTMAG5273::get_MagTempCoefficient()
{
    return getMagTemp();
}
void flxDevTMAG5273::set_ConvAvg(uint8_t average)
{
    setConvAvg(average);
}
uint8_t flxDevTMAG5273::get_ConvAvg()
{
    return getConvAvg();
}
void flxDevTMAG5273::set_ReadMode(uint8_t mode)
{
    setReadMode(mode);
}
uint8_t flxDevTMAG5273::get_ReadMode()
{
    return getReadMode();
}
void flxDevTMAG5273::set_IntThreshold(uint8_t threshold)
{
    setIntThreshold(threshold);
}
uint8_t flxDevTMAG5273::get_IntThreshold()
{
    return getIntThreshold();
}
void flxDevTMAG5273::set_LowPower(uint8_t lp)
{
    setLowPower(lp);
}
uint8_t flxDevTMAG5273::get_LowPower()
{
    return getLowPower();
}
void flxDevTMAG5273::set_GlitchFilter(uint8_t filter)
{
    setGlitchFilter(filter);
}
uint8_t flxDevTMAG5273::get_GlitchFilter()
{
    return getGlitchFilter();
}
void flxDevTMAG5273::set_TriggerMode(uint8_t mode)
{
    setTriggerMode(mode);
}
uint8_t flxDevTMAG5273::get_TriggerMode()
{
    return getTriggerMode();
}
void flxDevTMAG5273::set_OperatingMode(uint8_t mode)
{
    setOperatingMode(mode);
}
uint8_t flxDevTMAG5273::get_OperatingMode()
{
    return getOperatingMode();
}
void flxDevTMAG5273::set_MagneticChannel(uint8_t channel)
{
    setMagneticChannel(channel);
}
uint8_t flxDevTMAG5273::get_MagneticChannel()
{
    return getMagneticChannel();
}
void flxDevTMAG5273::set_Sleeptime(uint8_t sleep)
{
    setSleeptime(sleep);
}
uint8_t flxDevTMAG5273::get_Sleeptime()
{
    return getSleeptime();
}
void flxDevTMAG5273::set_ThresholdCross(uint8_t cross)
{
    setThresholdCross(cross);
}
uint8_t flxDevTMAG5273::get_ThresholdCross()
{
    return getThresholdCross();
}
void flxDevTMAG5273::set_MagDirection(uint8_t direction)
{
    setMagDir(direction); 
}
uint8_t flxDevTMAG5273::get_MagDirection()
{
    return getMagDir();
}
void flxDevTMAG5273::set_MagChannelSelect(uint8_t channel)
{
    setMagChannelSelect(channel);
}
uint8_t flxDevTMAG5273::get_MagChannelSelect()
{
    return getMagChannelSelect();
}
void flxDevTMAG5273::set_MagnitudeGain(float gain)
{
    setMagnitudeGain(gain);
}
uint8_t flxDevTMAG5273::get_MagnitudeGain()
{
    return getMagnitudeGain();
}
void flxDevTMAG5273::set_MagneticOffset1(float offset)
{
    setMagneticOffset1(offset);
}
uint8_t flxDevTMAG5273::get_MagneticOffset1()
{
    return getMagneticOffset1();
}
void flxDevTMAG5273::set_MagneticOffset2(float offset)
{
    setMagneticOffset2(offset);
}
uint8_t flxDevTMAG5273::get_MagneticOffset2()
{
    return getMagneticOffset2();
}
void flxDevTMAG5273::set_AngleEnable(uint8_t enable)
{
    setAngleEn(enable);
}
uint8_t flxDevTMAG5273::get_AngleEnable()
{
    return getAngleEn();
}
void flxDevTMAG5273::set_XYAxisRange(uint8_t range)
{
    setXYAxisRange(range);
}
uint8_t flxDevTMAG5273::get_XYAxisRange()
{
    return getXYAxisRange();
}
void flxDevTMAG5273::set_ZAxisRange(uint8_t range)
{
    setZAxisRange(range);
}
uint8_t flxDevTMAG5273::get_ZAxisRange()
{
    return getZAxisRange();
}
void flxDevTMAG5273::set_XThreshold(float threshold)
{
    setXThreshold(threshold);
}
float flxDevTMAG5273::get_XThreshold()
{
    return getXThreshold();
}
void flxDevTMAG5273::set_YThreshold(float threshold)
{
    setYThreshold(threshold);
}
float flxDevTMAG5273::get_YThreshold()
{
    return getYThreshold();
}
void flxDevTMAG5273::set_ZThreshold(float threshold)
{
    setZThreshold(threshold);
}
float flxDevTMAG5273::get_ZThreshold()
{
    return getZThreshold();
}
void flxDevTMAG5273::set_TemperatureThreshold(float threshold)
{
    setTemperatureThreshold(threshold);
}
float flxDevTMAG5273::get_TemparatureThreshold()
{
    return getTemperatureThreshold();
}
void flxDevTMAG5273::set_InterruptResult(bool result)
{
    setInterruptResult(result);
}
uint8_t flxDevTMAG5273::get_InterruptResult()
{
    return getInterruptResult();
}
void flxDevTMAG5273::set_ThresholdEnable(bool enable)
{
    setThresholdEn(enable);
}
uint8_t flxDevTMAG5273::get_ThresholdEnable()
{
    return getThresholdEn();
}
void flxDevTMAG5273::set_InterruptPinState(bool state)
{
    setIntPinState(state);
}
uint8_t flxDevTMAG5273::get_InterruptPinState()
{
    return getIntPinState();
}
void flxDevTMAG5273::set_InterruptPinMode(uint8_t state)
{
    setInterruptMode(state);
}
uint8_t flxDevTMAG5273::get_InterruptPinMode()
{
    return getInterruptMode();
}
void flxDevTMAG5273::set_I2CAddressEnable(bool enable)
{
    setI2CAddressEN(enable);
}
uint8_t flxDevTMAG5273::get_I2CAddressEnable()
{
    return getI2CAddressEN();
}
void flxDevTMAG5273::set_OscillatorError(bool err)
{
    setOscillatorError(err);
}
uint8_t flxDevTMAG5273::get_OscillatorError()
{
    return getOscillatorError();
}
void flxDevTMAG5273::set_POR(bool por)
{
    setPOR(por);
}
uint8_t flxDevTMAG5273::get_POR()
{
    return getPOR();
}
void flxDevTMAG5273::set_I2CAddress(uint8_t address)
{
    setI2CAddress(address);
}
uint8_t flxDevTMAG5273::get_I2CAddress()
{
    return getI2CAddress();
}

uint8_t flxDevTMAG5273::get_SetCount()
{
    return getSetCount();
}
uint8_t flxDevTMAG5273::get_DiagStatus()
{
    return getDiagStatus();
}
uint8_t flxDevTMAG5273::get_ResultStatus()
{
    return getResultStatus();
}
uint8_t flxDevTMAG5273::get_DeviceID()
{
    return getDeviceID();
}
uint8_t flxDevTMAG5273::get_ManufacturerID()
{
    return getManufacturerID();
}
uint8_t flxDevTMAG5273::get_InterruptPinStatus()
{
    return getInterruptPinStatus();
}
uint8_t flxDevTMAG5273::get_DeviceStatus()
{
    return getDeviceStatus();
}
uint8_t flxDevTMAG5273::get_Error()
{
    return getError();
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
static bool flxDevTMAG5273::isConnected(TwoWire &wire)
{
    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_i2cBus, address());

    // Give the I2C port provided by the user to the I2C bus class.
    _i2cBus.init(wirePort, true);

    // Initialize the system -
    bool result = this->TMAG5273_I2C::init();

    if (result)
        result = flxDevTMAG5273::onInitialize();
    else
        flxLog_E("TMAG5273 onInitialize: device did not begin");

    return result;
}


