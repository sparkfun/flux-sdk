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
        if ( deviceBuilder->getDeviceKind() != flxDeviceKindI2C)
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