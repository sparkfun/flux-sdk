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
        return;

    // Loop over our registered devices, and see if we have  service match

    for (auto deviceBuilder : _Builders)
    {

        if ( advertisedDevice.isAdvertisingService(BLEUUID(deviceBuilder->getServiceUUID()))) 
        {
            // We have a match for one of our devices - 

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
    
    BLEScan* pBLEScan = BLEDevice::getScan();

    if(!_bleScanInit){

        // register our scan callback class
        pBLEScan->setAdvertisedDeviceCallbacks(this);
        pBLEScan->setInterval(1349);
        pBLEScan->setWindow(449);
        pBLEScan->setActiveScan(true);

        _bleScanInit = true;
    }

    BLEDevice::getScan()->start(5);

    return _nDevs;
}