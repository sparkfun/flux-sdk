//
// Defines an observation storage class
// 

#pragma once

#include <Arduino.h>
#include "spOutput.h"

//-----------------------------------------------------------
// Simple serial output class. Making a singleton - this has no state. 
//
class spSerial_ : public spWriter {

public:
	// Singleton things
    static spSerial_* getInstance(void){

        static spSerial_ instance;
        return &instance;
    }


	// templated callback for the listener - so we can write out anything
	template<typename T>
	void write(T value){
		Serial.println(value);
	}

	void write(int value)
	{
		Serial.println(value);
	}
	void write(float value)
	{
		Serial.println(value);
	}
	void write(const char * value )
	{
		Serial.println(value);
	}

	void write(std::string& value){
		write(value.c_str());
	}
	// Overload listen, so we can type the events, and use the templated
	// write() method above.

	void listen(spSignalInt& theEvent){
		theEvent.call(this, &spSerial_::write);
	}
	void listen(spSignalFloat& theEvent){
		theEvent.call(this, &spSerial_::write);
	}
	void listen(spSignalBool& theEvent){
		theEvent.call(this, &spSerial_::write);
	}
	void listen(spSignalString& theEvent){
		theEvent.call(this, &spSerial_::write);
	}
 	// copy and assign constructors - delete them to prevent extra copys being 	
    // made -- this is a singleton object.
    spSerial_(spSerial_ const&) = delete;
    void operator=(spSerial_ const&) = delete;

private:
	spSerial_(){};

};

typedef spSerial_* spSerial;

// Accessor for the signleton
#define spSerial()  spSerial_::getInstance()


