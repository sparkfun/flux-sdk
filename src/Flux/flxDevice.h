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
#include "spSpark.h"

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
    bool initialize(TwoWire &wirePort)
    {
        if (B::address() == kSparkDeviceAddressNull)
            B::setAddress(getDefaultAddress());

        return onInitialize(wirePort);
    }
    bool initialize(flxBusI2C &i2cDriver)
    {

        // call the superclasses begin method.
        TwoWire *wirePort = i2cDriver.getWirePort();
        if (!wirePort)
            return false;

        // Add this device to the system
        B::initialize(i2cDriver);

        return this->initialize(*wirePort); // call  init routine.
    }
    bool initialize(void)
    {
        return initialize(flux.i2cDriver());
    }
    // version where a address is passed in
    bool initialize(uint8_t address)
    {
        if (!address)
            return false;

        B::setAddress(address);
        return initialize();
    }
    // For our sub-classes to overload
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
        // Magnetometer
        uint8_t cs = chipSelect();
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);

        return onInitialize(spiPort);
    }
    bool initialize(flxBusSPI &spiDriver)
    {

        // call the superclasses begin method.
        SPIClass *spiPort = spiDriver.getSPIPort();
        if (!spiPort)
            return false;

        B::initialize(spiDriver);

        return this->initialize(*spiPort); // call subclass virtual init routine.
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
        return flxDeviceKindSPI;
    }

    flxDeviceKind_t getKind(void)
    {
        return kind();
    }
};
