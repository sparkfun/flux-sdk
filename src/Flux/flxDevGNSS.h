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
    uint read_year();
    uint read_month();
    uint read_day();
    uint read_hour();
    uint read_min();
    uint read_sec();
    double read_latitude();
    double read_longitude();
    double read_altitude();
    double read_altitude_msl();
    uint read_siv();
    uint read_fix();
    uint read_carrier_soln();
    float read_ground_speed();
    float read_heading();
    float read_pdop();
    float read_horiz_acc();
    float read_vert_acc();
    uint read_tow();
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
    uint get_measurement_rate();
    void set_measurement_rate(uint);

    void jobHandlerCB(void);
    flxJob _theJob;

  public:
    // Define our read-write properties
    flxPropertyRWUint<flxDevGNSS, &flxDevGNSS::get_measurement_rate, &flxDevGNSS::set_measurement_rate> measurementRate;

    // Define our input parameters - specify the write functions to call.
    flxParameterInVoid<flxDevGNSS, &flxDevGNSS::factory_default> factoryDefault;

    // Define our output parameters - specify the read functions to call.
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_year> year;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_month> month;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_day> day;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_hour> hour;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_min> min;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_sec> sec;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_latitude> latitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_longitude> longitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_altitude> altitude;
    flxParameterOutDouble<flxDevGNSS, &flxDevGNSS::read_altitude_msl> altitudeMSL;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_siv> SIV;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_fix> fixType;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_carrier_soln> carrierSolution;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_ground_speed> groundSpeed;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_heading> heading;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_pdop> PDOP;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_horiz_acc> horizontalAccEst;
    flxParameterOutFloat<flxDevGNSS, &flxDevGNSS::read_vert_acc> verticalAccEst;
    flxParameterOutUint<flxDevGNSS, &flxDevGNSS::read_tow> TOW;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_iso8601> iso8601;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_yyyy_mm_dd> YYYYMMDD;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_yyyy_dd_mm> YYYYDDMM;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_dd_mm_yyyy> DDMMYYYY;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_hh_mm_ss> HHMMSS;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_fix_string> fixTypeStr;
    flxParameterOutString<flxDevGNSS, &flxDevGNSS::read_carrier_soln_string> carrierSolutionStr;

    //-----------------------------------------------------
    // Clock interface methods -- so the GNSS reciever can be used as a time reference.
    uint get_epoch(void)
    {
        uint32_t usec_t;

        return SFE_UBLOX_GNSS::getUnixEpoch(usec_t, 1);
    }

    void set_epoch(const uint &refEpoch)
    {
        // noop
    }

    bool valid_epoch(void)
    {
        return SFE_UBLOX_GNSS::getTimeValid(1);
    }
};
