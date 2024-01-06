/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevTMF882X.h
 *
 *  Spark Device object for the TMF882X device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_TMF882X_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kTMF882XDeviceName "TMF882x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevTMF882X : public flxDeviceI2CType<flxDevTMF882X>, public SparkFun_TMF882X
{

  public:
    flxDevTMF882X();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kTMF882XDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

  private:
    // methods used to get values for our output parameters
    // Strictly, these should be uint32_t
    bool read_confidence(flxDataArrayUint *);
    bool read_distance(flxDataArrayUint *);
    bool read_channel(flxDataArrayUint *);
    bool read_sub_capture(flxDataArrayUint *);
    uint read_photon_count();
    uint read_ref_photon_count();
    uint read_ambient_light();

    // methods for our read-write properties
    uint16_t get_report_period();
    void set_report_period(uint16_t);

    // methods for write properties
    void factory_calibration();

    // flags to prevent startMeasuring being called multiple times
    bool _confidence = false;
    bool _distance = false;
    bool _channel = false;
    bool _sub_capture = false;
    bool _photon_count = false;
    bool _ref_photon_count = false;
    bool _ambient_light = false;

    tmf882x_msg_meas_results _results;

    uint16_t _reportPeriod = 460;

  public:
    // Define our output parameters - specify the get functions to call.
    // Strictly, these should be uint32_t
    flxParameterOutArrayUint<flxDevTMF882X, &flxDevTMF882X::read_confidence> confidence;
    flxParameterOutArrayUint<flxDevTMF882X, &flxDevTMF882X::read_distance> distance;
    flxParameterOutArrayUint<flxDevTMF882X, &flxDevTMF882X::read_channel> channel;
    flxParameterOutArrayUint<flxDevTMF882X, &flxDevTMF882X::read_sub_capture> subCapture;
    flxParameterOutUint<flxDevTMF882X, &flxDevTMF882X::read_photon_count> photonCount;
    flxParameterOutUint<flxDevTMF882X, &flxDevTMF882X::read_ref_photon_count> refPhotonCount;
    flxParameterOutUint<flxDevTMF882X, &flxDevTMF882X::read_ambient_light> ambientLight;

    flxPropertyRWUint16<flxDevTMF882X, &flxDevTMF882X::get_report_period, &flxDevTMF882X::set_report_period>
        reportPeriod = {460, 6, 460};

    flxParameterInVoid<flxDevTMF882X, &flxDevTMF882X::factory_calibration> factoryCalibration;
};
