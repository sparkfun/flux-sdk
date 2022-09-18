/*
 *
 * QwiicDevButton.cpp
 *
 */

#pragma once 

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_Qwiic_Button.h"

// What is the name used to ID this device?
#define kButtonDeviceName  "button";
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevButton : public spDevice, public QwiicButton {

public:

	spDevButton();

	// Static Interface - used by the system to determine if this device is 
	// connected before the object is instantiated.
	static  bool isConnected(spDevI2C& i2cDriver, uint8_t address); 	
	static const char * getDeviceName(){ return kButtonDeviceName; };	
	static const uint8_t * getDefaultAddresses()
	{
		return defaultDeviceAddress;
	}
	// holds the class list of possible addresses/IDs for this objects
	static uint8_t defaultDeviceAddress[];
	// Method called to initialize the class
	bool onInitialize(TwoWire &);   

	// Called when a managed property is updated
	void onPropertyUpdate(const char *);	


	// Events
	// Our output event
	spSignalBool on_clicked;

	// Type testing: 
	// A static instance var - that is an object (can check instance pointer)
	static spType Type;
	spType* getType(void){ return &Type;}

	bool loop(void);

private:
	bool was_clicked = false;

};

