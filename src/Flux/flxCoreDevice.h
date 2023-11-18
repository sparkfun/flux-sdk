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

/////////////////////////////////////////////////////////////////////////////////////
// 11/2023  I2C Auto Load update
/////////////////////////////////////////////////////////////////////////////////////
//
// Issue:
//    One issue with the initial auto load logic was making decisions when
//    address collisions occurred - a device mapped to 2 or more device drivers.
//    Normally, the first driver loaded (which is indeterminate) that
//    matched the Device was selected. Sometimes this worked - the driver had
//    support for exact device identification - and other times it did not -
//    a device could not be identified.
//
// Solution:
//    The device drivers have an idea of how accurate the identification
//    capabilities they implement, so the new method has each driver
//    return a match *confidence* level as part of the autoload interface.
//
//    The system uses this confidence level to sort the device drivers
//    for each I2C address. For each address, higher match confidence
//    drivers query and address first, followed drivers with less
//    confidence. The last driver used is one that just performs a PING
//    to the attached address.
//
//    By prioritizing identification based on the driver confidence level
//    most if not all collision issue of the initial implementation are
//    resolved. Additionally, the system restricts the use of PING only
//    devices for an address to only one driver. Any driver beyond the
//    first PING-only driver would never be used.
//
// Implementation:
//    This system is implemented using the following:
//
//      * Each device driver implements a confidence method - connectedConfidence(),
//        which returns a confidence value
//      * When drivers register, the driver is added to a multimap, which maintains
//        a sorted list of available drivers.
//          - The map key is created using an I2C address and confidence value.
//               key =  address * 10 + confidence_value
//          - The confidence value ranges from 0 - 9, with 0 being high confidence, 9 low
//          - This key ensures higher confidence drivers are sorted before lower
//            confidence drivers.
//      * For each address a device driver supports, an entry for that driver is
//        added to the driver multi-map.
//      * When autoload occurs, the system traverses the sorted multimap, calling
//        the the "isConnected()" methods for the drivers at each address, starting
//        the high-confidence drivers.
//      * If a device is found at an address, a driver instance is created for that device
//        and any remaining drivers for that address skipped.
//
//  Potential Future Additions?
//      * User prioritization of a driver in the auto-load list
//      * User added addresses for a device
//      * User defined locked address-to-device/driver
//      * Device load prioritization based on previous use - <last loaded tested first>
//      * User defined load/device limits ....
//
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//
// Define a type that is used for qualifying the type of isConnected Algorithm results.
//
// This is used to define the *confidence* of isConnected() method employed.
// Some devices are definitive in being identified, others are not. A
// confidence value allows the system to prioritize device when a
// conflict occurs.
//
// A 0-9 value is used to quantify confidence - with 0 representing
// an exact match (high confidence) and a 9 representing low confidence.
//
// The device load algorithm uses this number to create a sorted list
// of devices to test at an address, with those having a high-confidence
// level tested (isConnected()) first.
//
// These values need to range from 0-9, with Exact =0, Ping (worse) = 0
typedef enum
{
    flxDevConfidenceExact = 0,
    flxDevConfidenceFuzzy = 5,
    flxDevConfidencePing = 9
} flxDeviceConfidence_t;

