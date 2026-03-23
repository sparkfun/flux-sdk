/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include "flxCoreParam.h"

// Define value types for our devices. These value types provide a broad parameter value type classification
// to parameter values.
//
// A Value type is set on a parameter using a below code and using the setValueType() method. The
// value type is retrieved using the valueType() method.
//
// Note - by default parameters have a value type of kParamValueNone - which is zero. Our types
//        are numbered starting at 1.

const flxParamValueType_t kParamValueTemperature = 1;
const flxParamValueType_t kParamValueHumidity = 2;
const flxParamValueType_t kParamValuePressure = 3;

// VOC - Volatile Organic Compounds concentration in ppb - uint16_t value
const flxParamValueType_t kParamValueVOC = 4;

// AQI - Air Quality Index - uint16_t value
const flxParamValueType_t kParamValueAQI = 5;

// CO2 - Carbon Dioxide concentration in ppb- uint16_t value
const flxParamValueType_t kParamValueCO2 = 6;

// ETOH - Ethanol concentration in ppb - uint16_t value
const flxParamValueType_t kParamValueETOH = 7;

// Humidity - Relative Humidity in percent - float value
const flxParamValueType_t kParamValueHumidity_F = 8;

// Pressure - Pressure in Pascals - float value
const flxParamValueType_t kParamValuePressure_F = 9;

// Temperature - Temperature in Celsius - float value
const flxParamValueType_t kParamValueTempC = 10;

// Temperature - Temperature in Fahrenheit - float value
const flxParamValueType_t kParamValueTempF = 11;

// TVOC - Total Volatile Organic Compounds concentration in ppb - float value
const flxParamValueType_t kParamValueTVOC = 12;

// CO2 - Carbon Dioxide concentration in ppb- float value
const flxParamValueType_t kParamValueCO2_F = 13;

// Meters Per Second - float value
const flxParamValueType_t kParamValueMPS = 14;

// Miles Per Hour - float value
const flxParamValueType_t kParamValueMPH = 15;

// Latitude - in degrees - float value
const flxParamValueType_t kParamValueLatitude = 16;

// Longitude in degrees - float value
const flxParamValueType_t kParamValueLongitude = 17;

// Altitude in meters - float value
const flxParamValueType_t kParamValueAltitude = 18;

// Acceleration X in milli-g - float value
const flxParamValueType_t kParamValueAccelX = 19;

// Acceleration Y in milli-g - float value
const flxParamValueType_t kParamValueAccelY = 20;

// Acceleration Z in milli-g - float value
const flxParamValueType_t kParamValueAccelZ = 21;

// Gyro X in milli-dps - float value
const flxParamValueType_t kParamValueGyroX = 22;

// Gyro Y in milli-dps - float value
const flxParamValueType_t kParamValueGyroY = 23;

// Gyro Z in milli-dps - float value
const flxParamValueType_t kParamValueGyroZ = 24;

// Pressure in milli-bar - float value
const flxParamValueType_t kParamValuePressure_mBar = 25;

// Weight in "user" units - float value
const flxParamValueType_t kParamValueWeightUserUnits = 26;

// X Coordinate - CIE 1931 Color Space - float value
const flxParamValueType_t kParamValueCIE_X = 27;

// Y Coordinate - CIE 1931 Color Space - float value
const flxParamValueType_t kParamValueCIE_Y = 28;

// CCT - Correlated Color Temperature in Kelvin - uint16_t value
const flxParamValueType_t kParamValueCCT = 29;

// LUX - Light intensity in Lux - uint32_t value
const flxParamValueType_t kParamValueLUX = 30;

// Time - Epoch time in seconds - uint32_t value
const flxParamValueType_t kParamValueEpoch = 31;

// CO2 - Carbon Dioxide concentration in ppm - uint32_t value
const flxParamValueType_t kParamValueCO2_U32 = 32;

// TVOC - Total Volatile Organic Compounds concentration in ppb - uint32_t value
const flxParamValueType_t kParamValueTVOC_U32 = 33;

// H2 - Hydrogen concentration in ppm - uint32_t value
const flxParamValueType_t kParamValueH2 = 34;

// ETOH - Ethanol concentration in ppb - uint32_t value
const flxParamValueType_t kParamValueETOH_U32 = 35;

// Presence - Presence detection 1/cm - int16_t value
const flxParamValueType_t kParamValuePresence = 36;

// Motion - Motion detection value - int16_t value
const flxParamValueType_t kParamValueMotion = 37;

// Proximity - Proximity detection value - uint16_t value
const flxParamValueType_t kParamValueProximity = 38;

// Lux - Light intensity in Lux - uint16_t value
const flxParamValueType_t kParamValueLUX_U16 = 39;

// UVA Index - float value
const flxParamValueType_t kParamValueUVAIndex = 40;

// UVB Index - float value
const flxParamValueType_t kParamValueUVBIndex = 41;

// UV Index - float value
const flxParamValueType_t kParamValueUVIndex = 42;

// Lux - Light intensity in Lux - float value
const flxParamValueType_t kParamValueLUX_F = 43;

// Ambient Light - Ambient light level - uint32_t value
const flxParamValueType_t kParamValueAmbientLight = 44;

// White Light - White light level - uint32_t value
const flxParamValueType_t kParamValueWhiteLight = 45;

// Distance - Distance in meters - uint32_t value
const flxParamValueType_t kParamValueDistance = 46;

// Battery Charge - Battery charge % - float value
const flxParamValueType_t kParamValueBatteryCharge = 47;

// Battery Voltage - Battery voltage in volts - float value
const flxParamValueType_t kParamValueBatteryVoltage = 48;

// Battery Charge Rate %/hr - Battery charge rate %/hr - float value
const flxParamValueType_t kParamValueBatteryChargeRate = 49;

// Temperature (C)  - Double value
const flxParamValueType_t kParamValueTempC_D = 50;

// Pressure - double value
const flxParamValueType_t kParamValuePressure_D = 51;

// Soil Moisture Reading Raw   - uint16
const flxParamValueType_t kParamValueSoilMoistureRaw = 52;

// Soil Moisture - percent
const flxParamValueType_t kParamValueSoilMoisturePercent_F = 53;

// Location - float array[2] - Lat, Lon
const flxParamValueType_t kParamValueLocation = 54;