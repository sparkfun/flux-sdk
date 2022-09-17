//
// Define interfaces/base classes for output
// 

#pragma once

#include "spCore.h"
#include "spOutput.h"
#include "spUtils.h"

#include <Arduino.h>

#define kMaxCVSHeaderTagSize 32

template <std::size_t BUFFER_SIZE>
class spFormatCSV : public spOutputFormat {

public:

	//-----------------------------------------------------------------
	spFormatCSV()
	{
		reset();
	};

	//-----------------------------------------------------------------
	// value methods
	void logValue(const char* tag, bool value)
	{
		// header?
		if ( _bFirstRun )
			if ( !append_to_header(tag) )
				warning_message("CSV - internal header buffer size exceeded.");

		if (!append_csv_value( value ? "true" : "false", _data_buffer, sizeof(_data_buffer)))
				error_message("CSV - internal data buffer size exceeded.");
	}

	//-----------------------------------------------------------------	
	void logValue(const char* tag, int value)
	{
		// header?
		if ( _bFirstRun )
			if ( !append_to_header(tag) )
				warning_message("CSV - internal header buffer size exceeded.");

		char szBuffer[32]={'\0'};
		snprintf(szBuffer, sizeof(szBuffer), "%d", value);
		if (!append_csv_value( szBuffer, _data_buffer, sizeof(_data_buffer)))
				error_message("CSV - internal data buffer size exceeded.");
	}

	//-----------------------------------------------------------------
	void logValue(const char* tag, float value)
	{
		// header?
		if ( _bFirstRun )
			if ( !append_to_header(tag) )
				warning_message("CSV - internal header buffer size exceeded.");

		char szBuffer[32]={'\0'};
		(void)sp_utils::dtostr(value, szBuffer, sizeof(szBuffer));

		if (!append_csv_value( szBuffer, _data_buffer, sizeof(_data_buffer)))
				error_message("CSV - internal data buffer size exceeded.");
		
	}

	//-----------------------------------------------------------------	
	void logValue(const char* tag, const char* value)
	{
		// header?
		if ( _bFirstRun )
			if ( !append_to_header(tag) )
				warning_message("CSV - internal header buffer size exceeded.");

		if (!append_csv_value( value, _header_buffer, sizeof(_data_buffer)))
				error_message("CSV - internal data buffer size exceeded.");
	}

	//-----------------------------------------------------------------
	// structure cycle 

	virtual void beginObservation(const char * szTitle=nullptr)
	{
		
		//if(szTitle) // TODO TITLE?
		
	}

	//-----------------------------------------------------------------
	void beginSection(const char * szName)
	{
		_section_name = (char*) szName;
	}

	//-----------------------------------------------------------------
	void endSection(const char * szName)
	{
		_section_name=nullptr;
	}
	//-----------------------------------------------------------------
	void endObservation(void)
	{
	}

	//-----------------------------------------------------------------	
	virtual void writeObservation()
	{
		
		// Write out the header?
		if ( _bFirstRun && strlen(_header_buffer) > 0 )
		{
			_bFirstRun = false;
			outputObservation(_header_buffer);
		}

		outputObservation(_data_buffer);
	}

	//-----------------------------------------------------------------	
	void clearObservation(void)
	{
		clear_buffers();
		_section_name = nullptr;
	}

	//-----------------------------------------------------------------
	void reset(void)
	{
		clear_buffers();
		_bFirstRun = true;
		_section_name = nullptr;
	}

private:

	//-----------------------------------------------------------------
	void clear_buffers(){
		memset(_header_buffer, '\0', sizeof(_header_buffer));
		memset(_data_buffer, '\0', sizeof(_data_buffer));
	}

	//-----------------------------------------------------------------
	bool append_csv_value(const char *value, char *buffer, size_t buffer_size){

		if (strlen(value)+ strlen(buffer) + 1 >= buffer_size)
			return false;

		if(strlen(buffer) > 0)
			strlcat(buffer, ",", buffer_size);

		strlcat(buffer, value, buffer_size);

		return true;
	}

	//-----------------------------------------------------------------
	bool append_to_header(const char *tag)
	{
		if (!tag)
			return true;

		// build up our header title for this element. Title is {section name}.{tag}
		char szBuffer[kMaxCVSHeaderTagSize]={'\0'};

		if ( _section_name )
		{
			strlcpy(szBuffer, _section_name, sizeof(szBuffer));
			strlcat(szBuffer, ".", sizeof(szBuffer));
		}
		strlcat(szBuffer, tag, sizeof(szBuffer));

		return append_csv_value(szBuffer, _header_buffer, sizeof(_header_buffer));
	}

	//-----------------------------------------------------------------
	char _header_buffer[BUFFER_SIZE];
	char _data_buffer[BUFFER_SIZE];	

	char * _section_name;

	bool _bFirstRun;

};
