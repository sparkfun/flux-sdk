/*
 *
 * spDevI2C.h
 *
 * Class to encapsulate I2C operations in support of qwiic operations
 *
 * Provides methods to perform common I2C operations in support of qwiic 
 * device drivers
 */

#pragma once

#include "Arduino.h"
#include <Wire.h>

class spDevI2C {

public:

	spDevI2C(void);

	void begin(TwoWire &wirePort = Wire); 

	TwoWire * getWirePort(void){ 
		return _i2cPort;
	}
	
	int receiveResponse(uint8_t i2c_address, uint8_t *outputPointer , uint8_t length);
	
	//ReadRegisterRegion takes a uint8 array address as input and reads
	//a chunk of memory into that array.
    bool readRegisterRegion(uint8_t i2c_address, uint8_t, uint8_t*, uint8_t );
	
	//readRegister reads one register
    uint8_t readRegister(uint8_t i2c_address, uint8_t);
    
    //Reads two regs, LSByte then MSByte order, and concatenates them
	//Used for two-byte reads
	int16_t readRegister16( uint8_t i2c_address, uint8_t offset );

	bool ping(uint8_t i2c_address); // ping the address ...

	//Writes nothing

    bool write(uint8_t i2c_address, uint8_t);	

    // write a block of data
	bool write(uint8_t i2c_address, uint8_t *pData, uint8_t length);

	//Writes a byte;
    bool writeRegister(uint8_t i2c_address, uint8_t, uint8_t);

    bool writeRegister16(uint8_t i2c_address, uint8_t, uint16_t);

    bool writeRegister24(uint8_t i2c_address, uint8_t, uint32_t);    

	// write a data region
	bool writeRegisterRegion(uint8_t, uint8_t, uint8_t * , uint8_t );		
	
private:

	TwoWire * _i2cPort;

};
