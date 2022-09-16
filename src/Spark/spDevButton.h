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
#define kButtonDeviceName  "BUTTON";
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevButton : public spDevice, public QwiicButton {

public:

	spDevButton();

	// Static Interface - used by the system to determine if this device is 
	// connected before the object is instantiated.
	static  bool isConnected(spDevI2C& i2cDriver); 	
	static const char * getDeviceName(){ return kButtonDeviceName; };	

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

