/*
 *
 *  spDevTwist.cpp
 *
 *  Device object for the Qwiic Twist device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevTwist.h"


// The Qwiic Button can be configured to have one of many I2C address (via I2C methods)
// The jumper link on the back of the board changes the default address from 0x3F to 0x3E
// We'll limit the supported addresses here to: 0x3F and 0x3E
uint8_t spDevTwist::defaultDeviceAddress[] = {0x3F, 0x3E, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevTwist);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevTwist::spDevTwist()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The SparkFun Qwiic Twist RGB Encoder");
    
    last_count = 0;
    last_button_state = false;
    this_button_state = false;
    toggle_state = false;

    // Register Property
    spRegister(pressMode, "Press Mode", "Select Press Mode or Click (Toggle) Mode");
    pressMode = true;
    spRegister(ledRed, "LED Red", "Set the red LED brightness: 0 - 255");
    ledRed = 128;
    spRegister(ledGreen, "LED Green", "Set the green LED brightness: 0 - 255");
    ledGreen = 0;
    spRegister(ledBlue, "LED Blue", "Set the blue LED brightness: 0 - 255");
    ledBlue = 128;

    // Register parameters
    spRegister(buttonState);
    spRegister(twistCount);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevTwist::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the version
    uint16_t version = 0;
    if (!i2cDriver.readRegister16(address, TWIST_VERSION, &version, false)) // Big Endian (Major, Minor)
        return false;
    return (version >= 0x0100);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevTwist::onInitialize(TwoWire &wirePort)
{

    bool rc = TWIST::begin(wirePort, address());

    if (!rc)
        spLog_E("TWIST - begin failed");

    this_button_state = TWIST::isPressed();
    last_button_state = this_button_state;

    last_count = TWIST::getCount();

    rc &= TWIST::setColor(0,0,0); // Make sure the LED is off

    return rc;
}

// GETTER methods for output params
bool spDevTwist::read_button_state()
{
    if (pressMode)
        return this_button_state;
    else
        return toggle_state;
}

int spDevTwist::get_twist_count()
{
    return last_count;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevTwist::onPropertyUpdate(const char *propName)
{

    // Serial.print("PROPERTY UPDATE: ");Serial.println(propName);
    // save();
}

bool spDevTwist::loop(void)
{
    bool result = false;

    // process events
    last_button_state = this_button_state; // Store the last button state
    this_button_state = TWIST::isPressed(); // Read the current button state

    if (pressMode)
    {
        if (last_button_state != this_button_state) // Has the button changed state?
        {
            if (this_button_state) // Is the button pressed now?
            {
                TWIST::setColor(ledRed, ledGreen, ledBlue);
            }
            else
            {
                TWIST::setColor(0,0,0);
            }

            on_clicked.emit(this_button_state);
            result = true;
        }
    }
    else // Click (Toggle) mode
    {
        if ((last_button_state == false) && (this_button_state == true)) // Has the button been pressed down?
        {
            toggle_state = !toggle_state; // Toggle toggle_state

            if (toggle_state) // Toggle the LED
                TWIST::setColor(ledRed, ledGreen, ledBlue);
            else
                TWIST::setColor(0,0,0);

            on_clicked.emit(toggle_state);
            result = true;
        }
    }

    int tmp = TWIST::getCount();
    if (tmp != last_count)
    {
        last_count = tmp;
        on_twist.emit(last_count);
        result = true;
    }

    return result;
}

