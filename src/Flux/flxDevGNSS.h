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

#include "flxDevice.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"

// What is the name used to ID this device?
#define kGNSSDeviceName "GNSS"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevGNSS : public flxDeviceI2CType<flxDevGNSS>, public SFE_UBLOX_GNSS
{

public:
    flxDevGNSS();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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

    bool loop(void);

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
};