/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevMAX17048.h
 *
 *  Spark Device object for the MAX17048 device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"
#include "flxDevice.h"

// SOC level to determine if we are on mains/usb and no battery connected
#define kBatteryNoBatterySOC 110.
#define kMAX17048DeviceName "MAX17048"

// Define our class
class flxDevMAX17048 : public flxDeviceI2CType<flxDevMAX17048>, public SFE_MAX1704X
{

  public:
    flxDevMAX17048();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kMAX17048DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_voltage();
    float read_state_of_charge();
    float read_change_rate();

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_voltage> voltageV;
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_state_of_charge> stateOfCharge;
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_change_rate> changeRate;
};
