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

#include "spDevTwist.h"

uint8_t spDevTwist::defaultDeviceAddress[] = {QWIIC_TWIST_ADDR, kSparkDeviceAddressNull};

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
    spSetupDeviceIdent(kTwistDeviceName);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevTwist::isConnected(spDevI2C &i2cDriver, uint8_t address)
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
bool spDevTwist::onInitialize(TwoWire &wirePort)
{

    bool rc = TWIST::begin(wirePort, address());

    if (!rc)
        Serial.println("TWIST - begin failed");

    last_count = 0;
    was_clicked = TWIST::isPressed();
    return rc;
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

    // process events
    if (TWIST::isPressed() != was_clicked)
    {
        was_clicked = !was_clicked;
        on_clicked.emit(was_clicked);
    }

    int tmp = TWIST::getCount();
    if (tmp != last_count)
    {
        last_count = tmp;
        on_twist.emit(last_count);
    }

    return false;
}

