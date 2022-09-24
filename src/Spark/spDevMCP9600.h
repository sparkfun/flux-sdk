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
    // These same properties are registered with the system in the object constructor
    //spPropertyBool ambient_resolution;
    //spPropertyInt  thermocouple_resolution;
    //spPropertyInt  thermocouple_type;
    //spPropertyInt  filter_coefficent;    

    // output args
    //spParamOutFlt thermocouple_temp;
    //spParamOutFlt ambient_temp;
    //spParamOutFlt temp_delta;
    //spParamOutInt raw_adc;



    // A static instance var - that is an object (can check instance pointer)
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }
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
};
