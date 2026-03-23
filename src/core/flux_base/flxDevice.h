/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 * flxDevice.h
 *
 * Class that defines the interface between the system and the underlying device driver
 *
 * Provides the following capabilities
 *
 *   - Method to determine if underlying device is connected to the system
 *   - Methods to manage, serialize and describe device properties
 *   - Device initialization method
 *   - Logging method interface (called to capture data to record)
 *
 * TODO:
 *    Name!
 */

#pragma once

#include <Arduino.h>
#include <vector>

#include "flxCoreDevice.h"
#include "flxDeviceValueTypes.h"
#include "flxFlux.h"

//----------------------------------------------------------------------------------
// I2C device classes
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// flxDeviceI2CType()
//
// This subclass of flxDevice via template allows the core device class to
// access the static list of addresses to get the default address without
// requiring the subclass to implement a method to do this.
//
// Devices should subclass from this object using the following pattern:
//
//   class <classname> : flxDeviceI2CType<classname>, ...
//
template <typename T, typename B = flxDevice> class flxDeviceI2CType : public B
{
  public:
    // get the default address for the device. If none exists,
    // return Null Address...
    virtual uint8_t getDefaultAddress(void)
    {
        // call the classes static method to get all addresses
        // supported by this device (or the core addrs)
        const uint8_t *addresses = T::getDefaultAddresses();
        if (!addresses)
            return kSparkDeviceAddressNull;

        return addresses[0];
    }

    //---------------------------------------------------------------
    // Arduino Wire version ...
    //
    // This method will call the onInitialize() method of the device object
    bool initialize(TwoWire &wirePort)
    {
        if (B::address() == kSparkDeviceAddressNull)
            B::setAddress(getDefaultAddress());

        bool status = onInitialize(wirePort);

        // save our status flag ...
        B::setIsInitialized(status);

        return status;
    }

    //---------------------------------------------------------------
    // i2c driver version
    //
    // Will call the super class and the wireport version of this method
    bool initialize(flxBusI2C &i2cDriver)
    {
        // call the superclasses begin method.
        TwoWire *wirePort = i2cDriver.getWirePort();
        if (!wirePort)
            return false;

        // call the Wire version of the init methods -- this will
        // dispatch to the actual device onInitialize() method

        if (!this->initialize(*wirePort))
            return false;

        // Call the super class version of this method.
        // It ensures that the device is added to the system
        B::initialize(i2cDriver);

        return true;
    }

    //---------------------------------------------------------------
    // void version
    //
    // Grabs i2c driver and calls that version of method.
    bool initialize(void)
    {
        return initialize(flux.i2cDriver());
    }

    //---------------------------------------------------------------
    // version where a address is passed in
    bool initialize(uint8_t address)
    {
        if (!address)
            return false;

        B::setAddress(address);
        return initialize();
    }

    //---------------------------------------------------------------
    // place holder - or our sub-classes to overload
    //
    virtual bool onInitialize(TwoWire &)
    {
        return true;
    };

    // Typing system for devices
    //
    // Defines a type specific static method - so can be called outside
    // of an instance.
    //
    // The typeID is determined by hashing the name of the class.
    // This way the type ID is consistant across invocations

    static flxTypeID type(void)
    {
        static flxTypeID _myTypeID = flxGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    flxTypeID getType(void)
    {
        return type();
    }

    bool isType(flxTypeID type)
    {
        return type == getType();
    }

    // Device Kind Typing
    static flxDeviceKind_t kind(void)
    {
        return flxDeviceKindI2C;
    }

    flxDeviceKind_t getKind(void)
    {
        return kind();
    }
};

//----------------------------------------------------------------------------------
// SPI device classes
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// flxDeviceSPIType()
//
//
template <typename T, typename B = flxDevice> class flxDeviceSPIType : public B
{
  public:
    bool initialize(SPIClass &spiPort)
    {

        // Everything is ready to have the driver start talking to the SPI BUS.
        // Last step, setup
        uint8_t cs = chipSelect();
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);

        // Call the devices onInitialize() method --
        bool status = onInitialize(spiPort);

        // set our is init success flag.
        B::setIsInitialized(status);

        return status;
    }

    bool initialize(flxBusSPI &spiDriver)
    {

        // call the superclasses begin method.
        SPIClass *spiPort = spiDriver.getSPIPort();
        if (!spiPort)
            return false;

        // call the spi driver version of this method - will ensure device is init'd
        if (!this->initialize(*spiPort))
            return false;

        // Startup success - call the super class version of this method
        // This will add the driver to the system
        B::initialize(spiDriver);

        return true;
    }

    bool initialize(void)
    {
        return initialize(flux.spiDriver());
    }

    // version where a CS pin is passed in
    bool initialize(uint8_t cs)
    {
        if (!cs)
            return false;

        setChipSelect(cs);
        return initialize();
    }
    // For our sub-classes to overload
    virtual bool onInitialize(SPIClass &)
    {
        return true;
    };
    void setChipSelect(uint8_t cs)
    {
        B::setAddress(cs);
    }

    uint8_t chipSelect(void)
    {
        return B::address();
    }
    // Typing system for devices
    //
    // Defines a type specific static method - so can be called outside
    // of an instance.
    //
    // The typeID is determined by hashing the name of the class.
    // This way the type ID is consistent across invocations

    static flxTypeID type(void)
    {
        static flxTypeID _myTypeID = flxGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    flxTypeID getType(void)
    {
        return type();
    }

    bool isType(flxTypeID type)
    {
        return type == getType();
    }

    // Device Kind Typing
    static flxDeviceKind_t kind(void)
    {
        return flxDeviceKindSPI;
    }

    flxDeviceKind_t getKind(void)
    {
        return kind();
    }
};

//----------------------------------------------------------------------------------
// GPIO device classes
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// flxDeviceGPIOType()
//
// A simple base class that a GPIO based device can be build off of.
// This basically exists to help with type definition.  The actual device impl needs to
// needs to provide the required initialize() method for it's specific needs
//
//
template <typename T> class flxDeviceGPIOType : public flxDevice
{
  public:
    // Typing system for devices
    //
    // Defines a type specific static method - so can be called outside
    // of an instance.
    //
    // The typeID is determined by hashing the name of the class.
    // This way the type ID is consistent across invocations

    // virtual catch method if subclass forgets to implement it
    virtual bool onInitialize(void)
    {
        return true;
    }

    // a wrapper that the system can use for GPIO device
    bool initialize(void)
    {

        bool status = onInitialize();
        setIsInitialized(status);

        // Call the super class version of this method.
        // It ensures that the device is added to the system
        flxDevice::initialize();

        return status;
    }

    static flxTypeID type(void)
    {
        static flxTypeID _myTypeID = flxGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    flxTypeID getType(void)
    {
        return type();
    }

    bool isType(flxTypeID type)
    {
        return type == getType();
    }

    // Device Kind Typing
    static flxDeviceKind_t kind(void)
    {
        return flxDeviceKindGPIO;
    }

    flxDeviceKind_t getKind(void)
    {
        return kind();
    }
};
