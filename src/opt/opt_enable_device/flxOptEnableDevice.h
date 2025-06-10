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
#include "flxFLux.h"

//----------------------------------------------------------------------------------------------------------
// Define our templateclass -

template <typename T> class flxOptEnableDevice : public flxActionType<T>
{
  private:
    bool get_is_enabled(void)
    {
        return _isEnabled;
    }
    //---------------------------------------------------------------------
    void set_is_enabled(bool enable)
    {
        if (enable == _isEnabled)
            return; // No change, do nothing

        _isEnabled = enable;

        if (_isEnabled)
        {
            // Create the device if needed
            if (_pDevice == nullptr)
            {
                _pDevice = new T;
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
                    delete _pDevice;
                    _pDevice = nullptr;
                    _isEnabled = false;
                    return; // No device allocated
                }
                // restore any saved settings for this device. Only do this if flux is initialized and running
                // If not initialized, the settings restore will happen as part of normal startup
                if (flux.initialized())
                    flxSettings.restore(_pDevice);
            }
            // already in the system? This would be a bookkeeping error
            if (flux.contains(_pDevice))
                flxLog_D(F("%s: Device already in system"), this->name());
            else
                flux.add(_pDevice);
        }
        else if (_pDevice != nullptr)
        {
            // Remove the device from the system
            flux.remove(_pDevice);
            delete _pDevice;
            _pDevice = nullptr;
        }
    }

    bool _isEnabled;
    T *_pDevice;

  public:
    // Constructor - no default, just one that takes a name and description
    flxOptEnableDevice() = delete;

    flxOptEnableDevice(const char *name, const char *desc) : _isEnabled{false}, _pDevice{nullptr}
    {

        // Setup unique identifiers for this device and basic device object systems
        this->setName(name, desc);
        // Register properties
        flxRegister(isEnabled, "Enabled", "Enable/Disable the device");

        flux_add(this);
    }
    // enable / disable device ...
    flxPropertyRWBool<flxOptEnableDevice, &flxOptEnableDevice::get_is_enabled, &flxOptEnableDevice::set_is_enabled>
        isEnabled = {false};
};

//----------------------------------------------------------------------------------------------------------
// Testing a method to capture args to pass to the underlying device constructor.

#include <functional>

template <typename T> struct flxNestBase
{
    virtual T *process(void) = 0;
};
template <typename T, typename... Args> struct flxNestNew : public flxNestBase<T>
{
    std::function<T *()> callBack;

    void setup(Args... args)
    {
        callBack = [args...]() { return new T(args...); };
    }

    T *process()
    {
        return callBack();
    }
};

template <typename T> class flxOptEnableDevice2 : public flxActionType<T>
{
  private:
    bool get_is_enabled(void)
    {
        return _isEnabled;
    }
    //---------------------------------------------------------------------
    void set_is_enabled(bool enable)
    {
        if (enable == _isEnabled)
            return; // No change, do nothing

        _isEnabled = enable;

        if (_isEnabled)
        {
            // Create the device if needed
            if (_pDevice == nullptr)
            {
                if (_nestNew != nullptr)
                    _pDevice = _nestNew->process();
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
                    delete _pDevice;
                    _pDevice = nullptr;
                    _isEnabled = false;
                    return; // No device allocated
                }
                // restore any saved settings for this device. Only do this if flux is initialized and running
                // If not initialized, the settings restore will happen as part of normal startup
                if (flux.initialized())
                    flxSettings.restore(_pDevice);
            }
            // already in the system? This would be a bookkeeping error
            if (flux.contains(_pDevice))
                flxLog_D(F("%s: Device already in system"), this->name());
            else
                flux.add(_pDevice);
        }
        else if (_pDevice != nullptr)
        {
            // Remove the device from the system
            flux.remove(_pDevice);
            delete _pDevice;
            _pDevice = nullptr;
        }
    }

    bool _isEnabled;
    T *_pDevice;

    flxNestBase<T> *_nestNew; // Pointer to the nested object that creates the device
  public:
    // Constructor - no default, just one that takes a name and description
    flxOptEnableDevice2() = delete;

    template <typename... Args>
    flxOptEnableDevice2(const char *name, const char *desc, Args &&...args)
        : _isEnabled{false}, _pDevice{nullptr}, _nestNew{nullptr}
    {

        // Setup unique identifiers for this device and basic device object systems
        this->setName(name, desc);
        // Register properties
        flxRegister(isEnabled, "Enabled", "Enable/Disable the device");

        flux_add(this);

        flxNestNew<T, Args...> *nestTmp = new flxNestNew<T, Args...>(); // Create a new nested object
        // Setup the callback to create the device
        nestTmp->setup(args...);
        _nestNew = nestTmp; // Store the nested object
    }
    // enable / disable device ...
    flxPropertyRWBool<flxOptEnableDevice2, &flxOptEnableDevice2::get_is_enabled, &flxOptEnableDevice2::set_is_enabled>
        isEnabled = {false};
};