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

    // Called when a managed property is updated
    void onPropertyUpdate(const char *);

    // Events
    // Our output event
    spSignalBool on_clicked;
    spSignalVoid on_clicked_event;

    bool loop(void);

  private:
    bool last_button_state = false;
    bool this_button_state = false;
    bool toggle_state = false;
    // methods used to get values for our output parameters
    bool read_button_state();

  public:
    spPropertyBool<spDevButton> pressMode; // true = Press mode. false = Click (Toggle) mode
    spPropertyUint8<spDevButton> ledBrightness;

    // Define our output parameters - specify the get functions to call.
    spParameterOutBool<spDevButton, &spDevButton::read_button_state> buttonState;
};
