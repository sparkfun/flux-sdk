
# Parameters

Parameters represent the method to read or pass in a specific data  _value_ to an operation object  withing the SDK. Parameters can be thought of as the data values passed into or returned from a function call. Parameter objects provide a means to support dynamically discoverable input and output data for a particular operation within the framework.

There are two types of Parameter objects with the framework: Input Parameters and Output Parameters

### Parameter Attributes
The following are key attributes of parameters within the framework
* Parameters can be added to any classed derived from the flxOperation class
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
flxParameterInType<ClassName, &ClassName::Writer>  input_name = {optional data limit values};
```
Where:
* flxParameterInType - the type of the input parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Writer - the name of the _write_ method the parameter should call when it's value is set. **NOTE**: A reference, `& operator`, to the writer is provided

##### Available Input Parameter Types:

* flxParameterInBool - bool parameter
* flxParameterInInt8  - integer8 parameter
* flxParameterInInt16  - integer16 parameter
* flxParameterInInt  - integer parameter
* flxParameterInUint8 - unsigned integer8
* flxParameterInUint16 - unsigned integer15
* flxParameterInUint - unsigned integer
* flxParameterInFloat - float
* flxParameterInDouble - double
* flxParameterInString - string -> std::string

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
class MyClass : public flxObject
{
private:
    void write_myInput(int value);

public:
   // Define an input parameter
   flxParameterInInt<MyClass, &MyClass::write_MyInput>  my_input;
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
    flxParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = { -28, 144 };
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
flxParameterOutType<ClassName, &ClassName::Reader>  output_name;
```
Where:
* flxParameterOutType - the type of the output parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Reader - the name of the _read_ method the parameter should call when it's value is retrieved. **NOTE**: A reference, `& operator`, to the reader is provided

##### Available Output Parameter Types:

* flxParameterOutBool - bool parameter
* flxParameterOutInt8  - integer 8 parameter
* flxParameterOutInt16  - integer 16 parameter
* flxParameterOutInt  - integer parameter
* flxParameterOutUint8 - unsigned 8 integer
* flxParameterOutUint16 - unsigned 16 integer
* flxParameterOutUint - unsigned integer
* flxParameterOutFloat - float
* flxParameterOutDouble - double
* flxParameterOutString - string -> std::string

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
class MyClass : public flxObject
{
private:
    int read_myOutput(void);

public:
   // Define an output parameter
   flxParameterOutInt<MyClass, &MyClass::read_MyOutput>  my_output;
}
```
Note
> * By convention declaring the output reader method as private. This can be optional
> * The reader method must be declared before defining the parameter
> * The use of the `read_` prefix on the writer methods help identify the methods as supporting a parameter.

### Output Array Parameters

For outputs that have array values, an array output parameter is declared. These are declared and operate in a similar fashion as scalar output parameters.

#### Declaring the Output Array Parameter
Within the definition of the class the parameter is for, the output array parameter is defined using the following pattern:

```C++
flxParameterOutArrayType<ClassName, &ClassName::Reader>  output_name;
```
Where:
* flxParameterOutArrayType - the type of the output parameter class to use 
* ClassName - the class name that the parameter is for. The name of the class type the parameter is being defined in. 
* Reader - the name of the _read_ method the parameter should call when it's value is retrieved. **NOTE**: A reference, `& operator`, to the reader is provided

##### Available Output Parameter Types:

* flxParameterOutArrayBool - bool parameter
* flxParameterOutArrayInt8  - integer 8 parameter
* flxParameterOutArrayInt16  - integer 16 parameter
* flxParameterOutArrayInt  - integer parameter
* flxParameterOutArrayUint8 - unsigned 8 integer
* flxParameterOutArrayUint16 - unsigned 16 integer
* flxParameterOutArrayUint - unsigned integer
* flxParameterOutArrayFloat - float
* flxParameterOutArrayDouble - double
* flxParameterOutArrayString - string -> std::string

##### Reader Methods

These methods are implemented on the containing class and are called when the value of a parameter is requested. These methods have the following signature:

```C++
bool ClassName::read_Name(flxDataArray<type> *array);
```
Where

* array  -  a pointer to an flxDataArray<type> object. Type is the type of the parameter (int, float, double ...etc)
* ClassName - the name of the containing class for the property
* On success, a true value is returned, false on error.

Note
>By convention, reader method names are prefixed by ```read_```

In the reader methods, the data and dimensions of the array are set in the array object.

```C++
   array->set(myArrayData, X );
```

or for two dimensional arrays...

```C++
   array->set(myArrayData, X, Y );
```

Note:
> * By default the set method will make a copy of the data array passed in. 
> * To prevent the array copy, pass in the optional no_copy parameter set to `true`

```C++
    array->set(myArrayData, X, true);
```

##### Example

```C++
class MyClass : public flxObject
{
private:
    bool read_myOutput(flxDataArrayInt *array);

public:
   // Define an output parameter
   flxParameterOutArrayInt<MyClass, &MyClass::read_MyOutput>  my_output;
}
```
Note
> * By convention declaring the output reader method as private. This can be optional
> * The reader method must be declared before defining the parameter
> * The use of the `read_` prefix on the writer methods help identify the methods as supporting a parameter.

### Runtime Registration

When an instance of the object that contains the parameter is created, the parameter is registered with that object using the ```flxRegister()``` function. This step connects the object instance with the parameter. 

The calling sequence for flxRegister is:

```C++
    flxRegister(Object [, Name][,Description]);
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
    flxRegister(my_input);
    flxRegister(my_output, "calibration", "Calibrated data output");
}
```
 