/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 * A test_device for array parameter testing
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "Flux/flxDevice.h"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class test_device : public flxDeviceI2CType<test_device>
{

  public:
    test_device();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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
    bool get_bool(flxDataArrayBool *);
    bool get_int8(flxDataArrayInt8 *);
    bool get_int16(flxDataArrayInt16 *);
    bool get_int(flxDataArrayInt *);
    bool get_uint8(flxDataArrayUint8 *);
    bool get_uint16(flxDataArrayUint16 *);
    bool get_uint(flxDataArrayUint *);
    bool get_float(flxDataArrayFloat *);
    bool get_double(flxDataArrayDouble *);
    bool get_string(flxDataArrayString *);

  public:
    // Define our output parameters - specify the get functions to call.

    flxParameterOutArrayBool<test_device, &test_device::get_bool> array_bool;
    flxParameterOutArrayInt8<test_device, &test_device::get_int8> array_int8;
    flxParameterOutArrayInt16<test_device, &test_device::get_int16> array_int16;
    flxParameterOutArrayInt<test_device, &test_device::get_int> array_int;
    flxParameterOutArrayUint8<test_device, &test_device::get_uint8> array_uint8;
    flxParameterOutArrayUint16<test_device, &test_device::get_uint16> array_uint16;
    flxParameterOutArrayUint<test_device, &test_device::get_uint> array_uint;
    flxParameterOutArrayFloat<test_device, &test_device::get_float> array_float;
    flxParameterOutArrayDouble<test_device, &test_device::get_double> array_double;
    flxParameterOutArrayString<test_device, &test_device::get_string> array_string;
};
