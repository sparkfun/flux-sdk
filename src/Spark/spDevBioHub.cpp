/*
 *
 *  spDevBioHub.h
 *
 *  Spark Device object for the BioHub device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevBioHub.h"

// Define our class static variables - allocs storage for them

#define kBioHubAddressDefault 0x55 // Unshifted

uint8_t spDevBioHub::defaultDeviceAddress[] = {kBioHubAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevBioHub);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBioHub::spDevBioHub()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Biometric Hub");

    // Register parameters
    spRegister(heartRate, "Heart Rate (BPM)", "Heart Rate (Beats Per Minute)");
    spRegister(confidence, "Confidence Level (%)", "Confidence Level (%)");
    spRegister(O2, "Oxygen Saturation (%)", "Oxygen Saturation (%)");
    spRegister(status, "Status", "Status");
    spRegister(eStatus, "Extended Status", "Extended Status");
    spRegister(O2R, "Oxygen R Value", "Oxygen R Value");

    // Register read-write properties
    spRegister(resetPin, "Reset Pin GPIO #", "Reset Pin GPIO #");
    resetPin.setDataLimit(reset_pin_limit);
    spRegister(mfioPin, "MFIO Pin GPIO #", "MFIO Pin GPIO #");
    mfioPin.setDataLimit(mfio_pin_limit);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBioHub::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
/* 
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t identity[2] = { 0xFF, 0x00 };
    bool couldBeBio = i2cDriver.write(address, identity, 2);
    uint8_t mcuType[2];
    couldBeBio &= i2cDriver.receiveResponse(address, mcuType, 2);
    couldBeBio &= (mcuType[0] == 0x00); // 0x00: SUCCESS
    couldBeBio &= (mcuType[1] == 0x01); // 0x00: MAX32625; 0x01: MAX32660/MAX32664 

    return (couldBeBio);
*/
    return true;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBioHub::onInitialize(TwoWire &wirePort)
{

    _bioI2cPort = &wirePort; // Cheat... Keep a local record of which wirePort is being used, so we can call begin again

    // begin should return 0x00, but will return 0xFF if the pins have not been defined
    uint8_t result = SparkFun_Bio_Sensor_Hub::begin(wirePort, _resetPin, _mfioPin);
    if (result == 0x00)
    {
        _beginAttempted = true;
        _beginSuccess = true;
        SparkFun_Bio_Sensor_Hub::configBpm(MODE_TWO); // MODE_TWO provides the oxygen R value
        return true;
    }
    else if (result == 0xFF)
    {
        // Pins have not (yet) been defined
        // Return true to indicate that the device is connected
        _beginAttempted = true;
        _beginSuccess = false;
        return true;
    }
    return false;
}

// get/set methods for our read-write properties
int spDevBioHub::get_reset_pin() { return _resetPin; }
void spDevBioHub::set_reset_pin(int pin)
{
    _resetPin = pin;
    if ((_beginAttempted) && (_resetPin >= 0) && (_mfioPin >= 0))
    {
        _beginSuccess = SparkFun_Bio_Sensor_Hub::begin(*_bioI2cPort, _resetPin, _mfioPin) == 0;
        SparkFun_Bio_Sensor_Hub::configBpm(MODE_TWO); // MODE_TWO provides the oxygen R value
    }
}
int spDevBioHub::get_mfio_pin() { return _mfioPin; }
void spDevBioHub::set_mfio_pin(int pin)
{
    _mfioPin = pin;
    if ((_beginAttempted) && (_resetPin >= 0) && (_mfioPin >= 0))
    {
        _beginSuccess = SparkFun_Bio_Sensor_Hub::begin(*_bioI2cPort, _resetPin, _mfioPin) == 0;
        SparkFun_Bio_Sensor_Hub::configBpm(MODE_TWO); // MODE_TWO provides the oxygen R value
    }
}

// GETTER methods for output params
uint16_t spDevBioHub::read_heart_rate()
{
    if (!_heartRate)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _confidence = true;
        _o2 = true;
        _status = true;
        _eStatus = true;
        _o2r = true;
    }
    _heartRate = false;
    return body.heartRate;
}
uint8_t spDevBioHub::read_confidence()
{
    if (!_confidence)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _heartRate = true;
        _o2 = true;
        _status = true;
        _eStatus = true;
        _o2r = true;
    }
    _confidence = false;
    return body.confidence;
}
uint16_t spDevBioHub::read_oxygen()
{
    if (!_o2)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _heartRate = true;
        _confidence = true;
        _status = true;
        _eStatus = true;
        _o2r = true;
    }
    _o2 = false;
    return body.oxygen;
}
uint8_t spDevBioHub::read_status()
{
    if (!_status)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _heartRate = true;
        _confidence = true;
        _o2 = true;
        _eStatus = true;
        _o2r = true;
    }
    _status = false;
    return body.status;
}
int8_t spDevBioHub::read_extended_status()
{
    if (!_eStatus)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _heartRate = true;
        _confidence = true;
        _o2 = true;
        _status = true;
        _o2r = true;
    }
    _eStatus = false;
    return body.extStatus;
}
float spDevBioHub::read_r_value()
{
    if (!_o2r)
    {
        body = SparkFun_Bio_Sensor_Hub::readBpm();
        _heartRate = true;
        _confidence = true;
        _o2 = true;
        _status = true;
        _eStatus = true;
    }
    _o2r = false;
    return body.rValue;
}

