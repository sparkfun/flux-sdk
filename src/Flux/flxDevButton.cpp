/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
/*
 *
 *  flxDevButton.cpp
 *
 *  Device object for the Qwiic Button device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevButton.h"

// The Qwiic Button can be configured to have any I2C address (via I2C methods)
// The four jumper links on the back of the board allow it to be given 16 addresses: 0x6F - 0x60
// To avoid collisions with other sensors (MCP9600, VCNL4040, SCD30) we'll limit the supported addresses here to: 0x6F - 0x68
uint8_t flxDevButton::defaultDeviceAddress[] = {0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevButton);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevButton::flxDevButton()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The SparkFun Qwiic Button");

    // Register Property
    flxRegister(pressMode, "Press Mode", "Select Press Mode or Click (Toggle) Mode");
    flxRegister(ledBrightness, "LED brightness", "Set the LED brightness");

    // Register parameters
    flxRegister(buttonState, "Button State", "The current state of the button");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevButton::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    if (address == 0x69)
    {
        // Check for a SEN54

        uint8_t productName[9]; // Product name is 48 bytes maximum, but we'll only read the first nine here ( Three * Two bytes plus CRC)
        uint16_t productNameReg = 0xD014;
        uint8_t productNameRegBytes[2] = { (uint8_t)(productNameReg >> 8), (uint8_t)(productNameReg & 0xFF)}; // MSB first
        bool couldBeSEN54 = i2cDriver.write(address, productNameRegBytes, 2);
        if (couldBeSEN54)
        {
            delay(20);
            couldBeSEN54 &= i2cDriver.receiveResponse(address, productName, 9) == 9;
        }

        if (couldBeSEN54)
        {
            // Check final CRC
            uint8_t crc = 0xFF; // Init with 0xFF
            for (uint8_t x = 6; x < 8; x++)
            {
                crc ^= productName[x]; // XOR-in the next input byte

                for (uint8_t i = 0; i < 8; i++)
                {
                    if ((crc & 0x80) != 0)
                        crc = (uint8_t)((crc << 1) ^ 0x31);
                    else
                        crc <<= 1;
                }
            }
            couldBeSEN54 &= crc == productName[8];
            couldBeSEN54 &= productName[0] == 'S';
            couldBeSEN54 &= productName[1] == 'E';
            couldBeSEN54 &= productName[3] == 'N';
            couldBeSEN54 &= productName[4] == '5';
            couldBeSEN54 &= productName[6] == '4';
        }

        if (couldBeSEN54)
            return false;
    }
    else
    {
        // For speed, ping the device address first
        if (!i2cDriver.ping(address))
            return false;
    }

    // Read the version
    uint16_t version = 0;
    if (!i2cDriver.readRegister16(address, SFE_QWIIC_BUTTON_FIRMWARE_MAJOR, &version, false)) // Big Endian (Major, Minor)
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
bool flxDevButton::onInitialize(TwoWire &wirePort)
{

    bool rc = QwiicButton::begin(address(), wirePort);

    if (!rc)
        flxLog_E("BUTTON - begin failed");

    _this_button_state = QwiicButton::isPressed();
    _last_button_state = _this_button_state;

    rc &= QwiicButton::LEDoff(); // Make sure the LED is off

    return rc;
}

// GETTER methods for output params
bool flxDevButton::read_button_state()
{
    if (_pressMode)
        return _this_button_state;
    else
        return _toggle_state;
}

// methods for the read-write properties
uint8_t flxDevButton::get_press_mode() { return (uint8_t)_pressMode; }
void flxDevButton::set_press_mode(uint8_t mode) { _pressMode = mode == 0 ? false : true; }
uint8_t flxDevButton::get_led_brightness() { return _ledBrightness; }
void flxDevButton::set_led_brightness(uint8_t brightness) { _ledBrightness = brightness; }


//----------------------------------------------------------------------------------------------------------
// Loop

bool flxDevButton::loop(void)
{
    bool result = false;

    // process events
    _last_button_state = _this_button_state; // Store the last button state
    _this_button_state = QwiicButton::isPressed(); // Read the current button state

    if (_pressMode)
    {
        if (_last_button_state != _this_button_state) // Has the button changed state?
        {
            if (_this_button_state) // Is the button pressed now?
            {
                QwiicButton::LEDon(_ledBrightness);
            }
            else
            {
                QwiicButton::LEDoff();
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
                QwiicButton::LEDon(_ledBrightness);
            else
                QwiicButton::LEDoff();

            on_clicked.emit(_toggle_state);
            result = true;
        }
    }

    return result;
}
