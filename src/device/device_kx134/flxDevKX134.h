/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevKX134.h
 *
 *  Spark Device object for the KX134 Qwiic device.
 */

#pragma once
#include "SparkFun_KX13X.h"
#include "flxCore.h"
#include "flxDevice.h"

#define kKX134DeviceName "KX134"

//TODO: Maybe generalize this for the KX134 and KX132 devices?
// OR: make separate classes (and possibly files) for each...
// Define our class
class flxDevKX134 : public flxDeviceI2CType<flxDevKX134>, public SfeKX134ArdI2C
{

  public:
    flxDevKX134();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kKX134DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects.
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    bool execute(void);
    
  private:
    // Constants and enums
    // TODO: should all of these be moved into the underlying base class?
    typedef enum TapType
    {
        kNoTap,      // No tap detected
        kSingleTap,  // Single tap detected
        kDoubleTap,  // Double tap detected
        kUnknownTap, // Unknown tap detected
    } tapType_t;

    // Data rate enums (threw them in enums for logical grouping)
    // because they are actually just counting up in binary,
    // their values don't really need to be explicitly assigned but
    // still are for easy mapping to the datasheet tables.

    // Output Data Rate (ODR) enum for main accelerometer data
    // See KX134 Technical Reference Manual, Table 13, page 26:
    // https://cdn.sparkfun.com/assets/4/7/f/9/9/KX134-1211-Technical-Reference-Manual-Rev-1.0.pdf
    enum outputDataRate_t : uint8_t {
        kOdr0_781Hz   = 0b0000,
        kOdr1_563Hz   = 0b0001,
        kOdr3_125Hz   = 0b0010,
        kOdr6_25Hz    = 0b0011,
        kOdr12_5Hz    = 0b0100,
        kOdr25Hz      = 0b0101,
        kOdr50Hz      = 0b0110,  // Default
        kOdr100Hz     = 0b0111,
        kOdr200Hz     = 0b1000,
        kOdr400Hz     = 0b1001,
        kOdr800Hz     = 0b1010,
        kOdr1600Hz    = 0b1011,
        kOdr3200Hz    = 0b1100,
        kOdr6400Hz    = 0b1101,
        kOdr12800Hz   = 0b1110,
        kOdr25600Hz   = 0b1111
    };

    // Tap Data Rate enum
    // See KX134 Technical Reference Manual, Table 10, page 20:
    // https://cdn.sparkfun.com/assets/4/7/f/9/9/KX134-1211-Technical-Reference-Manual-Rev-1.0.pdf
    enum tapDataRate_t : uint8_t {
        kTapOdr12_5Hz   = 0b0000,
        kTapOdr25Hz     = 0b0001,
        kTapOdr50Hz     = 0b0010,
        kTapOdr100Hz    = 0b0011,
        kTapOdr200Hz    = 0b0100,
        kTapOdr400Hz    = 0b0101, // Default
        kTapOdr800Hz    = 0b0110,
        kTapOdr1600Hz   = 0b0111
    };

    // Tilt Data Rate enum
    // See KX134 Technical Reference Manual, Table 9, page 20:
    // https://cdn.sparkfun.com/assets/4/7/f/9/9/KX134-1211-Technical-Reference-Manual-Rev-1.0.pdf
    enum tiltDataRate_t : uint8_t {
        kTiltOdr1_563Hz  = 0b0000,
        kTiltOdr6_25Hz   = 0b0001,
        kTiltOdr12_5Hz   = 0b0010, // Default
        kTiltOdr50Hz     = 0b0011
    };

