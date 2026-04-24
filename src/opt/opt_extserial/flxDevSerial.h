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
 *
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
 * @brief A class to interface with a Input serial device
 * This class provides methods to initialize the serial device, read serial values, and manage the device's state.
 * It also includes properties to enable the device and retrieve the serial values.
 */
class flxDevSerial : public flxDeviceGPIOType<flxDevSerial>
{

  public:
    /**
     * @brief Default constructor for the flxDevSerial class.
     */
    flxDevSerial();

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

    void setSerialPort(HardwareSerial *serialPort);

  private:
    // props
    // is enabled?
    bool get_is_enabled(void);
    void set_is_enabled(bool);

    // methods used to get values for our output parameters
    std::string read_serial_value();

    void checkJobState(void);
    void jobHandlerCB(void);

    std::unique_ptr<flxJob> _theJob;

    bool _isEnabled;

    HardwareSerial *_serialPort; // Pointer to the serial port used for communication

  public:
    // properties
    flxPropertyRWBool<flxDevSerial, &flxDevSerial::get_is_enabled, &flxDevSerial::set_is_enabled> isEnabled = {false};

    // Define our output parameters - specify the get functions to call.
    flxParameterOutString<flxDevSerial, &flxDevSerial::read_serial_value> serialValue;
};
