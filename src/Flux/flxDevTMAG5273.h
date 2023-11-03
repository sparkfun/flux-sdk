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
 *  flxDevTMAG5273.h 
 *
 *  Spark Device object for the STMicroelectronics TMAG5273
 *  low-power linear 3D Hall-effect sensor device. 
 *
 *
 */
#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFun_TMAG5273_Arduino_Library.h"

#define kTMAG5273DeviceName "TMAG5273"

//-----------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic library
class flxDevTMAG5273Base : public flxDevice, public TMAG5273_I2C
{
    public:
        flxDevTMAG5273Base();

    private:

        // Methods used to get values for our output parameters
        float read_Temperature();
        float read_XData();
        float read_YData();
        float read_ZData();
        float read_AngleResult();
        float read_MagnitudeResult();

        // Methods used to get and set values for our RW properties
        void set_CRCMode(uint8_t);
        uint8_t get_CRCMode();
        void set_MagTempCoefficient(uint8_t);
        uint8_t get_MagTempCoefficient();
        void set_ConvAvg(uint8_t);
        uint8_t get_ConvAvg();
        void set_ReadMode(uint8_t);
        uint8_t get_ReadMode();
        void set_IntThreshold(uint8_t);
        uint8_t get_IntThreshold();
        void set_LowPower(uint8_t);
        uint8_t get_LowPower();
        void set_GlitchFilter(uint8_t);
        uint8_t get_GlitchFilter();
        void set_TriggerMode(uint8_t);
        uint8_t get_TriggerMode();
        void set_OperatingMode(uint8_t);
        uint8_t get_OperatingMode();
        void set_MagneticChannel(uint8_t);
        uint8_t get_MagneticChannel();
        void set_Sleeptime(uint8_t);
        uint8_t get_Sleeptime();
        void set_ThresholdCross(uint8_t);
        uint8_t get_ThresholdCross();
        void set_MagDirection(uint8_t);
        uint8_t get_MagDirection();
        void set_MagChannelSelect(uint8_t);
        uint8_t get_MagChannelSelect();
        void set_MagnitudeGain(float);
        uint8_t get_MagnitudeGain();
        void set_MagneticOffset1(float);
        int8_t get_MagneticOffset1();
        void set_MagneticOffset2(float);
        int8_t get_MagneticOffset2();
        void set_AngleEnable(uint8_t);
        uint8_t get_AngleEnable();
        void set_XYAxisRange(uint8_t);
        uint8_t get_XYAxisRange();
        void set_ZAxisRange(uint8_t);
        uint8_t get_ZAxisRange();
        void set_XThreshold(float);
        float get_XThreshold();
        void set_YThreshold(float);
        float get_YThreshold();
        void set_ZThreshold(float);
        float get_ZThreshold();
        void set_TemperatureThreshold(float);
        float get_TemparatureThreshold();
        void set_TemperatureEnable(bool);
        uint8_t get_TemperatureEnable();
        void set_InterruptResult(bool);
        uint8_t get_InterruptResult();
        void set_ThresholdEnable(bool);
        uint8_t get_ThresholdEnable();
        void set_InterruptPinState(bool);
        uint8_t get_InterruptPinState();
        void set_InterruptPinMode(uint8_t);
        uint8_t get_InterruptPinMode();
        void set_MaskInterrupt(bool);        
        uint8_t get_MaskInterrupt();
        void set_I2CAddressEnable(bool);
        uint8_t get_I2CAddressEnable();
        void set_OscillatorError(bool);
        uint8_t get_OscillatorError();
        void set_POR(bool);
        uint8_t get_POR();
        void set_I2CAddress(uint8_t);
        uint8_t get_I2CAddress();

        uint8_t get_SetCount();
        uint8_t get_DiagStatus();
        uint8_t get_ResultStatus();
        uint8_t get_DeviceID();
        uint8_t get_ManufacturerID();
        uint8_t get_InterruptPinStatus();
        uint8_t get_DeviceStatus();
        int8_t get_Error();

        bool _begun = false;


    public:
        // Define our output parameters - specify the get functions to call.
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_XData> xData;
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_YData> yData;
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_ZData> zData;
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_Temperature> tempData;
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_AngleResult> angleResult;
        flxParameterOutFloat<flxDevTMAG5273Base, &flxDevTMAG5273Base::get_MagnitudeResult> magnitudeData;