    // Wake Data Rate enum
    // See KX134 Technical Reference Manual, Table 11, page 21:
    // https://cdn.sparkfun.com/assets/4/7/f/9/9/KX134-1211-Technical-Reference-Manual-Rev-1.0.pdf
    enum wakeDataRate_t : uint8_t {
        kWakeOdr0_781Hz  = 0b0000, // Default
        kWakeOdr1_563Hz  = 0b0001,
        kWakeOdr3_125Hz  = 0b0010,
        kWakeOdr6_25Hz   = 0b0011,
        kWakeOdr12_5Hz   = 0b0100,
        kWakeOdr25Hz     = 0b0101,
        kWakeOdr50Hz     = 0b0110,
        kWakeOdr100Hz    = 0b0111
    };

    // Internal members
     bool _initialized = false; // Flag to indicate if the device is initialized
    // float _lastAccelData[3] = {0.0f, 0.0f, 0.0f}; // Last accelerometer data read

//     // Enable Props
    // bool get_enable_acceleration(void);
    // void set_enable_acceleration(bool);
    // bool _enable_acceleration = false;

    // bool get_enable_data_engine(void);
    // void set_enable_data_engine(bool);
    // bool _enable_data_engine = false;

    // bool get_enable_tap_engine(void);
    // void set_enable_tap_engine(bool);
    // bool _enable_tap_engine = false; 

    // bool get_enable_tilt_engine(void);
    // void set_enable_tilt_engine(bool);
    // bool _enable_tilt_engine = false;

    // bool get_enable_sleep_engine(void);
    // void set_enable_sleep_engine(bool);
    // bool _enable_sleep_engine = false;

    // bool get_enable_wake_engine(void);
    // void set_enable_wake_engine(bool);
    // bool _enable_wake_engine = false;

    // bool get_enable_direct_tap_interrupt(void);
    // void set_enable_direct_tap_interrupt(bool);
    // bool _enable_direct_tap_interrupt = false;

    // bool get_enable_double_tap_interrupt(void);
    // void set_enable_double_tap_interrupt(bool);
    // bool _enable_double_tap_interrupt = false;

    // Other Prop getters/setters:
    // uint8_t get_range(void);
    // void set_range(uint8_t);
    // uint8_t _range = SFE_KX134_RANGE16G;

    // uint8_t get_output_data_rate(void);
    // void set_output_data_rate(uint8_t);
    // uint8_t _output_data_rate = kOdr50Hz;

    // uint8_t get_tap_data_rate(void);
    // void set_tap_data_rate(uint8_t);
    // uint8_t _tap_data_rate = kTapOdr400Hz;

    // uint8_t get_tilt_data_rate(void);
    // void set_tilt_data_rate(uint8_t);
    // uint8_t _tilt_data_rate = kTiltOdr12_5Hz;

    // uint8_t get_wake_data_rate(void);
    // void set_wake_data_rate(uint8_t);
    // uint8_t _wake_data_rate = kWakeOdr0_781Hz;

    // Since tap types share interrupts, if multiple taps are detected, there will be a priority order of what is returned:
    // kSingleTap > kDoubleTap > kUnknownTap
    // uint8_t get_tap_detected(void);

    // bool get_accel(flxDataArrayFloat *accelData);

//   public:
//     // our data accessors - use our base class for the simple readings

//     // Properties

//     // Enable Props:
    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_acceleration, &flxDevKX134::set_enable_acceleration>
    //     enableAcceleration = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_data_engine, &flxDevKX134::set_enable_data_engine>
    //     enableDataEngine = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_tap_engine, &flxDevKX134::set_enable_tap_engine>
    //     enableTapEngine = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_tilt_engine, &flxDevKX134::set_enable_tilt_engine>
    //     enableTiltEngine = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_sleep_engine, &flxDevKX134::set_enable_sleep_engine>
    //     enableSleepEngine = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_wake_engine, &flxDevKX134::set_enable_wake_engine>
    //     enableWakeEngine = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_direct_tap_interrupt, &flxDevKX134::set_enable_direct_tap_interrupt>
    //     enableDirectTapInterrupt = {false};

