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
    
    // Register Property
    spRegister(pressMode, "Press Mode", "Select Press Mode or Click (Toggle) Mode");
    spRegister(ledRed, "LED Red", "Sets the red LED brightness");
    spRegister(ledGreen, "LED Green", "Sets the green LED brightness");
    spRegister(ledBlue, "LED Blue", "Sets the blue LED brightness");

    // Register parameters
    spRegister(buttonState);
    spRegister(twistCount);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevTwist::isConnected(spBusI2C &i2cDriver, uint8_t address)
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

    _this_button_state = TWIST::isPressed();
    _last_button_state = _this_button_state;

    _last_count = TWIST::getCount();

    rc &= TWIST::setColor(0,0,0); // Make sure the LED is off

    return rc;
}

// GETTER methods for output params
bool spDevTwist::read_button_state()
{
    if (_pressMode)
        return _this_button_state;
    else
        return _toggle_state;
}

int spDevTwist::get_twist_count()
{
    return _last_count;
}

// methods for the read-write properties
uint8_t spDevTwist::get_press_mode() { return (uint8_t)_pressMode; }
void spDevTwist::set_press_mode(uint8_t mode) { _pressMode = mode == 0 ? false : true; }
uint8_t spDevTwist::get_led_red() { return _ledRed; }
void spDevTwist::set_led_red(uint8_t brightness) { _ledRed = brightness; }
uint8_t spDevTwist::get_led_green() { return _ledGreen; }
void spDevTwist::set_led_green(uint8_t brightness) { _ledGreen = brightness; }
uint8_t spDevTwist::get_led_blue() { return _ledBlue; }
void spDevTwist::set_led_blue(uint8_t brightness) { _ledBlue = brightness; }

//----------------------------------------------------------------------------------------------------------
// Loop

bool spDevTwist::loop(void)
{
    bool result = false;

    // process events
    _last_button_state = _this_button_state; // Store the last button state
    _this_button_state = TWIST::isPressed(); // Read the current button state

    if (_pressMode)
    {
        if (_last_button_state != _this_button_state) // Has the button changed state?
        {
            if (_this_button_state) // Is the button pressed now?
            {
                TWIST::setColor(_ledRed, _ledGreen, _ledBlue);
            }
            else
            {
                TWIST::setColor(0,0,0);
            }

            on_clicked.emit(_this_button_state);
            result = true;
        }
    }
    else // Click (Toggle) mode
    {
        if ((_last_button_state == false) && (_this_button_state == true)) // Has the button been pressed down?
        {
            _toggle_state = !_toggle_state; // Toggle toggle_state

            if (_toggle_state) // Toggle the LED
                TWIST::setColor(_ledRed, _ledGreen, _ledBlue);
            else
                TWIST::setColor(0,0,0);

            on_clicked.emit(_toggle_state);
            result = true;
        }
    }

    int tmp = TWIST::getCount();
    if (tmp != _last_count)
    {
        _last_count = tmp;
        on_twist.emit(_last_count);
        result = true;
    }

    return result;
}

