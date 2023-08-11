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
 * flxDevice Objects
 *
 */

#include "flxCoreDeviceBLE.h"
#include "flxDeviceBLE.h"
#include "flxFlux.h"



//----------------------------------------------------------------
// Device Factory
//----------------------------------------------------------------

void flxDeviceBLEFactory::onResult(BLEAdvertisedDevice advertisedDevice)
{

    // Does this BLE device have a service?
    if (!advertisedDevice.haveServiceUUID())
    {
        return;
    }

    // Loop over our registered devices, and see if we have  service match

    for (auto deviceBuilder : _Builders)
    {
        if (advertisedDevice.haveName())
        {
            flxLog_I("Found BLE DEVICE: %s", advertisedDevice.getName().c_str());
        }

        if ( advertisedDevice.isAdvertisingService(BLEUUID(deviceBuilder->getServiceUUID()))) 
        {
            // We have a match for one of our devices - 
            flxLog_E(">>>>>>>>>>>Found a device we know about");

            // Create 
            flxDeviceBLE *pDevice = deviceBuilder->create();
            if (!pDevice)
                flxLog_E("BLE Device create failed - %s", deviceBuilder->getDeviceName());
            else
            {
                pDevice->setName(deviceBuilder->getDeviceName());
                pDevice->setAutoload();
                if (!pDevice->initialize(&advertisedDevice) )
                {
                    // we had a device error, nuke it.
                    delete pDevice;
                }else 
                    _nDevs++;
            }
        }
     }
}

//-------------------------------------------------------------------------------
// buildConnectedDevices()
//
// Walks through the list of registered drivers and determines if the device is
// connected to the system. If it is, a driver is created and added to our driver list.
//
// Once this is completed, the "registered builders" list is cleared. This frees up the list,
// but the builder objects, which are globals (and small) remain.
//
// Return Value
//    The count of devices connected and the driver was successfully created...
//-------------------------------------------------------------------------------

int flxDeviceBLEFactory::buildDevices()
{

    // no Builders, no dice

    if ( _Builders.size() == 0 )
        return 0;

    _nDevs = 0;

    // Hack Time:

    BLEDevice::init(flux.name());

    // END HACK TIME
    
    BLEScan* pBLEScan = BLEDevice::getScan();

    if(!_bleScanInit){

        // register our scan callback class
        // NOTE: Setting "duplicates" to true helps with memory consumption.  Otherwise, 
        // the BLE system will keep a vector of everything found and CHEW up memory. If set to 
        // true, values are not cached - but deleted. 
        pBLEScan->setAdvertisedDeviceCallbacks(this, true);
        pBLEScan->setInterval(100);
        pBLEScan->setWindow(99);
        pBLEScan->setActiveScan(true);

        _bleScanInit = true;
    }

    flxLog_I("Start Scan");

    pBLEScan->start(5);

    flxLog_I("Back from Scan");
    
    pBLEScan->clearResults();
    pBLEScan->stop();

    BLEDevice::deinit(false);

    return _nDevs;
}