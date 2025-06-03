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

#define kflxDevSerialUpdateDelta 200

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

    // nothing to see here...
    return true;
}

//-----------------------------------------------------------------------
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

    flxLog_D("Serial read  - chars available: %d", _serialPort->available());
    // char cc;
    int bytesAvailable = _serialPort->available();
    while (bytesAvailable > 0)
    {
        // cc = _serialPort->read(); // Peek at the next byte without removing it
        // Read a byte from the serial port
        bytesRead = _serialPort->readBytes(
            (uint8_t *)szBuffer, bytesAvailable < sizeof(szBuffer) - 1 ? bytesAvailable : sizeof(szBuffer) - 1);
        flxLog_D("Serial read - Bytes read: %d", bytesRead);

        // if nothing read, we're done
        if (bytesRead < 0)
            break;
        szBuffer[bytesRead] = '\0'; // Null-terminate the string
        flxLog_D("Serial read - string: '%s'", szBuffer);

        // Append the byte to the result string
        result += szBuffer;
        bytesAvailable = _serialPort->available(); // Check how many bytes are left to read
    }

    // Log the received line
    flxLog_D("%s: Read %d characters, result: %s", name(), result.size(), result.c_str());

    // Return the line as a string
    return result;
}
//-----------------------------------------------------------------------
void flxDevSerial::checkJobState(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        flxLog_D("%s: Serial port is not enabled or not initialized %d: %x", __func__, _isEnabled,
                 (uint32_t)_serialPort);
        // why is the job being called
        flxRemoveJobFromQueue(_theJob);
    }
    else
    {
        flxLog_D("%s: Serial port is enabled - adding job to queue", __func__);
        // Add the job to the queue if we are enabling
        flxAddJobToQueue(_theJob);
    }
}
//-----------------------------------------------------------------------

void flxDevSerial::setSerialPort(HardwareSerial *serialPort)
{
    if (_serialPort == serialPort)
    {
        flxLog_D("%s: No change in serial port - returning", __func__);
        return; // No change
    }
    flxLog_D("%s: Setting serial port to %p", __func__, (void *)serialPort);
    _serialPort = serialPort; // Set the serial port

    checkJobState();
    // if (_serialPort != nullptr)
    // {
    //     // // Any pending data - empty it out
    //     // while (_serialPort->available() > 0)
    //     //     _serialPort->read(); // Read and discard any data
    //     // make sure the job is added
    // }
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

    checkJobState(); // Check the job state based on the new enabled state
    // // are we turning this on?
    // if (enable)
    //     flxAddJobToQueue(_theJob); // Add the job to the queue if we are enabling
    // else
    //     flxRemoveJobFromQueue(_theJob); // Remove the job from the queue if we are disabling
}

//----------------------------------------------------------------------------------------------------------
// Loop/timer job callback method

void flxDevSerial::jobHandlerCB(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        flxLog_D("Serial port is not enabled or not initialized %d: %x", _isEnabled, (uint32_t)_serialPort);

        // why is the job being called
        flxRemoveJobFromQueue(_theJob);
        return; // Nothing to do
    }

    // Any data available?
    if (_serialPort->available() > 0)
        flxSendEvent(flxEvent::kOnSerialDataAvailable);
}