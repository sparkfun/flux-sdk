/*
 *
 * QwiicDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
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
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevButton::spDevButton()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(kButtonDeviceName);

    was_clicked = false;
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

    return rc;
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
    if (QwiicButton::isPressed() != was_clicked)
    {
        was_clicked = !was_clicked;
        on_clicked.emit(was_clicked);
    }

    return false;
}
//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevButton);
