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
 *  flxDevSTHS34PF80.h
 *
 *  Spark Device object for the STMicroelectronics STHS34PF80
 *  human presence sensor device.
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_STHS34PF80_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device
#define kSTHS34PF80DeviceName "STHS34PF80"

//-----------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic library
class flxDevSTHS34PF80 : public flxDeviceI2CType<flxDevSTHS34PF80>, public STHS34PF80_I2C
{
  public:
    flxDevSTHS34PF80();
    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kSTHS34PF80DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

  private:
    // Methods used to get values for our output parameters
    int16_t read_Presence();
    int16_t read_Motion();
    float read_TemperatureC();

    // Methods used to get and set values for our RW properties
    bool get_DataReady();

    int32_t get_AverageTObjectNumber();
    void set_AverageTObjectNumber(int32_t);

    int32_t get_AverageTAmbientNumber();
    void set_AverageTAmbientNumber(int32_t);

    int32_t get_GainMode();
    void set_GainMode(int32_t);

    float get_TmosSensitivity();
    void set_TmosSensitivity(float);

    int32_t get_TmosODR();
    void set_TmosODR(int32_t);

    bool get_BlockDataUpdate();
    void set_BlockDataUpdate(bool);

    int32_t get_TmosOneShot();
    void set_TmosOneShot(int32_t);

    int32_t get_MemoryBank();
    void set_MemoryBank(int32_t);

    uint8_t get_BootOTP();
    void set_BootOTP(uint8_t);

    // int get_TmosFunctionStatus(); // TODO??

    int32_t get_LpfMotionBandwidth();
    void set_LpfMotionBandwidth(int32_t);

    int32_t get_LpfPresenceMotionBandwidth();
    void set_LpfPresenceMotionBandwidth(int32_t);

    int32_t get_LpfAmbientTempBandwidth();
    void set_LpfAmbientTempBandwidth(int32_t);

    int32_t get_LpfPresenceBandwidth();
    void set_LpfPresenceBandwidth(int32_t);

    int32_t get_TmosRouteInterrupt();
    void set_TmosRouteInterrupt(int32_t);

    int32_t get_DataReadyMode();
    void set_DataReadyMode(int32_t);

    // Embedded Functionality
    uint16_t get_PresenceThreshold();
    void set_PresenceThreshold(uint16_t);

    uint16_t get_MotionThreshold();
    void set_MotionThreshold(uint16_t);

    uint16_t get_TAmbientShockThreshold();
    void set_TAmbientShockThreshold(uint16_t);

    uint8_t get_MotionHysteresis();
    void set_MotionHysteresis(uint8_t);

    uint8_t get_PresenceHysteresis();
    void set_PresenceHysteresis(uint8_t);

    uint8_t get_TAmbientShockHysteresis();
    void set_TAmbientShockHysteresis(uint8_t);

    uint8_t get_TObjectAlgoCompensation();
    void set_TObjectAlgoCompensation(uint8_t);

    uint8_t get_PresenceAbsoluteValue();
    void set_PresenceAbsoluteValue(uint8_t);

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutInt16<flxDevSTHS34PF80, &flxDevSTHS34PF80::read_Presence> presence;
    flxParameterOutInt16<flxDevSTHS34PF80, &flxDevSTHS34PF80::read_Motion> motion;
    flxParameterOutFloat<flxDevSTHS34PF80, &flxDevSTHS34PF80::read_TemperatureC> temperature;

