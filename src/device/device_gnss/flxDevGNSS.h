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
 *  flxDevGNSS.h
 *
 *  Spark Device object for the u-blox GNSS device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "flxClock.h"
#include "flxCoreJobs.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kGNSSDeviceName "GNSS"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevGNSS : public flxDeviceI2CType<flxDevGNSS>, public flxIClock, public SFE_UBLOX_GNSS
{

  public:
    flxDevGNSS();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kGNSSDeviceName;
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
    uint32_t read_year();
    uint32_t read_month();
    uint32_t read_day();
    uint32_t read_hour();
    uint32_t read_min();
    uint32_t read_sec();
    double read_latitude();
    double read_longitude();
    double read_altitude();
    double read_altitude_msl();
    uint32_t read_siv();
    uint32_t read_fix();
    uint32_t read_carrier_soln();
    float read_ground_speed();
    float read_heading();
    float read_pdop();
    float read_horiz_acc();
    float read_vert_acc();
    uint32_t read_tow();
    std::string read_iso8601();
    std::string read_yyyy_mm_dd();
    std::string read_yyyy_dd_mm();
    std::string read_dd_mm_yyyy();
    std::string read_hh_mm_ss();
    std::string read_fix_string();
    std::string read_carrier_soln_string();

    // methods used to write our input parameters
    void factory_default();

    // methods to get/set our read-write properties
    uint32_t get_measurement_rate();
    void set_measurement_rate(uint32_t);

    void jobHandlerCB(void);
    flxJob _theJob;

    bool get_location(flxDataArrayFloat *);

  public:
    // Define our read-write properties
    flxPropertyRWUInt32<flxDevGNSS, &flxDevGNSS::get_measurement_rate, &flxDevGNSS::set_measurement_rate>
        measurementRate;

    // Define our input parameters - specify the write functions to call.
    flxParameterInVoid<flxDevGNSS, &flxDevGNSS::factory_default> factoryDefault;

    // Define our output parameters - specify the read functions to call.
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_year> year;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_month> month;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_day> day;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_hour> hour;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_min> min;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_sec> sec;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_latitude> latitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_longitude> longitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_altitude> altitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_altitude_msl> altitudeMSL;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_siv> SIV;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_fix> fixType;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_carrier_soln> carrierSolution;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_ground_speed> groundSpeed;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_heading> heading;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_pdop> PDOP;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_horiz_acc> horizontalAccEst;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_vert_acc> verticalAccEst;
    flxParameterOutUInt32<flxDevGNSS, &flxDevGNSS::read_tow> TOW;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_iso8601> iso8601;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_yyyy_mm_dd> YYYYMMDD;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_yyyy_dd_mm> YYYYDDMM;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_dd_mm_yyyy> DDMMYYYY;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_hh_mm_ss> HHMMSS;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_fix_string> fixTypeStr;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_carrier_soln_string> carrierSolutionStr;

    // position
    flxParameterOutArrayFloat<flxDevGNSS, &flxDevGNSS::get_location> location;

    //-----------------------------------------------------
    // Clock interface methods -- so the GNSS reciever can be used as a time reference.
    uint32_t get_epoch(void)
    {
        uint32_t usec_t;

        return SFE_UBLOX_GNSS::getUnixEpoch(usec_t, 1);
    }

    void set_epoch(const uint32_t &refEpoch)
    {
        // noop
    }

    bool valid_epoch(void)
    {
        return SFE_UBLOX_GNSS::getTimeValid(1);
    }
};
