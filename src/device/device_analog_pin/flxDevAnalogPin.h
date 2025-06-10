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
 * @file flxDevAnalogPin.h
 * @brief Header file for reading values from an Analog Pin.
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"

// What is the name used to ID this device?
#define kAnalogPinDeviceName "Analog Pin Reading"
//----------------------------------------------------------------------------------------------------------
// Define our class - This is a simple GPIO driven device

/**
 * @class flxDevAnalogPin
 * @brief A class to interface with an on-board Analog Pin GPIO device.
 *
 */
class flxDevAnalogPin : public flxDeviceGPIOType<flxDevAnalogPin>
{

  public:
    /**
     * @brief Default constructor for the flxDevAnalogPin class.
     */
    flxDevAnalogPin();

    flxDevAnalogPin(std::initializer_list<std::pair<const std::string, uint8_t>> limitSet) : flxDevAnalogPin()
    {
        sensorPin.addDataLimitValidValue(limitSet);
    }

    static const char *getDeviceName()
    {
        return kAnalogPinDeviceName;
    };

  private:
    // props
    // is enabled?
    bool get_is_enabled(void);
    void set_is_enabled(bool);

    uint8_t get_sensor_pin(void);
    void set_sensor_pin(uint8_t);

    // methods used to get values for our output parameters
    uint16_t read_pin_value();

    bool _isEnabled; // is this pin enabled?

    uint8_t _pinAnalog;

  public:
    void setAvailablePins(const int *pPins, char *const *pNames, const size_t length);

    // properties
    flxPropertyRWBool<flxDevAnalogPin, &flxDevAnalogPin::get_is_enabled, &flxDevAnalogPin::set_is_enabled> isEnabled = {
        false};

    flxPropertyRWUInt8<flxDevAnalogPin, &flxDevAnalogPin::get_sensor_pin, &flxDevAnalogPin::set_sensor_pin> sensorPin;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt16<flxDevAnalogPin, &flxDevAnalogPin::read_pin_value> pinValue;
};
