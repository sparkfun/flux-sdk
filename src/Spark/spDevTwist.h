/*
 *
 *  spDevTwist.h
 *
 *  Device object for the Qwiic Twist device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_Qwiic_Twist_Arduino_Library.h"
#include "spDevice.h"

// What is the name used to ID this device?
#define kTwistDeviceName "TWIST"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevTwist : public spDeviceI2CType<spDevTwist>, public TWIST
{

  public:
    spDevTwist();

    //-----------------------------------------------------------------
    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kTwistDeviceName;
    };
    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    //-----------------------------------------------------------------
    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // Events
    // Our output event
    spSignalBool on_clicked;
    spSignalInt on_twist;

    bool loop(void);

  private:
    int _last_count = 0;

    bool _pressMode = true;
    bool _last_button_state = false;
    bool _this_button_state = false;
    bool _toggle_state = false;

    // methods used to get values for our output parameters
    bool read_button_state();
    int get_twist_count();

    // methods for our read-write properties
    uint8_t get_press_mode();
    void set_press_mode(uint8_t);
    uint8_t get_led_red();
    void set_led_red(uint8_t);
    uint8_t get_led_green();
    void set_led_green(uint8_t);
    uint8_t get_led_blue();
    void set_led_blue(uint8_t);

    uint8_t _ledRed = 128;
    uint8_t _ledGreen = 0;
    uint8_t _ledBlue = 128;

  public:
    spPropertyRWUint8<spDevTwist, &spDevTwist::get_press_mode, &spDevTwist::set_press_mode> pressMode
      = { 1, { { "Click (Toggle) Mode", 0 }, { "Press Mode", 1 } } };
    spPropertyRWUint8<spDevTwist, &spDevTwist::get_led_red, &spDevTwist::set_led_red> ledRed;
    spPropertyRWUint8<spDevTwist, &spDevTwist::get_led_green, &spDevTwist::set_led_green> ledGreen;
    spPropertyRWUint8<spDevTwist, &spDevTwist::get_led_blue, &spDevTwist::set_led_blue> ledBlue;

    // Define our output parameters - specify the get functions to call.
    spParameterOutBool<spDevTwist, &spDevTwist::read_button_state> buttonState;
    spParameterOutInt<spDevTwist, &spDevTwist::get_twist_count> twistCount;
};
