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

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxDevSerial::flxDevSerial() : _isEnabled{false}, _serialPort{nullptr}
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Serial Input Device");

    // Register properties
    flxRegister(isEnabled, "Enable", "When true, this serial input is enabled");

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
        flxAddJobToQueue(_theJob); // Add the job to the queue if we are enabling
    else
        flxRemoveJobFromQueue(_theJob); // Remove the job from the queue if we are disabling
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