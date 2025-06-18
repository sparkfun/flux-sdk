/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/**
 * @file flxOptEnableDevice.h
 *
 *  A template that creates an "Action" that enables/disables a device driver.
 *
 *  The resultant action does the following:
 *      - Exposes an enable/disable property
 *      - Creates/Destroys the provided device when it's enabled/disabled
 *      - Adds/removes the device to/from the system
 *
 *  This is handy to manage the availability of a device driver in the active system.
 */

#pragma once

#include "flxCoreParam.h"
#include "flxFlux.h"

//----------------------------------------------------------------------------------------------------------
// Define our templateclass -
//
// The below template class is used to capture arguments passed in for later use when the specified object is
// created. This allows us to pass in args to a object, store those, and then use them later when creating a new
// object, whose type is specified by T.

#include <functional>

// First create a base class - interface that has a method which returns an instance of the desired class. Our
// class (below) that uses it, just works with the interface, since each defined class is different b/c it's a template.
template <typename T> struct flxObjFactoryArgsBase
{
    virtual T *create(void) = 0;
};

// Define the object factory w/ args class template. The template takes the type T of the object we want to create,
// and any number of arguments that will be passed to the constructor of the object we want to create.
//
// The way this works is that the passed in "Args" - a variadic parameter pack - is captured in a lambda function
// that is stored in the "callBack" member variable. When the "process" method is called, it invokes the lambda,
// which in turn calls the constructor of the object T with the captured arguments. This allows us to create an
// instance of T with the arguments provided at the time of the factory's setup.
template <typename T, typename... Args> struct flxObjFactoryArgs : public flxObjFactoryArgsBase<T>
{
    // Define the "callback function" that will be used to create an instance of T
    std::function<T *()> callBack = nullptr;

    // this setup method, takes the provided parameter pack and captures it in a lambda function.
    void setup(Args... args)
    {
        callBack = [args...]() { return new T(args...); };
    }

    // The  method calls the callback function, which creates an instance of T with the captured arguments.
    T *create()
    {
        if (callBack == nullptr)
            flxLog_E(F("flxObjFactoryArgs: Callback not set, cannot create object"));

        return callBack();
    }
};

