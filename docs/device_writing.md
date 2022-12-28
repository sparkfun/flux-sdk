
# Writing a Device Class

A framework device class wraps an underlying device driver, providing the needed functionally to bring the device withing the framework. 

The key capabilities provided by the implemented framework device class are:
* Enable device runtime detection and loading
* Property and Parameter definitions that support runtime introspection
* Common interfaces to fit within the defined structure of the framework implementation pattern


## Defining the Device Class

The device class should following the naming patter `spDev[Name]`, where Name is a unique name of the class. 

The implementation requires separate header and implementation files, since a several class variables and a global object are defined that required the use of an implementation file.  

The new device class should subclass from the frameworks ```flxDevice``` class, using the ```flxDeviceI2CType<DeviceName>``` template. Additionally, the device class subclasses from the underlying driver class. This allows the descriptor class to support the existing driver's interface. 

##### Example of a class definition

Implementing a driver for the `BME280` Device.

```C++
class flxDevBME280 : public flxDeviceI2CType<flxDevBME280>, public BME280
{

};
```

## Automatic Device Discovery

The framework supports runtime discovery of connected devices. This is performed using information from the framework device class, while not creating a device instance until the device is actually detected.

To accomplish this task, class level (static) methods and data are implemented by the device object. Each device class implements the following:

|Item | Description|
|----|--|
 ```bool isConnected(flxBusI2C, address)``` | Returns true if the device is connected |
 ```char* getDeviceName()``` | Returns the Device Name |
|```uint8_t *getDefaultAddresses()``` | Return a list of addresses for the device. This list terminates with the value of ```kSparkDeviceAddressNull``` |

Note
>* Often the device implements the address list as a class level variable
>* It is common to define a constant or macro for the device name and return it from ```getDeviceName()```

### Device Detection
#### Device Addresses

The first step for a given driver is the retrieval of default addresses for the device.In this step, the system calls the ```getDefaultAddresses()``` method of the driver. The driver should return an array of type uint8_t, with the last value of the array being the sentinel value of ```kSparkDeviceAddressNull``` 

##### Method Signature
```C++
static const uint8_t *getDefaultAddresses();
```
#### Is Connected
For each of the addresses returned, the system calls the drivers ```isConnected()``` method. The driver should returns `true` if the device is connected at the given address, otherwise `false`.

How the driver determines if a device is connected is determined by the implementation

##### Method Signature
```C++
static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
```

##### Arguments
* i2cDriver - a I2C Bus driver object the driver can use to query the I2C bus
* address - The address to check for a device 

#### Device Name

The static interface for the device also includes a method to return the name of the device. 

##### Method Signature
```C++
static const char *getDeviceName()
```
This method returns a constant C string. 

#### Example Method Definition

This example is taken from the device driver for the BME280 device. 

The class definition
```C++
// What is the name used to ID this device?
#define kBME280DeviceName "bme280";

// Define our class - note sub-classing from the Qwiic Library
class flxDevBME280 : public flxDevice<flxDevBME280>, public BME280
{

  public:
    flxDevBME280();

    // Device is connected methods
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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
* Default device addresses are contained in a class variable  ``` defaultDeviceAddress[];```
* The method ```onInitialize()``` is called after the object is instantiated. 
* The class subclasses from flxDevice, passing in the class name to the template

### Auto Discovery - Class Implementation

To complete the auto-discovery capabilities of the system, besides the implementation of the above methods, the classes implementation file must include to register the device.

This is call is placed before the class implementation and has the following signature:
```C++
flxRegisterDevice(DeviceName);
```
Where `DeviceName` is the class name of the device being registered. 

Once a device is registered, it is available for auto-detection and loading by the framework during the startup process of system. 

Building off the above BME280 example, the implementation looks like:
```C++
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
    spSetupDeviceIdent(getDeviceName());

}
```

Notes
* This example includes the implementation of ```defaultDeviceAddress[]```, the class variable holding the addresses for the device.
* The device is registered before the class constructor
* In the constructor, the device identity is set, which is based of runtime conditions.

The isConnected() method for this example is:
```C++
bool flxDevBME280::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{

    uint8_t chipID = i2cDriver.readRegister(address, BME280_CHIP_ID_REG); 
    // Should return 0x60 or 0x58

    return (chipID == 0x58 || chipID == 0x60);
}
```
Note
* This is a static (has no `this` instance) and as such uses the methods on the passed in I2C bus driver to determine in a BME280 is connected to the system


### Startup Sequence
The last part of implementing a device descriptor/driver class is the addition of an initialization method, named ``onInitialize()``. 

##### Method Signature
```C++
 bool onInitialize(TwoWire &);
 ```
The only argument to this methods is the Arduino I2C `TwoWire` class, which the class can use to initialize the device. The method returns `true` on success, `false` on failure. 

The BME280 example implementation:
```C++
bool flxDevBME280::onInitialize(TwoWire &wirePort)
{

    // set the device address
    BME280::setI2CAddress(address());
    return BME280::beginI2C(wirePort);
}
```
Note
* The ```address()``` method returns the device address for this instance of the driver. 

## Device Properties
See the detailed description of [Properties](properties.md)

## Device Prarameters
See the detailed description of [Parameters](parameters.md)