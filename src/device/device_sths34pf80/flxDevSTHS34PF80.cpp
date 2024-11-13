/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
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

#include "SparkFun_STHS34PF80_Arduino_Library.h"
#include <Arduino.h>

#include "flxDevSTHS34PF80.h"

// Define our class static variables - allocs storage for them
uint8_t flxDevSTHS34PF80::defaultDeviceAddress[] = {STHS34PF80_I2C_ADDRESS, kSparkDeviceAddressNull};

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
    flxRegister(presence, "Presence (cm^-1)", "Presence Value (1/cm)", kParamValuePresence);
    flxRegister(motion, "Motion (LSB)", "Motion Value", kParamValueMotion);
    flxRegister(temperature, "Temperature (C)", "Temperature Value (C)");

    // Register Parameters
    flxRegister(avgTObjectNum, "Avg Samples for TObject", "Number of Average Samples for TObject");
    flxRegister(avgTAmbientNum, "Avg Samples for TAmbient", "Number of Average Samples for TAmbient");
    flxRegister(gainMode, "Enable Gain Mode", "Enables the Gain Mode");
    flxRegister(tmosODR, "Output Data Rate", "Output Data Rate");
    flxRegister(tmosOneShot, "One Shot", "Trigger One-Shot Acquisition");
    flxRegister(memBank, "Memory Bank", "Enable Access to Embedded Functions");

    flxRegister(lpfMotionBandwidth, "Low Pass Motion Filter", "Low Pass Motion Filter Bandwidth");
    flxRegister(lpfPresenceMotionBandwidth, "Low Pass Presence/Motion Filter",
                "Low Pass Presence and Motion Filter Bandwidth");
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
bool flxDevSTHS34PF80::onInitialize(TwoWire &wirePort)
{
    return STHS34PF80_I2C::begin(address(), wirePort);
}

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

// GETTER methods for output params
int16_t flxDevSTHS34PF80::read_Presence()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if (drdy == 1)
    {
        int16_t presenceValue;
        getPresenceValue(&presenceValue);
        return presenceValue;
    }
    return 0;
}
int16_t flxDevSTHS34PF80::read_Motion()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if (drdy == 1)
    {
        int16_t value;
        getMotionValue(&value);
        return value;
    }
    return 0;
}
float flxDevSTHS34PF80::read_TemperatureC()
{
    // Check if data is ready for device, then return presence value
    bool drdy = get_DataReady();
    if (drdy == 1)
    {
        float value;
        getTemperatureData(&value);
    }
    return 0;
}

// GETTER and SETTER methods for device properties

bool flxDevSTHS34PF80::get_DataReady()
{
    sths34pf80_tmos_drdy_status_t dataReady;
    getDataReady(&dataReady);
    return dataReady.drdy == 1;
}

int32_t flxDevSTHS34PF80::get_AverageTObjectNumber()
{
    sths34pf80_avg_tobject_num_t value;
    getAverageTObjectNumber(&value);
    return (int)value;
}
void flxDevSTHS34PF80::set_AverageTObjectNumber(int32_t avg)
{
    if (isInitialized())
        setAverageTObjectNumber((sths34pf80_avg_tobject_num_t)avg);
}

int32_t flxDevSTHS34PF80::get_AverageTAmbientNumber()
{
    sths34pf80_avg_tambient_num_t value;
    getAverageTAmbientNumber(&value);
    return (int)value;
}

void flxDevSTHS34PF80::set_AverageTAmbientNumber(int32_t avg)
{
    if (isInitialized())
        setAverageTAmbientNumber((sths34pf80_avg_tambient_num_t)avg);
}

int32_t flxDevSTHS34PF80::get_GainMode()
{
    sths34pf80_gain_mode_t value;
    getGainMode(&value);
    return (int)value;
}

void flxDevSTHS34PF80::set_GainMode(int32_t gain)
{
    if (isInitialized())
        setGainMode((sths34pf80_gain_mode_t)gain);
}

float flxDevSTHS34PF80::get_TmosSensitivity()
{
    float sense;
    getTmosSensitivity(&sense);
    return sense;
}

void flxDevSTHS34PF80::set_TmosSensitivity(float sense)
{
    if (isInitialized())
        setTmosSensitivity(sense);
}

int32_t flxDevSTHS34PF80::get_TmosODR()
{
    sths34pf80_tmos_odr_t value;
    getTmosODR(&value);
    return (int)value;
}

void flxDevSTHS34PF80::set_TmosODR(int32_t odr)
{
    if (isInitialized())
        setTmosODR((sths34pf80_tmos_odr_t)odr);
}

bool flxDevSTHS34PF80::get_BlockDataUpdate()
{
    bool update;
    getBlockDataUpdate(&update);
    return update;
}

void flxDevSTHS34PF80::set_BlockDataUpdate(bool update)
{
    if (isInitialized())
        setBlockDataUpdate(update);
}

int32_t flxDevSTHS34PF80::get_TmosOneShot()
{
    sths34pf80_tmos_one_shot_t value;

    getTmosOneShot(&value);
    return value;
}

void flxDevSTHS34PF80::set_TmosOneShot(int32_t shot)
{
    if (isInitialized())
        setTmosOneShot((sths34pf80_tmos_one_shot_t)shot);
}

int32_t flxDevSTHS34PF80::get_MemoryBank()
{
    sths34pf80_mem_bank_t value;
    getMemoryBank(&value);
    return value;
}

void flxDevSTHS34PF80::set_MemoryBank(int32_t bank)
{
    if (isInitialized())
        setMemoryBank((sths34pf80_mem_bank_t)bank);
}

