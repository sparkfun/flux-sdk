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
 *  flxDevAS7265X.h
 *
 *  Spark Device object for the AS7265X Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_AS7265X.h"
#include "flxDevice.h"

#define kAS7265xDeviceName "AS7265X"

// Define our class
class flxDevAS7265X : public flxDeviceI2CType<flxDevAS7265X>, public AS7265X
{

  public:
    flxDevAS7265X();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kAS7265xDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects.
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    bool execute(void);

  private:
    // Parameter methods
    float read_calA();
    float read_calB();
    float read_calC();
    float read_calD();
    float read_calE();
    float read_calF();

    float read_calG();
    float read_calH();
    float read_calI();
    float read_calJ();
    float read_calK();
    float read_calL();

    float read_calR();
    float read_calS();
    float read_calT();
    float read_calU();
    float read_calV();
    float read_calW();

  public:

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calA> outCalA;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calB> outCalB;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calC> outCalC;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calD> outCalD;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calE> outCalE;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calF> outCalF;      

    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calG> outCalG;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calH> outCalH;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calI> outCalI;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calJ> outCalJ;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calK> outCalK;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calL> outCalL;

    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calR> outCalR;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calS> outCalS;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calT> outCalT;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calU> outCalU;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calV> outCalV;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_calW> outCalW;              
};
