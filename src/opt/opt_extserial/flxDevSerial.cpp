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

#include "flxDevSerial.h"

// update period for the device.

#define kflxDevSerialUpdateDelta 50

const uint32_t kflxDevSerialDefaultBaudRate = 115200; // Default baud rate for the serial connection
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxDevSerial::flxDevSerial()
    : _pinRX{kNoPinSet}, _pinTX{kNoPinSet}, _isEnabled{false}, _baudRate{kflxDevSerialDefaultBaudRate},
      _serialPort{nullptr}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Serial Input Device");

    // Register properties
    flxRegister(isEnabled, "Enable", "When true, this serial input is enabled");
    flxRegister(rxPin, "RX Pin", "The read (RX) GPIO pin for the serial connection. 255 = disabled");
    flxRegister(txPin, "TX Pin", "The transmission (TX) pin cfor the serial connection. 255 = disabled");

    flxRegister(serialBaudRate, "Baud Rate", "Baud rate for the serial connection");

    flxRegister(serialValue, "Value", "The next line/value from the serial connection");

    // The update job used for this device
    _theJob.setup(name(), kflxDevSerialUpdateDelta, this, &flxDevSerial::jobHandlerCB);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
//
//-----------------------------------------------------------------------
bool flxDevSerial::onInitialize(void)
{
    // already initialized?
    if (isInitialized())
        return true;
}
//-----------------------------------------------------------------------
// setupSensor()
//
// Sets up the sensor - if the pin are defined, will setup the GPIO pins as needed.
//
bool flxDevSerial::setupSerial(void)
{

    // Pins define yet?
    if (_pinRX == kNoPinSet || _pinRX == kNoPinSet || !_isEnabled)
        return false;

    // If we have a serial port, delete it
    if (_serialPort != nullptr)
    {
        _serialPort->end();    // End the serial port if it was initialized
        delete _serialPort;    // Delete the serial port object
        _serialPort = nullptr; // Set the pointer to null
    }
    _serialPort = new HardwareSerial(1); // Create a new serial port object
    if (_serialPort == nullptr)
    {
        flxLog_E("Failed to create serial port object");
        return false; // Failed to create the serial port object
    }
    _serialPort->begin(_baudRate, SERIAL_8N1, _pinRX,
                       _pinTX); // Initialize the serial port with the specified baud rate and pins

    if ((*_serialPort) == false)
    {
        flxLog_E("Failed to initialize serial port on RX pin %u and TX pin %u", _pinRX, _pinTX);
        delete _serialPort;    // Clean up the serial port object
        _serialPort = nullptr; // Set the pointer to null
        return false;          // Failed to initialize the serial port
    }
    flxLog_V("Serial port initialized on RX pin %u and TX pin %u with baud rate %u", _pinRX, _pinTX, _baudRate);

    // Enable our update job
    flxAddJobToQueue(_theJob);

    // we are ready to rock
    setIsInitialized(true);

    return true;
}

std::string flxDevSerial::read_serial_value(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        flxLog_V("%s: Serial port is not initialized", name());
        return "";
    }

    // just read everything available on the serial port and jam into a string
    std::string result = "";
    char szBuffer[256]; // Buffer to hold the incoming data
    size_t bytesRead = 0;
    uint32_t iterations = 30; // set a cap on run iterations ...
    while (_serialPort->available() > 0 && iterations-- > 0)
    {
        // Read a byte from the serial port
        bytesRead = _serialPort->read((uint8_t *)szBuffer, sizeof(szBuffer) - 1);

        // if nothing read, we're done
        if (bytesRead < 0)
            break;
        szBuffer[bytesRead] = '\0'; // Null-terminate the string

        // Append the byte to the result string
        result += szBuffer;
    }

    // Log the received line
    flxLog_V("%s: Read %d characters", name(), result.size());

    // Return the line as a string
    return result;
}
//-----------------------------------------------------------------------
//  Properties
//-----------------------------------------------------------------------
//
// ENABLED?
bool flxDevSerial::get_is_enabled(void)
{
    return _isEnabled;
}
void flxDevSerial::set_is_enabled(bool enable)
{
    if (enable == _isEnabled)
        return;

    _isEnabled = enable;
    // are we turning this on?
    if (enable)
        setupSerial();
    else
        flxRemoveJobFromQueue(_theJob); // Remove the job from the queue if we are disabling
}

//-----------------------------------------------------------------------
// RX PIN
uint8_t flxDevSerial::get_rx_pin(void)
{
    return _pinRX;
}
void flxDevSerial::set_rx_pin(uint8_t newPin)
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
uint8_t flxDevSerial::get_tx_pin(void)
{
    return _pinTX;
}
void flxDevSerial::set_tx_pin(uint8_t newPin)
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
uint32_t flxDevSerial::get_baud_rate(void)
{
    return _baudRate;
}
void flxDevSerial::set_baud_rate(uint32_t baudRate)
{
    // same baud rate, same state
    if (_baudRate == baudRate)
        return;

    _baudRate = baudRate;

    // Baud rate changed, we need to reinitialize the serial port
    if (_serialPort != nullptr)
        setupSerial(); // Reinitialize the serial port with the new baud rate
}

//----------------------------------------------------------------------------------------------------------
// Loop/timer job callback method

void flxDevSerial::jobHandlerCB(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        flxLog_D("Serial port is not enabled or not initialized");

        // why is the job being called
        flxRemoveJobFromQueue(_theJob);
        return; // Nothing to do
    }

    // Any data available?
    if (_serialPort->available() > 0)
        flxSendEvent(flxEvent::kOnSerialDataAvailable); // Send an event that data is available
}