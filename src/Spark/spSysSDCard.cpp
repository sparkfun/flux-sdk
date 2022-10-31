


// Object wrapper around the SD Card object. This allows this object to 
// be part of the framework.


#include "spSysSDCard.h"


bool spSysSDCard::initialize()
{
	if (_isInitalized)
		return true;

	if (!_pinCS || !_pinPower)
	{
		spLog_E("Unable to initialize SD Card device - hardware not configured.");
		return false;
	}

	pinMode(_pinPower, OUTPUT);
	pinMode(_pinCS, OUTPUT);

	digitalWrite(_pinCS, HIGH); 

	delay(5);

	powerOn();

	// FYI - the following is taken from OpenLog Artemis ...
	//Max power up time is 250ms: https://www.kingston.com/datasheets/SDCIT-specsheet-64gb_en.pdf
  	//Max current is 200mA average across 1s, peak 300mA
  	for (int i = 0; i < 10; i++) //Wait
  	{
  		// checkBattery(); from OLA
	    delay(1);
  	}

  	// let's startup the SD
  	SdSpiConfig sdConfig(_pinCS, SHARED_SPI, SD_SCK_MHZ(24));

  	if (_sd.begin(sdConfig) == false) // Try to begin the SD card using the correct chip select
  	{
    	spLog_D(F("SD init failed (first attempt). Trying again...\r\n"));
    	for (int i = 0; i < 250; i++) //Give SD more time to power up, then try again
    	{
      		//checkBattery();
      		delay(1);
    	}
    	
    	if (_sd.begin(sdConfig) == false) // Try to begin the SD card using the correct chip select
    	{
        	spLog_W(F("SD init failed (second attempt). Is card present? Formatted?"));
        	spLog_W(F("Please ensure the SD card is formatted correctly using https://www.sdcard.org/downloads/formatter/"));
	      	digitalWrite(_pinCS, HIGH); //Be sure SD is deselected
	      	return false;
      	}
	}

	// Can CD to root?
	if (_sd.chdir() == false)
	{
		spLog_W("SD change directory failed. SD system failed to initalize");
		return false;
	}

	_isInitalized = true;
	// If we are here, we are ready.
	return true; 
}

//-----------------------------------------------------------------------

bool spSysSDCard::initialize(uint8_t pinCS)
{
	if (!pinCS)
		return false;

	_pinCS = pinCS;
	return initialize();

}

//-----------------------------------------------------------------------
bool spSysSDCard::initialize(uint8_t pinCS, uint8_t pinPower) 
{
	if (!pinCS || !pinPower)
		return false;

	_pinCS = pinCS;
	_pinPower =  pinPower;

	return initialize();
}

// Power interface
void spSysSDCard::setPower(bool powerOn)
{
	if ( !_pinPower || (_isInitalized && powerOn == _powerOn))
		return; // no need to continue

	pinMode(_pinPower, OUTPUT);
	digitalWrite(_pinPower, (powerOn ? LOW : HIGH));
	_powerOn = powerOn;

}