uint8_t flxDevSTHS34PF80::get_BootOTP()
{
    uint8_t boot;
    getBootOTP(&boot);
    return boot;
}
void flxDevSTHS34PF80::set_BootOTP(uint8_t boot)
{
    if (isInitialized())
        setBootOTP(boot);
}

// KDB - Is this needed?
// int flxDevSTHS34PF80::get_TmosFunctionStatus()
// {
//     sths34pf80_tmos_func_status_t status;
//     getTmosFunctionStatus(&status);
//     return (int)status;
// }

int32_t flxDevSTHS34PF80::get_LpfMotionBandwidth()
{
    sths34pf80_lpf_bandwidth_t value;
    getLpfMotionBandwidth(&value);
    return (int)value;
}

void flxDevSTHS34PF80::set_LpfMotionBandwidth(int32_t band)
{
    if (isInitialized())
        setLpfMotionBandwidth((sths34pf80_lpf_bandwidth_t)band);
}

int32_t flxDevSTHS34PF80::get_LpfPresenceMotionBandwidth()
{
    sths34pf80_lpf_bandwidth_t value;
    getLpfPresenceMotionBandwidth(&value);
    return (int)value;
}
void flxDevSTHS34PF80::set_LpfPresenceMotionBandwidth(int32_t band)
{
    if (isInitialized())
        setLpfPresenceMotionBandwidth((sths34pf80_lpf_bandwidth_t)band);
}

int32_t flxDevSTHS34PF80::get_LpfAmbientTempBandwidth()
{
    sths34pf80_lpf_bandwidth_t value;
    getLpfAmbientTempBandwidth(&value);
    return value;
}

void flxDevSTHS34PF80::set_LpfAmbientTempBandwidth(int32_t band)
{
    if (isInitialized())
        setLpfAmbientTempBandwidth((sths34pf80_lpf_bandwidth_t)band);
}

int32_t flxDevSTHS34PF80::get_LpfPresenceBandwidth()
{
    sths34pf80_lpf_bandwidth_t value;
    getLpfPresenceBandwidth(&value);
    return (int)value;
}
void flxDevSTHS34PF80::set_LpfPresenceBandwidth(int32_t band)
{
    if (isInitialized())
        setLpfPresenceBandwidth((sths34pf80_lpf_bandwidth_t)band);
}

int32_t flxDevSTHS34PF80::get_TmosRouteInterrupt()
{
    sths34pf80_tmos_route_int_t value;
    getTmosRouteInterrupt(&value);
    return value;
}

void flxDevSTHS34PF80::set_TmosRouteInterrupt(int32_t intr)
{
    if (isInitialized())
        setTmosRouteInterrupt((sths34pf80_tmos_route_int_t)intr);
}

int32_t flxDevSTHS34PF80::get_DataReadyMode()
{
    sths34pf80_drdy_mode_t value;
    getDataReadyMode(&value);
    return value;
}

void flxDevSTHS34PF80::set_DataReadyMode(int32_t mode)
{
    if (isInitialized())
        setDataReadyMode((sths34pf80_drdy_mode_t)mode);
}

// GETTER and SETTER methods for device properties for embedded functions
uint16_t flxDevSTHS34PF80::get_PresenceThreshold()
{
    uint16_t threshold;
    getPresenceThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80::set_PresenceThreshold(uint16_t threshold)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setPresenceThreshold(threshold);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80::get_MotionThreshold()
{
    uint16_t threshold;
    getMotionThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80::set_MotionThreshold(uint16_t threshold)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setMotionThreshold(threshold);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint16_t flxDevSTHS34PF80::get_TAmbientShockThreshold()
{
    uint16_t threshold;
    getTAmbientShockThreshold(&threshold);
    return threshold;
}
void flxDevSTHS34PF80::set_TAmbientShockThreshold(uint16_t threshold)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setTAmbientShockThreshold(threshold);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80::get_MotionHysteresis()
{
    uint8_t hysteresis;
    getMotionHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80::set_MotionHysteresis(uint8_t hysteresis)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setTAmbientShockThreshold(hysteresis);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80::get_PresenceHysteresis()
{
    uint8_t hysteresis;
    getPresenceHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80::set_PresenceHysteresis(uint8_t hysteresis)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setPresenceHysteresis(hysteresis);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80::get_TAmbientShockHysteresis()
{
    uint8_t hysteresis;
    getTAmbientShockHysteresis(&hysteresis);
    return hysteresis;
}
void flxDevSTHS34PF80::set_TAmbientShockHysteresis(uint8_t hysteresis)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setTAmbientShockHysteresis(hysteresis);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80::get_TObjectAlgoCompensation()
{
    uint8_t compensation;
    getTobjectAlgoCompensation(&compensation);
    return compensation;
}
void flxDevSTHS34PF80::set_TObjectAlgoCompensation(uint8_t compensation)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setTobjectAlgoCompensation(compensation);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
uint8_t flxDevSTHS34PF80::get_PresenceAbsoluteValue()
{
    uint8_t value;
    getPresenceAbsValue(&value);
    return value;
}
void flxDevSTHS34PF80::set_PresenceAbsoluteValue(uint8_t value)
{
    if (isInitialized())
        // Steps for setting embedded functions - see Section 2.1 of Application Note AN5867
        setTmosODR(STHS34PF80_TMOS_ODR_OFF);
    setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);
    setPresenceAbsValue(value);
    setMemoryBank(STHS34PF80_MAIN_MEM_BANK);
    setTmosODR(STHS34PF80_TMOS_ODR_AT_1Hz);
}
