
# Writing a Device Class {#writing_a_device_class}

A framework device class wraps an underlying device driver, providing the needed functionally to bring the device withing the framework.

The key capabilities provided by the implemented framework device class are:

* Enable device runtime detection and loading
* Property and Parameter definitions that support runtime introspection
* Common interfaces to fit within the defined structure of the framework implementation pattern

## Defining the Device Class

The device class should following the naming pattern `flxDev[Name]`, where Name is a unique name of the class.

The implementation requires separate header and implementation files, since a several class variables and a global object are defined that required the use of an implementation file.  

The new device class should subclass from the frameworks ```flxDevice``` class, using the ```flxDeviceI2CType<DeviceName>``` template. Additionally, the device class subclasses from the underlying driver class in most cases. This allows the descriptor class to support the existing driver's interface.

> Note - In some cases, because of the underlying Arduino Library design, an alternative > implementation pattern is required - such as object containment.

### Example of a class definition

Implementing a driver for the `BME280` Device.

```cpp
class flxDevBME280 : public flxDeviceI2CType<flxDevBME280>, public BME280
{

};
```

## Automatic Device Discovery

The framework supports runtime discovery of connected devices. This is performed using information from the framework device class, while not creating a device instance until the device is actually detected.

To accomplish this task, class level (static) methods and data are implemented by the device object. Each device class implements the following:

|Item | Description|
|----|--|
| ```bool isConnected(flxBusI2C, address)``` | Returns true if the device is connected |
| ```char* getDeviceName()``` | Returns the Device Name |
|```uint8_t *getDefaultAddresses()``` | Return a list of addresses for the device. This list terminates with the value of ```kSparkDeviceAddressNull``` |
|```flxDeviceConfidence_t connectedConfidence()``` | Returns the confidence level of the drivers ```isConnected()``` algorithm. Values supported range from *Exact* to *Ping* |

> [!note]
>
>* Often the device implements the address list as a class level variable
>* It is common to define a constant or macro for the device name and return it from ```getDeviceName()```

### Device Detection

#### Device Addresses

The first step for a given driver is the retrieval of default addresses for the device. In this step, the system calls the ```getDefaultAddresses()``` method of the driver. The driver should return an array of type uint8_t, with the last value of the array being the sentinel value of ```kSparkDeviceAddressNull```.

The system uses the array of addresses to determine what addresses are currently available, and call the ```isConnected()``` with the possible and available addresses until a connection is found, or it hits the end of the possible addresses for the device.

##### Method Signature {#device-address-method}

```cpp
static const uint8_t *getDefaultAddresses();
```

#### Is Connected

For each of the addresses returned, the system calls the drivers ```isConnected()``` method. The driver should returns `true` if the device is connected at the given address, otherwise `false`.

How the driver determines if a device is connected is determined by the implementation

##### Method Signature {#is-connected-method}

```cpp
static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
```

##### Arguments

* i2cDriver - a I2C Bus driver object the driver can use to query the I2C bus
* address - The address to check for a device

#### Device Name

The static interface for the device also includes a method to return the name of the device.

##### Method Signature {#device-name-method}

```cpp
static const char *getDeviceName()
```

This method returns a constant C string.

#### Connected Confidence

This method returns the confidence level for the algorithm in the devices ```isConnected()``` algorithm in exactly determining if a device is connected at the specific address.

This confidence level is used to resolve detection conflicts between devices that support the same address on the I2C bus. Drivers that have a higher confidence level are evaluated first.

#### Method Signature {#confidence-level-method}

```cpp
static flxDeviceConfidence_t connectedConfidence(void)
```

The return value should be one of the following:

| | |
|---|---|
|```flxDevConfidenceExact``` | The algorithm can perform an exact match|
|```flxDevConfidenceFuzzy``` | The algorithm has high-confidence in a match, but it's not exact|
|```flxDevConfidencePing``` | An address "ping" is used - just detecting a device at a location, but not verifying the device type.|

