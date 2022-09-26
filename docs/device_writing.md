
# Writing a Device Class

A framework device class wraps an underlying device driver, providing the needed functionally to bring the device withing the framework. 

The key capabilities provided by the implemented framework device class are:
* Enable device runtime detection and loading
* Property and Parameter definitions that support runtime introspection
* Common interfaces to fit within the defined structure of the framework implementation pattern


## Defining the Device Class

The device class should following the naming patter `spDev[Name]`, where Name is a unique name of the class. 

The implementation requires separate header and implementation files, since a several class variables and a global object are defined that required the use of an implementation file.  

The new device class should subclass from the frameworks ```spDevice``` class, using the ```spDevice<DeviceName>``` template. Additionally, the device class subclasses from the underlying driver class. This allows the descriptor class to support the existing driver's interface. 

##### Example of a class definition

Implementing a driver for the `BME280` Device.

```C++
class spDevBME280 : public spDevice<spDevBME280>, public BME280
{

};
```

## Automatic Device Discovery

The framework supports runtime discovery of connected devices. This is performed using information from the framework device class, while not creating a device instance until the device is actually detected.

To accomplish this task, class level (static) methods and data are implemented by the device object. Each device class implements the following:

|Item | Description|
|----|--|
 ```bool isConnected(spDevI2C, address)``` | Returns true if the device is connected |
 ```char* getDeviceName()``` | Returns the Device Name |
|```uint8_t *getDefaultAddresses()``` | Return a list of addresses for the device. This list terminates with the value of ```kSparkDeviceAddressNull``` |

Note
>* Often the device implements the address list as a class level variable
>* It is common to define a constant or macro for the device name and return it from ```getDeviceName()```

### Device Detection

The first step for a given driver is the retrieval of default addresses for the device.In this step, the system calls the ```getDefaultAddresses()``` method of the driver. The driver should return an array of type uint8_t, with the last value of the array being the sentinel value of ```kSparkDeviceAddressNull``` 

##### Method Signature
```C++
static const uint8_t *getDefaultAddresses();
```

For each of the addresses returned, the system calls the drivers ```isConnected()``` method. The driver should returns `true` if the device is connected at the given address, otherwise `false`.

How the driver determines if a device is connected is determined by the implementation

##### Method Signature
```C++
static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
```

##### Arguments
* i2cDriver - a I2C Bus driver object the driver can use to query the I2C bus
* address - The address to check for a device 




.
### Parameter Attributes
The following are key attributes of parameters within the framework
* Parameters can be added to any classed derived from the spOperation class
* Parameter objects are typed
* Parameter objects can act like a function
* Parameter objects allow introspection - they can be discovered and manipulated at runtime via software

#### Parameter Data Types
The following types are available for properties
* bool
* int
* uint
* float
* double
* string 

#### Parameter Use
Setting an value of a Input Parameter - named ```input``` in this example:
```c++
    anObject.input(value);
    anObject.input.set(value);
```
Getting a value of an Output Parameter - named ```output``` in this example:
```C++
    value = anObject.output();
    value = anObject.output.get();
```

### Defining a Parameter
For the framework, two types of Parameter classes exist. 
* Input Parameter - Defines a Parameter to set input value to an operation
* Output Parameter - Defines a Parameter to get an output value from an operation

Note
> The get and set operations on a parameter are mapped to methods implemented by the containing class. 

### Input Parameter Objects
These parameter objects are used to define a input parameter to an operation. Besides allowing introspection at runtime, they also pass on the provided value to a method.

#### Declaring the Input Parameter
Within the definition of the class the parameter is for, the input parameter is defined using the following pattern:

```C++
spParameterInType<ClassName, &ClassName::Writer>  input_name;
```
Where:
* spParameterInType - the type of the input parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Writer - the name of the _write_ method the parameter should call when it's value is set. **NOTE**: A reference, `& operator`, to the writer is provided

##### Available Input Parameter Types:

* spParameterInBool - bool property
* spParameterInInt  - integer property
* spParameterInUint - unsigned integer
* spParameterInFloat - float
* spParameterInDouble - double
* spParameterInString - string -> std::string

##### Writer Methods

These methods are implemented on the containing class and are called when the value of a parameter is set. These methods have the following signature:

```C++
void ClassName::write_Name(parameter_type value);
```
Where

* parameter_type - the type of the parameter (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
>By convention, writer method names are prefixed by ```write_```

##### Example

```C++
class MyClass : public spObject
{
private:
    void write_myInput(int value);

public:
   // Define an input parameter
   spParameterInInt<MyClass, &MyClass::write_MyInput>  my_input;
}
```
Note
> * By convention declaring the input writer method as private. This can be optional
> * The writer method must be declared before defining the parameter
> * The use of the `write_` prefix on the writer methods help identify the methods as supporting a parameter.

### Output Parameter Objects
These parameter objects are used to define a output parameter to an operation. Besides allowing introspection at runtime, they also retrieve the desired value from a method.

#### Declaring the Output Parameter
Within the definition of the class the parameter is for, the output parameter is defined using the following pattern:

```C++
spParameterOutType<ClassName, &ClassName::Reader>  output_name;
```
Where:
* spParameterOutType - the type of the output parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Reader - the name of the _read_ method the parameter should call when it's value is retrieved. **NOTE**: A reference, `& operator`, to the reader is provided

##### Available Input Parameter Types:

* spParameterOutBool - bool property
* spParameterOutInt  - integer property
* spParameterOutUint - unsigned integer
* spParameterOutFloat - float
* spParameterOutDouble - double
* spParameterOutString - string -> std::string

##### Reader Methods

These methods are implemented on the containing class and are called when the value of a parameter is requested. These methods have the following signature:

```C++
parameter_type ClassName::read_Name(void);
```
Where

* parameter_type - the type of the parameter (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
>By convention, reader method names are prefixed by ```read_```

##### Example

```C++
class MyClass : public spObject
{
private:
    int read_myOutput(void);

public:
   // Define an output parameter
   spParameterOutInt<MyClass, &MyClass::read_MyOutput>  my_output;
}
```
Note
> * By convention declaring the output reader method as private. This can be optional
> * The reader method must be declared before defining the parameter
> * The use of the `read_` prefix on the writer methods help identify the methods as supporting a parameter.

#### Runtime Registration

When an instance of the object that contains the parameter is created, the parameter is registered with that object using the ```spRegister()``` function. This step connects the object instance with the parameter. 

The calling sequence for spRegister is:

```C++
    spRegister(Object [, Name][,Description]);
```
Where:

* Object - the object to register - either a property or parameter
* Name - Optional human readable name for the object
* Description - Optional human readable description for the object

For the example above, the registration call looks like:

```C++
// MyClass Constructor
MyClass()     
{
    // Register our input parameter
    spRegister(my_input);
    spRegister(my_output, "calibration", "Calibrated data output");
}
```
 