    // Define our read-write properties
    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_AverageTObjectNumber,
                       &flxDevSTHS34PF80::set_AverageTObjectNumber>
        avgTObjectNum = {STHS34PF80_AVG_TMOS_8,
                         {{"8x", STHS34PF80_AVG_TMOS_8},
                          {"4x", STHS34PF80_AVG_TMOS_32},
                          {"2x", STHS34PF80_AVG_TMOS_128},
                          {"2x", STHS34PF80_AVG_TMOS_256},
                          {"2x", STHS34PF80_AVG_TMOS_512},
                          {"2x", STHS34PF80_AVG_TMOS_1024},
                          {"2x", STHS34PF80_AVG_TMOS_2048},
                          {"1x", STHS34PF80_AVG_T_1}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_AverageTAmbientNumber,
                       &flxDevSTHS34PF80::set_AverageTAmbientNumber>
        avgTAmbientNum = {STHS34PF80_AVG_T_1,
                          {{"8x", STHS34PF80_AVG_T_8},
                           {"4x", STHS34PF80_AVG_T_4},
                           {"2x", STHS34PF80_AVG_T_2},
                           {"1x", STHS34PF80_AVG_T_1}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_GainMode, &flxDevSTHS34PF80::set_GainMode> gainMode = {
        STHS34PF80_GAIN_DEFAULT_MODE, {{"Wide", STHS34PF80_GAIN_WIDE_MODE}, {"Default", STHS34PF80_GAIN_DEFAULT_MODE}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_TmosODR, &flxDevSTHS34PF80::set_TmosODR> tmosODR = {
        STHS34PF80_TMOS_ODR_AT_1Hz,
        {{"Off", STHS34PF80_TMOS_ODR_OFF},
         {"0.25Hz", STHS34PF80_TMOS_ODR_AT_0Hz25},
         {"0.50Hz", STHS34PF80_TMOS_ODR_AT_0Hz50},
         {"1Hz", STHS34PF80_TMOS_ODR_AT_1Hz},
         {"2Hz", STHS34PF80_TMOS_ODR_AT_2Hz},
         {"4Hz", STHS34PF80_TMOS_ODR_AT_4Hz},
         {"8Hz", STHS34PF80_TMOS_ODR_AT_8Hz},
         {"15Hz", STHS34PF80_TMOS_ODR_AT_15Hz},
         {"30Hz", STHS34PF80_TMOS_ODR_AT_30Hz}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_TmosOneShot, &flxDevSTHS34PF80::set_TmosOneShot>
        tmosOneShot = {STHS34PF80_TMOS_IDLE_MODE,
                       {{"Idle", STHS34PF80_TMOS_IDLE_MODE}, {"One Shot", STHS34PF80_TMOS_ONE_SHOT}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_MemoryBank, &flxDevSTHS34PF80::set_MemoryBank> memBank =
        {STHS34PF80_MAIN_MEM_BANK,
         {{"Main Memory", STHS34PF80_MAIN_MEM_BANK}, {"Embedded Function Memory", STHS34PF80_EMBED_FUNC_MEM_BANK}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_LpfMotionBandwidth,
                       &flxDevSTHS34PF80::set_LpfMotionBandwidth>
        lpfMotionBandwidth = {STHS34PF80_LPF_ODR_DIV_9,
                              {{"9", STHS34PF80_LPF_ODR_DIV_9},
                               {"20", STHS34PF80_LPF_ODR_DIV_20},
                               {"50", STHS34PF80_LPF_ODR_DIV_50},
                               {"100", STHS34PF80_LPF_ODR_DIV_100},
                               {"200", STHS34PF80_LPF_ODR_DIV_200},
                               {"400", STHS34PF80_LPF_ODR_DIV_400},
                               {"800", STHS34PF80_LPF_ODR_DIV_800}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_LpfPresenceMotionBandwidth,
                       &flxDevSTHS34PF80::set_LpfPresenceMotionBandwidth>
        lpfPresenceMotionBandwidth = {STHS34PF80_LPF_ODR_DIV_9,
                                      {{"9", STHS34PF80_LPF_ODR_DIV_9},
                                       {"20", STHS34PF80_LPF_ODR_DIV_20},
                                       {"50", STHS34PF80_LPF_ODR_DIV_50},
                                       {"100", STHS34PF80_LPF_ODR_DIV_100},
                                       {"200", STHS34PF80_LPF_ODR_DIV_200},
                                       {"400", STHS34PF80_LPF_ODR_DIV_400},
                                       {"800", STHS34PF80_LPF_ODR_DIV_800}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_LpfAmbientTempBandwidth,
                       &flxDevSTHS34PF80::set_LpfAmbientTempBandwidth>
        lpfTempBandwidth = {STHS34PF80_LPF_ODR_DIV_9,
                            {{"9", STHS34PF80_LPF_ODR_DIV_9},
                             {"20", STHS34PF80_LPF_ODR_DIV_20},
                             {"50", STHS34PF80_LPF_ODR_DIV_50},
                             {"100", STHS34PF80_LPF_ODR_DIV_100},
                             {"200", STHS34PF80_LPF_ODR_DIV_200},
                             {"400", STHS34PF80_LPF_ODR_DIV_400},
                             {"800", STHS34PF80_LPF_ODR_DIV_800}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_LpfPresenceBandwidth,
                       &flxDevSTHS34PF80::set_LpfPresenceBandwidth>
        lpfPresenceBandwidth = {STHS34PF80_LPF_ODR_DIV_9,
                                {{"9", STHS34PF80_LPF_ODR_DIV_9},
                                 {"20", STHS34PF80_LPF_ODR_DIV_20},
                                 {"50", STHS34PF80_LPF_ODR_DIV_50},
                                 {"100", STHS34PF80_LPF_ODR_DIV_100},
                                 {"200", STHS34PF80_LPF_ODR_DIV_200},
                                 {"400", STHS34PF80_LPF_ODR_DIV_400},
                                 {"800", STHS34PF80_LPF_ODR_DIV_800}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_TmosRouteInterrupt,
                       &flxDevSTHS34PF80::set_TmosRouteInterrupt>
        tmosInterruptRoute = {STHS34PF80_TMOS_INT_OR,
                              {{"OR", STHS34PF80_TMOS_INT_OR},
                               {"Data Ready", STHS34PF80_TMOS_INT_DRDY},
                               {"Interrupt OR", STHS34PF80_TMOS_INT_OR}}};

    flxPropertyRWInt32<flxDevSTHS34PF80, &flxDevSTHS34PF80::get_DataReadyMode, &flxDevSTHS34PF80::set_DataReadyMode>
        dataReady = {STHS34PF80_DRDY_PULSED,
                     {{"OR", STHS34PF80_DRDY_PULSED}, {"Interrupt OR", STHS34PF80_DRDY_LATCHED}}};
};
