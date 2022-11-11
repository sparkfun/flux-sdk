/*
 *
 * spDevice.h
 *
 * Class that defines the interface between the system and the underlying device driver
 *
 * Provides the following capabiliites
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

#include "spCoreDevice.h"
#include "spSpark.h"

//----------------------------------------------------------------------------------
// I2C device classes
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// spDeviceI2CType()
//
// This subclass of spDevice via template allows the core device class to
// access the static list of addresses to get the default address without
// requiring the subclass to implement a method to do this.
//
// Devices should subclass from this object using the following pattern:
//
//   class <classname> : spDeviceI2CType<classname>, ...
//
template <typename T, typename B = spDevice> class spDeviceI2CType : public B
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
    bool initialize(spBusI2C &i2cDriver)
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
        return initialize(spark.i2cDriver());
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

    static spTypeID type(void)
    {
        static spTypeID _myTypeID = spGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    spTypeID getType(void)
    {
        return type();
    }

    bool isType(spTypeID type)
    {
        return type == getType();
    }

    // Device Kind Typing
    static spDeviceKind_t kind(void)
    {
        return spDeviceKindI2C;
    }

    spDeviceKind_t getKind(void)
    {
        return kind();
    }
};

//----------------------------------------------------------------------------------
// SPI device classes
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// spDeviceSPIType()
//
//
template <typename T, typename B = spDevice> class spDeviceSPIType : public B
{
  public:
    bool initialize(SPIClass &spiPort)
    {

        // Everything is ready to have the driver start talking to the SPI BUS.
        // Last step, setup ings
        // Magnetometer
        uint8_t cs = chipSelect();
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);

        return onInitialize(spiPort);
    }
    bool initialize(spBusSPI &spiDriver)
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
        return initialize(spark.spiDriver());
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

    static spTypeID type(void)
    {
        static spTypeID _myTypeID = spGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    spTypeID getType(void)
    {
        return type();
    }

    bool isType(spTypeID type)
    {
        return type == getType();
    }

    // Device Kind Typing
    static spDeviceKind_t kind(void)
    {
        return spDeviceKindSPI;
    }

    spDeviceKind_t getKind(void)
    {
        return kind();
    }
};