    // flxPropertyRWBool<flxDevKX134, &flxDevKX134::get_enable_double_tap_interrupt, &flxDevKX134::set_enable_double_tap_interrupt>
    //     enableDoubleTapInterrupt = {false};

    // Other Prop getters/setters:

    // ODR Properties:
    // flxPropertyRWUInt8<flxDevKX134, &flxDevKX134::get_output_data_rate, &flxDevKX134::set_output_data_rate> outputDataRate = {
    //     kOdr50Hz, // Default to 50 Hz.
    //     {{"0.781", kOdr0_781Hz},
    //      {"1.563", kOdr1_563Hz},
    //      {"3.125", kOdr3_125Hz},
    //      {"6.25", kOdr6_25Hz},
    //      {"12.5", kOdr12_5Hz},
    //      {"25", kOdr25Hz},
    //      {"50", kOdr50Hz},
    //      {"100", kOdr100Hz},
    //      {"200", kOdr200Hz},
    //      {"400", kOdr400Hz},
    //      {"800", kOdr800Hz},
    //      {"1600", kOdr1600Hz},
    //      {"3200", kOdr3200Hz},
    //      {"6400", kOdr6400Hz},
    //      {"12800", kOdr12800Hz},
    //      {"25600", kOdr25600Hz}}
    // };

    // flxPropertyRWUInt8<flxDevKX134, &flxDevKX134::get_tap_data_rate, &flxDevKX134::set_tap_data_rate> tapDataRate = {
    //     kTapOdr400Hz, // Default to 400 Hz.
    //     {{"12.5", kTapOdr12_5Hz},
    //      {"25", kTapOdr25Hz},
    //      {"50", kTapOdr50Hz},
    //      {"100", kTapOdr100Hz},
    //      {"200", kTapOdr200Hz},
    //      {"400", kTapOdr400Hz},
    //      {"800", kTapOdr800Hz},
    //      {"1600", kTapOdr1600Hz}}
    // };

    // flxPropertyRWUInt8<flxDevKX134, &flxDevKX134::get_tilt_data_rate, &flxDevKX134::set_tilt_data_rate> tiltDataRate = {
    //     kTiltOdr12_5Hz, // Default to 12.5 Hz.
    //     {{"1.563", kTiltOdr1_563Hz},
    //      {"6.25", kTiltOdr6_25Hz},
    //      {"12.5", kTiltOdr12_5Hz},
    //      {"50", kTiltOdr50Hz}}
    // };

    // flxPropertyRWUInt8<flxDevKX134, &flxDevKX134::get_wake_data_rate, &flxDevKX134::set_wake_data_rate> wakeDataRate = {
    //     kWakeOdr0_781Hz, // Default to 0.781 Hz.
    //     {{"0.781", kWakeOdr0_781Hz},
    //      {"1.563", kWakeOdr1_563Hz},
    //      {"3.125", kWakeOdr3_125Hz},
    //      {"6.25", kWakeOdr6_25Hz},
    //      {"12.5", kWakeOdr12_5Hz},
    //      {"25", kWakeOdr25Hz},
    //      {"50", kWakeOdr50Hz},
    //      {"100", kWakeOdr100Hz}}
    // };

    // // Range Property:
    // flxPropertyRWUInt8<flxDevKX134, &flxDevKX134::get_range, &flxDevKX134::set_range> range = {
    //     SFE_KX134_RANGE16G,
    //     {{"8g", SFE_KX134_RANGE8G},
    //      {"16g", SFE_KX134_RANGE16G},
    //      {"32g", SFE_KX134_RANGE32G},
    //      {"64g", SFE_KX134_RANGE64G}}};
    
    // // Data parameters

    // // Tap detection Parameter

    // flxParameterOutUInt8<flxDevKX134, &flxDevKX134::get_tap_detected> tapDetected;

    // // Acceleration data parameter
    // flxParameterOutArrayFloat<flxDevKX134, &flxDevKX134::get_accel> accelData;
};