        // Define our read-write properties
        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_CRCMode, &flxDevTMAG5273::set_CRCMode> 
         crcEnable = {TMAG5273_CRC_ENABLE , 
                        {{"CRC Disable", TMAG5273_CRC_DISABLE},
                        {"CRC Enable", TMAG5273_CRC_ENABLE}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_ConvAvg, &flxDevTMAG5273::set_ConvAvg> 
         convAvg = {TMAG5273_X1_CONVERSION , 
                        {{"x1", TMAG5273_X1_CONVERSION},
                        {"x2", TMAG5273_X2_CONVERSION},
                        {"x4", TMAG5273_X4_CONVERSION},
                        {"x8", TMAG5273_X8_CONVERSION},
                        {"x16", TMAG5273_X16_CONVERSION},
                        {"x32", TMAG5273_X32_CONVERSION}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_OperatingMode, &flxDevTMAG5273::set_OperatingMode> 
         opMode = {TMAG5273_CONTINUOUS_MEASURE_MODE , 
                        {{"Standy-by", TMAG5273_STANDY_BY_MODE},
                        {"Sleep", TMAG5273_SLEEP_MODE},
                        {"Continuous Measure", TMAG5273_CONTINUOUS_MEASURE_MODE},
                        {"Wake-Up and Sleep", TMAG5273_WAKE_UP_AND_SLEEP_MODE}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_MagneticChannel, &flxDevTMAG5273::set_MagneticChannel> 
         magEnable = {TMAG5273_X_Y_Z_ENABLE , 
                        {{"Off", TMAG5273_CHANNELS_OFF},
                        {"X Enabled", TMAG5273_X_ENABLE},
                        {"Y Enabled", TMAG5273_Y_ENABLE},
                        {"X, Y Enabled", TMAG5273_X_Y_ENABLE},
                        {"Z Enabled", TMAG5273_Z_ENABLE},
                        {"Z, X Enabled", TMAG5273_Z_X_ENABLE},
                        {"Y, Z Enabled", TMAG5273_Y_Z_ENABLE},
                        {"X, Y, Z Enabled", TMAG5273_X_Y_Z_ENABLE},
                        {"XYX Enabled", TMAG5273_XYX_ENABLE},
                        {"YXY Enabled", TMAG5273_YXY_ENABLE},
                        {"YZY Enabled", TMAG5273_YZY_ENABLE},
                        {"XZX Enabled", TMAG5273_XZX_ENABLE}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_Sleeptime, &flxDevTMAG5273::set_Sleeptime> 
         sleeptime = {TMAG5273_SLEEP_30MS , 
                        {{"1 ms", TMAG5273_SLEEP_1MS},
                        {"5 ms", TMAG5273_SLEEP_5MS},
                        {"10 ms", TMAG5273_SLEEP_10MS},
                        {"15 ms", TMAG5273_SLEEP_15MS},
                        {"20 ms", TMAG5273_SLEEP_20MS},
                        {"30 ms", TMAG5273_SLEEP_30MS},
                        {"50 ms", TMAG5273_SLEEP_50MS},
                        {"100 ms", TMAG5273_SLEEP_100MS},
                        {"500 ms", TMAG5273_SLEEP_500MS},
                        {"1000 ms", TMAG5273_SLEEP_1000MS},
                        {"2000 ms", TMAG5273_SLEEP_2000MS},
                        {"5000 ms", TMAG5273_SLEEP_5000MS},
                        {"20000 ms", TMAG5273_SLEEP_20000MS}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_ThresholdCross, &flxDevTMAG5273set_ThresholdCross> 
         numThreshold = {TMAG5273_THRESHOLD_1 , 
                        {{"1 Threshold", TMAG5273_THRESHOLD_1},
                        {"4 Thresholds", TMAG5273_THRESHOLD_4}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_MagDirection, &flxDevTMAG5273::set_MagDirection> 
         thresholdCross = {TMAG5273_THRESHOLD_INT_ABOVE , 
                        {{"Int Above Threshold", TMAG5273_THRESHOLD_INT_ABOVE},
                        {"Int Below Threshold", TMAG5273_THRESHOLD_INT_BELOW}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_MagnitudeChannelSelect, &flxDevTMAG5273::set_MagnitudeGain> 
         channelAdjust = {TMAG5273_GAIN_ADJUST_CHANNEL_1 , 
                        {{"Adjust Channel 1", TMAG5273_GAIN_ADJUST_CHANNEL_1},
                        {"Adjust Channel 2", TMAG5273_GAIN_ADJUST_CHANNEL_2}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_AngleEnable, &flxDevTMAG5273::set_AngleEnable> 
         angleCalc = {TMAG5273_NO_ANGLE_CALCULATION , 
                        {{"No Angle", TMAG5273_NO_ANGLE_CALCULATION},
                        {"XY Angle", TMAG5273_XY_ANGLE_CALCULATION},
                        {"YZ Angle", TMAG5273_YZ_ANGLE_CALCULATION},
                        {"XZ Angle", TMAG5273_XZ_ANGLE_CALCULATION}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_XYAxisRange, &flxDevTMAG5273::set_XYAxisRange> 
         xyRange = {TMAG5273_RANGE_80MT , 
                        {{"40mT", TMAG5273_RANGE_40MT},
                        {"80mT", TMAG5273_RANGE_80MT}}};
        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_ZAxisRange, &flxDevTMAG5273::set_ZAxisRange> 
         zRange = {TMAG5273_RANGE_80MT , 
                        {{"40mT", TMAG5273_RANGE_40MT},
                        {"80mT", TMAG5273_RANGE_80MT}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_TemperatureEnable, &flxDevTMAG5273::set_TemperatureEnable> 
         tempEnable = {TMAG5273_TEMPERATURE_ENABLE , 
                        {{"Temperature Disabled", TMAG5273_TEMPERATURE_DISABLE},
                        {"Temperature Enabled", TMAG5273_TEMPERATURE_ENABLE}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_InterruptResult, &flxDevTMAG5273::set_InterruptResult> 
         intAsserted = {TMAG5273_INTERRUPT_NOT_ASSERTED , 
                        {{"Interrupt not asserted", TMAG5273_INTERRUPT_NOT_ASSERTED},
                        {"Interrupt asserted", TMAG5273_INTERRUPT_ASSERTED}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_InterruptPinMode, &flxDevTMAG5273::set_InterruptPinMode> 
         intPinState = {TMAG5273_NO_ANGLE_CALCULATION , 
                        {{"No Interrupt", TMAG5273_NO_INTERRUPT},
                        {"Interrupt thru INT", TMAG5273_INTERRUPT_THROUGH_INT},
                        {"Interrupt thru INT I2C", TMAG5273_INTERRUPT_THROUGH_INT_I2C},
                        {"Interrupt thru SCL", TMAG5273_INTERRUPT_THROUGH_SCL},
                        {"Interrupt thru SCL I2C", TMAG5273_INTERRUPT_THROUGH_SCL_I2C}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_InterruptPinState, &flxDevTMAG5273::set_InterruptPinState> 
         intPinEnable = {TMAG5273_INTERRUPT_NOT_ASSERTED , 
                        {{"40mT", TMAG5273_INTERRUPT_NOT_ASSERTED},
                        {"80mT", TMAG5273_INTERRUPT_ASSERTED}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_I2CAddressEnable, &flxDevTMAG5273::set_I2CAddressEnable> 
         i2cAddressEn = {TMAG5273_I2C_ADDRESS_CHANGE_DISABLE , 
                        {{"Disable I2C Address Change", TMAG5273_I2C_ADDRESS_CHANGE_DISABLE},
                        {"Enable I2C Address Change", TMAG5273_I2C_ADDRESS_CHANGE_ENABLE}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_POR, &flxDevTMAG5273::set_POR> 
         porEn = {TMAG5273_NO_POR , 
                        {{"No POR", TMAG5273_NO_POR},
                        {"POR Occur", TMAG5273_YES_POR}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::get_OscillatorError, &flxDevTMAG5273::set_OscillatorError> 
         oscError = {TMAG5273_OSCILLATOR_ERROR_UNDETECTED , 
                        {{"OSC error undetected", TMAG5273_OSCILLATOR_ERROR_UNDETECTED},
                        {"OSC error detected", TMAG5273_OSCILLATOR_ERROR_DETECTED}}};

        flxPropertyRWUint<flxDevTMAG5273, &flxDevTMAG5273::/*get*/, &flxDevTMAG5273::/*set*/> 
         intError = {TMAG5273_INT_ERROR_UNDETECTED , 
                        {{"Interrupt Error Undetected", TMAG5273_INT_ERROR_UNDETECTED},
                        {"Interrupt Error Detected", TMAG5273_INT_ERROR_DETECTED}}};


    protected: 
        // Base method called to initialize the class


};

//----------------------------------------------------------------------------------------------------------
// I2C version of the driver
//----------------------------------------------------------------------------------------------------------
//
// Define our I2C version of the class. NOTE: We add our base class to the template call
class flxDevTMAG5273 : public flxDeviceI2CType<flxDevTMAG5273, flxDevTMAG5273Base>
{

    public: 
        flxDevTMAG5273(){};

        // Static Interface - used by the system to determine if this device is
        // connected before the object is instantiated.
        static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
        static const char *getDeviceName()
        {
            return kTMAG5273DeviceName;
        };

        static const uint8_t *getDefaultAddresses()
        {
            return defaultDeviceAddress();
        }
        // holds the class list of possible addresses/IDs for this objects
        static uint8_t defaultDeviceAddress[];

        // Method called to initialize the class
        bool onInitialize(TwoWire &);

    private: 
        // The low-level driver I2C bus class
        sfe_TMAG5273::SFE_BusI2C _i2cBus; //QwI2C
};
