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

// Found 9600 works well overall
const uint32_t kflxExtSerialDefaultBaudRate = 9600; // Default baud rate for the serial connection
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxOptExtSerial::flxOptExtSerial()
    : _pinRX{kNoPinSet}, _pinTX{kNoPinSet}, _isEnabled{false}, _baudRate{kflxExtSerialDefaultBaudRate},
      _serialPort{nullptr}, _devSerial{nullptr}, _started{false}
{

    // Setup unique identifiers for this device and basic device object systems
    setName("External Serial", "External Serial Connections");

    // Register properties
    rxPin.setTitle("Serial Settings");
    flxRegister(rxPin, "RX Pin", "The read (RX) GPIO pin for the serial connection. 255 = disabled");
    flxRegister(txPin, "TX Pin", "The transmission (TX) pin cfor the serial connection. 255 = disabled");

    flxRegister(serialBaudRate, "Baud Rate", "Baud rate for the serial connection");

    serialDeviceEnabled.setTitle("Functionality");
    flxRegister(serialDeviceEnabled, "Serial Input Device", "Enable the serial input device for this connection");

    flux_add(this);
}

//----------------------------------------------------------------------------------------------------------
// initialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
//
//-----------------------------------------------------------------------
bool flxOptExtSerial::begin(void)
{
    _started = true;
    return setupDeviceSerialDriver();
}
//-----------------------------------------------------------------------
// setupSerial()
//
// Sets up the serial device/port/uart
//
bool flxOptExtSerial::setupSerial(void)
{
    // If we are not enabled or not started, just return
    if (!_isEnabled || !_started)
        return false;

    // Pins define yet?
    if (_pinRX == kNoPinSet || _pinTX == kNoPinSet)
    {
        flxLog_E(F("RX(%d) or TX(%d) pin not set for serial port"), _pinRX, _pinTX);
        return false;
    }

#ifdef ESP32

    _serialPort = &Serial1; // just use Serial1 for ESP32

    _serialPort->end(); // end the current settings -- reset the device
    delay(300);

    _serialPort->setRxBufferSize(512); // Set the RX buffer size for the serial port

    // Initialize the serial port with the specified baud rate and pins
    _serialPort->begin(_baudRate, SERIAL_8N1, _pinRX, _pinTX);
    delay(100);
    // Wait on serial - not sure if a timeout is needed ... but added for safety
    for (uint32_t startMS = millis(); !(*_serialPort) && millis() - startMS <= 5000;)
        delay(250);

#elif defined(ARDUINO_PICO_MAJOR)
    // NOTE: Not Tested
    // Serial2.setRX(_pinRX);    // Set the RX pin for Serial2
    // Serial2.setTX(_pinTX);    // Set the TX pin for Serial2
    // _bSerialIsAlloc = false;  // Serial2 is not dynamically allocated, so we don't delete it later
    // Serial2.begin(_baudRate); // Initialize Serial2 with the specified baud rate
    // _serialPort = &Serial2;
#endif

    if ((*_serialPort) == false)
    {
        flxLog_E("Failed to initialize serial port on RX pin %u and TX pin %u", _pinRX, _pinTX);
        _serialPort = nullptr; // Set the pointer to null
        return false;          // Failed to initialize the serial port
    }

    // do we have a serial device?
    if (_devSerial != nullptr)
        _devSerial->setSerialPort(_serialPort); // Set the serial port for the device

    return true;
}

//-----------------------------------------------------------------------
//  Properties
//-----------------------------------------------------------------------

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
    if (newPin != kNoPinSet && _isEnabled)
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
    if (newPin != kNoPinSet && _isEnabled)
        setupSerial(); // new pin, try setup
}

//-----------------------------------------------------------------------
// Baud Rate
uint32_t flxOptExtSerial::get_baud_rate(void)
{
    return _baudRate;
}
//-----------------------------------------------------------------------
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

//-----------------------------------------------------------------------
//
bool flxOptExtSerial::setupDeviceSerialDriver(void)
{
    if (!_isEnabled)
        return false;

    // do we have a serial port?
    if (_serialPort == nullptr)
    {
        if (!setupSerial())
        {
            flxLog_E("%s:Failed to setup serial port for device", name());
            return false; // Failed to setup the serial port
        }
    }

    // do we have a serial device?
    if (_devSerial == nullptr)
    {
        _devSerial = new flxDevSerial(); // Create a new serial device object

        if (_devSerial == nullptr)
        {
            flxLog_E("%s:Failed to create serial device object", name());
            return false; // Failed to create the serial device object
        }

        _devSerial->initialize(); // Initialize the device
    }
    _devSerial->setSerialPort(_serialPort); // Set the serial port for the device

    // Now enable the device and add to the internal device list
    _devSerial->isEnabled = true;

    // Make sure the device is not in the system already.
    if (!flux.contains(_devSerial))
        flux.add(_devSerial); // Add the serial device to the system

    return true;
}
//-----------------------------------------------------------------------
// enable or disable the serial device in the system
bool flxOptExtSerial::get_enable_serialdevice(void)
{
    return _devSerial != nullptr && _devSerial->isEnabled();
}
//-----------------------------------------------------------------------

void flxOptExtSerial::set_enable_serialdevice(bool enable)
{
    if (enable == get_enable_serialdevice())
        return; // No change, do nothing

    // if not started , just stash.
    if (!_started)
    {
        _isEnabled = enable; // Set the device as enabled or disabled
        return;
    }
    // if enable - do we have a serial port and device?
    if (enable)
    {
        _isEnabled = true; // Set the device as enabled
        setupDeviceSerialDriver();
    }
    else
    {
        // if we have a device, just pull it from the device list and disable it.
        if (_devSerial != nullptr)
        {
            _devSerial->isEnabled = false; // Disable the serial device
            flux.remove(_devSerial);       // Remove the serial device from the system       // Set the pointer to null
        }
    }
}
