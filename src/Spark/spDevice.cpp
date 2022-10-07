/*
 *
 * spDevice Objects
 *
 */

#include "spDevice.h"
#include "spSpark.h"

spDevice::spDevice()
{

    _address = kSparkDeviceAddressNull;
    _autoload = false;
}

bool spDevice::initialize(TwoWire &wirePort)
{
    if (_address == kSparkDeviceAddressNull)
        _address = getDefaultAddress();

    // Add this device to the system
    spark.add(this);

    return onInitialize(wirePort);
}
//----------------------------------------------------------------
// Device Factory
//----------------------------------------------------------------

//-------------------------------------------------------------------------------
bool spDeviceFactory::addressInUse(uint8_t address)
{
    // loop over connected/created devices - if the address is a match, return true
    for (auto device : spark.connectedDevices())
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

int spDeviceFactory::buildDevices(spDevI2C &i2cDriver)
{

    // walk the list of registered drivers

    int nDevs = 0;
    const uint8_t *deviceAddresses;

    for (auto deviceBuilder : _Builders)
    {

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
                spDevice *pDevice = deviceBuilder->create();
                if (!pDevice)
                {
                    spLog_E("Device create failed - %s", deviceBuilder->getDeviceName());
                }
                else
                {
                    nDevs++;
                    pDevice->setName(deviceBuilder->getDeviceName());
                    pDevice->setAddress(deviceAddresses[i]);
                    pDevice->setAutoload();
                    pDevice->initialize(i2cDriver);
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

void spDeviceFactory::purneAutoload(spDevice *theDevice, spDeviceContainer &devList)
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
                spDevice *pTmp = *itDevice;
                itDevice = devList.erase(itDevice);
                delete pTmp;
                break;
            }
        }
        itDevice++;
    }
}