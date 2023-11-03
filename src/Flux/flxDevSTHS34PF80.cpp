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
 *  flxDevSTHS34PF80.cpp 
 *
 *  Spark Device object for the STMicroelectronics STHS34PF80
 *  human presence sensor device. 
 *
 *
 */

#include <Arduino.h>
#include "SparkFun_STHS34PF80_Arduino_Library.h"

#include "flxDevSTHS34PF80.h" 

#define kSTHS34PF80AddressDefault STHS34PF80_I2C_ADD

// Define our class static variables - allocs storage for them
uint8_t flxDevSTHS34PF80::defaultDeviceAddress[] = {kSTHS34PF80AddressDefault, kSparkDeviceAddressNull}; 

//-----------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.
flxRegisterDevice(flxDevSTHS34PF80);

//-----------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including
// device identifiers (name, I2C address) and managed properties.
flxDevSTHS34PF80::flxDevSTHS34PF80() 
{
    // Setup unique identifiers for this device and basic device object systems
    setName(kSTHS34PF80DeviceName, "Human Presence Sensor");

    // Register the properties with the system - this makes the connections
    // needed to support managed properties/public properties and parameters

    // Register Properties
    flxRegister(presence, "Presence (cm^-1)", "Presence Value (1/cm)");
    flxRegister(motion, "Motion (LSB)", "Motion Value");
    flxRegister(temperature, "Temperature (C)", "Temperature Value (C)");

    // Register Parameters
    flxRegister(avgTObjectNum, "Avg Samples for TObject", "Number of Average Samples for TObject");
    flxRegister(avgTAmbientNum, "Avg Samples for TAmbient", "Number of Average Samples for TAmbient");
    flxRegister(gainMode, "Enable Gain Mode", "Enables the Gain Mode");
    flxRegister(tmosODR, "Output Data Rate", "Output Data Rate");
    flxRegister(tmosOneShot, "SENSE1 NAME", "SENSE1 DESCRIPTION");
    flxRegister(memBank, "Memory Bank", "Enable Access to Embedded Functions");

    flxRegister(lpfMotionBandwidth, "Low Pass Motion Filter", "Low Pass Motion Filter Bandwidth");
    flxRegister(lpfPresenceMotionBandwidth, "Low Pass Presence/Motion Filter", "Low Pass Presence and Motion Filter Bandwidth");
    flxRegister(lpfTempBandwidth, "Low Pass Temperature Filter", "Low Pass Ambient Temperature Filter Bandwidth");
    flxRegister(lpfPresenceBandwidth, "Low Pass Presence Filter", "Low Pass Presence Filter Bandwidth");

    flxRegister(tmosInterruptRoute, "Interrupt Output Type", "Interrupt Output Type");
    flxRegister(dataReady, "Data Ready", "Data Ready for Presence, Motion, and Temperature");

}

//-----------------------------------------------------------------------------
// onInitialize - Base Version
//
// Specific I2C and SPI versions below
// Called during the startup/initialization of the driver (after the 
// constructor is called).
//
// Place to initialize the underlying device library/driver.
bool flxDevSTHS34PF80Base::onInitialize(void)
{

    bool result = true;

    deviceReset();
    unsigned long startTime = millis();
    int32_t reset = false;

    while (!reset && (millis() < (startTime + 2000))) // Time out after 2 seconds
    {
        reset = reset();
        delay(1);
    }
    if (reset)
    {
        delay(100);
        result &= setDeviceConfig();
        result &= setBlockDataUpdate();
        result &= setAccelDataRate(_accel_data_rate);
        result &= setAccelFullScale(_accel_full_scale);
        result &= setGyroDataRate(_gyro_data_rate);
        result &= setGyroFullScale(_gyro_full_scale);
        result &= setAccelFilterLP2(_accel_filter_lp2);
        result &= setAccelSlopeFilter(_accel_slope_filter);
        result &= setGyroFilterLP1(_gyro_filter_lp1);
        result &= setGyroLP1Bandwidth(_gyro_lp1_bandwidth);
        if (!result)
            flxLog_E("STHS34PF80 onInitialize: device configuration failed");
    }
    else
    {
        flxLog_E("STHS34PF80 onInitialize: device did not reset");
        result = false;
    }
    _begun = result;
    return (bool)result;
}

// GETTER methods for output params
int16_t flxDevSTHS34PF80Base::read_Presence()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if(drdy == 1)
    {
        getPresenceValue(&presence);
    }
    return presence;
}
int16_t flxDevSTHS34PF80Base::read_Motion()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if(drdy == 1)
    {
        getMotionValue(&motion);
    }
    return motion;
}
float flxDevSTHS34PF80Base::read_TemperatureC()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if(drdy == 1)
    {
        getTemperatureData(&temperature);
    }
    return temperature;
}

