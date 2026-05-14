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
 *  flxDevLidarLiteV4.cpp
 *
 *  Spark Device object for the LIDAR-Lite v4 LED device.
 *
 */

#include "Arduino.h"

#include "flxDevLidarLiteV4.h"

// The LidarLite supports reprogrammable addresses, but we only support the factory default.
static const uint8_t kLidarLiteV4AddressDefault = 0x62;

// Hardware version read from register 0xE1; used to disambiguate from other devices at 0x62
// (e.g. ACS37800).  Value 0x08 identifies the v4 LED variant.
static const uint8_t kLidarLiteV4HardwareVersion = 0x08;

uint8_t flxDevLidarLiteV4::defaultDeviceAddress[] = {kLidarLiteV4AddressDefault, kSparkDeviceAddressNull};

flxRegisterDevice(flxDevLidarLiteV4);

//-------------------------------------------------------------------------------------
/**
 * @brief Registers the device name, description, and all output parameters and properties.
 */
flxDevLidarLiteV4::flxDevLidarLiteV4()
{
    setName(getDeviceName());
    setDescription("Garmin LIDAR-Lite v4 LED Distance Sensor");

    flxRegister(distance, "Distance (cm)", "Measured distance in centimeters", kParamValueDistance);
    flxRegister(boardTemperature, "Board Temperature (C)", "Board temperature in degrees Celsius", kParamValueTempC);
    flxRegister(socTemperature, "SoC Temperature (C)", "SoC temperature in degrees Celsius", kParamValueTempC);

    flxRegister(configuration, "Configuration", "Measurement configuration preset (0=Max Range, 5=Very Short/Fastest)");
}

//-------------------------------------------------------------------------------------
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
bool flxDevLidarLiteV4::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    if (!i2cDriver.ping(address))
        return false;

    uint8_t hwver = i2cDriver.readRegister(address, 0xE1);

    return hwver == kLidarLiteV4HardwareVersion;
}

//-------------------------------------------------------------------------------------
/**
 * @brief Initializes the sensor on the given I2C bus and applies the stored configuration preset.
 *
 * @param wirePort  TwoWire instance to use for I2C communication.
 * @return true on success, false if begin() fails.
 */
bool flxDevLidarLiteV4::onInitialize(TwoWire &wirePort)
{
    bool status = LIDARLite_v4LED::begin(address(), wirePort);
    // Apply the preset here rather than in set_configuration() because the sensor
    // ignores configure() calls before begin() completes.
    if (status)
        LIDARLite_v4LED::configure(_configuration);
    return status;
}

//-------------------------------------------------------------------------------------
/**
 * @brief Triggers a distance measurement and returns the result.
 * @return Distance in centimeters.
 */
uint16_t flxDevLidarLiteV4::read_distance()
{
    return LIDARLite_v4LED::getDistance();
}

//-------------------------------------------------------------------------------------
/**
 * @brief Reads the PCB board temperature.
 *
 * Casts the library's unsigned return value to int8_t because the sensor
 * encodes signed temperatures using two's-complement in a uint8_t register.
 *
 * @return Board temperature in degrees Celsius.
 */
int8_t flxDevLidarLiteV4::read_board_temperature()
{
    return (int8_t)LIDARLite_v4LED::getBoardTemp();
}

//-------------------------------------------------------------------------------------
/**
 * @brief Reads the System-on-Chip (SoC) internal temperature.
 *
 * Same signed reinterpretation as read_board_temperature().
 *
 * @return SoC temperature in degrees Celsius.
 */
int8_t flxDevLidarLiteV4::read_soc_temperature()
{
    return (int8_t)LIDARLite_v4LED::getSOCTemp();
}

//-------------------------------------------------------------------------------------
/**
 * @brief Returns the currently cached configuration preset index.
 * @return Preset value (0–5).
 */
uint8_t flxDevLidarLiteV4::get_configuration()
{
    return _configuration;
}

//-------------------------------------------------------------------------------------
/**
 * @brief Sets the measurement configuration preset.
 *
 * Caches the value and, if the device is already initialized, pushes it
 * to the sensor immediately. Otherwise onInitialize() applies it at startup.
 *
 * @param config  Preset index (0=Max Range … 5=Very Short Range, Highest Speed).
 */
void flxDevLidarLiteV4::set_configuration(uint8_t config)
{
    _configuration = config;
    // Push the new preset to the sensor immediately if already running;
    // otherwise onInitialize() will apply _configuration when the device starts.
    if (isInitialized())
        LIDARLite_v4LED::configure(_configuration);
}
