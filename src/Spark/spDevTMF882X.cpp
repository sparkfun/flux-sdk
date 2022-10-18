/*
 *
 *  spDevTMF882X.h
 *
 *  Spark Device object for the TMF882X device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevTMF882X.h"

// Define our class static variables - allocs storage for them

#define kTMF882XAddressDefault 0x41 // Unshifted

#define kTMF882XIDReg 0xE3 // Reads 0x08. Ignore bits 6 and 7.

uint8_t spDevTMF882X::defaultDeviceAddress[] = {kTMF882XAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevTMF882X);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevTMF882X::spDevTMF882X()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("TMF882X Distance Sensor");

    // Register parameters
    spRegister(confidence, "Confidence", "Confidence");
    spRegister(distance, "Distance (mm)", "Distance (mm)");
    spRegister(channel, "Channel", "Channel");
    spRegister(subCapture, "Sub Capture", "Sub Capture");
    spRegister(photonCount, "Photon Count", "Photon Count");
    spRegister(refPhotonCount, "Ref Photon Count", "Reference Photon Count");
    spRegister(ambientLight, "Ambient Light", "Ambient Light");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevTMF882X::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t idReg;
    bool couldBeTMF882 = i2cDriver.readRegister(address, kTMF882XIDReg, &idReg);
    couldBeTMF882 &= (idReg & 0x3F) == 0x08;

    return (couldBeTMF882);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevTMF882X::onInitialize(TwoWire &wirePort)
{

    bool result = SparkFun_TMF882X::begin(wirePort);
    if (!result)
        spLog_E("TMF882X - begin failed");
    return result;
}

// GETTER methods for output params
uint spDevTMF882X::read_confidence()
{
    if (!_confidence)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _confidence = false;
    uint32_t _numResults = _results.num_results;
    if (_numResults > 0)
        _numResults -= 1;
    return _results.results[_numResults].confidence;
}
uint spDevTMF882X::read_distance()
{
    if (!_distance)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _distance = false;
    uint32_t _numResults = _results.num_results;
    if (_numResults > 0)
        _numResults -= 1;
    return _results.results[_numResults].distance_mm;
}
uint spDevTMF882X::read_channel()
{
    if (!_channel)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _channel = false;
    uint32_t _numResults = _results.num_results;
    if (_numResults > 0)
        _numResults -= 1;
    return _results.results[_numResults].channel;
}
uint spDevTMF882X::read_sub_capture()
{
    if (!_sub_capture)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _photon_count = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _sub_capture = false;
    uint32_t _numResults = _results.num_results;
    if (_numResults > 0)
        _numResults -= 1;
    return _results.results[_numResults].sub_capture;
}
uint spDevTMF882X::read_photon_count()
{
    if (!_photon_count)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _ref_photon_count = true;
            _ambient_light = true;
        }
    }
    _photon_count = false;
    return _results.photon_count;
}
uint spDevTMF882X::read_ref_photon_count()
{
    if (!_ref_photon_count)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ambient_light = true;
        }
    }
    _ref_photon_count = false;
    return _results.ref_photon_count;
}
uint spDevTMF882X::read_ambient_light()
{
    if (!_ambient_light)
    {
        if (SparkFun_TMF882X::startMeasuring(_results))
        {
            _confidence = true;
            _distance = true;
            _channel = true;
            _sub_capture = true;
            _photon_count = true;
            _ref_photon_count = true;
        }
    }
    _ambient_light = false;
    return _results.ambient_light;
}

