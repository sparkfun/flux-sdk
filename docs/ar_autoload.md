# Architecture - Device Detection and Loading

One of the key features of the Flux framework is the ability to automatically detect and load different I2C devices, while placing minimal requirements on the device driver developer.

The key goals set for devices and auto load in the Flux framework include:

* Auto detection of devices connected to the I2C bus
* Device driver instantiation performed after the device is detected
* A device driver is self-describing, enabling device detection without device instantiation
* All information related to a device is maintained in the device driver - no outside registry or implementation is required or maintained.
* Device detection detection includes a method to resolve address conflicts

## Architecture Overview

To meet the goals for this subsystem, the Flux framework follows a ***Factory Pattern***, using device "builders" to describe a device, and a "factory" to manage the registration, detection and instantiation of devices using these device specific builders.

The key classes to support this pattern are:

| | |
|------|-------|
**Device Driver** | The device specific driver, often implemented based on an existing Arduino Library |
**Device Builder** | A device specific class that is automatically generated and used by the Framework to detect and instantiate a device
**Device Factor** | An overall singleton within the system that enables device registration at startup and device discovery, instantiation and initialization at runtime

### Device Class

For device discovery and instantiation (not operation) the Device class in the Flux framework provides enough information for the Device Factory along with Device Builders to discover a device on the system and instantiate it.

To provide this introspection capability, while not requiring the actual instantiation of a device driver class, an "interface" of static methods are used for each class. As static methods, no actual device driver object is required to perform device - the static methods are just called.

The class hierarchy for the Device class is outlined in the following diagram:

![Device Class](images/ar-flux-device-class.png)

The following static methods form the device discovery interface:

|||
|----|---|
```isConnected()``` | Called with an I2C bus object - should return true of the device is connected
```connectedConfidence()``` | Returns a confidence level to indicate the accuracy of the ```isConnected()``` algorithm used. Helpful when resolving device address conflicts
```getDeviceName()``` | Returns the name of the device - should be a static constant
```getDefaultAddress()``` | Returns the default I2C address for the device. *This method is deprecated*
```getDefaultAddresses()``` | Returns a list of I2C addresses the device can use. The first address should be the default address for the device. This array is terminated with the value ```kSparkDeviceAddressNull```

Additionally, the device implements a non-static  ```onInitialize()``` method that returns a boolean. When a device object is created, this method is called to "initialize" the device.
