/*
 *
 *  flxDevBioHub.h
 *
 *  Spark Device object for the BioHub device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBioHub.h"

// Define our class static variables - allocs storage for them

#define kBioHubAddressDefault 0x55 // Unshifted

uint8_t flxDevBioHub::defaultDeviceAddress[] = {kBioHubAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBioHub);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBioHub::flxDevBioHub()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("MAX30101 Biometric Hub");

    // Register parameters
    spRegister(heartRate, "Heart Rate (BPM)", "Heart Rate (Beats Per Minute)");
    spRegister(confidence, "Confidence Level (%)", "Confidence Level (%)");
    spRegister(O2, "Oxygen Saturation (%)", "Oxygen Saturation (%)");
    spRegister(status, "Status", "Status");
    spRegister(eStatus, "Extended Status", "Extended Status");
    spRegister(O2R, "Oxygen R Value", "Oxygen R Value");
}

// The Bio Hub is a special case. We need to know the reset and mfio pins in order to
// check if it is connected and to initialize it. During auto-detect the pins aren't known,
// so auto-detect will fail. We need to manually create an instance of the flxDevBioHub,
// initialize the pin numbers with `initialize`, check if it is connected, call onInitialize
// and then we can add it to the logger.
bool flxDevBioHub::initialize( int connectResetPin, int connectMfioPin )
{
    _resetPin = connectResetPin;
    _mfioPin = connectMfioPin;
    
    if ((_resetPin < 0) || (_mfioPin < 0))
        return false;

    // Reset the max32664
    pinMode(_mfioPin, OUTPUT);
    digitalWrite(_mfioPin, HIGH);
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);
    delay(10);
    digitalWrite(_resetPin, HIGH);
    delay(1000);
    pinMode(_mfioPin, INPUT_PULLUP); // To be used as an interrupt later

    uint8_t identity[2] = { 0xFF, 0x00 };
    bool couldBeBio = flux.i2cDriver().write(getDefaultAddress(), identity, 2);
    if (couldBeBio)
    {
        uint8_t mcuType[2];
        couldBeBio &= flux.i2cDriver().receiveResponse(getDefaultAddress(), mcuType, 2) == 2;
        couldBeBio &= (mcuType[0] == 0x00); // 0x00: SUCCESS
        couldBeBio &= (mcuType[1] == 0x01); // 0x00: MAX32625; 0x01: MAX32660/MAX32664
    }

    if (couldBeBio)
    {
        TwoWire *wirePort = flux.i2cDriver().getWirePort();
        if (onInitialize(*wirePort))
            return true;
    }
    
    return false;
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBioHub::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // The MAX32664 does not respond on the I2C bus until it has been reset correctly via its reset pin.
    // So we cannot ping it, or read its registers here - unless we know the pins.

    return false;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBioHub::onInitialize(TwoWire &wirePort)
{
    // On success, begin will return 0x00. It will return 0xFF if the pins have not been defined
    _begun = SparkFun_Bio_Sensor_Hub::begin(wirePort, _resetPin, _mfioPin) == 0x00;
    if (_begun)
        SparkFun_Bio_Sensor_Hub::configBpm(MODE_TWO); // MODE_TWO provides the oxygen R value
    return _begun;
}

// GETTER methods for output params
uint16_t flxDevBioHub::read_heart_rate()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0;

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
uint8_t flxDevBioHub::read_confidence()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0;

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
uint16_t flxDevBioHub::read_oxygen()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0;

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
uint8_t flxDevBioHub::read_status()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0;

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
int8_t flxDevBioHub::read_extended_status()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0;

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
float flxDevBioHub::read_r_value()
{
    if (!_begun) // Only call readBpm if the device has been begun sucessfully
        return 0.0;

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

