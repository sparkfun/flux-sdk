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

#include "flxCoreDevice.h"
#include "flxFlux.h"

bool flxDevice::initialize()
{
    flux.add(this);

    return true;
}

// Input param/function methods to enable/disable all parameters
void flxDevice::disable_all_parameters(void)
{
    for (auto param : getOutputParameters())
        param->setEnabled(false);
}

void flxDevice::enable_all_parameters(void)
{
    for (auto param : getOutputParameters())
        param->setEnabled(true);
}

//----------------------------------------------------------------
// Add the address to the device name. Helps ID a device 
void flxDevice::addAddressToName()
{
    // add the device address to the name of the device.
    // Does it already exist?

    if (strchr(name(), '[') != NULL)
        return;

    char szBuffer[64];
    snprintf(szBuffer, sizeof(szBuffer), getKind() == flxDeviceKindSPI ? "%s [p%d]" : "%s [x%x]", name(), address());

    setNameAlloc(szBuffer);
}

//----------------------------------------------------------------
// Device Factory
//----------------------------------------------------------------

//-------------------------------------------------------------------------------
bool flxDeviceFactory::addressInUse(uint8_t address)
{
    // loop over connected/created devices - if the address is a match, return true
    for (auto device : flux.connectedDevices())
    {
        if (device->address() == address)
            return true;
    }
    return false;
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

int flxDeviceFactory::buildDevices(flxBusI2C &i2cDriver)
{

    // walk the list of registered drivers

    int nDevs = 0;
    const uint8_t *deviceAddresses;

    for (auto deviceBuilder : _Builders)
    {

        // Only autoload i2c devices
        if (deviceBuilder->getDeviceKind() != flxDeviceKindI2C)
            continue;

        deviceAddresses = deviceBuilder->getDefaultAddresses();
        if (!deviceAddresses)
            break;

        for (int i = 0; deviceAddresses[i] != kSparkDeviceAddressNull; i++)
        {
            // Address already in use? If so, skip to next address
            if (addressInUse(deviceAddresses[i]))
                continue; // next

            // See if the device is connected
            if (deviceBuilder->isConnected(i2cDriver, deviceAddresses[i]))
            {
                flxDevice *pDevice = deviceBuilder->create();
                if (!pDevice)
                {
                    flxLog_E("Device create failed - %s", deviceBuilder->getDeviceName());
                }
                else
                {
                    pDevice->setName(deviceBuilder->getDeviceName());
                    pDevice->setAddress(deviceAddresses[i]);
                    pDevice->setAutoload();
                    if (!pDevice->initialize(i2cDriver))
                    {
                        // device failed to init - delete it ...
                        flxLog_E(F("Deviced %s failed to initialize."), deviceBuilder->getDeviceName());
                        deviceBuilder->destroy(pDevice);
                        continue;
                    }
                    nDevs++;
                }
            }
        }
    }

    // Okay, we are done - clear out the builders list.
    _Builders.clear();

    return nDevs;
}

//----------------------------------------------------------------------------------
// pruneAutoload()
//
// Called when a non-autoload device is created.
//
// If a new device is created by the user outside of this factory, but that
// device was "auto loaded", we prune the autoload device.
//
// A device match = Device::type is the same and the address is the same.

void flxDeviceFactory::pruneAutoload(flxDevice *theDevice, flxDeviceContainer &devList)
{

    if (theDevice->autoload() || devList.size() == 0)
        return; // makes no sense.

    auto itDevice = devList.begin(); // get the iterator for the list

    while (itDevice != devList.end())
    {
        // only check auto loads
        if ((*itDevice)->autoload())
        {
            if (theDevice->getType() == (*itDevice)->getType() && theDevice->address() == (*itDevice)->address())
            {
                // remove the device - returns updated iterator
                flxDevice *pTmp = *itDevice;
                itDevice = devList.erase(itDevice);
                delete pTmp;
                break;
            }
        }
        itDevice++;
    }
}