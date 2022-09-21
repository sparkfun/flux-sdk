#!/bin/bash
cd drivers/BME280
curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_BME280_Arduino_Library/master/src/SparkFunBME280.h
curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_BME280_Arduino_Library/master/src/SparkFunBME280.cpp
cd ../..
