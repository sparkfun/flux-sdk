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
 * @file flxDevSerial.h
 * @brief Header file for the SparkFun Soil Moisture sensor device object.
 *
 * This file contains the definition of the device object for the SparkFun Soil Moisture sensor.
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

#pragma once

#include "Arduino.h"

#include "flxCoreEvent.h"
#include "flxCoreJobs.h"
#include "flxDevice.h"
#include <HardwareSerial.h>

// Define an event for serial data available
flxDefineEventID(kOnSerialDataAvailable);

// What is the name used to ID this device?
#define kSerialDeviceName "Serial Device"
//----------------------------------------------------------------------------------------------------------
// Define our class - This is a simple GPIO driven device

/**
 * @class flxDevSerial
 * @brief A class to interface with the SparkFun Soil Moisture sensor.
 *
 * This class provides methods to initialize the sensor, read moisture values, and calibrate the sensor for dry and wet
 * states. It also includes properties to enable the sensor, set the GPIO pins, and retrieve the moisture values.
 */
class flxDevSerial : public flxDeviceGPIOType<flxDevSerial>
{

  public:
    /**
     * @brief Default constructor for the flxDevSerial class.
     */
    flxDevSerial();
    ~flxDevSerial()
    {
        if (_serialPort != nullptr)
        {
            _serialPort->end();    // End the serial port if it was initialized
            delete _serialPort;    // Delete the serial port object
            _serialPort = nullptr; // Set the pointer to null
        }
    }
    /**
     * @brief Parameterized constructor for the flxDevSerial class.
     * @param pinRX The GPIO pin connected to the RX of the serial connection
     * @param pinTX The GPIO pin connected to the TX of the serial connection
     */
    flxDevSerial(uint8_t pinRX, uint8_t pinTX) : _pinRX{pinRX}, _pinTX{pinTX}
    {
    }
    /**
     * @brief Get the device name.
     * @return The name of the device.
     */
    static const char *getDeviceName()
    {
        return kSerialDeviceName;
    };
    /**
     * @brief Method called to initialize the class.
     * @return True if initialization is successful, false otherwise.
     */
    bool onInitialize(void);

  private:
    bool setupSerial(void);
    // consts
    static constexpr uint8_t kNoPinSet = 255;

    // props
    // is enabled?
    bool get_is_enabled(void);
    void set_is_enabled(bool);

    uint8_t get_rx_pin(void);
    void set_rx_pin(uint8_t);

    uint8_t get_tx_pin(void);
    void set_tx_pin(uint8_t);

    uint32_t get_baud_rate(void);
    void set_baud_rate(uint32_t);

    // methods used to get values for our output parameters
    std::string read_serial_value();

    void jobHandlerCB(void);
    flxJob _theJob;

    uint8_t _pinRX;
    uint8_t _pinTX;

    bool _isEnabled;

    uint32_t _baudRate;

    HardwareSerial *_serialPort; // Pointer to the serial port used for communication

  public:
    // properties
    flxPropertyRWBool<flxDevSerial, &flxDevSerial::get_is_enabled, &flxDevSerial::set_is_enabled> isEnabled = {false};

    flxPropertyRWUInt8<flxDevSerial, &flxDevSerial::get_rx_pin, &flxDevSerial::set_rx_pin> rxPin;
    flxPropertyRWUInt8<flxDevSerial, &flxDevSerial::get_tx_pin, &flxDevSerial::set_tx_pin> txPin;

    // Serial Baud rate setting
    flxPropertyRWUInt32<flxDevSerial, &flxDevSerial::get_baud_rate, &flxDevSerial::set_baud_rate> serialBaudRate = {
        1200, 500000};

    // Define our output parameters - specify the get functions to call.
    flxParameterOutString<flxDevSerial, &flxDevSerial::read_serial_value> serialValue;
};
