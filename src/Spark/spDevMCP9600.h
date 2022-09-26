/*
 *
 * Driver for the MCP9600 Thermocouple Amp
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include <SparkFun_MCP9600.h>

// What is the name used to ID this device?
#define kMCP9600DeviceName "mcp9600";
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevMCP9600 : public spDevice<spDevMCP9600>, public MCP9600
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
    // Define our public/managed properites callbacks for this class.

    void set_AmbientResolution(bool);
    bool get_AmbientResolution(void);
    void set_ThermocoupleResolution(uint);
    uint get_ThermocoupleResolution(void);

    void set_ThermocoupleType(uint);
    uint get_ThermocoupleType(void);

    void set_FilterCoefficient(uint);
    uint get_FilterCoefficient(void);

    void set_BurstSamples(uint);
    uint get_BurstSamples(void);

  public:
    // property objs
    spPropertyRWBool<spDevMCP9600, &spDevMCP9600::get_AmbientResolution, &spDevMCP9600::set_AmbientResolution>
        ambient_resolution;

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::get_ThermocoupleResolution, &spDevMCP9600::set_ThermocoupleResolution>
        thermocouple_resolution;

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::get_ThermocoupleType, &spDevMCP9600::set_ThermocoupleType>
        thermocouple_type;

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::get_FilterCoefficient, &spDevMCP9600::set_FilterCoefficient>
        filter_coefficent;

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::get_BurstSamples, &spDevMCP9600::set_BurstSamples> burst_samples;

  private:
    // For the output param call - no args
    float _getThermocoupleTemp(void)
    {
        return MCP9600::getThermocoupleTemp();
    }
    float _getAmbientTemp(void)
    {
        return MCP9600::getAmbientTemp();
    }
    float _getTempDelta(void)
    {
        return MCP9600::getTempDelta();
    }

  public:
    // Output args

    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::_getThermocoupleTemp> thermocouple_temp;
    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::_getAmbientTemp> ambient_temp;
    spParameterOutFloat<spDevMCP9600, &spDevMCP9600::_getTempDelta> temp_delta;

    // A static instance var - that is an object (can check instance pointer)
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }
};
