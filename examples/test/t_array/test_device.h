/*
 *
 * A test_device for array parameter testing
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "Spark/spDevice.h"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class test_device : public spDeviceType<test_device>
{

public:
    test_device();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return "test_device";
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
    // our "getter" methods for the array output parameters
    bool  get_bool(spDataArrayBool *);
    bool  get_int8(spDataArrayInt8 *);
    bool  get_int16(spDataArrayInt16 *);
    bool  get_int(spDataArrayInt *);
    bool  get_uint8(spDataArrayUint8 *);
    bool  get_uint16(spDataArrayUint16 *);
    bool  get_uint(spDataArrayUint *);
    bool  get_float(spDataArrayFloat *);
    bool  get_double(spDataArrayDouble *);
    bool  get_string(spDataArrayString *);                                    

public:
    // Define our output parameters - specify the get functions to call.

    spParameterOutArrayBool<test_device, &test_device::get_bool>        array_bool;
    spParameterOutArrayInt8<test_device, &test_device::get_int8>        array_int8;
    spParameterOutArrayInt16<test_device, &test_device::get_int16>      array_int16;
    spParameterOutArrayInt<test_device, &test_device::get_int>          array_int;
    spParameterOutArrayUint8<test_device, &test_device::get_uint8>      array_uint8;
    spParameterOutArrayUint16<test_device, &test_device::get_uint16>    array_uint16;
    spParameterOutArrayUint<test_device, &test_device::get_uint>        array_uint;        
    spParameterOutArrayFloat<test_device, &test_device::get_float>     array_float;
    spParameterOutArrayDouble<test_device, &test_device::get_double>   array_double;    
    //spParameterOutArrayString<test_device, &test_device::get_string>   array_string;        

};
