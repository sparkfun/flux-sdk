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
        spLog_E("BUTTON - begin failed");

    return rc;
}

bool spDevButton::loop(void)
{
    // process events
    if (QwiicButton::isPressed() != was_clicked)
    {
        was_clicked = !was_clicked;
        on_clicked.emit(was_clicked);      
    }

    return false;
}

