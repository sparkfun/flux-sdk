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

    template <typename T> flxDevAnalogPin(std::vector<std::pair<const std::string, T>> limitSet) : flxDevAnalogPin()
    {
        setAvailablePins(limitSet);
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
    template <typename T> void setAvailablePins(std::vector<std::pair<const std::string, T>> &limitSet)
    {
        // setup available values for the sensor pin property
        if (limitSet.size() == 0)
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
        for (auto item : limitSet)
            thePinLimitSet->addItem(item.first.c_str(), (uint8_t)item.second);

        _pinAnalog = limitSet[1].second;

        sensorPin.setDataLimit(thePinLimitSet);
    }

    // properties
    flxPropertyRWBool<flxDevAnalogPin, &flxDevAnalogPin::get_is_enabled, &flxDevAnalogPin::set_is_enabled> isEnabled = {
        false};

    flxPropertyRWUInt8<flxDevAnalogPin, &flxDevAnalogPin::get_sensor_pin, &flxDevAnalogPin::set_sensor_pin> sensorPin;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt16<flxDevAnalogPin, &flxDevAnalogPin::read_pin_value> pinValue;
};
