/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
/*
 *
 *  flxDevTEMPLATE.h // REPLACE ME //
 *
 *  Spark Device object for the TEMPLATE device. // REPLACE ME //
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "DEVICE_LIBRARY_REPLACE_ME.h" /* REPLACE ME */

// What is the name used to ID this device
#define kTEMPLATEDeviceName "TEMPLATE" /* REPLACE ME */

//-----------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic library
class flxDevTEMPLATE /* REPLACE ME */ : public flxDeviceI2CType<flxDevTEMPLATE /* REPLACE ME */>, public TEMPLATECLASS /* REPLACE ME */
{
    public:
        flxDevTEMPLATE(); /* REPLACE ME */

        // Static Interface - used by the system to determine if this device is
        // connected before the object is instantiated.
        static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
        static const char *getDeviceName()
        {
            return kTEMPLATEDeviceName; /* REPLACE ME */
        };

        static const uint8_t *getDefaultAddresses()
        {
            return defaultDeviceAddress;
        };
        // This holds the class list of possible addresses/IDs for this object
        static uint8_t defaultDeviceAddress[];

        // Method called to initialize the class
        bool onInitialize(TwoWire &);

    private:
        // Methods used to get values for our output parameters
        uint read_SENSORVALUE1(); /* REPLACE ME */

        // Methods used to set values for our input parameters
        void write_COMPENSATIONVALUE1(const uint16_t &); /* REPLACE ME */

        // Methods used to get values for our RW properties
        bool get_DEVICEPROPERTY1(); /* REPLACE ME */
        uint get_DEVICEPROPERTY2(); /* REPLACE ME */

        // Methods used to set values for our RW properties
        void set_DEVICEPROPERTY1(bool); /* REPLACE ME */
        void set_DEVICEPROPERTY2(uint); /* REPLACE ME */

        // OPTIONAL: flags for controlling measurement methods
        bool _sense1 = false; /* REPLACE ME */

        // OPTIONAL: class variables for tracking state
        bool _state1 = false; /* REPLACE ME */
        uint16_t _state2 = 0; /* REPLACE ME */

        // OPTIONAL: class variables for storing intermediate variables
        uint32_t _deviceValue = 0; /* REPLACE ME */

    public:
        // Define the sensor XYZ property in UNITS. 
        // Default value is REPLACEME.
        flxPropertyRWBool<flxDevTEMPLATE /* REPLACE ME */, &flxDevTEMPLATE::get_DEVICEPROPERTY1 /* REPLACE ME */, &flxDevTEMPLATE::set_DEVICEPROPERTY1 /* REPLACE ME */> deviceProperty1; /* REPLACE ME */

        // Define the sensor YZX property in UNITS. Valid range is VALUE1 UNIT to VALUE2 UNIT.
        // Default value is REPLACEME.
        flxPropertyRWUint<flxDevTEMPLATE /* REPLACE ME */, &flxDevTEMPLATE::get_DEVICEPROPERTY2 /* REPLACE ME */, &flxDevTEMPLATE::set_DEVICEPROPERTY2 /* REPLACE ME */> deviceProperty2 = {1 /* INIT */, 0 /* MIN */, 2 /* MAX */}; /* REPLACE ME */

        // Define our output parameters
        flxParameterOutUint<flxDevTEMPLATE /* REPLACE ME */, &flxDevTEMPLATE::read_SENSORVALUE1 /* REPLACE ME */> senseValue1; /* REPLACE ME */

        // Define our input parameters
        flxParameterInUint16<flxDevTEMPLATE /* REPLACE ME */, &flxDevTEMPLATE::write_COMPENSATIONVALUE1 /* REPLACE ME */> compensationValue1; /* REPLACE ME */

};