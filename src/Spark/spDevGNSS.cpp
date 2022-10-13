/*
 *
 *  spDevGNSS.h
 *
 *  Spark Device object for the GNSS device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevGNSS.h"

// Define our class static variables - allocs storage for them

#define kGNSSAddressDefault 0x42 // GNSS_ADDR

uint8_t spDevGNSS::defaultDeviceAddress[] = {kGNSSAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevGNSS);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevGNSS::spDevGNSS()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("u-blox GNSS");

    // Register parameters
    spRegister(year, "Year", "Year");
    spRegister(month, "Month", "Month");
    spRegister(day, "Day", "Day Of Month");
    spRegister(hour, "Hour", "Hour");
    spRegister(min, "Minute", "Minute");
    spRegister(sec, "Second", "Second");
    spRegister(latitude, "Latitude (deg)", "Latitude in degrees");
    spRegister(longitude, "Longitude (deg)", "Longitude in degrees");
    spRegister(altitude, "Altitude (m)", "Altitude above geoid in metres");
    spRegister(altitudeMSL, "Altitude MSL (m)", "Altitude above Mean Sea Level in metres");
    spRegister(SIV, "SIV", "Satellites In View");
    spRegister(fixType, "Fix Type", "Fix Type");
    spRegister(carrierSolution, "Carrier Solution", "Carrier Solution");
    spRegister(groundSpeed, "Ground Speed (m/s)", "Ground speed in metres per second");
    spRegister(heading, "Heading (deg)", "Heading / course in degrees");
    spRegister(PDOP, "PDOP", "Position Dilution Of Precision");
    spRegister(TOW, "TOW (ms)", "Time Of Week in milliseconds");
    spRegister(iso8601, "ISO8601", "Date and time in ISO 8601 format");
    spRegister(YYYYMMDD, "YYYYMMDD", "Year/Month/Date");
    spRegister(YYYYDDMM, "YYYYDDMM", "Year/Date/Month");
    spRegister(DDMMYYYY, "DDMMYYYY", "Date/Month/Year");
    spRegister(HHMMSS, "HHMMSS", "Hour:Minute:Second");
    spRegister(fixTypeStr, "Fix Type (String)", "Fix type in string format");
    spRegister(carrierSolutionStr, "Carrier Solution (String)", "Carrier solution in string format");

    // Register read-write properties
    spRegister(measurementRate, "Measurement Rate (ms)", "Set the measurement interval in milliseconds");

    // Register our input parameters
    spRegister(factoryDefault, "Restore Factory Defaults", "Restore Factory Defaults - takes 5 seconds");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevGNSS::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t firstBufferWaiting = 0;
    uint16_t bufferWaiting = 0;
    bool trafficSeen = false;
    unsigned long startTime = millis();

    bool i2cOK = i2cDriver.readRegister16(address, 0xFD, &firstBufferWaiting, false); // Big Endian

    if (!i2cOK)
        return false; // Return now of the read fails

    // Wait for up to 2 seconds for traffic to be seen
    while(i2cOK && (!trafficSeen) && (millis() < (startTime + 2000)))
    {
        delay(100); // Don't pound the bus
        i2cOK = i2cDriver.readRegister16(address, 0xFD, &bufferWaiting, false); // Big Endian
        if (i2cOK)
            trafficSeen = (bufferWaiting != firstBufferWaiting); // Has traffic been seen?
    }

    if (i2cOK && trafficSeen)
        return true; // Return now if traffic has been seen

    // If the GNSS has been powered on for some time, the buffer could be full
    // Try to read some data from the buffer and see if the count changes
    if (bufferWaiting < 8)
        return false; // Bail of there are less than 8 bytes in the buffer

    uint8_t buffer[8];
    i2cOK = i2cDriver.receiveResponse(address, buffer, 8); // Will read from address 0xFF
    i2cOK &= i2cDriver.readRegister16(address, 0xFD, &bufferWaiting, false); // Big Endian
    trafficSeen = (bufferWaiting != firstBufferWaiting);

    return (i2cOK && trafficSeen);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevGNSS::onInitialize(TwoWire &wirePort)
{

    bool result = SFE_UBLOX_GNSS::begin(wirePort);
    if (result)
    {
        SFE_UBLOX_GNSS::setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
        SFE_UBLOX_GNSS::saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
        SFE_UBLOX_GNSS::setAutoPVT(true);                           // Enable PVT at the navigation rate
        delay(1100);
        SFE_UBLOX_GNSS::getPVT(); // Ensure we get fresh data
    }
    return result;
}

// GETTER methods for output params
uint spDevGNSS::read_year() { return SFE_UBLOX_GNSS::getYear(); }
uint spDevGNSS::read_month() { return SFE_UBLOX_GNSS::getMonth(); }
uint spDevGNSS::read_day() { return SFE_UBLOX_GNSS::getDay(); }
uint spDevGNSS::read_hour() { return SFE_UBLOX_GNSS::getHour(); }
uint spDevGNSS::read_min() { return SFE_UBLOX_GNSS::getMinute(); }
uint spDevGNSS::read_sec() { return SFE_UBLOX_GNSS::getSecond(); }
double spDevGNSS::read_latitude() { return (((double)SFE_UBLOX_GNSS::getLatitude()) / 10000000); }
double spDevGNSS::read_longitude() { return (((double)SFE_UBLOX_GNSS::getLongitude()) / 10000000); }
double spDevGNSS::read_altitude() { return (((double)SFE_UBLOX_GNSS::getAltitude()) / 1000); }
double spDevGNSS::read_altitude_msl() { return (((double)SFE_UBLOX_GNSS::getAltitudeMSL()) / 1000); }
uint spDevGNSS::read_siv() { return SFE_UBLOX_GNSS::getSIV(); }
uint spDevGNSS::read_fix() { return SFE_UBLOX_GNSS::getFixType(); }
uint spDevGNSS::read_carrier_soln() { return SFE_UBLOX_GNSS::getCarrierSolutionType(); }
float spDevGNSS::read_ground_speed() { return (((float)SFE_UBLOX_GNSS::getGroundSpeed()) / 1000); }
float spDevGNSS::read_heading() { return (((float)SFE_UBLOX_GNSS::getHeading()) / 100000); }
float spDevGNSS::read_pdop() { return (((float)SFE_UBLOX_GNSS::getPDOP()) / 100); }
uint spDevGNSS::read_tow() { return SFE_UBLOX_GNSS::getTimeOfWeek(); }

std::string spDevGNSS::read_iso8601()
{
    uint y = SFE_UBLOX_GNSS::getYear();
    uint M = SFE_UBLOX_GNSS::getMonth();
    uint d = SFE_UBLOX_GNSS::getDay();
    uint h = SFE_UBLOX_GNSS::getHour();
    uint m = SFE_UBLOX_GNSS::getMinute();
    uint s = SFE_UBLOX_GNSS::getSecond();

    char szBuffer[32] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%04d-%02d-%02dT%02d:%02d:%02dZ", y, M, d, h, m, s);

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_yyyy_mm_dd()
{
    uint y = SFE_UBLOX_GNSS::getYear();
    uint M = SFE_UBLOX_GNSS::getMonth();
    uint d = SFE_UBLOX_GNSS::getDay();

    char szBuffer[24] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%04d/%02d/%02d", y, M, d);

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_yyyy_dd_mm()
{
    uint y = SFE_UBLOX_GNSS::getYear();
    uint M = SFE_UBLOX_GNSS::getMonth();
    uint d = SFE_UBLOX_GNSS::getDay();

    char szBuffer[24] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%04d/%02d/%02d", y, d, M);

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_dd_mm_yyyy()
{
    uint y = SFE_UBLOX_GNSS::getYear();
    uint M = SFE_UBLOX_GNSS::getMonth();
    uint d = SFE_UBLOX_GNSS::getDay();

    char szBuffer[24] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%02d/%02d/%04d", d, M, y);

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_hh_mm_ss()
{
    uint h = SFE_UBLOX_GNSS::getHour();
    uint m = SFE_UBLOX_GNSS::getMinute();
    uint s = SFE_UBLOX_GNSS::getSecond();

    char szBuffer[24] = {'\0'};
    snprintf(szBuffer, sizeof(szBuffer), "%02d:%02d:%02d", h, m, s);

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_fix_string()
{
    uint fix = SFE_UBLOX_GNSS::getFixType();

    char szBuffer[24] = {'\0'};

    switch (fix)
    {
    case 0:
        snprintf(szBuffer, sizeof(szBuffer), "none");
        break;
    case 1:
        snprintf(szBuffer, sizeof(szBuffer), "dead reckoning");
        break;
    case 2:
        snprintf(szBuffer, sizeof(szBuffer), "2D");
        break;
    case 3:
        snprintf(szBuffer, sizeof(szBuffer), "3D");
        break;
    case 4:
        snprintf(szBuffer, sizeof(szBuffer), "GNSS + dead reckoning");
        break;
    case 5:
        snprintf(szBuffer, sizeof(szBuffer), "time only");
        break;
    default:
        snprintf(szBuffer, sizeof(szBuffer), "unknown");
        break;
    }

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

std::string spDevGNSS::read_carrier_soln_string()
{
    uint carrSoln = SFE_UBLOX_GNSS::getCarrierSolutionType();

    char szBuffer[24] = {'\0'};

    switch (carrSoln)
    {
    case 0:
        snprintf(szBuffer, sizeof(szBuffer), "none");
        break;
    case 1:
        snprintf(szBuffer, sizeof(szBuffer), "floating");
        break;
    case 2:
        snprintf(szBuffer, sizeof(szBuffer), "fixed");
        break;
    default:
        snprintf(szBuffer, sizeof(szBuffer), "unknown");
        break;
    }

    theString = sp_utils::to_string(szBuffer);

    return theString;
}

// methods for read-write properties
uint spDevGNSS::get_measurement_rate() { return SFE_UBLOX_GNSS::getMeasurementRate(); }
void spDevGNSS::set_measurement_rate(uint rate) { SFE_UBLOX_GNSS::setMeasurementRate(rate); }

// methods for in parameters
void spDevGNSS::factory_default()
{
    SFE_UBLOX_GNSS::factoryDefault();
    delay(5000);
    SFE_UBLOX_GNSS::setI2COutput(COM_TYPE_UBX);
    SFE_UBLOX_GNSS::saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);
    SFE_UBLOX_GNSS::setAutoPVT(true);
}