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
 * @file flxOptExtSerial.h
 *
 *  A action to manage an externally connected serial device or devices.
 *
 *  The RX connection can be used as an input device to the system and the TX connection can be used to send log data
 *  out of the system.s
 */

#pragma once

#include "Arduino.h"
#include "flxDevSerial.h"
#include "flxFlux.h"
#include <HardwareSerial.h>

//----------------------------------------------------------------------------------------------------------
// Define our class -

/**
 * @class flxOptExtSerial
 * This class provides methods to initialize the sensor, read moisture values, and calibrate the sensor for dry and wet
 * states. It also includes properties to enable the sensor, set the GPIO pins, and retrieve the moisture values.
 */
class flxOptExtSerial : public flxActionType<flxOptExtSerial>
{

  public:
    /**
     * @brief Default constructor for the flxDevSerial class.
     */
    flxOptExtSerial();
    ~flxOptExtSerial()
    {
        if (_serialPort != nullptr)
        {
            if (_devSerial != nullptr)
            {
                delete _devSerial;    // Delete the serial device object if it was allocated
                _devSerial = nullptr; // Set the pointer to null
            }
            _serialPort->end(); // End the serial port if it was initialized
            if (_bSerialIsAlloc)
                delete _serialPort; // Delete the serial port object if it was allocated
            _serialPort = nullptr;  // Set the pointer to null
        }
    }
    /**
     * @brief Parameterized constructor for the flxDevSerial class.
     * @param pinRX The GPIO pin connected to the RX of the serial connection
     * @param pinTX The GPIO pin connected to the TX of the serial connection
     */
    flxOptExtSerial(uint8_t pinRX, uint8_t pinTX) : _pinRX{pinRX}, _pinTX{pinTX}
    {
    }

    /**
     * @brief Method called to initialize the class.
     * @return True if initialization is successful, false otherwise.
     */
    bool initialize(void);

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

    bool get_enable_serialdevice(void);
    void set_enable_serialdevice(bool);

    uint8_t _pinRX;
    uint8_t _pinTX;

    bool _isEnabled;

    uint32_t _baudRate;

    HardwareSerial *_serialPort; // Pointer to the serial port used for communication
    bool _bSerialIsAlloc;        // Flag to indicate if the serial port is allocated

    flxDevSerial *_devSerial; // Pointer to the serial device object, if needed

  public:
    // properties
    flxPropertyRWBool<flxOptExtSerial, &flxOptExtSerial::get_is_enabled, &flxOptExtSerial::set_is_enabled> isEnabled = {
        false};

    flxPropertyRWUInt8<flxOptExtSerial, &flxOptExtSerial::get_rx_pin, &flxOptExtSerial::set_rx_pin> rxPin;
    flxPropertyRWUInt8<flxOptExtSerial, &flxOptExtSerial::get_tx_pin, &flxOptExtSerial::set_tx_pin> txPin;

    // enable the serial input device
    flxPropertyRWBool<flxOptExtSerial, &flxOptExtSerial::get_enable_serialdevice,
                      &flxOptExtSerial::set_enable_serialdevice>
        serialDeviceEnabled = {false};

    // Serial Baud rate setting
    flxPropertyRWUInt32<flxOptExtSerial, &flxOptExtSerial::get_baud_rate, &flxOptExtSerial::set_baud_rate>
        serialBaudRate = {1200, 500000};
};
