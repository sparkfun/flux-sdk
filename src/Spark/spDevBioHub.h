/*
 *
 *  spDevBioHub.h
 *
 *  Spark Device object for the BioHub device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_Bio_Sensor_Hub_Library.h"

// What is the name used to ID this device?
#define kBioHubDeviceName "BioHub"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBioHub : public spDeviceI2CType<spDevBioHub>, public SparkFun_Bio_Sensor_Hub
{

public:
    spDevBioHub();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected( spBusI2C &i2cDriver, uint8_t address, int connectResetPin = -1, int connectMfioPin = -1 );
    static const char *getDeviceName()
    {
        return kBioHubDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize( TwoWire &, int connectResetPin = -1, int connectMfioPin = -1 );

private:

    // methods used to get values for our output parameters
    bioData body;
    uint16_t read_heart_rate();
    uint8_t read_confidence();
    uint16_t read_oxygen();
    uint8_t read_status(); // 0: Success, 1: Not Ready, 2: Object Detectected, 3: Finger Detected
    int8_t read_extended_status();
    float read_r_value();

    // methods to get/set our read-write properties
    int get_reset_pin();
    void set_reset_pin(int);
    int get_mfio_pin();
    void set_mfio_pin(int);

    int _resetPin = -1;
    int _mfioPin = -1;
    bool _begun = false;
    bool _initialized = false;

    // flags to prevent readBpm being called multiple times
    bool _heartRate = false;
    bool _confidence = false;
    bool _o2 = false;
    bool _status = false;
    bool _eStatus = false;
    bool _o2r = false;

    TwoWire *_bioI2cPort; // Cheat... Keep a local record of which wirePort is being used, so we can call begin again

public:
    // Define our read-write properties
    spPropertyRWInt<spDevBioHub, &spDevBioHub::get_reset_pin, &spDevBioHub::set_reset_pin> resetPin = { -1, -1, 255 } ;
    spPropertyRWInt<spDevBioHub, &spDevBioHub::get_mfio_pin, &spDevBioHub::set_mfio_pin> mfioPin = { -1, -1, 255 } ;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint16<spDevBioHub, &spDevBioHub::read_heart_rate> heartRate;    
    spParameterOutUint8<spDevBioHub, &spDevBioHub::read_confidence> confidence;    
    spParameterOutUint16<spDevBioHub, &spDevBioHub::read_oxygen> O2;    
    spParameterOutUint8<spDevBioHub, &spDevBioHub::read_status> status;    
    spParameterOutInt8<spDevBioHub, &spDevBioHub::read_extended_status> eStatus;    
    spParameterOutFloat<spDevBioHub, &spDevBioHub::read_r_value> O2R;    
};
