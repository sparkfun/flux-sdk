//
// Define interfaces/base classes for output
// 

#pragma once

#include "spOutput.h"

#include <Arduino.h>
#include <ArduinoJson.h>

// TO DO - make this configurable ...
#define JSON_DOCUMENT_SIZE  400


class spFormatJSON: public spOutputFormat {

public:

	spFormatJSON(){};

	// value methods
	void logValue(const char* tag, bool value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}
	void logValue(const char* tag, int value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}
	void logValue(const char* tag, float value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}
	void logValue(const char* tag, const char* value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}

	// structure cycle 

	virtual void beginObservation(const char * szTitle=nullptr)
	{
		reset(); // just incase
		if(szTitle)
			_jDoc["title"] = szTitle;
	}

	//-----------------------------------------------------------------
	void beginSection(const char * szName)
	{
		_jSection = _jDoc.createNestedObject(szName); 
	}

	//-----------------------------------------------------------------
	void endObservation(void)
	{
		// no op
	}

	//-----------------------------------------------------------------	
	virtual void writeObservation()
	{
		char szBuffer[JSON_DOCUMENT_SIZE+1];
		size_t n = serializeJson(_jDoc, szBuffer, JSON_DOCUMENT_SIZE);

		// TODO: Add Error output
		if ( n > JSON_DOCUMENT_SIZE + 1){
			Serial.println("[WARNING] - JSON document buffer output buffer trimmed");
			szBuffer[JSON_DOCUMENT_SIZE] = '\0';
		}

		outputObservation(szBuffer);
	}

	//-----------------------------------------------------------------	
	void clearObservation(void)
	{
		reset();
	}

	//-----------------------------------------------------------------
	void reset(void)
	{
		_jDoc.clear();
		
	}

protected:
	JsonObject _jSection;

	StaticJsonDocument<JSON_DOCUMENT_SIZE> _jDoc;

};
