/*
 *
 * spDevice Objects
 *
 */

#include "spDevice.h"
#include "spSpark.h"

_spDevice::_spDevice() : _address{kSparkDeviceAddressNull}
{
    // The new device is added to the system on creation
    spark.add(this);
}

//----------------------------------------------------------------
// Device Factory
//----------------------------------------------------------------

//-------------------------------------------------------------------------------
bool spDeviceFactory_::addressInUse(uint8_t address)
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

int spDeviceFactory_::buildDevices(spDevI2C &i2cDriver)
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
                _spDevice *pDevice = deviceBuilder->create();
                if (!pDevice)
                {
                    Serial.print("ERROR: Device create failed - ");
                    Serial.println(deviceBuilder->getDeviceName());
                }
                else
                {
                    nDevs++;
                    pDevice->name = deviceBuilder->getDeviceName();
                    pDevice->setAddress(deviceAddresses[i]);
                }
            }
        }
    }

    // Okay, we are done - clearout the builders list.
    _Builders.clear();

    return nDevs;
}
void spDeviceFactory_::initDevices(spDeviceContainer &devList, spDevI2C &i2cDriver)
{

    for (int i = 0; i < devList.size(); i++)
        devList.at(i)->initialize(i2cDriver);
}
