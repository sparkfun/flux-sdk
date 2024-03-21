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
 *  flxDevRV8803.h
 *
 *  Spark Device object for the RV8803 device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_RV8803.h"
#include "flxClock.h"
#include "flxDevice.h"

#define kRV8803DeviceName "RV8803"

// Define our class
class flxDevRV8803 : public flxDeviceI2CType<flxDevRV8803>, public flxIClock, public RV8803
{

  public:
    flxDevRV8803();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

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

    // for the IClock interface

    uint32_t get_epoch(void);
    void set_epoch(const uint32_t &);
    bool valid_epoch(void);

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

    void set_seconds(const uint8_t &);
    void set_minutes(const uint8_t &);
    void set_hours(const uint8_t &);
    void set_date(const uint8_t &);
    void set_weekday(const uint8_t &);
    void set_month(const uint8_t &);
    void set_year(const uint16_t &);

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
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_date_USA> readDateUSA;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_date> readDate;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_time> readTime;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_iso8601> readISO8601;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_iso8601_tz> readISO8601TZ;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_day> readDayOfWeek;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_day_short> readDayOfWeekShort;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_ordinal> readOrdinal;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_month> readMonth;
    flxParameterOutString<flxDevRV8803, &flxDevRV8803::read_month_short> readMonthShort;
    flxParameterOutUInt16<flxDevRV8803, &flxDevRV8803::read_year> readYear;
    flxParameterOutUInt32<flxDevRV8803, &flxDevRV8803::get_epoch> getEpoch;

    // Define our input parameters
    flxParameterInUInt32<flxDevRV8803, &flxDevRV8803::set_epoch> setEpoch;
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_seconds> setSeconds = {0, 59};
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_minutes> setMinutes = {0, 59};
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_hours> setHours = {0, 23};
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_date> setDate = {1, 31};
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_weekday> setWeekday{
        {"Sunday", 0},   {"Monday", 1}, {"Tuesday", 2}, {"Wednesday", 3},
        {"Thursday", 4}, {"Friday", 5}, {"Saturday", 6}};
    flxParameterInUInt8<flxDevRV8803, &flxDevRV8803::set_month> setMonth = {1, 12};
    flxParameterInUInt16<flxDevRV8803, &flxDevRV8803::set_year> setYear = {1970, 2200};
};
