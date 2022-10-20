/*
 *
 *  spDevRV8803.cpp
 *
 *  Spark Device object for the RV8803 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevRV8803.h"


uint8_t spDevRV8803::defaultDeviceAddress[] = { RV8803_ADDR, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevRV8803);

spDevRV8803::spDevRV8803()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("RV8803 Real Time Clock");

    // Register output params
    spRegister(readDateUSA, "Read Date (MM/DD/YYYY)", "Read the date in USA format");
    spRegister(readDate, "Read Date (DD/MM/YY)", "Read the date");
    spRegister(readTime, "Read Time (HH:MM:SS)", "Read the time");
    spRegister(readISO8601, "Read ISO8601 DateTime (YYYY-MM-DDTHH:MM:SS)", "Read the date and time in ISO8601 format");
    spRegister(getEpoch, "Get Epoch", "Get the time in seconds since the Epoch");

    // Register input params
    spRegister(setEpoch, "Set Epoch", "Set the time since the Epoch");
    spRegister(setSeconds, "Set the seconds", "Set the seconds");
    spRegister(setMinutes, "Set the minutes", "Set the minutes");
    spRegister(setHours, "Set the hours", "Set the hours");
    spRegister(setDate, "Set the date", "Set the date");
    spRegister(setWeekday, "Set the weekday", "Set the weekday: 0=Sunday, 6=Saturday");
    spRegister(setMonth, "Set the month", "Set the month");
    spRegister(setYear, "Set the year", "Set the year");

    // Register read-write properties
    spRegister(offsetEpoch, "Offset Epoch", "Default false. Set to true if time.h requires an offset to 1970");
}

// Static method used to determine if this device is connected

bool spDevRV8803::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Write 0xAA to RAM (register 0x07). Read it back. Set it to 0x00.
    if (!i2cDriver.writeRegister(address, 0x07, 0xAA))
        return false;

    uint8_t ramByte = 0;
    if (!i2cDriver.readRegister(address, 0x07, &ramByte))
        return false;

    if (!i2cDriver.writeRegister(address, 0x07, 0x00))
        return false;

    return (ramByte == 0xAA);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevRV8803::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = RV8803::begin(wirePort);

    RV8803::set24Hour();

    RV8803::setEpoch(1640995200, _offsetEpoch); // Jan 1st 2022

    if (result)
        _begun = true;
    else
        spLog_E("RV8803 - begin failed");

    return result;
}

// Output parameters
std::string spDevRV8803::read_date_USA()
{
    if (!_dateUSA)
    {
        RV8803::updateTime();
        _date = true;
        _time = true;
        _iso8601 = true;
        _epoch = true;
    }
    _dateUSA = false;

    char szBuffer[12] = {'\0'};

    if (RV8803::stringDateUSA(szBuffer, sizeof(szBuffer)) != szBuffer) // Should return MM/DD/YYYY
        spLog_E("RV8803 - read_date_USA failed");

    std::string theString = szBuffer;

    return theString;
}
std::string spDevRV8803::read_date()
{
    if (!_date)
    {
        RV8803::updateTime();
        _dateUSA = true;
        _time = true;
        _iso8601 = true;
        _epoch = true;
    }
    _date = false;

    char szBuffer[12] = {'\0'};

    if (RV8803::stringDate(szBuffer, sizeof(szBuffer)) != szBuffer) // Should return DD/MM/YYYY
        spLog_E("RV8803 - read_date failed");

    std::string theString = szBuffer;

    return theString;
}
std::string spDevRV8803::read_time()
{
    if (!_time)
    {
        RV8803::updateTime();
        _dateUSA = true;
        _date = true;
        _iso8601 = true;
        _epoch = true;
    }
    _time = false;

    char szBuffer[12] = {'\0'};

    if (RV8803::stringTime(szBuffer, sizeof(szBuffer)) != szBuffer) // Should return HH:MM:SS
        spLog_E("RV8803 - read_time failed");

    std::string theString = szBuffer;

    return theString;
}
std::string spDevRV8803::read_iso8601()
{
    if (!_iso8601)
    {
        RV8803::updateTime();
        _dateUSA = true;
        _date = true;
        _time = true;
        _epoch = true;
    }
    _iso8601 = false;

    char szBuffer[21] = {'\0'};

    if (RV8803::stringTime8601(szBuffer, sizeof(szBuffer)) != szBuffer) // Should return YYYY-MM-DDTHH:MM:SS
        spLog_E("RV8803 - read_iso8601 failed");

    std::string theString = szBuffer;

    return theString;
}
uint spDevRV8803::get_epoch()
{
    if (!_epoch)
    {
        RV8803::updateTime();
        _dateUSA = true;
        _date = true;
        _time = true;
        _iso8601 = true;
    }
    _epoch = false;

    return RV8803::getEpoch(_offsetEpoch);
}

// Output parameters
void spDevRV8803::set_epoch(const uint &epoch)
{
    if (!RV8803::setEpoch(epoch, _offsetEpoch))
        spLog_E("RV8803 - set_epoch failed");
}
void spDevRV8803::set_seconds(const uint8_t &secs)
{
    if (!RV8803::setSeconds(secs))
        spLog_E("RV8803 - set_seconds failed");
    else
        RV8803::setHundredthsToZero();
}
void spDevRV8803::set_minutes(const uint8_t &min)
{
    if (!RV8803::setMinutes(min))
        spLog_E("RV8803 - set_minutes failed");
}
void spDevRV8803::set_hours(const uint8_t &hour)
{
    if (!RV8803::setHours(hour))
        spLog_E("RV8803 - set_hour failed");
}
void spDevRV8803::set_date(const uint8_t &date)
{
    if (!RV8803::setDate(date))
        spLog_E("RV8803 - set_date failed");
}
void spDevRV8803::set_month(const uint8_t &month)
{
    if (!RV8803::setMonth(month))
        spLog_E("RV8803 - set_month failed");
}
void spDevRV8803::set_year(const uint16_t &year)
{
    if (!RV8803::setYear(year))
        spLog_E("RV8803 - set_year failed");
}
void spDevRV8803::set_weekday(const uint8_t &dow)
{
    if (!RV8803::setWeekday(dow))
        spLog_E("RV8803 - set_weekday failed");
}

// Read-write properties
bool spDevRV8803::get_offset_epoch() { return _offsetEpoch; }
void spDevRV8803::set_offset_epoch(bool offset) { _offsetEpoch = offset; }
