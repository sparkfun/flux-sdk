/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/**
 * @file flxDevAnalogPin.cpp
 * @brief Device object for the SparkFun Soil Moisture sensor.
 *
 * This file contains the implementation of the device object for the SparkFun Soil Moisture sensor.
 * Note - this is a GPIO device, which depends on the Soil Moisture Sensor being connected to defined GPIO pins.
 * The required pins are VCC - a digitally controlled pin (to set low and high to power the sensor during reading),
 * and Sensor - which is an ANALOG pin to read the sensor value. These pins are settable via properties.
 *
 * @details
 * The class provides methods to initialize the sensor, read moisture values, and calibrate the sensor for dry and wet
 * states. It also includes properties to enable the sensor, set the GPIO pins, and retrieve the moisture values.
 *
 * @date 2025-03-05
 * @version 1.0
 * @note This file is part of the SparkFun Electronics Flux SDK.
 */

#include "Arduino.h"

#include "flxDevAnalogPin.h"

// When calibration is performed (average over N values) - this const is N.
const uint8_t kCalibrationIterations = 5;

// Read values can vary greatly between reads - even ms apart (very noisy sensor).
// Normally this is okay, but when multiple reads are made for a single "observation" (sensor value, %moist),
// the values should match - so we cache the value for a kCachedValueDeltaTicks.

const uint16_t kCachedValueDeltaTicks = 1000;

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxDevAnalogPin::flxDevAnalogPin() : _isEnabled{false}, _pinAnalog{0}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "Analog Pin Value Device");

    // Register properties
    flxRegister(isEnabled, "Enable this sensor", "When true, this sensor is enabled");

    flxRegister(sensorPin, "Sensor Pin", "The sensor GPIO pin connected to the soil sensor. 0 = disabled");

    // Register parameters
    flxRegister(pinValue, "Analog Pin Value", "The value of the Analog Pin");
}

//-----------------------------------------------------------------------
void flxDevAnalogPin::setAvailablePins(const int *pPins, char *const *pNames, const size_t length)
{
    // setup available values for the sensor pin property
    if (pPins == nullptr || pNames == nullptr || length == 0)
    {
        flxLog_E(F("%s:Invalid pin list for Analog Pin Device"), name());
        return;
    }
    flxDataLimitSetUInt8 *thePinLimitSet = new flxDataLimitSetUInt8; // reset the previous pin limit set
    if (thePinLimitSet == nullptr)
    {
        flxLog_W(F("%s:Failed to allocate pin limit set"), name());
        return;
    }

    // We set the provided pins as limits on the pin property. Let's set that up
    for (int i = 0; i < length; i++)
    {
        thePinLimitSet->addItem((const char *)pNames[i], pPins[i]);
        // first pin is our winner
        if (i == 1)
            _pinAnalog = pPins[i]; // set the first pin as the default
    }

    sensorPin.setDataLimit(thePinLimitSet);
}
//-----------------------------------------------------------------------
//  Properties
//-----------------------------------------------------------------------
//
// ENABLED?
bool flxDevAnalogPin::get_is_enabled(void)
{
    return _isEnabled;
}
void flxDevAnalogPin::set_is_enabled(bool enable)
{
    if (enable == _isEnabled)
        return;

    _isEnabled = enable;
}

//-----------------------------------------------------------------------
// Sensor Pin
uint8_t flxDevAnalogPin::get_sensor_pin(void)
{
    return _pinAnalog;
}
void flxDevAnalogPin::set_sensor_pin(uint8_t newPin)
{

    // same pin, same state
    if (_pinAnalog == newPin)
        return;

    _pinAnalog = newPin;
    setAddress(_pinAnalog);

    if (!_isEnabled)
        return;
}

//-----------------------------------------------------------------------
// GETTER methods for output params
//-----------------------------------------------------------------------
// Raw sensor value.
uint16_t flxDevAnalogPin::read_pin_value()
{
    // This returns the RAW sensor value
    if (!isInitialized() || !_isEnabled)
        return 0;

    return analogRead(_pinAnalog);

    // Read values can vary greatly between reads - even ms apart (very noisy sensor).
    // Normally this is okay, but when multiple reads are made for a single "observation" (sensor value, %moist),
    // the values should match - so we cache the value for a second or so. to clear out some noise.
    //
    // do we have a cached value?
    // if ((millis() - _lastValueTick) > kCachedValueDeltaTicks)
    // {
    //     // get the value from the sensor
    //     // enable power
    //     digitalWrite(_pinVCC, HIGH);
    //     delay(30);
    //     _lastValue = analogRead(_pinSensor);
    //     _lastValueTick = millis();
    //     // power off
    //     digitalWrite(_pinVCC, LOW);
    // }
    // return _lastValue;
}