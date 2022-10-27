/*
 *
 *  spDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "test_device.h"

// dummy ids
uint8_t test_device::defaultDeviceAddress[] = {01, 02};

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

test_device::test_device()
{

    // Setup unique identifiers for this device and basic device object systems
    setName("test_device", "For testing output array parameters");

    // Register parameters
    spRegister(array_bool, "bool.array", "Array Parameter Test");
    spRegister(array_int8, "int8.array", "Array Parameter Test");
    spRegister(array_int16, "int16.array", "Array Parameter Test");
    spRegister(array_int, "int.array", "Array Parameter Test");
    spRegister(array_uint8, "uint8.array", "Array Parameter Test");
    spRegister(array_uint16, "uint16.array", "Array Parameter Test");
    spRegister(array_uint, "uint.array", "Array Parameter Test");
    
    spRegister(array_float, "float.array", "Array Parameter Test");
    array_float.setPrecision(4);
    
    spRegister(array_double, "double.array", "Array Parameter Test");
    array_double.setPrecision(6);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool test_device::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // the test device is always connected
    return true;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool test_device::onInitialize(TwoWire &wirePort)
{
    // the test device always initializes
    return true;

}

// The array Getter methods ...

// our "getter" methods for the array output parameters
bool  test_device::get_bool(spDataArrayBool *theArray)
{
    bool mydata[] = {true, true, true, false, true, false};

    // set the value, single dimension, a copy is made
    theArray->set(mydata, sizeof(mydata)/sizeof(bool));

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_int8(spDataArrayInt8 *theArray)
{
    static int8_t mydata[] = {-10, -12, 0, 3, 11, 13};

    // set the value, 2 Dims, no copy
    theArray->set(mydata, 2, 3, true);

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_int16(spDataArrayInt16 *theArray)
{
    int16_t mydata[] = {300, 201, 102, 3, -10, -20, -30, 1000};

    // set the value, single dimension, a copy is made
    theArray->set(mydata, sizeof(mydata)/sizeof(int16_t));

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_int(spDataArrayInt *theArray)
{
    static int mydata[] = {-1000, -800, -400, -200, 200, 400, 800, 1000};

    // set the value, 3 dimensions, no copy
    theArray->set(mydata, 2, 2, 2, true);

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_uint8(spDataArrayUint8 *theArray)
{
    uint8_t mydata[] = {0, 12, 24, 254};

    // set the value, single dimension, a copy is made
    theArray->set(mydata, sizeof(mydata)/sizeof(uint8_t));

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_uint16(spDataArrayUint16 *theArray)
{
    static uint16_t mydata[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000};

    // set the value, single dimension, no copy
    theArray->set(mydata, sizeof(mydata)/sizeof(uint16_t), true);

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_uint(spDataArrayUint *theArray)
{
    uint mydata[] = {1, 10, 100, 1000, 10000, 2, 20, 200, 2000, 20000};

    // set the value, single dimension, a copy is made
    theArray->set(mydata, sizeof(mydata)/sizeof(uint));

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_float(spDataArrayFloat *theArray)
{
    static float mydata[] = {1.1111111, 2.222222, 3.33333, 4.444444, 53.55555, 6.666666};

    // multi dim, no copy
    theArray->set(mydata, 2, 3, true); // don't copy

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_double(spDataArrayDouble *theArray)
{
    static double mydata[] = {-1.1111199, -2.22222299, -3.3333399, 4.44444499, -53.5555599, 6.66666699};

    // multi dim, no copy
    theArray->set(mydata, 3, 2, true); // don't copy

    return true;
}

//-------------------------------------------------------------------
bool  test_device::get_string(spDataArrayString *theArray)
{
    // not there yet ... return false -- system should handle this ...
    return false;
}