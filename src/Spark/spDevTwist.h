/*
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_Qwiic_Twist_Arduino_Library.h"
#include "spDevice.h"

// What is the name used to ID this device?
#define kTwistDeviceName "twist";
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevTwist : public spDevice<spDevTwist>, public TWIST
{

  public:
    spDevTwist();

    //-----------------------------------------------------------------
    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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

    // Called when a managed property is updated
    void onPropertyUpdate(const char *);

    // Events
    // Our output event
    spSignalBool on_clicked;
    spSignalInt on_twist;

    // Type testing:
    // A static instance var - that is an object (can check instance pointer)
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }

    bool loop(void);

  private:
    int last_count = 0;
    bool was_clicked = false;
};
