# Parameters Overview  {#parameters_overview}

Parameters represent the method to read or pass in a specific data  _value_ to an operation object  withing the SDK. Parameters can be thought of as the data values passed into or returned from a function call. Parameter objects provide a means to support dynamically discoverable input and output data for a particular operation within the framework.

There are two types of Parameter objects with the framework: Input Parameters ("functions") and Output Parameters

## Parameter Attributes

The following are key attributes of parameters within the framework

* Parameters can be added to any classed derived from the flxOperation class
* Parameter objects are typed
* Parameter objects can act like a function
* Parameter objects allow introspection - they can be discovered and manipulated at runtime via software

### Parameter Data Types

The following types are available for properties

* bool
* int8
* int16
* int32
* uint8
* uint16
* uint32
* float
* double
* string

#### Parameter Use

Setting an value of a Input Parameter - named ```input``` in this example:

```cpp
    anObject.input(value);
    anObject.input.set(value);
```

Getting a value of an Output Parameter - named ```output``` in this example:

```cpp
    value = anObject.output();
    value = anObject.output.get();
```

### Defining a Parameter

For the framework, two types of Parameter classes exist.

* Input Parameter - Defines a Parameter to set input value to an operation. These are referred to as functions at times - especially in the user experience of an application.
* Output Parameter - Defines a Parameter to get an output value from an operation

> The get and set operations on a parameter are mapped to methods implemented by the containing class.

### Input Parameter Objects

These parameter objects are used to define a input parameter to an operation. Besides allowing introspection at runtime, they also pass on the provided value to a method.

#### Declaring the Input Parameter

Within the definition of the class the parameter is for, the input parameter is defined using the following pattern:

```cpp
flxParameterInType<ClassName, &ClassName::Writer>  input_name = {optional data limit values};
```

Where:

* flxParameterInType - the type of the input parameter class to use
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in.
* Writer - the name of the _write_ method the parameter should call when it's value is set. **NOTE**: A reference, `& operator`, to the writer is provided

##### Available Input Parameter Types

* #flxParameterInBool - bool parameter
* #flxParameterInInt8  - integer8 parameter
* #flxParameterInInt16  - integer16 parameter
* #flxParameterInInt32  - integer parameter
* #flxParameterInUInt8 - unsigned integer8
* #flxParameterInUInt16 - unsigned integer15
* #flxParameterInUInt32 - unsigned integer
* #flxParameterInFloat - float
* #flxParameterInDouble - double
* #flxParameterInString - string -> std::string

##### Writer Methods

These methods are implemented on the containing class and are called when the value of a parameter is set. These methods have the following signature:

```cpp
void ClassName::write_Name(parameter_type value);
```

Where:

