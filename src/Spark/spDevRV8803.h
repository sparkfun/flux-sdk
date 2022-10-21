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
    std::string read_iso8601_tz();
    std::string read_day();
    std::string read_day_short();
    std::string read_ordinal();
    std::string read_month();
    std::string read_month_short();
    uint16_t read_year();
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
    int8_t get_time_zone_quarter_hours();
    void set_time_zone_quarter_hours(int8_t);

    bool _begun = false;
    bool _offsetEpoch = false;

    // Flags to prevent updateTime from being called multiple times
    bool _dateUSA = false;
    bool _date = false;
    bool _time = false;
    bool _iso8601 = false;
    bool _iso8601tz = false;
    bool _epoch = false;
    bool _day = false;
    bool _day_short = false;
    bool _ordinal = false;
    bool _month = false;
    bool _month_short = false;
    bool _year = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutString<spDevRV8803, &spDevRV8803::read_date_USA> readDateUSA;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_date> readDate;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_time> readTime;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_iso8601> readISO8601;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_iso8601_tz> readISO8601TZ;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_day> readDayOfWeek;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_day_short> readDayOfWeekShort;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_ordinal> readOrdinal;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_month> readMonth;
    spParameterOutString<spDevRV8803, &spDevRV8803::read_month_short> readMonthShort;
    spParameterOutUint16<spDevRV8803, &spDevRV8803::read_year> readYear;
    spParameterOutUint<spDevRV8803, &spDevRV8803::get_epoch> getEpoch;

    // Define our input parameters
    spParameterInUint<spDevRV8803, &spDevRV8803::set_epoch> setEpoch;
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_seconds> setSeconds = { 0, 59 };
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_minutes> setMinutes = { 0, 59 };
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_hours> setHours = { 0, 23 };
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_date> setDate = { 1, 31 };
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_weekday> setWeekday
        { {"Sunday",0}, {"Monday",1}, {"Tuesday",2}, {"Wednesday",3}, {"Thursday",4}, {"Friday",5}, {"Saturday",6}};
    spParameterInUint8<spDevRV8803, &spDevRV8803::set_month> setMonth = { 1, 12 };
    spParameterInUint16<spDevRV8803, &spDevRV8803::set_year> setYear = { 1970, 2200 };

    // Define our read-write properties
    spPropertyRWBool<spDevRV8803, &spDevRV8803::get_offset_epoch, &spDevRV8803::set_offset_epoch> offsetEpoch;
    spPropertyRWInt8<spDevRV8803, &spDevRV8803::get_time_zone_quarter_hours, &spDevRV8803::set_time_zone_quarter_hours> timeZoneQuarterHours = { -48, 56 };
};