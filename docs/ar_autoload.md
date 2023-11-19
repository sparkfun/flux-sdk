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

#### Static Methods

The following **static** methods form the device discovery interface:

|||
|----|---|
```isConnected()``` | Called with an I2C bus object - should return true of the device is connected
```connectedConfidence()``` | Returns a confidence level to indicate the accuracy of the ```isConnected()``` algorithm used. Helpful when resolving device address conflicts
```getDeviceName()``` | Returns the name of the device - should be a static constant
```getDefaultAddress()``` | Returns the default I2C address for the device. *This method is deprecated*
```getDefaultAddresses()``` | Returns a list of I2C addresses the device can use. The first address should be the default address for the device. This array is terminated with the value ```kSparkDeviceAddressNull```

#### Instance Methods

For the startup sequence the following instance methods are important
|||
|------|--------|
```onInitialize()``` | Called during the initialization process allowing the performance of the driver specific startup sequence. The Arduino TwoWire (Wire) object is passed in for use by the driver. Note: to get the address to use for the device, the driver calls the ```address()``` method.
```address()``` | Inherited - this method returns the address for the attached device
```isInitialized()``` | Returns true of the method ```onInitialized()``` returned true - indicating the driver is initialized.

### Device Builder Class

This class provides a common interface for the Factory class to use during the discovery and instantiation phase of device creation. The class is defined as a template, with the only template parameter being the class name of the Driver it represents.

The template definition for the ```DeviceBuilder``` class:

```c++
template <class DeviceType> class DeviceBuilder : public flxDeviceBuilderI2C
```

For the most part, all the methods in this class just wrap the *introspection* methods provided by the underlying Device class it represents. This allows allows the Factory class to work with object instances that bridge calls to the *static* methods of a Device object.

Example of a wrapped method in the ```DeviceBuilder``` template:

```C++
bool isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    return DeviceType::isConnected(i2cDriver, address);
}
```

#### DeviceBuilder Instantiation

A goal of the Flux framework was to *automatically* register a device driver only by its implementation, not leveraging any external registration or implementation. The ```DeviceBuilder``` object is the key to meeting this goal.

In the implementation file of each device driver, a static ```global``` instance of the device drivers DeviceBuilder class is defined. Since all global objects are created during the system environment initialization, the DeviceBuilder can *register* itself with the Flux device Factory in its factory method. This is the ***trick*** to meeting the goal of device driver "auto registration".

The definition of the ```DeviceBuilder``` constructor:

```C++
DeviceBuilder()
{
    flxDeviceFactory::get().registerDevice(this);
}
```

The Flux Factory class ```flxDeviceFactory``` is a *singleton*, and globally accessible. The first call to ```get()``` will create the only instance of the object, enabling creation at startup.

#### Developer Device Registration

To register a device driver, a static ```DeviceBuilder``` is added to the drivers implementation file.

```C++
static DeviceBuilder<kDevice> global_##kDevice##Builder;
```

But to make this easier, the following macro is defined.

```C++
#define flxRegisterDevice(kDevice) static DeviceBuilder<kDevice> global_##kDevice##Builder;
```

Using this macro, device registration looks like the following (using the BME280 driver)

```C++
flxRegisterDevice(flxDevBME280);
```

Easy, and similar to other use patters throughout the Flux framework.
