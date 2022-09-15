/*
 *
 * spDevice Objects
 *
 */


#include "spDevice.h"
#include "spSpark.h"

spDevice::spDevice(){
	// The new device is added to the system on creation
	spark.add(this);
}

//----------------------------------------------------------------
// Device Factory
//----------------------------------------------------------------

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

int spDeviceFactory_::buildDevices(spDevI2C& i2cDriver){

	// walk the list of registered drivers

    spDeviceBuilder *deviceBuilder; 
    int nDevs=0;

	for( auto deviceBuilder : _Builders){

		// See if the device is connected
		if( deviceBuilder->isConnected(i2cDriver)){

			spDevice * pDevice = deviceBuilder->create();
			if(!pDevice){
				Serial.print("ERROR: Device create failed - "); 
				Serial.println(deviceBuilder->getDeviceName());
			}else{
				nDevs++;
				pDevice->name = deviceBuilder->getDeviceName();
			}
		}
		//else{
		//	Serial.print("[Factory] Debug -  device not connected: ");Serial.println(deviceBuilder->getDeviceName());
		//}
	}

	// Okay, we are done - clearout the builders list.
	_Builders.clear();

	return nDevs;
}
void spDeviceFactory_::initDevices(spDeviceContainer& devList, spDevI2C& i2cDriver){

	for(int i=0; i < devList.size(); i++)
		devList.at(i)->initialize(i2cDriver);
}




