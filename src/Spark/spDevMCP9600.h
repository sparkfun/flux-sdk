/*
 *
 *  spDevMCP9600.h
 *
 *  Driver for the MCP9600 Thermocouple Amp
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_MCP9600.h"

// What is the name used to ID this device?
#define kMCP9600DeviceName "mcp9600"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevMCP9600 : public spDeviceType<spDevMCP9600>, public MCP9600
{

  public:
    spDevMCP9600();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kMCP9600DeviceName;
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
    // Define our public/managed properties callbacks for this class.

    void set_AmbientResolution(uint8_t);
    uint8_t get_AmbientResolution(void);

    void set_ThermocoupleResolution(uint8_t);
    uint8_t get_ThermocoupleResolution(void);

    void set_ThermocoupleType(uint8_t);
    uint8_t get_ThermocoupleType(void);

    void set_FilterCoefficient(uint8_t);
    uint8_t get_FilterCoefficient(void);

    void set_BurstSamples(uint8_t);
    uint8_t get_BurstSamples(void);

  public:
    // property objs
    spPropertyRWUint8<spDevMCP9600, &spDevMCP9600::get_AmbientResolution, &spDevMCP9600::set_AmbientResolution> ambient_resolution;
    spDataLimitSetUint8 ambient_resolution_limit = { { "0.625", RES_ZERO_POINT_0625 }, { "0.25", RES_ZERO_POINT_25 } };

    spPropertyRWUint8<spDevMCP9600, &spDevMCP9600::get_ThermocoupleResolution, &spDevMCP9600::set_ThermocoupleResolution> thermocouple_resolution;
    spDataLimitSetUint8 thermocouple_resolution_limit = { { "18-bit", RES_18_BIT }, { "16-bit", RES_16_BIT },
                                                          { "14-bit", RES_14_BIT }, { "12-bit", RES_12_BIT } };

    spPropertyRWUint8<spDevMCP9600, &spDevMCP9600::get_ThermocoupleType, &spDevMCP9600::set_ThermocoupleType> thermocouple_type;
    spDataLimitSetUint8 thermocouple_type_limit = { { "Type K", TYPE_K }, { "Type J", TYPE_J },
                                                    { "Type T", TYPE_T }, { "Type N", TYPE_N },
                                                    { "Type S", TYPE_S }, { "Type E", TYPE_E },
                                                    { "Type B", TYPE_B }, { "Type R", TYPE_R } };

    spPropertyRWUint8<spDevMCP9600, &spDevMCP9600::get_FilterCoefficient, &spDevMCP9600::set_FilterCoefficient> filter_coefficent;
    spDataLimitRangeUint8 filter_coefficient_limit = { 0, 7 };

    spPropertyRWUint8<spDevMCP9600, &spDevMCP9600::get_BurstSamples, &spDevMCP9600::set_BurstSamples> burst_samples;
    spDataLimitSetUint8 burst_samples_limit = { { "1", SAMPLES_1 }, { "2", SAMPLES_2 },
                                                { "4", SAMPLES_4 }, { "8", SAMPLES_8 },
                                                { "16", SAMPLES_16 }, { "32", SAMPLES_32 },
                                                { "64", SAMPLES_64 }, { "128", SAMPLES_128 } };

  private:
    // For the output param call - no args
    float read_ThermocoupleTemp(void);
    float read_AmbientTemp(void);
    float read_TempDelta(void);

  public:
    // Output args

    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::read_ThermocoupleTemp> thermocouple_temp;
    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::read_AmbientTemp> ambient_temp;
    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::read_TempDelta> temp_delta;
    
};