// GETTER and SETTER methods for device properties
uint8_t flxDevSTHS34PF80Base::get_DeviceID()
{
    uint8_t devID;
    return getDeviceID(devID);
}
bool flxDevSTHS34PF80Base::get_DataReady()
{
    sths34pf80_tmos_drdy_status_t dataReady;
    getDataReady(&dataReady);
    return (bool)dataReady;
}
uint8_t flxDevSTHS34PF80Base::get_AverageTObjectNumber()
{
    getAverageTObjectNumber(&avgTObjectNum);
    return (uint8_t)avgTObjectNum;
}
void flxDevSTHS34PF80Base::set_AverageTObjectNumber(uint8_t avg)
{
    if (_begun)
        setAverageTObjectNumber(avg);
}
uint8_t flxDevSTHS34PF80Base::get_AverageTAmbientNumber()
{
    getAverageTAmbientNumber(&avgTAmbientNum);
    return (uint8_t)avgTAmbientNum;
}
void flxDevSTHS34PF80Base::set_AverageTAmbientNumber(uint8_t avg)
{
    if (_begun)
        setAverageTAmbientNumber(avg);
}
uint8_t flxDevSTHS34PF80Base::get_GainMode()
{
    getGainMode(&gainMode);
    return (uint8_t)gainMode;
}
void flxDevSTHS34PF80Base::set_GainMode(uint8_t gain)
{
    if(_begun)
        setGainMode(gain);
}
float flxDevSTHS34PF80Base::get_TmosSensitivity()
{
    float sense;
    getTmosSensitivity(&sense);
    return (float)sense;
}
void flxDevSTHS34PF80Base::set_TmosSensitivity(float sense)
{
    if(_begun)
        setTmosSensitivity(sense);
}
uint8_t flxDevSTHS34PF80Base::get_TmosODR()
{
    getTmosODR(&tmosODR);
    return (uint8_t)tmosODR;
}
void flxDevSTHS34PF80Base::set_TmosODR(uint8_t odr)
{
    if(_begun)
        setTmosODR(odr);
}
bool flxDevSTHS34PF80Base::get_BlockDataUpdate()
{
    bool update;
    getBlockDataUpdate(&update);
    return (bool)update;
}
void flxDevSTHS34PF80Base::set_BlockDataUpdate(bool update)
{
    if(_begun)
        setBlockDataUpdate(update);
}
uint8_t flxDevSTHS34PF80Base::get_TmosOneShot()
{
    getTmosOneShot(&tmosOneShot);
    return (uint8_t)tmosOneShot;
}
void flxDevSTHS34PF80Base::set_TmosOneShot(uint8_t shot)
{
    if(_begun)
        setTmosOneShot(shot);
}
uint8_t flxDevSTHS34PF80Base::get_MemoryBank()
{
    getMemoryBank(memBank);
    return (uint8_t)memBank;
}
void flxDevSTHS34PF80Base::set_MemoryBank(uint8_t bank)
{
    if(_begun)
        setMemoryBank(bank);
}
uint8_t flxDevSTHS34PF80Base::get_BootOTP()
{
    uint8_t boot;
    getBootOTP(&boot);
    return (uint8_t)boot;
}
void flxDevSTHS34PF80Base::set_BootOTP(uint8_t boot)
{
    if(_begun)
        setBootOTP(boot);
}
uint8_t flxDevSTHS34PF80Base::get_TmosFunctionStatus()
{
    sths34pf80_tmos_func_status_t status;
    getTmosFunctionStatus(&status);
    return (uint8_t)status;
}
uint8_t flxDevSTHS34PF80Base::get_LpfMotionBandwidth()
{
    getLpfMotionBandwidth(&lpfMotionBandwidth);
    return lpfMotionBandwidth;
}
void flxDevSTHS34PF80Base::set_LpfMotionBandwidth(uint8_t band)
{
    if(_begun)
        setLpfMotionBandwidth(band);
}
uint8_t flxDevSTHS34PF80Base::get_LpfPresenceMotionBandwidth()
{
    getLpfPresenceMotionBandwidth(&lpfPresenceMotionBandwidth);
    return lpfPresenceMotionBandwidth;
}
void flxDevSTHS34PF80Base::set_LpfPresenceMotionBandwidth(uint8_t band)
{
    if(_begun)
        setLpfPresenceMotionBandwidth(band);
}
uint8_t flxDevSTHS34PF80Base::get_LpfAmbientTempBandwidth()
{
    getLpfAmbientTempBandwidth(&lpfTempBandwidth);
    return lpfTempBandwidth;
}
void flxDevSTHS34PF80Base::set_LpfAmbientTempBandwidth(uint8_t band)
{
    if(_begun)
        setLpfAmbientTempBandwidth(band);
}
uint8_t flxDevSTHS34PF80Base::get_LpfPresenceBandwidth()
{
    getLpfPresenceBandwidth(&lpfPresenceBandwidth);
    return lpfPresenceBandwidth;
}
void flxDevSTHS34PF80Base::set_LpfPresenceBandwidth(uint8_t band)
{
    if(_begun)
        setLpfPresenceBandwidth(band);
}
uint8_t flxDevSTHS34PF80Base::get_TmosRouteInterrupt()
{
    getTmosRouteInterrupt(&tmosInterrupt);
    return tmosInterrupt;
}
void flxDevSTHS34PF80Base::set_TmosRouteInterrupt(uint8_t int)
{
    if(_begun)
        setTmosRouteInterrupt(int);
}
uint8_t flxDevSTHS34PF80Base::get_DataReadyMode()
{
    getDataReadyMode(&dataReady);
    return dataReady;
}
void flxDevSTHS34PF80Base::set_DataReadyMode(uint8_t mode)
{
    if(_begun)
        setDataReadyMode(mode);
}