> Note: Only one device with a PING confidence is allowed at an address.

#### Example Method Definition

This example is taken from the device driver for the BME280 device.

The class definition - ```flxDevBME280.h```

```cpp
// What is the name used to ID this device?
#define kBME280DeviceName "bme280";

// Define our class - note sub-classing from the Qwiic Library
class flxDevBME280 : public flxDevice<flxDevBME280>, public BME280
{

  public:
    flxDevBME280();

    // Device is connected methods
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }
    static const char *getDeviceName()
    {
        return kBME280DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);
};
```

#### Notes

* This device defined its name, bme280, using the macro `kBME280DeviceName`
* Default device addresses are contained in a class variable  ```defaultDeviceAddress[];```
* The method ```onInitialize()``` is called after the object is instantiated.
* The class subclasses from flxDevice, passing in the class name to the template. The class also subclasses from the Arduino Library class - ```BME280```

### Auto Discovery - Class Implementation

To complete the auto-discovery capabilities of the system, besides the implementation of the above methods, the classes implementation file must include a call to register the device with the system.

This is call is placed before the class implementation and has the following signature:

```cpp
flxRegisterDevice(DeviceClassName);
```

Where `DeviceClassName` is the class name of the device being registered.

Once a device is registered, it is available for auto-detection and loading by the framework during the startup process of system.

> ![note]
> The ```flxRegisterDevice()``` call is a macro that defines a global object using a c++ template. The object is instantiated on system startup (all globals are), and in the constructor of the object, it registers itself with the device discovery system.

Building off the above BME280 example, the implementation looks like:

```cpp
#define kBMEAddressDefault 0x77
#define kBMEAddressAlt1 0x76

// Define our class static variables - allocs storage for them

uint8_t flxDevBME280::defaultDeviceAddress[] = {kBMEAddressDefault, 
                    kBMEAddressAlt1, 
                    kSparkDeviceAddressNull};

// Register this class with the system,
flxRegisterDevice(flxDevBME280);

//----------------------------------------
// Constructor

flxDevBME280::flxDevBME280()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BME280 Atmospheric Sensor");

}
```

> [!note]
>
> * This example includes the implementation of ```defaultDeviceAddress[]```, the class variable holding the addresses for the device.
> * The device is registered before the class constructor
> * In the constructor, the device identity is set, which is based of runtime conditions.

The isConnected() method for this example is:

```cpp
bool flxDevBME280::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{

    uint8_t chipID = i2cDriver.readRegister(address, BME280_CHIP_ID_REG); 
    // Should return 0x60 or 0x58

    return (chipID == 0x58 || chipID == 0x60);
}
```

> [!note]
>
> * This is a static (has no `this` instance) and as such uses the methods on the passed in I2C bus driver to determine in a BME280 is connected to the system

### Startup Sequence

The last part of implementing a device descriptor/driver class is the addition of an initialization method, named ``onInitialize()``.

#### Method Signature {#on-init-method}

```cpp
 bool onInitialize(TwoWire &);
 ```

The only argument to this methods is the Arduino I2C `TwoWire` class, which the class can use to initialize the device. The method returns `true` on success, `false` on failure.

The BME280 example implementation:

```cpp
bool flxDevBME280::onInitialize(TwoWire &wirePort)
{

    // set the device address
    BME280::setI2CAddress(address());
    return BME280::beginI2C(wirePort);
}
```

> [!note]
> The ```address()``` method returns the device address for this instance of the driver.

### Determining if a Device is Initialized

To determine if a device is initialized, the Device Driver implementation should call the method ```isInitialized()```, which returns the value that was returned by ```onInitialize()```. It also returns false before the call to ```isInitialized()```

## Device Properties

See the detailed description of [Properties](properties.md)

## Device Parameters

See the detailed description of [Parameters](parameters.md)
