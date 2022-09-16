//
// Defines an observation storage class
// 

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

// TO DO - make this configurable ...
#define JSON_DOCUMENT_SIZE  400


// Abstracts out the defaults of the JSON object being used. 
//
// The expectation is that an observation object is stack based when used.
//
class spObservation{

public:

	spObservation(){};

	// Use a template to define how we log a value.
	template< typename T > void logValue(const char * tag, const T &data){
			(_jSection)[tag] = data;
    }

    void beginSection(const char * szName){   _jSection = _jDoc.createNestedObject(szName); }

    template<typename T > void serialize(T &output){
    		serializeJson(_jDoc, output);
	}

protected:
	JsonObject _jSection;

	StaticJsonDocument<JSON_DOCUMENT_SIZE> _jDoc;

};

//-----------------------------------------
// Define an interface for output of log information.

class spIWriter{

public:
	virtual bool write(spObservation& theObservation)=0;

};



