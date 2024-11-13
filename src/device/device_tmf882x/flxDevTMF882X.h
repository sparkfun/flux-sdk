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
    bool read_confidence(flxDataArrayUInt32 *);
    bool read_distance(flxDataArrayUInt32 *);
    bool read_channel(flxDataArrayUInt32 *);
    bool read_sub_capture(flxDataArrayUInt32 *);
    uint32_t read_photon_count();
    uint32_t read_ref_photon_count();
    uint32_t read_ambient_light();

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
    flxParameterOutArrayUInt32<flxDevTMF882X, &flxDevTMF882X::read_confidence> confidence;
    flxParameterOutArrayUInt32<flxDevTMF882X, &flxDevTMF882X::read_distance> distance;
    flxParameterOutArrayUInt32<flxDevTMF882X, &flxDevTMF882X::read_channel> channel;
    flxParameterOutArrayUInt32<flxDevTMF882X, &flxDevTMF882X::read_sub_capture> subCapture;
    flxParameterOutUInt32<flxDevTMF882X, &flxDevTMF882X::read_photon_count> photonCount;
    flxParameterOutUInt32<flxDevTMF882X, &flxDevTMF882X::read_ref_photon_count> refPhotonCount;
    flxParameterOutUInt32<flxDevTMF882X, &flxDevTMF882X::read_ambient_light> ambientLight;

    flxPropertyRWUInt16<flxDevTMF882X, &flxDevTMF882X::get_report_period, &flxDevTMF882X::set_report_period>
        reportPeriod = {460, 6, 460};

    flxParameterInVoid<flxDevTMF882X, &flxDevTMF882X::factory_calibration> factoryCalibration;
};
