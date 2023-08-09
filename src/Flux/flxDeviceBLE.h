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
 * flxDeviceBLE.h
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

#include "flxCoreDeviceBLE.h"
#include "flxFlux.h"

//----------------------------------------------------------------------------------
// Ble Device
//----------------------------------------------------------------------------------


class flxDeviceBLE : public flxDevice
{
  public:

    flxDeviceBLE(): _pService{nullptr}{}

    virtual const char *serviceAddress(void) = 0;

    //-------------------------------------------------------------------------------
    bool initialize(BLEAdvertisedDevice *advertisedDevice)
    {
        // setup the client
        if (!_bleClient.connect(advertisedDevice))
        {
            flxLog_E(F("Unable to connect to BLE Device"));
            return false;
        }

        // Grab our target service

        BLEUUID theServiceUUID(serviceAddress());
        _pService = _bleClient.getService(theServiceUUID);
        if (!_pService){
            flxLog_E(F("Unable to connect access BLE Service for device %s"), name());
            _bleClient.disconnect();
            return false;
        }

        // Call subclass onInit() method

        bool status = onInitialize(_pService);

        if (status)
            flxDevice::initialize();
        
        return status;
    }
    
    //-------------------------------------------------------------------------------
    // For our sub-classes to overload
    virtual bool onInitialize(BLERemoteService *pService)
    {
        return true;
    };

    //-------------------------------------------------------------------------------
    BLERemoteService * bleService(void)
    {
        return _pService;
    }

    //-------------------------------------------------------------------------------
    BLEClient & bleClient(void)
    {
        return _bleClient;
    }

private:

    // Our BLE Client Device
    BLEClient _bleClient;

    // The target service
    BLERemoteService *_pService;
};

template <typename T> class flxDeviceBLEType : public flxDeviceBLE
{
  public:

    virtual const char * serviceAddress(void)
    {
        // call the classes static method to get all addresses
        // supported by this device (or the core addrs)
        return T::getServiceUUID();

    }

    //-------------------------------------------------------------------------------
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
        return flxDeviceKindBLE;
    }

    flxDeviceKind_t getKind(void)
    {
        return kind();
    }
};