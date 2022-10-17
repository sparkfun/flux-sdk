
# Parameters

Parameters represent the method to read or pass in a specific data  _value_ to an operation object  withing the SDK. Parameters can be thought of as the data values passed into or returned from a function call. Parameter objects provide a means to support dynamically discoverable input and output data for a particular operation within the framework.

There are two types of Parameter objects with the framework: Input Parameters and Output Parameters

### Parameter Attributes
The following are key attributes of parameters within the framework
* Parameters can be added to any classed derived from the spOperation class
* Parameter objects are typed
* Parameter objects can act like a function
* Parameter objects allow introspection - they can be discovered and manipulated at runtime via software

#### Parameter Data Types
The following types are available for properties
* bool
* int8
* int16
* int
* uint8
* uint16
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
spParameterInType<ClassName, &ClassName::Writer>  input_name = {optional data limit values};
```
Where:
* spParameterInType - the type of the input parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Writer - the name of the _write_ method the parameter should call when it's value is set. **NOTE**: A reference, `& operator`, to the writer is provided

##### Available Input Parameter Types:

* spParameterInBool - bool parameter
* spParameterInInt8  - integer8 parameter
* spParameterInInt16  - integer16 parameter
* spParameterInInt  - integer parameter
* spParameterInUint8 - unsigned integer8
* spParameterInUint16 - unsigned integer15
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

#### Data Limit Values
Data limits define restrictions on the values the input parameter accepts. There are two types of data limits: range and valid value sets.

*Data Range*
This represents the minimum and maximum values a input parameter will accept. The values can be specified at parameter definition and also set at runtime. 

To set the range at parameter definition, just set the declared parameter to the range using a C++ initializer list ```{ min, max}```

Additionally, the method `clearDataLimit()` can be called to delete the current limit.

Using the example from above:
```C++
    // Define an input parameter with a range of -29 to 144
    spParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = { -28, 144 };
```

To set/change the range value at runtime, the method ```setDataLimitRange(min, max)``` is called on the input parameter object.

Using the example parameter from above:
```C++
    // change the data range
    my_input.setDataLimitRange(100, 198);
```

This changes the data range accepted by the input parameter and deletes any existing data limit.

*Data Valid Value Set*
This represents data limit provides a defined set of valid values for the input parameter. The limit is defined by a set of *name,value* pairs that enable a human readable presentation for the values a input parameter will accept. The values can be specified at parameter definition and also set at runtime. 

To set the valid values at parameter definition, just set the declared parameter to the range using a C++ initializer list of name value pairs:  
```
    {
        { NAME0, value0},
        { NAME1, value1},
        ...
        };
```

Using the example from above:
```C++
// Define an input parameter with a range of -29 to 144
spParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = {
                                        {"Value One", 22},
                                        {"Value Two", 44},
                                        {"Value Three", 66},        
                                        {"Value Four", 88},      
                                        {"Value Five", 110}
                                    };
```

To set/change the range value at runtime, the method ```addDataLimitValidValue()``` is called on the input parameter object. This object has two calling sequences:

* Called with a name and a value
* Called with a name value list, similar to the above initializer list.

Additionally, the method `clearDataLimit()` can be called to delete the current limit

Using the example parameter from above:
```C++
    // Add valid values ...
    my_input.addDataLimitValidValue("ONE K", 100.);
    my_input.addDataLimitValidValue("ONE K", 100.);    
```

Or for an entire parameter list:

```C++
    my_input.addDataLimitValidValue( {
                                        {"Value One", 22},
                                        {"Value Two", 44},
                                        {"Value Three", 66},        
                                        {"Value Four", 88},      
                                        {"Value Five", 10}                       
                                        });
```

The values are added to the current valid value list. If a *ValidValue* data limit was not it i place when called, the current limit is deleted and a valid value limit is put in place.



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
 