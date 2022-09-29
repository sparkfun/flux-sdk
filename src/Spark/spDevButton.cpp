/*
 *
 * QwiicDevButton.h
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

    was_clicked = false;

    // Register Property
    spRegister(toggleLEDonClick, "Toggle LED on click", "Toggle the LED state when the button is clicked");
    toggleLEDonClick = true;
    spRegister(ledBrightness, "LED brightness", "Set the LED brightness: 0 - 255");
    ledBrightness = 128;

    // Register parameters
    spRegister(clickedState);
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

    was_clicked = false; // Make sure was_clicked is false
    QwiicButton::LEDoff(); // Make sure the LED is off

    return rc;
}

// GETTER methods for output params
bool spDevButton::read_clicked_state()
{
    return was_clicked;
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
    // process events
    if (QwiicButton::isClickedQueueEmpty() == false) // If there are click events in the queue
    {
        QwiicButton::popClickedQueue(); // Pop the click event
        was_clicked = !was_clicked; // Toggle was_clicked
        if (toggleLEDonClick) // Toggle the LED
        {
            if (was_clicked)
                QwiicButton::LEDon(ledBrightness);
            else
                QwiicButton::LEDoff();
        }
        on_clicked.emit(was_clicked); // Emit was_clicked
    }

    return false;
}