/////////////////////////////////////////////////////////////////////////////////////
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

  private:
    void disable_all_parameters(void);
    void enable_all_parameters(void);

  public:
    flxDevice() : _autoload{false}, _address{kSparkDeviceAddressNull}, _isInitalized{false}
    {
        flxRegister(disableAllParameters, "Disable All Parameters", "Disables all output parameters");
        flxRegister(enableAllParameters, "Enable All Parameters", "Enable all output parameters");
    };

    virtual ~flxDevice()
    {
    }

    // override our operation class
    virtual bool execute(void)
    {
        return true;
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

    // autoload property
    bool autoload(void)
    {
        return false;
    }
    void setAutoload()
    {
        _autoload = true;
    }

    // Device address property
    void setAddress(uint8_t address)
    {
        _address = address;
    }

    uint8_t address(void)
    {
        return _address;
    }

    void addAddressToName();

    // device is initialized property
    void setIsInitialized(bool isInit)
    {
        _isInitalized = isInit;
    }

    bool isInitialized(void)
    {
        return _isInitalized;
    }

    virtual flxDeviceKind_t getKind(void)
    {
        return flxDeviceKindNone;
    }

    flxParameterInVoid<flxDevice, &flxDevice::disable_all_parameters> disableAllParameters;
    flxParameterInVoid<flxDevice, &flxDevice::enable_all_parameters> enableAllParameters;

  private:
    bool _autoload;
    uint8_t _address;
    bool _isInitalized;
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
class flxIDeviceBuilderWr;

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
    bool registerDevice(flxIDeviceBuilderWr *deviceBuilder);

    int factory_count(void)
    {
        return _buildersByAddress != nullptr ? _buildersByAddress->size() : 0;
    };

    // Called to build a list of device objects for the devices connected to the system.
    int buildDevices(flxBusI2C &);

    void pruneAutoload(flxDevice *, flxDeviceContainer &);

    // Delete copy and assignment constructors - b/c this is singleton.
    flxDeviceFactory(flxDeviceFactory const &) = delete;
    void operator=(flxDeviceFactory const &) = delete;

    // void debugMapDump(void);

  private:
    bool addressInUse(uint8_t);
    // hide constructor - this is a singleton
    flxDeviceFactory()
    {
        _buildersByAddress = new _BuilderMMap_t;
    };

    // 11/2023 -- the multi map use to store registered device drivers. Key [addr & confidence level] -> *builder]

    typedef std::multimap<uint16_t, flxIDeviceBuilderWr *> _BuilderMMap_t;

    _BuilderMMap_t *_buildersByAddress;

    // std::vector<flxDeviceBuilderI2C *> _Builders;
};

//----------------------------------------------------------------------------------
// Define our builder class.

// Base class - defines the builder interface.
//
class flxDeviceBuilderI2C
{
  public:
    virtual flxDevice *create(void) = 0; // create the underlying device obj.
    // Destroy a device - common method
    void destroy(flxDevice *oldDev)
    {
        if (oldDev)
            delete oldDev;
    }
    virtual bool isConnected(flxBusI2C &i2cDriver, uint8_t address) = 0; // used to determine if a device is connected
    virtual flxDeviceConfidence_t connectedConfidence(void) = 0;         // 11/2023 update add
    virtual const char *getDeviceName(void);                             // To report connected devices.
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
        // flxDeviceFactory::get().registerDevice(this);
    }

    DeviceType *create()
    {
        return new DeviceType();
    }

    bool isConnected(flxBusI2C &i2cDriver, uint8_t address)
    {
        return DeviceType::isConnected(i2cDriver, address); // calls device object static isConnected method()
    }

    // pass through the confidence level from the underlying driver.
    flxDeviceConfidence_t connectedConfidence(void)
    {
        return DeviceType::connectedConfidence();
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

class flxIDeviceBuilderWr
{
  public:
    virtual flxDeviceBuilderI2C *get() = 0;
    virtual void reset(void) = 0;
};
// a "smart wrapper" for the builder class - allows us to free up a little memory
// after autoload
template <class T> class flxDeviceBuilderWr : public flxIDeviceBuilderWr
{
  public:
    flxDeviceBuilderWr()
    {
        _pDevBuilder = new DeviceBuilder<T>;
        flxDeviceFactory::get().registerDevice(this);
    }

    flxDeviceBuilderI2C *get()
    {
        return _pDevBuilder;
    }

    void reset(void)
    {
        if (_pDevBuilder)
        {
            delete _pDevBuilder;
            _pDevBuilder = nullptr;
        }
    }

  private:
    DeviceBuilder<T> *_pDevBuilder;
};
// Macro to define the global builder object.
// #define flxRegisterDevice(kDevice) static DeviceBuilder<kDevice> global_##kDevice##Builder;
#define flxRegisterDevice(kDevice) static flxDeviceBuilderWr<kDevice> global2_##kDevice##Builder;
