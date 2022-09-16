//
// Defines an observation storage class
// 

#pragma once

#include <Arduino.h>
#include "spObservation.h"

//-----------------------------------------------------------
// Simple serial output class. Making a singleton - this has no state. 
//
class spSerial_ : public spIWriter {

public:
	// Singleton things
    static spSerial_* getInstance(void){

        static spSerial_ instance;
        return &instance;
    }

	// Simple serial output 
	bool write(spObservation& theObservation){

		theObservation.serialize(Serial);
		Serial.println(); // ^^ doesn't end output with a \n
		return true;
	}

	// templated callback for the listener - so we can write out anything
	template<typename T>
	void write(T value){
		Serial.println(value);
	}
	// strings are special
	void write(std::string& value){Serial.println(value.c_str());}

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


