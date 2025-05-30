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
 * @file flxDevSerial.cpp
 * @brief Device object receiving data via a serial port.
 *
 * DEV NOTE:
 *  Currently - May 2025 - this system supports only an ESP32 implementation.
 *
 *  To port to rp2350 - The Serial port would probably just be Serial2, with the internal pointer not used.
 *
 */

#include "Arduino.h"

#include "flxOptExtSerial.h"

#if defined(ARDUINO_PICO_MAJOR)
#include <SerialUART.h>
#endif

const uint32_t kflxExtSerialDefaultBaudRate = 115200; // Default baud rate for the serial connection
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxOptExtSerial::flxOptExtSerial()
    : _pinRX{kNoPinSet}, _pinTX{kNoPinSet}, _isEnabled{false}, _baudRate{kflxExtSerialDefaultBaudRate},
      _serialPort{nullptr}, _bSerialIsAlloc{false}
{

    // Setup unique identifiers for this device and basic device object systems
    setName("External Serial", "External Serial Connections");

    // Register properties
    flxRegister(rxPin, "RX Pin", "The read (RX) GPIO pin for the serial connection. 255 = disabled");
    flxRegister(txPin, "TX Pin", "The transmission (TX) pin cfor the serial connection. 255 = disabled");

    flxRegister(serialBaudRate, "Baud Rate", "Baud rate for the serial connection");
}

//----------------------------------------------------------------------------------------------------------
// initialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
//
//-----------------------------------------------------------------------
bool flxOptExtSerial::initialize(void)
{
    return true;
}
//-----------------------------------------------------------------------
// setupSensor()
//
// Sets up the sensor - if the pin are defined, will setup the GPIO pins as needed.
//
bool flxOptExtSerial::setupSerial(void)
{

    // Pins define yet?
    if (_pinRX == kNoPinSet || _pinRX == kNoPinSet || !_isEnabled)
        return false;

    // If we have a serial port, delete it
    if (_serialPort != nullptr)
    {
        _serialPort->end(); // End the serial port if it was initialized
        if (_bSerialIsAlloc)
            delete _serialPort; // Delete the serial port object
        _serialPort = nullptr;  // Set the pointer to null
    }
    // TODO: Check platform type here and take dif
#ifdef ESP32
    _serialPort = new HardwareSerial(1); // Create a new serial port object

    if (_serialPort == nullptr)
    {
        flxLog_E("Failed to create serial port object");
        return false; // Failed to create the serial port object
    }
    _bSerialIsAlloc = true; // Set the flag indicating that the serial port is allocated
    _serialPort->begin(_baudRate, SERIAL_8N1, _pinRX,
                       _pinTX); // Initialize the serial port with the specified baud rate and pins
#elif defined(ARDUINO_PICO_MAJOR)
    Serial2.setRX(_pinRX);    // Set the RX pin for Serial2
    Serial2.setTX(_pinTX);    // Set the TX pin for Serial2
    _bSerialIsAlloc = false;  // Serial2 is not dynamically allocated, so we don't delete it later
    Serial2.begin(_baudRate); // Initialize Serial2 with the specified baud rate
    _serialPort = &Serial2;
#endif

    if ((*_serialPort) == false)
    {
        flxLog_E("Failed to initialize serial port on RX pin %u and TX pin %u", _pinRX, _pinTX);
        delete _serialPort;    // Clean up the serial port object
        _serialPort = nullptr; // Set the pointer to null
        return false;          // Failed to initialize the serial port
    }
    flxLog_V("Serial port initialized on RX pin %u and TX pin %u with baud rate %u", _pinRX, _pinTX, _baudRate);

    return true;
}

//-----------------------------------------------------------------------
//  Properties
//-----------------------------------------------------------------------
//
// ENABLED?
bool flxOptExtSerial::get_is_enabled(void)
{
    return _isEnabled;
}
void flxOptExtSerial::set_is_enabled(bool enable)
{
    if (enable == _isEnabled)
        return;

    _isEnabled = enable;
    // are we turning this on?
    if (enable)
        setupSerial();
}

//-----------------------------------------------------------------------
// RX PIN
uint8_t flxOptExtSerial::get_rx_pin(void)
{
    return _pinRX;
}
void flxOptExtSerial::set_rx_pin(uint8_t newPin)
{
    // Same pin, same state
    if (_pinRX == newPin)
        return;

    _pinRX = newPin;

    // If this is a no pin set value, disable sensor
    if (newPin == kNoPinSet)
        set_is_enabled(false);
    else
        setupSerial(); // new pin, try setup
}

//-----------------------------------------------------------------------
// TX Pin
uint8_t flxOptExtSerial::get_tx_pin(void)
{
    return _pinTX;
}
void flxOptExtSerial::set_tx_pin(uint8_t newPin)
{
    // same pin, same state
    if (_pinTX == newPin)
        return;

    _pinTX = newPin;

    // If this is a no pin set value, disable sensor
    if (newPin == kNoPinSet)
        set_is_enabled(false);
    else
        setupSerial(); // new pin, try setup
}

//-----------------------------------------------------------------------
// Baud Rate
uint32_t flxOptExtSerial::get_baud_rate(void)
{
    return _baudRate;
}
void flxOptExtSerial::set_baud_rate(uint32_t baudRate)
{
    // same baud rate, same state
    if (_baudRate == baudRate)
        return;

    _baudRate = baudRate;

    // Baud rate changed, we need to reinitialize the serial port
    if (_serialPort != nullptr)
        setupSerial(); // Reinitialize the serial port with the new baud rate
}
