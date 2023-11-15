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
 *  flxDevBioHub.h
 *
 *  Spark Device object for the BioHub device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFun_Bio_Sensor_Hub_Library.h"

// What is the name used to ID this device?
#define kBioHubDeviceName "BioHub"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBioHub : public flxDeviceI2CType<flxDevBioHub>, public SparkFun_Bio_Sensor_Hub
{

public:
    flxDevBioHub();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.

    // The Bio Hub is a special case. We need to know the reset and mfio pins in order to
    // check if it is connected and to initialize it. During auto-detect the pins aren't known,
    // so auto-detect will fail. We need to manually create an instance of the flxDevBioHub,
    // initialize the pin numbers with `initialize`, check if it is connected, call onInitialize
    // and then add it to the logger.
    bool initialize( int connectResetPin, int connectMfioPin );

    static bool isConnected( flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidencePing;
    }
    
    static const char *getDeviceName()
    {
        return kBioHubDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize( TwoWire &);

private:

    // methods used to get values for our output parameters
    bioData body;
    uint16_t read_heart_rate();
    uint8_t read_confidence();
    uint16_t read_oxygen();
    uint8_t read_status(); // 0: Success, 1: Not Ready, 2: Object Detectected, 3: Finger Detected
    int8_t read_extended_status();
    float read_r_value();

    int _resetPin = -1;
    int _mfioPin = -1;

    // flags to prevent readBpm being called multiple times
    bool _heartRate = false;
    bool _confidence = false;
    bool _o2 = false;
    bool _status = false;
    bool _eStatus = false;
    bool _o2r = false;

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutUint16<flxDevBioHub, &flxDevBioHub::read_heart_rate> heartRate;    
    flxParameterOutUint8<flxDevBioHub, &flxDevBioHub::read_confidence> confidence;    
    flxParameterOutUint16<flxDevBioHub, &flxDevBioHub::read_oxygen> O2;    
    flxParameterOutUint8<flxDevBioHub, &flxDevBioHub::read_status> status;    
    flxParameterOutInt8<flxDevBioHub, &flxDevBioHub::read_extended_status> eStatus;    
    flxParameterOutFloat<flxDevBioHub, &flxDevBioHub::read_r_value> O2R;    
};
