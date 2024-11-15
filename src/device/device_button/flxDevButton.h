/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevButton.h
 *
 *  Device object for the Qwiic Button device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_Qwiic_Button.h"
#include "flxCoreJobs.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kButtonDeviceName "BUTTON"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevButton : public flxDeviceI2CType<flxDevButton>, public QwiicButton
{

  public:
    flxDevButton();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceFuzzy;
    }

    static const char *getDeviceName()
    {
        return kButtonDeviceName;
    };
    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];
    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // Events
    // Our output event
    flxSignalBool on_clicked;

  private:
    // methods used to get values for our output parameters
    bool read_button_state();
    void checkButton(void);

    // methods for our read-write properties
    uint8_t get_press_mode();
    void set_press_mode(uint8_t);
    uint8_t get_led_brightness();
    void set_led_brightness(uint8_t);
    bool _pressMode = true;
    bool _last_button_state = false;
    bool _this_button_state = false;
    bool _toggle_state = false;
    uint8_t _ledBrightness = 128;

    flxJob _theJob;

  public:
    flxPropertyRWUInt8<flxDevButton, &flxDevButton::get_press_mode, &flxDevButton::set_press_mode> pressMode = {
        1, {{"Click (Toggle) Mode", 0}, {"Press Mode", 1}}}; // 0 = Click (Toggle) mode. 1 = Press mode.
    flxPropertyRWUInt8<flxDevButton, &flxDevButton::get_led_brightness, &flxDevButton::set_led_brightness>
        ledBrightness;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutBool<flxDevButton, &flxDevButton::read_button_state> buttonState;
};