// Template class used to manage the creation and destruction of a device driver via the flux menu system.
// This preserves the overhead of a device driver object until its needed.
//
// The is several key aspects for this class:
//   - The template takes the type of the device we want to create, which is specified by T.
//   - The constructor takes a name and description, which are used to identify the device in the system.
//   - The constructor also takes any number of arguments that will be passed to the constructor of the device we want.
//     This can be any number of arguments, but the created device class must have a constructor that matches the
//     arguments passed.
//   - The class provides a property to enable/disable the device, which will create or destroy the device as needed.
template <typename T> class flxOptEnableDevice : public flxActionType<T>
{
  private:
    //---------------------------------------------------------------------
    // Getter and setter for the isEnabled property
    bool get_is_enabled(void)
    {
        return _isEnabled;
    }
    //---------------------------------------------------------------------
    // This method does all the work
    void set_is_enabled(bool enable)
    {
        if (enable == _isEnabled)
            return; // No change, do nothing

        _isEnabled = enable;

        // enabling the device - need to make sure the object is created and setup
        if (_isEnabled)
        {
            // Create the device if needed
            if (_pDevice == nullptr)
            {
                // if we have a device factory, use it to create the new device.
                // The factory will pass along any arguments that were captured in the constructor of this object
                // to the new call
                if (_deviceFactory != nullptr)
                    _pDevice.reset(_deviceFactory->create());

                // success?
                if (_pDevice == nullptr)
                {
                    flxLog_E(F("%s: Unable to create device"), this->name());
                    _isEnabled = false;
                    return; // No device allocated
                }
                // Initialize the device
                if (_pDevice->initialize() == false)
                {
                    flxLog_E(F("%s: Device initialization failed"), this->name());
                    // delete _pDevice
                    _pDevice.reset(); // Use smart pointer to delete the device
                    _pDevice = nullptr;
                    _isEnabled = false;
                    return; // No device allocated
                }
                // restore any saved settings for this device. Only do this if flux is initialized and running
                // If not initialized, the settings restore will happen as part of normal startup
                if (flux.initialized())
                {
                    // helps with output formatting .. useful at time.
                    flxLog_N("");
                    flxLog_I_("%s: ", this->name());
                    flxSettings.restore(_pDevice.get());
                }
            }
            // already in the system? This would be a bookkeeping error
            if (flux.contains(_pDevice.get()))
                flxLog_D(F("%s: Device already in system"), this->name());
            else
                flux.add(_pDevice.get());
        }
        else if (_pDevice != nullptr)
        {
            // Remove the device from the system
            flux.remove(_pDevice.get());
            // delete _pDevice;
            _pDevice.reset(); // Use smart pointer to delete the device
            _pDevice = nullptr;
        }
    }

    // ---------------------------------------------------------------------------------
    // General method to setup the factory object/class for this device
    template <typename... Args> void setupFactory(Args &&...args)
    {
        // Any additional arguments passed to the constructor will be passed into the constructor for
        // the "device" this object manages. To capture these args, we use the above define nest template class.
        // This is created dynamically, since it's based on what was passed into this constructor.

        flxObjFactoryArgs<T, Args...> *factoryTmp = new flxObjFactoryArgs<T, Args...>(); // Create a new nested object
        if (factoryTmp == nullptr)
        {
            flxLog_E(F("%s: Unable to capture device arguments"), this->name());
            _isEnabled = false;
            return; // No nested object allocated
        }
        // Initialize the nested object with the arguments.
        factoryTmp->setup(args...);

        // Store the nested object in our smart pointer
        _deviceFactory.reset(factoryTmp);
    }
    bool _isEnabled;

    std::unique_ptr<T> _pDevice; // Use a smart pointer to manage the device object

    // A smart pointer for our nested object, which captures any arguments that are passed
    // to the constructor of the device we want to create. This allows us to pass
    // parameters to the device constructor without having to know the exact type at compile time.

    std::unique_ptr<flxObjFactoryArgsBase<T>> _deviceFactory; // Pointer to the nested object that creates the device

  public:
    // Constructor - no default, just one that takes a name and description
    flxOptEnableDevice() = delete;

    // the core constructor
    flxOptEnableDevice(const char *name, const char *desc)
        : _isEnabled{false}, _pDevice{nullptr}, _deviceFactory{nullptr}
    {
        // Setup unique identifiers for this device and basic device object systems
        this->setName(name, desc);
        // Register properties
        flxRegister(isEnabled, "Enabled", "Enable/Disable the device");

        flux_add(this);
    }
    // This constructor takes a name, description, and any number of arguments that will be passed to the
    // constructor of the device we want to create. The arguments are captured by the nested object template class
    // above.
    template <typename... Args>
    flxOptEnableDevice(const char *name, const char *desc, Args &&...args) : flxOptEnableDevice(name, desc)
    {
        // setup the factory obj to build the underlying device.
        setupFactory(args...);
    }

    // this works for our analog device - takes a initializer list of pairs, which are the name and value

    // This constructor will take the name, desc, and an initializer list of pairs that represent the a name-value
    // The above variadic constructor won't deal with initializer lists, because of unclear typing. So
    // made a constructor that takes an initializer list of pairs, which are the name and value.
    flxOptEnableDevice(const char *name, const char *desc,
                       std::initializer_list<std::pair<const std::string, int>> &&limitSet)
        : flxOptEnableDevice(name, desc)
    {
        // move the init list to something more resident in the system. - a vector. initializer_list is temporary
        // and won't hang around for when the factory is called.
        std::vector<std::pair<const std::string, int>> vLimitset = limitSet;

        setupFactory(vLimitset);
    }

    // support two initializer lists - for the case where we have two sets of limits
    flxOptEnableDevice(const char *name, const char *desc,
                       std::initializer_list<std::pair<const std::string, int>> &&limitSet,
                       std::initializer_list<std::pair<const std::string, int>> &&limitSet2)
        : flxOptEnableDevice(name, desc)
    {
        // move the init list to something more resident in the system. - a vector. initializer_list is temporary
        // and won't hang around for when the factory is called.
        std::vector<std::pair<const std::string, int>> vLimitset = limitSet;
        std::vector<std::pair<const std::string, int>> vLimitset2 = limitSet2;

        setupFactory(vLimitset, vLimitset2);
    }
    // enable / disable device ...
    flxPropertyRWBool<flxOptEnableDevice, &flxOptEnableDevice::get_is_enabled, &flxOptEnableDevice::set_is_enabled>
        isEnabled = {false};
};