/*
 *
 *  spDevButton.h
 *
 *  Device object for the Qwiic Button device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_Qwiic_Button.h"
#include "spDevice.h"

// What is the name used to ID this device?
#define kButtonDeviceName "button"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevButton : public spDeviceType<spDevButton>, public QwiicButton
{

  public:
    spDevButton();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    spSignalBool on_clicked;

    bool loop(void);

  private:
    // methods used to get values for our output parameters
    bool read_button_state();
    // methods for our read-write properties
    uint8_t get_press_mode();
    void set_press_mode(uint8_t);
    uint8_t get_led_brightness();
    void set_led_brightness(uint8_t);
    bool _pressMode;
    bool _last_button_state;
    bool _this_button_state;
    bool _toggle_state;
    uint8_t _ledBrightness;

  public:
    spPropertyRWUint8<spDevButton, &spDevButton::get_press_mode, &spDevButton::set_press_mode> pressMode; // true = Press mode. false = Click (Toggle) mode
    spDataLimitSetUint8 mode_limit = { { "Click (Toggle) Mode", 0 }, { "Press Mode", 1 } };
    spPropertyRWUint8<spDevButton, &spDevButton::get_led_brightness, &spDevButton::set_led_brightness> ledBrightness;

    // Define our output parameters - specify the get functions to call.
    spParameterOutBool<spDevButton, &spDevButton::read_button_state> buttonState;
};
