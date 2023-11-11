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
flxDevAS7265X::flxDevAS7265X() 
{

    setName(getDeviceName(), "AS7265X Triad Spectroscopy Sensor");

    // Register output params
    flxRegister(outCalA, "Calibrated A", "Calibrated channel A");
    flxRegister(outCalB, "Calibrated B", "Calibrated channel B");
    flxRegister(outCalC, "Calibrated C", "Calibrated channel C");
    flxRegister(outCalD, "Calibrated D", "Calibrated channel D");
    flxRegister(outCalE, "Calibrated E", "Calibrated channel E");
    flxRegister(outCalF, "Calibrated F", "Calibrated channel F");

    // Register output params
    flxRegister(outCalG, "Calibrated G", "Calibrated channel G");
    flxRegister(outCalH, "Calibrated H", "Calibrated channel H");
    flxRegister(outCalI, "Calibrated I", "Calibrated channel I");
    flxRegister(outCalJ, "Calibrated J", "Calibrated channel J");
    flxRegister(outCalK, "Calibrated K", "Calibrated channel K");
    flxRegister(outCalL, "Calibrated L", "Calibrated channel L"); 

    // Register output params
    flxRegister(outCalR, "Calibrated R", "Calibrated channel R");
    flxRegister(outCalS, "Calibrated S", "Calibrated channel S");
    flxRegister(outCalT, "Calibrated T", "Calibrated channel T");
    flxRegister(outCalU, "Calibrated U", "Calibrated channel U");
    flxRegister(outCalV, "Calibrated V", "Calibrated channel V");
    flxRegister(outCalW, "Calibrated W", "Calibrated channel W");                     

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
    return AS7265X::begin(wirePort);
        return false;
}


bool flxDevAS7265X::execute(void)
{
    AS7265X::takeMeasurements();

    return true;
}

//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

float flxDevAS7265X::read_calA(void)
{
    return AS7265X::getCalibratedA();
}

float flxDevAS7265X::read_calB(void)
{
    return AS7265X::getCalibratedB();
}

float flxDevAS7265X::read_calC(void)
{
    return AS7265X::getCalibratedC();
}

float flxDevAS7265X::read_calD(void)
{
    return AS7265X::getCalibratedD();
}

float flxDevAS7265X::read_calE(void)
{
    return AS7265X::getCalibratedE();
}

float flxDevAS7265X::read_calF(void)
{
    return AS7265X::getCalibratedF();
}

float flxDevAS7265X::read_calG(void)
{
    return AS7265X::getCalibratedG();
}
float flxDevAS7265X::read_calH(void)
{
    return AS7265X::getCalibratedH();
}
float flxDevAS7265X::read_calI(void)
{
    return AS7265X::getCalibratedI();
}
float flxDevAS7265X::read_calJ(void)
{
    return AS7265X::getCalibratedJ();
}
float flxDevAS7265X::read_calK(void)
{
    return AS7265X::getCalibratedK();
}
float flxDevAS7265X::read_calL(void)
{
    return AS7265X::getCalibratedL();
}
float flxDevAS7265X::read_calR(void)
{
    return AS7265X::getCalibratedR();
}
float flxDevAS7265X::read_calS(void)
{
    return AS7265X::getCalibratedS();
}
float flxDevAS7265X::read_calT(void)
{
    return AS7265X::getCalibratedT();
}
float flxDevAS7265X::read_calU(void)
{
    return AS7265X::getCalibratedU();
}
float flxDevAS7265X::read_calV(void)
{
    return AS7265X::getCalibratedV();
}
float flxDevAS7265X::read_calW(void)
{
    return AS7265X::getCalibratedW();
}
