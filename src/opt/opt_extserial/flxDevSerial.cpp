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

flxDevSerial::flxDevSerial() : _isEnabled{false}, _serialPort{nullptr}, _theJob(nullptr)
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Serial Input Device");

    // Register properties
    flxRegister(isEnabled, "Enable", "When true, this serial input is enabled");

    flxRegister(serialValue, "Value", "The next line/value from the serial connection");
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
        return "";

    // just read everything available on the serial port and jam into a string
    std::string result = "";
    char szBuffer[256]; // Buffer to hold the incoming data
    size_t bytesRead = 0;

    int bytesAvailable = _serialPort->available();
    while (bytesAvailable > 0)
    {
        bytesRead = _serialPort->readBytes(
            (uint8_t *)szBuffer, bytesAvailable < sizeof(szBuffer) - 1 ? bytesAvailable : sizeof(szBuffer) - 1);

        // if nothing read, we're done
        if (bytesRead < 0)
            break;
        szBuffer[bytesRead] = '\0'; // Null-terminate the string

        // Append the byte to the result string
        result += szBuffer;
        bytesAvailable = _serialPort->available(); // Check how many bytes are left to read
    }

    // Return the line as a string
    return result;
}
//-----------------------------------------------------------------------
void flxDevSerial::checkJobState(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        if (_theJob != nullptr)
            flxRemoveJobFromQueue(*_theJob);
    }
    else
    {
        // If the job is not initialized, create it
        if (_theJob == nullptr)
        {
            _theJob.reset(new flxJob);
            // do we have a job?
            if (_theJob == nullptr)
            {
                flxLog_E("%s: Unable to create job - external serial device disabled", name());
                _isEnabled = false;
                return; // No job allocated
            }
            _theJob->setup(name(), kflxDevSerialUpdateDelta, this, &flxDevSerial::jobHandlerCB);
        }
        flxAddJobToQueue(*_theJob);
    }
}
//-----------------------------------------------------------------------

void flxDevSerial::setSerialPort(HardwareSerial *serialPort)
{
    if (_serialPort == serialPort)
        return; // No change

    _serialPort = serialPort; // Set the serial port

    checkJobState();
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
}

//----------------------------------------------------------------------------------------------------------
// Loop/timer job callback method

void flxDevSerial::jobHandlerCB(void)
{
    if (!_isEnabled || _serialPort == nullptr)
    {
        flxLog_D("Serial port is not enabled or not initialized %d: %x", _isEnabled, (uint32_t)_serialPort);

        // why is the job being called
        flxRemoveJobFromQueue(*_theJob);
        return; // Nothing to do
    }

    // Any data available?
    if (_serialPort->available() > 0)
        flxSendEvent(flxEvent::kOnSerialDataAvailable);
}