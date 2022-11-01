/*
 *
 *  spDevTMF882X.h
 *
 *  Spark Device object for the TMF882X device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_TMF882X_Library.h"

// What is the name used to ID this device?
#define kTMF882XDeviceName "tmf882x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevTMF882X : public spDeviceType<spDevTMF882X>, public SparkFun_TMF882X
{

public:
    spDevTMF882X();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    bool read_confidence(spDataArrayUint *);
    bool read_distance(spDataArrayUint *);
    bool read_channel(spDataArrayUint *);
    bool read_sub_capture(spDataArrayUint *);
    uint read_photon_count();
    uint read_ref_photon_count();
    uint read_ambient_light();

    //methods for our read-write properties
    uint16_t get_report_period();
    void set_report_period(uint16_t);

    //methods for write properties
    void factory_calibration();

    //flags to prevent startMeasuring being called multiple times
    bool _confidence = false;
    bool _distance = false;
    bool _channel = false;
    bool _sub_capture = false;
    bool _photon_count = false;
    bool _ref_photon_count = false;
    bool _ambient_light = false;

    tmf882x_msg_meas_results _results;

    bool _begun = false;
    uint16_t _reportPeriod = 460;

public:
    // Define our output parameters - specify the get functions to call.
    // Strictly, these should be uint32_t
    spParameterOutArrayUint<spDevTMF882X, &spDevTMF882X::read_confidence> confidence;    
    spParameterOutArrayUint<spDevTMF882X, &spDevTMF882X::read_distance> distance;    
    spParameterOutArrayUint<spDevTMF882X, &spDevTMF882X::read_channel> channel;    
    spParameterOutArrayUint<spDevTMF882X, &spDevTMF882X::read_sub_capture> subCapture;    
    spParameterOutUint<spDevTMF882X, &spDevTMF882X::read_photon_count> photonCount;    
    spParameterOutUint<spDevTMF882X, &spDevTMF882X::read_ref_photon_count> refPhotonCount;    
    spParameterOutUint<spDevTMF882X, &spDevTMF882X::read_ambient_light> ambientLight;    

    spPropertyRWUint16<spDevTMF882X, &spDevTMF882X::get_report_period, &spDevTMF882X::set_report_period> reportPeriod
        = { 460, 6, 460 };

    spParameterInVoid<spDevTMF882X, &spDevTMF882X::factory_calibration> factoryCalibration;    
};