* parameter_type - the type of the parameter (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

> By convention, writer method names are prefixed by ```write_```

##### Example

```cpp
class MyClass : public flxObject
{
private:
    void write_myInput(int value);

public:
   // Define an input parameter
   flxParameterInInt<MyClass, &MyClass::write_MyInput>  my_input;
}
```

> * By convention declaring the input writer method as private. This can be optional
> * The writer method must be declared before defining the parameter
> * The use of the `write_` prefix on the writer methods help identify the methods as supporting a parameter.

#### Data Limit Values

Data limits define restrictions on the values the input parameter accepts. There are two types of data limits: range and valid value sets.

_Data Range_

This represents the minimum and maximum values a input parameter will accept. The values can be specified at parameter definition and also set at runtime.

To set the range at parameter definition, just set the declared parameter to the range using a cpp initializer list ```{ min, max}```

Additionally, the method `clearDataLimit()` can be called to delete the current limit.

Using the example from above:

```cpp
    // Define an input parameter with a range of -29 to 144
    flxParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = { -28, 144 };
```

To set/change the range value at runtime, the method ```setDataLimitRange(min, max)``` is called on the input parameter object.

Using the example parameter from above:

```cpp
    // change the data range
    my_input.setDataLimitRange(100, 198);
```

This changes the data range accepted by the input parameter and deletes any existing data limit.

_Data Valid Value Set_

This represents data limit provides a defined set of valid values for the input parameter. The limit is defined by a set of _name,value_ pairs that enable a human readable presentation for the values a input parameter will accept. The values can be specified at parameter definition and also set at runtime.

To set the valid values at parameter definition, just set the declared parameter to the range using a cpp initializer list of name value pairs:  

```cpp
    {
        { NAME0, value0},
        { NAME1, value1},
        ...
        };
```

Using the example from above:

```cpp
// Define an input parameter with a range of -29 to 144
flxParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = {
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

```cpp
    // Add valid values ...
    my_input.addDataLimitValidValue("ONE K", 100.);
    my_input.addDataLimitValidValue("TWO K", 200.);    
```

Or for an entire parameter list:

```cpp
    my_input.addDataLimitValidValue( {
                                        {"Value One", 22},
                                        {"Value Two", 44},
                                        {"Value Three", 66},        
                                        {"Value Four", 88},      
                                        {"Value Five", 10}                       
                                        });
```

The values are added to the current valid value list. If a _ValidValue_ data limit was not it i place when called, the current limit is deleted and a valid value limit is put in place.

### Output Parameter Objects

These parameter objects are used to define a output parameter to an operation. Besides allowing introspection at runtime, they also retrieve the desired value from a method.

#### Declaring the Output Parameter

Within the definition of the class the parameter is for, the output parameter is defined using the following pattern:

```cpp
flxParameterOutType<ClassName, &ClassName::Reader>  output_name;
```

Where:

* flxParameterOutType - the type of the output parameter class to use
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in.
* Reader - the name of the _read_ method the parameter should call when it's value is retrieved. **NOTE**: A reference, `& operator`, to the reader is provided

##### Available Output Parameter Types

* #flxParameterOutBool - bool parameter
* #flxParameterOutInt8  - integer 8 parameter
* #flxParameterOutInt16  - integer 16 parameter
* #flxParameterOutInt  - integer parameter
* #flxParameterOutUint8 - unsigned 8 integer
* #flxParameterOutUint16 - unsigned 16 integer
* #flxParameterOutUint - unsigned integer
* #flxParameterOutFloat - float
* #flxParameterOutDouble - double
* #flxParameterOutString - string -> std::string

##### Reader Methods

These methods are implemented on the containing class and are called when the value of a parameter is requested. These methods have the following signature:

```cpp
parameter_type ClassName::read_Name(void);
```

Where:

* parameter_type - the type of the parameter (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

> [!note]
>
> By convention, reader method names are prefixed by ```read_```

##### Example {#param-out-example}

```cpp
class MyClass : public flxObject
{
private:
    int read_myOutput(void);

public:
   // Define an output parameter
   flxParameterOutInt<MyClass, &MyClass::read_MyOutput>  my_output;
}
```

> * By convention declaring the output reader method as private. This can be optional
> * The reader method must be declared before defining the parameter
> * The use of the `read_` prefix on the writer methods help identify the methods as supporting a parameter.

### Output Array Parameters

For outputs that have array values, an array output parameter is declared. These are declared and operate in a similar fashion as scalar output parameters.

#### Declaring the Output Array Parameter

Within the definition of the class the parameter is for, the output array parameter is defined using the following pattern:

```cpp
flxParameterOutArrayType<ClassName, &ClassName::Reader>  output_name;
```

Where:

* flxParameterOutArrayType - the type of the output parameter class to use
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in.
* Reader - the name of the _read_ method the parameter should call when it's value is retrieved. **NOTE**: A reference, `& operator`, to the reader is provided

##### Available Output Parameter Types {#output-param-types}

* #flxParameterOutArrayBool - bool parameter
* #flxParameterOutArrayInt8  - integer 8 parameter
* #flxParameterOutArrayInt16  - integer 16 parameter
* #flxParameterOutArrayInt  - integer parameter
* #flxParameterOutArrayUInt8 - unsigned 8 integer
* #flxParameterOutArrayUInt16 - unsigned 16 integer
* #flxParameterOutArrayUInt - unsigned integer
* #flxParameterOutArrayFloat - float
* #flxParameterOutArrayDouble - double
* #flxParameterOutArrayString - string -> std::string

##### Reader Methods {#reader-methods-sig}

These methods are implemented on the containing class and are called when the value of a parameter is requested. These methods have the following signature:

```cpp
bool ClassName::read_Name(flxDataArray<type> *array);
```

Where

* array  -  a pointer to an flxDataArray<type> object. Type is the type of the parameter (int, float, double ...etc)
* ClassName - the name of the containing class for the property
* On success, a true value is returned, false on error.

> By convention, reader method names are prefixed by ```read_```

In the reader methods, the data and dimensions of the array are set in the array object.

```cpp
   array->set(myArrayData, X );
```

or for two dimensional arrays...

```cpp
   array->set(myArrayData, X, Y );
```

> [!note]
>
> * By default the set method will make a copy of the data array passed in.
> * To prevent the array copy, pass in the optional no_copy parameter set to `true`

```cpp
    array->set(myArrayData, X, true);
```

##### Example  {#reader-method-example}

```cpp
class MyClass : public flxObject
{
private:
    bool read_myOutput(flxDataArrayInt *array);

public:
   // Define an output parameter
   flxParameterOutArrayInt<MyClass, &MyClass::read_MyOutput>  my_output;
}
```

> * By convention declaring the output reader method as private. This can be optional
> * The reader method must be declared before defining the parameter
> * The use of the `read_` prefix on the writer methods help identify the methods as supporting a parameter.

### Runtime Registration

When an instance of the object that contains the parameter is created, the parameter is registered with that object using the ```flxRegister()``` function. This step connects the object instance with the parameter.

The calling sequence for flxRegister is:

```cpp
    flxRegister(Object [, Name][,Description]);
```

Where:

* Object - the object to register - either a property or parameter
* Name - Optional human readable name for the object
* Description - Optional human readable description for the object

For the example above, the registration call looks like:

```cpp
// MyClass Constructor
MyClass()     
{
    // Register our input parameter
    flxRegister(my_input);
    flxRegister(my_output, "calibration", "Calibrated data output");
}
```
