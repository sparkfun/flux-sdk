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
    setDescription("TMF882X Time-Of-Flight Sensor");

    // Register parameters
    spRegister(confidence, "Confidence", "Confidence");
    spRegister(distance, "Distance (mm)", "Distance (mm)");
    spRegister(channel, "Channel", "Channel");
    spRegister(subCapture, "Sub Capture", "Sub Capture");
    spRegister(photonCount, "Photon Count", "Photon Count");
    spRegister(refPhotonCount, "Ref Photon Count", "Reference Photon Count");
    spRegister(ambientLight, "Ambient Light", "Ambient Light");

    spRegister(reportPeriod, "Report Period (ms)", "Report Period (ms)");

    spRegister(factoryCalibration, "Perform Factory Calibration", "Perform Factory Calibration - requires minimal ambient light and no target within 40 cm");
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

    _begun = SparkFun_TMF882X::begin(wirePort, address());
    if (!_begun)
        spLog_E("TMF882X - begin failed");
    return _begun;
}

//methods for our read-write properties
uint16_t spDevTMF882X::get_report_period()
{
    if (_begun)
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X set_report_period - unable to get device configuration");
            return _reportPeriod;
        }
        
        _reportPeriod = tofConfig.report_period_ms;
    }
    return _reportPeriod;
}
void spDevTMF882X::set_report_period(uint16_t period)
{
    if (_begun)
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X set_report_period - unable to get device configuration");
            return;
        }
        
        // Change the APP configuration
        //  - set the reporting period
        tofConfig.report_period_ms = period;
        _reportPeriod = period;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X set_report_period- unable to set device configuration");
            return;
        }
    }
}

//methods for write properties
void spDevTMF882X::factory_calibration()
{
    if (_begun)
    {
        // First set some config parameters to support the calibration
        struct tmf882x_mode_app_config tofConfig;
        if (!SparkFun_TMF882X::getTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X factory_calibration - unable to get device configuration");
            return;
        }
        
        // Change the APP configuration
        //  - set the reporting period to 460 milliseconds
        //  - set the iterations to 4,000,000 (4M) to perform factory calibration
        tofConfig.report_period_ms = 460;
        uint16_t originalIterations = tofConfig.kilo_iterations;
        tofConfig.kilo_iterations = 4000;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X factory_calibration - unable to set device configuration");
            return;
        }

        struct tmf882x_mode_app_calib factoryCal;
        if (!SparkFun_TMF882X::factoryCalibration(factoryCal))
            spLog_E("TMF882X factory_calibration - factory calibration failed");

        tofConfig.report_period_ms = _reportPeriod;
        tofConfig.kilo_iterations = originalIterations;

        if (!SparkFun_TMF882X::setTMF882XConfig(tofConfig)) 
        {
            spLog_E("TMF882X factory_calibration - unable to restore device configuration");
            return;
        }
    }
}

// GETTER methods for output params
bool spDevTMF882X::read_confidence(spDataArrayUint *conf)
{
    static uint32_t theConfidence[TMF882X_MAX_MEAS_RESULTS] = {0};

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

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theConfidence[result] = _results.results[result].confidence;
    }

    conf->set(theConfidence, 1, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool spDevTMF882X::read_distance(spDataArrayUint *dist)
{
    static uint32_t theDistance[TMF882X_MAX_MEAS_RESULTS] = {0};

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

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theDistance[result] = _results.results[result].distance_mm;
    }

    dist->set(theDistance, 1, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool spDevTMF882X::read_channel(spDataArrayUint *chan)
{
    static uint32_t theChannel[TMF882X_MAX_MEAS_RESULTS] = {0};

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

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theChannel[result] = _results.results[result].channel;
    }

    chan->set(theChannel, 1, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
}
bool spDevTMF882X::read_sub_capture(spDataArrayUint *sub)
{
    static uint32_t theSubCapture[TMF882X_MAX_MEAS_RESULTS] = {0};

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

    for (uint32_t result = 0; (result < _results.num_results) && (result < TMF882X_MAX_MEAS_RESULTS); result++)
    {
        theSubCapture[result] = _results.results[result].sub_capture;
    }

    sub->set(theSubCapture, 1, (_results.num_results < TMF882X_MAX_MEAS_RESULTS ? _results.num_results : TMF882X_MAX_MEAS_RESULTS), true); // don't copy

    return true;
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

