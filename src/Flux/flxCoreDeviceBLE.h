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
 * flxCoreBLEDevice.h
 *
 * Class that defines the interface between the system and a BLE device driver
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
#include <BLEClient.h>
#include <BLEDevice.h>

#include <vector>


#include "flxCoreDevice.h"




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
class flxDeviceBuilderBLE;

// Our factory class
class flxDeviceBLEFactory :  public BLEAdvertisedDeviceCallbacks 
{

  public:
    // This is a singleton
    static flxDeviceBLEFactory &get(void)
    {

        static flxDeviceBLEFactory instance;
        return instance;
    }
    // The callback Builders use to register themselves.
    bool registerDevice(flxDeviceBuilderBLE *deviceBuilder)
    {
        _Builders.push_back(deviceBuilder);
        return true;
    }

    int factory_count(void)
    {
        return _Builders.size();
    };

    // Called to build a list of device objects for the devices connected to the system.
    int buildDevices();

    // BLE scan callbacks
    void onResult(BLEAdvertisedDevice advertisedDevice);

    // Delete copy and assignment constructors - b/c this is singleton.
    flxDeviceBLEFactory(flxDeviceBLEFactory const &) = delete;
    void operator=(flxDeviceBLEFactory const &) = delete;

  private:
    flxDeviceBLEFactory(): _bleScanInit{false}{}; // hide constructor - this is a singleton
    bool _bleScanInit;

    std::vector<flxDeviceBuilderBLE *> _Builders;

    int _nDevs;
};

class flxDeviceBLE;

//----------------------------------------------------------------------------------
// Define our builder class.

// Base class - defines the builder interface.
//
class flxDeviceBuilderBLE
{
  public:
    virtual flxDeviceBLE *create(void) = 0;                                 // create the underlying device obj.
    virtual const char *getDeviceName(void);                            // To report connected devices.
    virtual const char *getServiceUUID(void) = 0;
    virtual flxDeviceKind_t getDeviceKind(void) = 0;
};

// Define a class template used to register a device, then use this template to
// have a device create a builder for it's specific device class.
//
// The trick is the macro flxRegisterDevice, which sets up a static object
// of the builder class. This object is created at startup (when *globals* are inst),
// and the constructor of the class registers the builder in the factory class.
//

template <class DeviceType> class DeviceBLEBuilder : public flxDeviceBuilderBLE
{
  public:
    DeviceBLEBuilder()
    {
        flxDeviceBLEFactory::get().registerDevice(this);
    }

    DeviceType *create()
    {
        return new DeviceType();
    }

    const char *getDeviceName(void)
    { // Calls device objects static method.
        return DeviceType::getDeviceName();
    }

    const char *getServiceUUID(void)
    {
        return DeviceType::getServiceUUID();
    }

    flxDeviceKind_t getDeviceKind(void)
    {
        return DeviceType::kind();
    }
};

// Macro to define the global builder object.
#define flxRegisterDeviceBLE(kDevice) static DeviceBLEBuilder<kDevice> global_##kDevice##BLEBuilder;
