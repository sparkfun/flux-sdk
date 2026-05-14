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
 *  flxDevLidarLiteV4.h
 *
 *  Spark Device object for the LIDAR-Lite v4 LED device.
 *
 */

#pragma once

#include "Arduino.h"

#include "LIDARLite_v4LED.h"
#include "flxDevice.h"

#define kLidarLiteV4DeviceName "LidarLiteV4"

/**
 * @brief Driver for the Garmin LIDAR-Lite v4 LED distance sensor.
 *
 * Inherits LIDARLite_v4LED directly so its I2C methods drive the hardware without an
 * extra pointer indirection; flxDeviceI2CType provides auto-discovery and registration.
 */
class flxDevLidarLiteV4 : public flxDeviceI2CType<flxDevLidarLiteV4>, public LIDARLite_v4LED
{

  public:
    /**
     * @brief Registers the device name, description, and all output parameters and properties.
     */
    flxDevLidarLiteV4();

    /**
     * @brief Checks whether a LIDAR-Lite v4 is present at the given I2C address.
     *
     * Pings the address and then reads the hardware-version register (0xE1) to
     * distinguish this device from others that share the same default address (e.g. ACS37800).
     *
     * @param i2cDriver  I2C bus driver to use for communication.
     * @param address    I2C address to probe.
     * @return true if a v4 LED is found, false otherwise.
     */
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    /**
     * @brief Returns the confidence level for connection detection.
     *
     * Reports Exact because the hardware-version register check in isConnected()
     * uniquely identifies this device.
     *
     * @return flxDevConfidenceExact
     */
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    /**
     * @brief Returns the human-readable device name string.
     * @return Pointer to the device name constant.
     */
    static const char *getDeviceName()
    {
        return kLidarLiteV4DeviceName;
    };

    /**
     * @brief Returns the null-terminated list of supported I2C addresses.
     * @return Pointer to the defaultDeviceAddress array.
     */
    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    static uint8_t defaultDeviceAddress[];

    /**
     * @brief Initializes the sensor on the given I2C bus and applies the stored configuration preset.
     *
     * @param wirePort  TwoWire instance to use for I2C communication.
     * @return true on success, false if begin() fails.
     */
    bool onInitialize(TwoWire &);

  private:
    /**
     * @brief Triggers a distance measurement and returns the result.
     * @return Distance in centimeters.
     */
    uint16_t read_distance();

    /**
     * @brief Reads the PCB board temperature.
     *
     * Casts the library's unsigned return value to int8_t because the sensor
     * encodes signed temperatures using two's-complement in a uint8_t register.
     *
     * @return Board temperature in degrees Celsius.
     */
    int8_t read_board_temperature();

    /**
     * @brief Reads the System-on-Chip (SoC) internal temperature.
     *
     * Same signed reinterpretation as read_board_temperature().
     *
     * @return SoC temperature in degrees Celsius.
     */
    int8_t read_soc_temperature();

    /**
     * @brief Returns the currently cached configuration preset index.
     * @return Preset value (0–5).
     */
    uint8_t get_configuration();

    /**
     * @brief Sets the measurement configuration preset.
     *
     * Caches the value and, if the device is already initialized, pushes it
     * to the sensor immediately. Otherwise onInitialize() applies it at startup.
     *
     * @param config  Preset index (0=Max Range … 5=Very Short Range, Highest Speed).
     */
    void set_configuration(uint8_t);

    // Cached so the preset can be set before onInitialize() runs and applied at init time.
    uint8_t _configuration = 0;

  public:
    // Output parameters — each triggers a single blocking measurement when read.
    flxParameterOutUInt16<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_distance> distance;
    flxParameterOutInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_board_temperature> boardTemperature;
    flxParameterOutInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::read_soc_temperature> socTemperature;

    // Preset 0 (Max Range) is the power-on default; values map directly to the
    // LIDARLite_v4LED::configure() preset argument.
    flxPropertyRWUInt8<flxDevLidarLiteV4, &flxDevLidarLiteV4::get_configuration, &flxDevLidarLiteV4::set_configuration>
        configuration = {0,
                         {{"Max Range", 0},
                          {"Balanced", 1},
                          {"Short Range, High Speed", 2},
                          {"Mid Range, Higher Speed", 3},
                          {"Max Range, Higher Speed", 4},
                          {"Very Short Range, Highest Speed", 5}}};
};