// GETTER and SETTER methods for device properties for embedded functions
uint16_t flxDevSTHS34PF80Base::get_PresenceThreshold()
{
    uint16_t threshold;
    getPresenceThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80Base::set_PresenceThreshold(uint16_t threshold)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setPresenceThreshold(threshold);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80Base::get_MotionThreshold()
{
    uint16_t threshold;
    getMotionThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80Base::set_MotionThreshold(uint16_t threshold)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setMotionThreshold(threshold);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80Base::get_TAmbientShockThreshold()
{
    uint16_t threshold;
    getTAmbientShockThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80Base::set_TAmbientShockThreshold(uint16_t threshold)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setTAmbientShockThreshold(threshold);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80Base::get_MotionHysteresis()
{
    uint8_t hysteresis;
    getMotionHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80Base::set_MotionHysteresis(uint8_t hysteresis)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setTAmbientShockThreshold(hysteresis);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80Base::get_PresenceHysteresis()
{
    uint8_t hysteresis;
    getPresenceHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80Base::set_PresenceHysteresis(uint8_t hysteresis)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setPresenceHysteresis(hysteresis);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80Base::get_TAmbientShockHysteresis()
{
    uint8_t hysteresis;
    getTAmbientShockHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80Base::set_TAmbientShockHysteresis(uint16_t hysteresis)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setTAmbientShockHysteresis(hysteresis);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80Base::get_TObjectAlgoCompensation()
{
    uint8_t compensation;
    getTobjectAlgoCompensation(&compensation);
    return compensation;
}
void flxDevSTHS34PF80Base::set_TObjectAlgoCompensation(uint8_t compensation)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setTobjectAlgoCompensation(compensation);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80Base::get_PresenceAbsoluteValue()
{
    uint8_t value;
    getPresenceAbsValue(&value);
    return value;
}
void flxDevSTHS34PF80Base::set_PresenceAbsoluteValue(uint8_t value)
{
    if(_begun)
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
        setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
        setPresenceAbsValue(value);
        setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
        setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}

//----------------------------------------------------------------------------------------------------------
// I2C Version of the driver
//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevSTHS34PF80::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t devID = 0;
    bool couldBeSTHS340 = i2cDriver.readRegister(address, STHS34PF80_WHO_AM_I, &devID);
    couldBeSTHS340 &= devID == STHS34PF80_ID;
    return couldBeSTHS340;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSTHS34PF80::onInitialize(TwoWire &wirePort)
{
    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_i2cBus, address());

    // Give the I2C port provided by the user to the I2C bus class.
    _i2cBus.init(wirePort, true);

    // Initialize the system -
    bool result = this->STHS34PF80_I2C::init();

    if (result)
        result = flxDevSTHS34PF80Base::onInitialize();
    else
        flxLog_E("STHS34PF80 onInitialize: device did not begin");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// SPI Version of the driver
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSTHS34PF80_SPI::onInitialize(SPIClass &spiPort)
{

    // We're using the lower level driver of this devices driver, which
    // allows the use of a common base class. But we need to tweak how we
    // setup the device

    setCommunicationBus(_spiBus);

    SPISettings spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);

    _spiBus.init(spiPort, spiSettings, chipSelect(), true);

    bool result = this->STHS34PF80_SPI::init();
    if (result)
        result = flxDevSTHS34PF80Base::onInitialize();
    else
        flxLog_E("STHS34PF80 SPI onInitialize: device did not begin");

    return result;
}