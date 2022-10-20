/*
 *
 *  spDevRV8803.h
 *
 *  Spark Device object for the RV8803 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_RV8803.h"
#include "spDevice.h"



#define kRV8803DeviceName "rv8803"

// Define our class
class spDevRV8803 : public spDeviceType<spDevRV8803>, public RV8803
{

public:
    spDevRV8803();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kRV8803DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    std::string read_date_USA();
    std::string read_date();
    std::string read_time();
    std::string read_iso8601();
    uint get_epoch();

    void set_epoch(const uint &);
    void set_seconds(const uint8_t &);
    void set_minutes(const uint8_t &);
    void set_hours(const uint8_t &);
    void set_date(const uint8_t &);
    void set_weekday(const uint8_t &);
    void set_month(const uint8_t &);
    void set_year(const uint16_t &);

    bool get_offset_epoch();
    void set_offset_epoch(bool);

    bool _begun = false;
    bool _offsetEpoch = false;

    // Flags to prevent updateTime from being called multiple times
    bool _dateUSA = false;
    bool _date = false;
    bool _time = false;
    bool _iso8601 = false;
    bool _epoch = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutString<spDevRV8803, &spDevRV8803::read_date_USA> readDateUSA;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_date> readDate;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_time> readTime;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_iso8601> readISO8601;
    spParameterOutUint<spDevRV8803, &spDevRV8803::get_epoch> getEpoch;

    // Define our input parameters
    spParameterInUint<spDevRV8803, &spDevRV8803::set_epoch> setEpoch;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_seconds> setSeconds;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_minutes> setMinutes;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_hours> setHours;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_date> setDate;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_weekday> setWeekday;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_month> setMonth;
    spParameterInUint16<spDevRV8803, &spDevRV8803::set_year> setYear;

    // Define our read-write properties
    spPropertyRWBool<spDevRV8803, &spDevRV8803::get_offset_epoch, &spDevRV8803::set_offset_epoch> offsetEpoch;
};
