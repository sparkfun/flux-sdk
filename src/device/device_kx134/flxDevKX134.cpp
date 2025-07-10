/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevKX134.cpp
 *
 *  Spark Device object for the KX134  Qwiic device.
 */

#include "Arduino.h"

#include "flxDevKX134.h"

#include <algorithm>

// -------------------- Macros for property getters and setters ---------------------------------------------
// Enable properties' getters and setters
// We will have a set_<PropertyName> and get_<PropertyName> methods for each property.
// Note how we disable acceleration while setting each value, if it is enabled.
// This is because, according to the Arduino lib, many settings for KX13X can only be applied when accel is disabled
// We could also explicitly define these if we don't want to use macros. 
// If this is a common pattern, we could also move this up a level to a header file
#define FLX_KX134_CREATE_PROPERTY(TYPE, NAME, MEMBER, SETTER_FN)                     \
TYPE flxDevKX134::get_##NAME(void)                                                   \
{                                                                                    \
    return MEMBER;                                                                   \
}                                                                                    \
                                                                                     \
void flxDevKX134::set_##NAME(TYPE value)                                             \
{                                                                                    \
    if ((MEMBER == value) && !_in_setup)                                             \
        return; /* no change */                                                      \
                                                                                     \
    MEMBER = value;                                                                  \
                                                                                     \
    if (!isInitialized() && !_in_setup)                                              \
        return;                                                                      \
                                                                                     \
    bool prevAccelState = _enable_acceleration;                                      \
    if (prevAccelState && (std::string(#NAME) != "enable_acceleration"))             \
        SfeKX134ArdI2C::enableAccel(false);                                          \
                                                                                     \
    if (!SfeKX134ArdI2C::SETTER_FN(value))                                           \
        flxLog_W(F("%s : Failed to set " #NAME " to %d."), name(), value);           \
                                                                                     \
    if (prevAccelState && (std::string(#NAME) != "enable_acceleration"))             \
        SfeKX134ArdI2C::enableAccel(true);                                           \
}

// device addresses for our device interface -- using macros from qwiic/arduino library
uint8_t flxDevKX134::defaultDeviceAddress[] = {KX13X_ADDRESS_HIGH, kSparkDeviceAddressNull};

///
/// @brief Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevKX134);

//----------------------------------------------------------------------------------------------------------
/// @brief Constructor
///

flxDevKX134::flxDevKX134()
:   _in_setup{false},
    _enable_acceleration{true},    
    _enable_data_engine{true},
    _enable_tap_engine{false},
    _enable_tilt_engine{false},
    _enable_sleep_engine{false},
    _enable_wake_engine{false},
    _enable_direct_tap_interrupt{false},
    _enable_double_tap_interrupt{false},
    _range{SFE_KX134_RANGE16G},
    _output_data_rate{kOdr50Hz},
    _tap_data_rate{kTapOdr400Hz},
    _tilt_data_rate{kTiltOdr12_5Hz},
    _wake_data_rate{kWakeOdr0_781Hz},
    _lastAccelData{0.0f, 0.0f, 0.0f}
{

    setName(getDeviceName(), "KX134 Accelerometer");

    // Enable Props:
    flxRegister(enableAcceleration, "Enable Acceleration", "Enable acceleration data");
    flxRegister(enableDataEngine, "Enable Data Engine", "Enable the data engine for acceleration");
    flxRegister(enableTapEngine, "Enable Tap Engine", "Enable the tap detection engine");
    flxRegister(enableTiltEngine, "Enable Tilt Engine", "Enable the tilt detection engine");
    flxRegister(enableSleepEngine, "Enable Sleep Engine", "Enable the sleep engine");
    flxRegister(enableWakeEngine, "Enable Wake Engine", "Enable the wake engine");
    flxRegister(enableDirectTapInterrupt, "Enable Direct Tap Interrupt", "Enable direct tap interrupt");
    flxRegister(enableDoubleTapInterrupt, "Enable Double Tap Interrupt", "Enable double tap interrupt");

    // Other Props
    flxRegister(outputDataRate, "Output Data Rate", "Set output data rate (Hz)");
    flxRegister(tapDataRate, "Tap Data Rate", "Set tap detection data rate (Hz)");
    flxRegister(tiltDataRate, "Tilt Data Rate", "Set tilt detection data rate (Hz)");
    flxRegister(wakeDataRate, "Wake Data Rate", "Set wake detection data rate (Hz)");
    flxRegister(range, "Range", "Set accelerometer range");

    // Params
    flxRegister(tapDetected, "Tap Detected", "Tap detection status");
    flxRegister(accelData, "Acceleration Data", "Acceleration data (x, y, z)");
}

//----------------------------------------------------------------------------------------------------------
/// @brief  Static method called to determine if device is connected
///
/// @param  i2cDriver - Framework i2c bus driver
/// @param  address - The address to check
///
/// @return true if the device is connected
///
bool flxDevKX134::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (i2cDriver.ping(address))
    {
        uint8_t devID; // Create a variable to hold the device ID.

        uint8_t whoAmI = 0;

        // Read the WHO_AM_I register, if it errors then return 0.
        if (!i2cDriver.readRegister(address, SFE_KX13X_WHO_AM_I, &whoAmI))
            return false;

        // Check against the expected value
        return whoAmI == KX134_WHO_AM_I;
    }
    return false;
}

//----------------------------------------------------------------------------------------------------------
///
/// @brief Called during the startup/initialization of the driver (after the constructor is called).
///
/// @param wirePort - The Arduino wire port for the I2C bus
///
/// @return true on success
///

bool flxDevKX134::onInitialize(TwoWire &wirePort)
{
    // Initialize the sensor
    if (!SfeKX134ArdI2C::begin())
    {
        flxLog_D(F("%s : Failed to begin sensor."), name());
        return false;
    }

    _in_setup = true;

    // According to the Arduino lib, many settings for KX13X can only be applied when accel is disabled
    bool enableAccelInitial = _enable_acceleration; // Store the initial state of acceleration
    set_enable_acceleration(false);

    // Set the default range
    set_range(_range);

    // Set the default output data rate
    set_output_data_rate(_output_data_rate);

    // Set the default tap data rate
    set_tap_data_rate(_tap_data_rate);

    // Set the default tilt data rate
    set_tilt_data_rate(_tilt_data_rate);

    // Set the default wake data rate
    set_wake_data_rate(_wake_data_rate);

    // Enable/disable engines as needed
    set_enable_data_engine(_enable_data_engine);
    set_enable_tap_engine(_enable_tap_engine);
    set_enable_tilt_engine(_enable_tilt_engine);
    set_enable_sleep_engine(_enable_sleep_engine);
    set_enable_wake_engine(_enable_wake_engine);
    set_enable_direct_tap_interrupt(_enable_direct_tap_interrupt);
    set_enable_double_tap_interrupt(_enable_double_tap_interrupt);

    set_enable_acceleration(enableAccelInitial);

    _in_setup = false;

    return true;
}

// Boolean properties
FLX_KX134_CREATE_PROPERTY(bool, enable_acceleration, _enable_acceleration, enableAccel)
FLX_KX134_CREATE_PROPERTY(bool, enable_data_engine, _enable_data_engine, enableDataEngine)
FLX_KX134_CREATE_PROPERTY(bool, enable_tap_engine, _enable_tap_engine, enableTapEngine)
FLX_KX134_CREATE_PROPERTY(bool, enable_tilt_engine, _enable_tilt_engine, enableTiltEngine)
FLX_KX134_CREATE_PROPERTY(bool, enable_sleep_engine, _enable_sleep_engine, enableSleepEngine)
FLX_KX134_CREATE_PROPERTY(bool, enable_wake_engine, _enable_wake_engine, enableWakeEngine)
FLX_KX134_CREATE_PROPERTY(bool, enable_direct_tap_interrupt, _enable_direct_tap_interrupt, enableDirecTapInterupt)
FLX_KX134_CREATE_PROPERTY(bool, enable_double_tap_interrupt, _enable_double_tap_interrupt, enableDoubleTapInterrupt)

// Other Properties
FLX_KX134_CREATE_PROPERTY(uint8_t, output_data_rate, _output_data_rate, setOutputDataRate)
FLX_KX134_CREATE_PROPERTY(uint8_t, tap_data_rate, _tap_data_rate, setTapDataRate)
FLX_KX134_CREATE_PROPERTY(uint8_t, tilt_data_rate, _tilt_data_rate, setTiltDataRate)
FLX_KX134_CREATE_PROPERTY(uint8_t, wake_data_rate, _wake_data_rate, setWakeDataRate)
FLX_KX134_CREATE_PROPERTY(uint8_t, range, _range, setRange)

//---------------------------------------------------------------------------
///
/// @brief Called right before data parameters are read - take measurements called
///

bool flxDevKX134::execute(void)
{
    // enable data engine and enable accel should already be set so we shouldn't need to 
    // explicitly do it each time here...

    return true; // We don't need to anything here for the KX134
}

//---------------------------------------------------------------------------
// Outputs
//---------------------------------------------------------------------------
// GETTER methods for output params
// Since tap types share interrupts, if multiple taps are detected, there will be a priority order of what is returned:
// kSingleTap > kDoubleTap > kUnknownTap
uint8_t flxDevKX134::get_tap_detected(void) {
    tapType_t tapDetected = kNoTap;
    if (SfeKX134ArdI2C::tapDetected())
        tapDetected = kSingleTap;
    else if (SfeKX134ArdI2C::doubleTapDetected())
        tapDetected = kDoubleTap;
    else if (SfeKX134ArdI2C::unknownTap())
        tapDetected = kUnknownTap;
    
    if (tapDetected != kNoTap)
        SfeKX134ArdI2C::clearInterrupt();

    return static_cast<uint8_t>(tapDetected);
}

bool flxDevKX134::get_accel(flxDataArrayFloat *accelData)
{
    if (!SfeKX134ArdI2C::dataReady())
    {
        flxLog_W("%s : No valid new accelerometer data available. Using cached last measurement...", name());
        accelData->set(_lastAccelData, 3);
        return true; // TODO: should we return false here since no NEW data was fetched
    }

    outputData fetchAccelData;
    if (SfeKX134ArdI2C::getAccelData(&fetchAccelData))
    {

        // Store the last accelerometer data read
        _lastAccelData[0] = fetchAccelData.xData;
        _lastAccelData[1] = fetchAccelData.yData;
        _lastAccelData[2] = fetchAccelData.zData;

        // Set the output data
        accelData->set(_lastAccelData, 3);

        return true;
    }

    flxLog_E("%s : Failed to read accelerometer data.", name());
    return false;
}