/*
 *
 *  spDevGNSS.h
 *
 *  Spark Device object for the u-blox GNSS device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"

// What is the name used to ID this device?
#define kGNSSDeviceName "GNSS"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevGNSS : public spDeviceType<spDevGNSS>, public SFE_UBLOX_GNSS
{

public:
    spDevGNSS();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    spPropertyRWUint<spDevGNSS, &spDevGNSS::get_measurement_rate, &spDevGNSS::set_measurement_rate> measurementRate;

    // Define our input parameters - specify the write functions to call.
    spParameterInVoid<spDevGNSS, &spDevGNSS::factory_default> factoryDefault;    

    // Define our output parameters - specify the read functions to call.
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_year> year;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_month> month;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_day> day;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_hour> hour;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_min> min;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_sec> sec;    
    spParameterOutDouble<spDevGNSS, &spDevGNSS::read_latitude> latitude;    
    spParameterOutDouble<spDevGNSS, &spDevGNSS::read_longitude> longitude;    
    spParameterOutDouble<spDevGNSS, &spDevGNSS::read_altitude> altitude;
    spParameterOutDouble<spDevGNSS, &spDevGNSS::read_altitude_msl> altitudeMSL;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_siv> SIV;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_fix> fixType;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_carrier_soln> carrierSolution;    
    spParameterOutFloat<spDevGNSS, &spDevGNSS::read_ground_speed> groundSpeed;    
    spParameterOutFloat<spDevGNSS, &spDevGNSS::read_heading> heading;    
    spParameterOutFloat<spDevGNSS, &spDevGNSS::read_pdop> PDOP;    
    spParameterOutFloat<spDevGNSS, &spDevGNSS::read_horiz_acc> horizontalAccEst;    
    spParameterOutFloat<spDevGNSS, &spDevGNSS::read_vert_acc> verticalAccEst;    
    spParameterOutUint<spDevGNSS, &spDevGNSS::read_tow> TOW;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_iso8601> iso8601;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_yyyy_mm_dd> YYYYMMDD;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_yyyy_dd_mm> YYYYDDMM;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_dd_mm_yyyy> DDMMYYYY;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_hh_mm_ss> HHMMSS;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_fix_string> fixTypeStr;    
    spParameterOutString<spDevGNSS, &spDevGNSS::read_carrier_soln_string> carrierSolutionStr;    
};
