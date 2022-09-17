//
// Define interfaces/base classes for output
// 

#pragma once

#include "spOutput.h"

#include <Arduino.h>
#include <ArduinoJson.h>


template <std::size_t BUFFER_SIZE>
class spFormatJSON: public spOutputFormat {

public:

	//-----------------------------------------------------------------
	spFormatJSON() : buffer_size{BUFFER_SIZE}
	{};

	//-----------------------------------------------------------------
	// value methods
	void logValue(const char* tag, bool value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}

	//-----------------------------------------------------------------
	void logValue(const char* tag, int value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}

	//-----------------------------------------------------------------
	void logValue(const char* tag, float value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}

	//-----------------------------------------------------------------
	void logValue(const char* tag, const char* value)
	{
		if (!_jSection.isNull())
			(_jSection)[tag] = value;
	}

	//-----------------------------------------------------------------
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
		char szBuffer[buffer_size+1];
		size_t n = serializeJson(_jDoc, szBuffer, buffer_size);

		// TODO: Add Error output
		if ( n > buffer_size + 1){
			Serial.println("[WARNING] - JSON document buffer output buffer trimmed");
			szBuffer[buffer_size] = '\0';
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

	size_t buffer_size;

protected:
	JsonObject _jSection;

	StaticJsonDocument<BUFFER_SIZE> _jDoc;

};
