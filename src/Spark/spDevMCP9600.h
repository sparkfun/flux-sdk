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

    // Called when a managed property is updated
    //void onPropertyUpdate(const char *);

    // Define our public/managed properites for this class.
    
    void set_AmbientResolution(bool);
    bool get_AmbientResolution(void);

    spPropertyRWBool<spDevMCP9600, &spDevMCP9600::get_AmbientResolution, &spDevMCP9600::set_AmbientResolution> ambient_resolution;

    void setThermocoupleResolution(uint);
    uint getThermocoupleResolution(void);
    
    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::getThermocoupleResolution, 
                                    &spDevMCP9600::setThermocoupleResolution> thermocouple_resolution;

    void setThermocoupleType(uint);
    uint  getThermocoupleType(void);

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::getThermocoupleType, 
                                    &spDevMCP9600::setThermocoupleType> thermocouple_type;

    void setFilterCoefficient(uint);
    uint getFilterCoefficient(void);

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::getFilterCoefficient, 
                                    &spDevMCP9600::setFilterCoefficient> filter_coefficent;

    void setBurstSamples(uint);
    uint getBurstSamples(void);

    spPropertyRWUint<spDevMCP9600, &spDevMCP9600::getBurstSamples, 
                                    &spDevMCP9600::setBurstSamples> burst_samples;


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
