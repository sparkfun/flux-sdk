
# SparkFun flux-sdk

The SparkFun flux-sdk is an embedded framework focused on the rapid development of fully featured embedded applications. Build around the concept of adding high-level "capabilities" to an application with minimal development effort, the sdk provides a wide-variety of high-level objects that automatically fit together to form a cohesive and rapidly developed application.

Key functionality provided by the framework:

* Automatic I2C (qwiic) device discovery, initialization and use.
* Automatic serial driven menu configuration system based on the application definition
* Automatic system persistance - local and SD card file
* Multi-format Data logging support - serial and SD card files
* Support for a wide range of IoT device services (AWS IoT, Azure IoT, Arduino IoT, Thingspeak, MQTT,..)
* Extensive I2C (qwiic) device support
* Easy to extend and customize development

For further details, a detailed documentation for flux is provided [here](https://docs.sparkfun.com/flux-sdk/)

## Status

### November 2024

The flux-sdk was developed as an internal tool initially, but is now being made public. The current functionality is a key component of several SparkFun products and relatively stable, but the sdk supporting functionality (documentation and examples) are in an **alpha** form at best.

Over the next several quarters improvements to the sdk include:

* Improved documentation on using the sdk, as well as adding functionality to it (device drivers for example)
* Remove any out of date remnants from the original version of the sdk - "spark" (which was started in 2020).
* Add support for additional platforms. Currently the focus is ESP32, but a Raspberry Pi RP2350 solution is being developed
* Add additional devices as needed.

## Documentation

As noted above, the current documentation for the flux-sdk is located [here](https://docs.sparkfun.com/flux-sdk/)
