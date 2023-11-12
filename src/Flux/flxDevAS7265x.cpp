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
 *  flxDevAS7265x.cpp
 *
 *  Spark Device object for the AS7265x  Qwiic device.
 */

#include "Arduino.h"

#include "flxDevAS7265x.h"

// device addresses for our device interface -- using macros from qwiic/arduino library
uint8_t flxDevAS7265X::defaultDeviceAddress[] = {AS7265X_ADDR, kSparkDeviceAddressNull};

///
/// @brief Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevAS7265X);

//----------------------------------------------------------------------------------------------------------
/// @brief Constructor
///
flxDevAS7265X::flxDevAS7265X() : _indicator{true}, _gain{AS7265X_GAIN_16X}
{

    setName(getDeviceName(), "AS7265X Triad Spectroscopy Sensor");

    // Properties
    flxRegister(readWithLED, "Enable LED", "Measure with LED enabled");
    flxRegister(enableIndicator, "Enable Indicator", "Indicator LED enabled");
    flxRegister(sensorGain, "Gain", "Gain settings for sensor");
    flxRegister(outputCal, "Calibrated Output", "Return calibrated values");

    // Register output params
    flxRegister(outputType, "Calibrated", "True if output data is calibrated");

    flxRegister(outA, "Channel A");
    flxRegister(outB, "Channel B");
    flxRegister(outC, "Channel C");
    flxRegister(outD, "Channel D");
    flxRegister(outE, "Channel E");
    flxRegister(outF, "Channel F");

    // Register output params
    flxRegister(outG, "Channel G");
    flxRegister(outH, "Channel H");
    flxRegister(outI, "Channel I");
    flxRegister(outJ, "Channel J");
    flxRegister(outK, "Channel K");
    flxRegister(outL, "Channel L");

    // Register output params
    flxRegister(outR, "Channel R");
    flxRegister(outS, "Channel S");
    flxRegister(outT, "Channel T");
    flxRegister(outU, "Channel U");
    flxRegister(outV, "Channel V");
    flxRegister(outW, "Channel W");
}

//----------------------------------------------------------------------------------------------------------
/// @brief  Static method called to determine if device is connected
///
/// @param  i2cDriver - Framework i2c bus driver
/// @param  address - The address to check
///
/// @return true if the device is connected
///
bool flxDevAS7265X::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    return i2cDriver.ping(address);
}

//----------------------------------------------------------------------------------------------------------
///
/// @brief Called during the startup/initialization of the driver (after the constructor is called).
///
/// @param wirePort - The Arduino wire port for the I2C bus
///
/// @return true on success
///
bool flxDevAS7265X::onInitialize(TwoWire &wirePort)
{
    bool status = AS7265X::begin(wirePort);

    if (status)
    {
        AS7265X::setGain(_gain);
    }
    return status;
}

//---------------------------------------------------------------------------
// Indicator property
//---------------------------------------------------------------------------
bool flxDevAS7265X::get_indicator(void)
{
    return _indicator;
}

void flxDevAS7265X::set_indicator(bool isOn)
{
    if (_indicator == isOn)
        return;

    if (isOn)
        AS7265X::enableIndicator();
    else
        AS7265X::disableIndicator();

    _indicator = isOn;
}

//---------------------------------------------------------------------------
// Gain property
//---------------------------------------------------------------------------

uint8_t flxDevAS7265X::get_gain(void)
{
    return _gain;
}

void flxDevAS7265X::set_gain(uint8_t value)
{
    _gain = value;
    if (isInitialized())
        AS7265X::setGain(value);
}
//---------------------------------------------------------------------------
///
/// @brief Called right before data parameters are read - take measurments called
///

bool flxDevAS7265X::execute(void)
{
    if (readWithLED())
        AS7265X::takeMeasurementsWithBulb();
    else
        AS7265X::takeMeasurements();

    return true;
}

//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------

bool flxDevAS7265X::read_output_type(void)
{
    return outputCal();
}
//---------------------------------------------------------------------------

float flxDevAS7265X::read_A(void)
{
    return outputCal() ? AS7265X::getCalibratedA() : AS7265X::getA();
}

float flxDevAS7265X::read_B(void)
{
    return outputCal() ? AS7265X::getCalibratedB() : AS7265X::getB();
}

float flxDevAS7265X::read_C(void)
{
    return outputCal() ? AS7265X::getCalibratedC() : AS7265X::getC();
}

float flxDevAS7265X::read_D(void)
{
    return outputCal() ? AS7265X::getCalibratedD() : AS7265X::getD();
}

float flxDevAS7265X::read_E(void)
{
    return outputCal() ? AS7265X::getCalibratedE() : AS7265X::getE();
}

float flxDevAS7265X::read_F(void)
{
    return outputCal() ? AS7265X::getCalibratedF() : AS7265X::getF();
}

float flxDevAS7265X::read_G(void)
{
    return outputCal() ? AS7265X::getCalibratedG() : AS7265X::getG();
}
float flxDevAS7265X::read_H(void)
{
    return outputCal() ? AS7265X::getCalibratedH() : AS7265X::getH();
}
float flxDevAS7265X::read_I(void)
{
    return outputCal() ? AS7265X::getCalibratedI() : AS7265X::getI();
}
float flxDevAS7265X::read_J(void)
{
    return outputCal() ? AS7265X::getCalibratedJ() : AS7265X::getJ();
}
float flxDevAS7265X::read_K(void)
{
    return outputCal() ? AS7265X::getCalibratedK() : AS7265X::getK();
}
float flxDevAS7265X::read_L(void)
{
    return outputCal() ? AS7265X::getCalibratedL() : AS7265X::getL();
}
float flxDevAS7265X::read_R(void)
{
    return outputCal() ? AS7265X::getCalibratedR() : AS7265X::getR();
}
float flxDevAS7265X::read_S(void)
{
    return outputCal() ? AS7265X::getCalibratedS() : AS7265X::getS();
}
float flxDevAS7265X::read_T(void)
{
    return outputCal() ? AS7265X::getCalibratedT() : AS7265X::getT();
}
float flxDevAS7265X::read_U(void)
{
    return outputCal() ? AS7265X::getCalibratedU() : AS7265X::getU();
}
float flxDevAS7265X::read_V(void)
{
    return outputCal() ? AS7265X::getCalibratedV() : AS7265X::getV();
}
float flxDevAS7265X::read_W(void)
{
    return outputCal() ? AS7265X::getCalibratedW() : AS7265X::getW();
}
