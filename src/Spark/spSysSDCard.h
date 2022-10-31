

#pragma once


// Object wrapper around the SD Card object. This allows this object to 
// be part of the framework.


#include "spCore.h"
#include <SdFat.h>  //SdFat by Bill Greiman: http://librarymanager/All#SdFat_exFAT



class spSysSDCard : public spSystemType<spSysSDCard>
{

public:

	spSysSDCard() : _isInitalized{false}, _pinCS{0}, _pinPower{0}, _powerOn{false}{}

	spSysSDCard(uint8_t pinCS, uint8_t pinPower) : 
				_isInitalized{false}, _pinCS{pinCS}, _pinPower{pinPower}, _powerOn{false}{}

	// setup and lifecycle of the file system interface
	// TODO - make this uniform ...
	bool initialize();
	bool initialize(uint8_t pinCS);
	bool initialize(uint8_t pinCS, uint8_t pinPower);

	// Power interface
	void setPower(bool powerOn);
	bool power(void)
	{
		return _powerOn;
	};

	void setPowerPin(uint8_t pin)
	{
		_pinPower = pin;
	};

	void setCSPin(uint8_t pin)
	{
		_pinCS = pin;
	};

private:

	bool 	_isInitalized;

	uint8_t  _pinCS;
	uint8_t  _pinPower;

	bool    _powerOn;
	// SD Card object
	SdFs 	_sd;

};