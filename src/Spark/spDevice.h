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

#include "spCore.h"
#include "spDevI2C.h"

// define a value that marks the end of a device address/id list

#define kSparkDeviceAddressNull 0

/////////////////////////////////////////////////////////////////////////////
//
// spDevice()
//
// Define our device base class used for each device in the system.
//
// New devices create an object that subclasses from this object.
//
// The device object defines the interface and underlying logic that
// integracts the device into the overall system, and simplifies the r
// implementation requirements for each new device.
//
// Key capabilities provided:
//
//      - object state peristance/seralization
//      - Managed properties -
//      - System data collection from the device
//      - Works with the device Factory/Builder pattern.
//
//

class spDeviceFactory_;

class _spDevice : public spOperation
{

  public:
    _spDevice();

    virtual ~_spDevice(){}
    // Interface

    // Methods for Sub-class to override - for device activities.
    // Method called on initialize
    virtual bool onInitialize(TwoWire &)
    {
        return true;
    };

    

    virtual uint8_t getDefaultAddress(void){
    	return kSparkDeviceAddressNull;
    };

    bool initialize(TwoWire &wirePort = Wire);

    void setAddress(uint8_t address)
    {
        _address = address;
    }

    uint8_t address(void)
    {
        return _address;
    }

    bool autoload()
    {
    	return _autoload;
    }

  private:
    // want to prevent sub-classes calling methods here, but want an
    // entry point call for the system ....

    friend class spDeviceFactory;

    bool initialize(spDevI2C &i2cDriver)
    {

        // call the superclasses begin method.
        TwoWire *wirePort = i2cDriver.getWirePort();
        if (!wirePort)
            return false;

        return this->initialize(*wirePort); // call subclass virtual init routine.
    }
    void setAutoload()
    {
    	_autoload = true;
    }
    
    uint8_t _address;
    bool 	_autoload; 

};

//using spDeviceContainer = spContainer<_spDevice>;
using spDeviceContainer = _spOperationContainer<_spDevice>;

using spDeviceList = spContainer<_spDevice>;

// Macro used to simplfy device setup
#define spSetupDeviceIdent(_name_) this->name = _name_;

//------------------------------------------------------------------------
// spDevice()
// 
// This subclass of _spDevice via template allows the core device class to 
// access the static list of addresses to get the default address without 
// requiring the subclass to implement a method to do this. 
//
// Devices should subclass from this object using the following pattern:
//
//   class <classname> : spDevice<classname>, ...
//
template <typename T>
class spDevice : public _spDevice
{
	// get the default address for the device. If none exists,
	// return Null Address...
	virtual uint8_t getDefaultAddress(void)
    {
    	// call the classes static method to get all addresses
    	// supported by this device (or the core addrs)
    	const uint8_t *addresses = T::getDefaultAddresses();
    	if(!addresses)
    		return kSparkDeviceAddressNull;

    	return addresses[0];
    }
};

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
//    Builder classes are defined for each new device driver (of type spDevice) using a
//    template. In the implementation of a device driver, a global builder object for that
//    class is defined.
//
//    Builder objects include a registration call to the overall Factory class (which is a
//    singleton) in their constructor. At system startup, global objects are instatiated,
//    which causes the builder object to register itself with the device factory class. Later,
//    the factory is used to discover and instantiate device classes using the registered
//    builder classes.
//
//    Key Point: Using this pattern, just adding the implentation files of a new device driver
//               enables this device for the system. No system updates or code modificatiosn
//               required.
//
//    Note: This pattern does leave stale *global* builder objects in the system - but they are
//          small (10's of B).
//
//    On system startup, this global object This global class calls the device Factory object On system startup, this
// Define the spDeviceFactory class
//

// it's c++ - you have to do this
class spDeviceBuilder;

// Our factory class
class spDeviceFactory
{

  public:
    // This is a singleton
    static spDeviceFactory &get(void)
    {

        static spDeviceFactory instance;
        return instance;
    }
    // The callback Builders use to register themselves.
    bool registerDevice(spDeviceBuilder *deviceBuilder)
    {
        _Builders.push_back(deviceBuilder);
        return true;
    }

    int factory_count(void)
    {
        return _Builders.size();
    };

    // Called to build a list of device objects for the devices connected to the system.
    int buildDevices(spDevI2C &);

	void purneAutoload(_spDevice &, spDeviceContainer &);

    // Delete copy and assignment constructors - b/c this is singleton.
    spDeviceFactory(spDeviceFactory const &) = delete;
    void operator=(spDeviceFactory const &) = delete;

  private:
    bool addressInUse(uint8_t);
    spDeviceFactory(){}; // hide constructor - this is a singleton

    std::vector<spDeviceBuilder *> _Builders;
};


//----------------------------------------------------------------------------------
// Define our builder class.

// Base class - defines the builder interface.
//
class spDeviceBuilder
{
  public:
    virtual _spDevice *create(void) = 0;                                 // create the underlying device obj.
    virtual bool isConnected(spDevI2C &i2cDriver, uint8_t address) = 0; // used to determine if a device is connected
    virtual const char *getDeviceName(void);                            // To report connected devices.
    virtual const uint8_t *getDefaultAddresses(void) = 0;
};

// Define a class template used to register a device, then use this template to
// have a device create a builder for it's specific device class.
//
// The trick is the macro QwiicRegisterDevice, which setups up a static object
// of the builder class. This object is created at startup (when *globals* are inst),
// and the constructor of the class registers the builder in the factory class.
//

template <class DeviceType> class DeviceBuilder : public spDeviceBuilder
{
  public:
    DeviceBuilder()
    {
        spDeviceFactory::get().registerDevice(this);
    }

    DeviceType *create()
    {
        return new DeviceType();
    }

    bool isConnected(spDevI2C &i2cDriver, uint8_t address)
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
};


// Macro to define the global builder object.
#define spRegisterDevice(kDevice) static DeviceBuilder<kDevice> global_##kDevice##Builder;
