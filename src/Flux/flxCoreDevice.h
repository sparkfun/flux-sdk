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
 * flxCoreDevice.h
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

#include "flxBusI2C.h"
#include "flxBusSPI.h"
#include "flxCore.h"
#include "flxUtils.h"

// define a value that marks the end of a device address/id list

#define kSparkDeviceAddressNull 0

typedef enum
{
    flxDeviceKindI2C,
    flxDeviceKindSPI,
    flxDeviceKindNone
} flxDeviceKind_t;

/////////////////////////////////////////////////////////////////////////////
//
// flxDevice()
//
// Define our device base class used for each device in the system.
//
// New devices create an object that subclasses from this object.
//
// The device object defines the interface and underlying logic that
// integrates the device into the overall system, and simplifies the r
// implementation requirements for each new device.
//
// Key capabilities provided:
//
//      - object state persistance/serialization
//      - Managed properties -
//      - System data collection from the device
//      - Works with the device Factory/Builder pattern.
//
//

class flxDeviceFactory_;

class flxDevice : public flxOperation
{

  public:
    flxDevice() : _autoload{false}, _address{kSparkDeviceAddressNull} {};

    virtual ~flxDevice()
    {
    }

    // Methods called on initialize
    bool initialize();
    virtual bool initialize(flxBusI2C &)
    {
        return initialize();
    };
    virtual bool initialize(flxBusSPI &)
    {
        return initialize();
    };

    bool autoload(void)
    {
        return false;
    }
    void setAutoload()
    {
        _autoload = true;
    }
    void setAddress(uint8_t address)
    {
        _address = address;
    }

    uint8_t address(void)
    {
        return _address;
    }

    virtual flxDeviceKind_t getKind(void)
    {
        return flxDeviceKindNone;
    }
    
  private:
    bool _autoload;
    uint8_t _address;
};

using flxDeviceContainer = flxContainer<flxDevice *>;

// Macro used to simplify device setup
#define spSetupDeviceIdent(_name_) this->setName(_name_);


//----------------------------------------------------------------------------------
// Factory/Builder pattern to dynamically register devices at runtime.
//----------------------------------------------------------------------------------
//
// A factory pattern is used to allow new device objects to be easily added to the system.
//
// This implementation includes the following:
//
//    - Factory singleton object - manages object *builders*, which implement the logic to
//                                 discover and create a device object.
//
//    - Builder objects - Classes that implement methods to detect an underlying device and
//                        create an instance of that class.
//
//
// How this works:
//
//    Builder classes are defined for each new device driver (of type flxDevice) using a
//    template. In the implementation of a device driver, a global builder object for that
//    class is defined.
//
//    Builder objects include a registration call to the overall Factory class (which is a
//    singleton) in their constructor. At system startup, global objects are instantiated,
//    which causes the builder object to register itself with the device factory class. Later,
//    the factory is used to discover and instantiate device classes using the registered
//    builder classes.
//
//    Key Point: Using this pattern, just adding the implementation files of a new device driver
//               enables this device for the system. No system updates or code modifications
//               required.
//
//    Note: This pattern does leave stale *global* builder objects in the system - but they are
//          small (10's of B).
//

// it's c++ - you have to do this
class flxDeviceBuilderI2C;

// Our factory class
class flxDeviceFactory
{

  public:
    // This is a singleton
    static flxDeviceFactory &get(void)
    {

        static flxDeviceFactory instance;
        return instance;
    }
    // The callback Builders use to register themselves.
    bool registerDevice(flxDeviceBuilderI2C *deviceBuilder)
    {
        _Builders.push_back(deviceBuilder);
        return true;
    }

    int factory_count(void)
    {
        return _Builders.size();
    };

    // Called to build a list of device objects for the devices connected to the system.
    int buildDevices(flxBusI2C &);

    void pruneAutoload(flxDevice *, flxDeviceContainer &);

    // Delete copy and assignment constructors - b/c this is singleton.
    flxDeviceFactory(flxDeviceFactory const &) = delete;
    void operator=(flxDeviceFactory const &) = delete;

  private:
    bool addressInUse(uint8_t);
    flxDeviceFactory(){}; // hide constructor - this is a singleton

    std::vector<flxDeviceBuilderI2C *> _Builders;
};

//----------------------------------------------------------------------------------
// Define our builder class.

// Base class - defines the builder interface.
//
class flxDeviceBuilderI2C
{
  public:
    virtual flxDevice *create(void) = 0;                                 // create the underlying device obj.
    virtual bool isConnected(flxBusI2C &i2cDriver, uint8_t address) = 0; // used to determine if a device is connected
    virtual const char *getDeviceName(void);                            // To report connected devices.
    virtual const uint8_t *getDefaultAddresses(void) = 0;
    virtual flxDeviceKind_t getDeviceKind(void) = 0;
};

// Define a class template used to register a device, then use this template to
// have a device create a builder for it's specific device class.
//
// The trick is the macro flxRegisterDevice, which sets up a static object
// of the builder class. This object is created at startup (when *globals* are inst),
// and the constructor of the class registers the builder in the factory class.
//

template <class DeviceType> class DeviceBuilder : public flxDeviceBuilderI2C
{
  public:
    DeviceBuilder()
    {
        flxDeviceFactory::get().registerDevice(this);
    }

    DeviceType *create()
    {
        return new DeviceType();
    }

    bool isConnected(flxBusI2C &i2cDriver, uint8_t address)
    {
        return DeviceType::isConnected(i2cDriver, address); // calls device object static isConnected method()
    }

    const char *getDeviceName(void)
    { // Calls device objects static method.
        return DeviceType::getDeviceName();
    }

    const uint8_t *getDefaultAddresses(void)
    {
        return DeviceType::getDefaultAddresses();
    }

    flxDeviceKind_t getDeviceKind(void)
    {
        return DeviceType::kind();
    }
};

// Macro to define the global builder object.
#define flxRegisterDevice(kDevice) static DeviceBuilder<kDevice> global_##kDevice##Builder;
