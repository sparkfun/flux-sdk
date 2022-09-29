/*
 *
 * QwiicDevButton.cpp
 *
 *  Device object for the Qwiic Button device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevButton.h"

// For type system testing
spType spDevButton::Type;
uint8_t spDevButton::defaultDeviceAddress[] = {SFE_QWIIC_BUTTON_DEFAULT_ADDRESS, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevButton);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevButton::spDevButton()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(kButtonDeviceName);

    last_button_state = false;
    this_button_state = false;
    toggle_state = false;

    // Register Property
    spRegister(pressMode, "Press Mode", "Select Press Mode or Click (Toggle) Mode");
    pressMode = true;
    spRegister(ledBrightness, "LED brightness", "Set the LED brightness: 0 - 255");
    ledBrightness = 128;

    // Register parameters
    spRegister(buttonState);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevButton::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

    return i2cDriver.ping(address);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevButton::onInitialize(TwoWire &wirePort)
{

    bool rc = QwiicButton::begin(address(), wirePort);

    if (!rc)
        Serial.println("BUTTON - begin failed");

    this_button_state = QwiicButton::isPressed();
    last_button_state = this_button_state;

    rc &= QwiicButton::LEDoff(); // Make sure the LED is off

    return rc;
}

// GETTER methods for output params
bool spDevButton::read_button_state()
{
    if (pressMode)
        return this_button_state;
    else
        return toggle_state;
}

//----------------------------------------------------------------------------------------------------------
// onPropertyUpdate()
//
// Called when the value of a managed property was updated.
//
void spDevButton::onPropertyUpdate(const char *propName)
{

    // Serial.print("PROPERTY UPDATE: ");Serial.println(propName);
    // save();
}

bool spDevButton::loop(void)
{

    // process events
    last_button_state = this_button_state; // Store the last button state
    this_button_state = QwiicButton::isPressed(); // Read the current button state

    if (pressMode)
    {
        if (last_button_state != this_button_state) // Has the button changed state?
        {
            if (this_button_state) // Is the button pressed now?
            {
                QwiicButton::LEDon(ledBrightness);
            }
            else
            {
                QwiicButton::LEDoff();
            }

            on_clicked.emit(this_button_state);
        }
    }
    else // Click (Toggle) mode
    {
        if ((last_button_state == false) && (this_button_state == true)) // Has the button cbeen pressed down?
        {
            toggle_state = !toggle_state; // Toggle toggle_state

            if (toggle_state) // Toggle the LED
                QwiicButton::LEDon(ledBrightness);
            else
                QwiicButton::LEDoff();

            on_clicked.emit(toggle_state);
        }
    }

    return false;
}

