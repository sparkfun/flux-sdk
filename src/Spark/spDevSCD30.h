/*
 *
 *  spDevSCD30.h
 *
 *  Spark Device object for the SCD30 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_SCD30_Arduino_Library.h"

// What is the name used to ID this device?
#define kSCD30DeviceName "scd30"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSCD30 : public spDeviceType<spDevSCD30>, public SCD30
{

public:
    spDevSCD30();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSCD30DeviceName;
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
    void onPropertyUpdate(const char *);

private:

    // methods used to get values for our output parameters
    uint read_CO2();
    float read_temperature_C();
    float read_humidity();
    uint read_measurement_interval();
    uint read_altitude_compensation();
    float read_temperature_offset();

public:
    spPropertyUint<spDevSCD30> measurementInterval;
    spPropertyUint<spDevSCD30> altitudeCompensation;
    // Temp offset is only positive. See: https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library/issues/27#issuecomment-971986826
    //"The SCD30 offset temperature is obtained by subtracting the reference temperature from the SCD30 output temperature"
    // https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/9.5_CO2/Sensirion_CO2_Sensors_SCD30_Low_Power_Mode.pdf
    spPropertyFloat<spDevSCD30> temperatureOffset;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevSCD30, &spDevSCD30::read_CO2> co2PPM;    
    spParameterOutFloat<spDevSCD30, &spDevSCD30::read_temperature_C> temperatureC;    
    spParameterOutFloat<spDevSCD30, &spDevSCD30::read_humidity> humidity;    
    spParameterOutUint<spDevSCD30, &spDevSCD30::read_measurement_interval> measurementInterval_output;    
    spParameterOutUint<spDevSCD30, &spDevSCD30::read_altitude_compensation> altitudeCompensation_output;    
    spParameterOutFloat<spDevSCD30, &spDevSCD30::read_temperature_offset> temperatureOffset_output;    
};
