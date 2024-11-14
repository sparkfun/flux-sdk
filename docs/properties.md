
# Properties Overview {#properties_overview}

Properties represent the "_settings_" for a particular object within the system. This property values describe their object, as well as how the object behaves/operates within the system.

It's worth noting that properties don't reflect the input or output data from an object within the framework - this managed by _parameter_ objects.

## Property Attributes

The following are key attributes of properties within the framework

* Properties can be added to any classed derived from the flxObject class
* Properties can be read (get) and written (set) to
* Property objects are typed
* Property objects can act like a variable
* Property objects allow introspection - they can be discovered and manipulated at runtime via software

## Property Types

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

### Property Use

Setting an value:

```cpp
    anObject.property = value;
    anObject.property(value); 
    anObject.property.set(value);
```

Getting a value:

```cpp
    value = anObject.property;
    value = anObject.property();
    value = anObject.property.get();
```

## Defining a Property

For the framework, two types of property classes exist.

* Standard Property - Defines a property that acts like a variable
* Read/Write Property - Defines a property that calls a ```get()``` method on a value request and calls a ```set()``` method when it's value is set.

### Different Property "Kinds"

For each of the above types, besides _normal_ property types that are visible and stored as is, there are three different attributes available:

* Hidden - ```flxPropertyHidden<type>```, ```flxPropertyRWHidden<type>``` - The property isn't presented in menu systems, but can be used by an object to store/persist it's value.
* Secure - ```flxPropertySecure<type>```, ```flxPropertyRWSecure<type>``` -The value of the property is encrypted before saving the value. This value is only written internally (not to a public JSON file)
* Secret - ```flxPropertySecret<type>```, ```flxPropertyRWSecret<type>``` - The value is _hidden_ and _secure_.

## Standard Property Objects

These property objects define a typed property and provided storage for this property. As such, they act like a instance variable for their containment class.

### Declaring the Property

Within the definition of the class the property is for, the property is defined using the following pattern:

```cpp
    flxPropertyType<ClassName>  property_name = {optional initial value, optional data limit};
```

Where:

* flxPropertyType - the type of the property class being used.
* ClassName - the class name that the property is for. The name of the class type the property is being defined in.

Available Property Types:

* #flxPropertyBool
* #flxPropertyInt8
* #flxPropertyInt16
* #flxPropertyInt32
* #flxPropertyUInt8
* #flxPropertyUInt16
* #flxPropertyUInt32
* #flxPropertyFloat
* #flxPropertyDouble
* #flxPropertyString

#### Example

```cpp
class MyClass : public flxObject
{
public:
   // Define my property
   flxPropertyInt<MyClass>  my_property;

}
```

### Setting an Initial Value

The initial value for a property can be set in it's declaration statement by assigning the desired value to the declared variable. The value is set using a standard C++ initialization list syntax - aka `{}` braces.

In the above example, setting an initial value of 42 to _my_property_ looks like:

```cpp
class MyClass : public flxObject
{
public:
   // Define my property
   flxPropertyInt<MyClass>  my_property = {42};

}
```

### Runtime Registration

When an instance of the object that contains the property is created, the property is registered with that object using the ```flxRegister()``` function. This step connects the object instance with the property.

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
    // Register our property
    flxRegister(my_property, "this is my favorite property");
}
```

> Internally, the flxRegister() call makes the containing object aware of the property object - adding it to an internal _property list_. This allows the system to enumerate properties at runtime as part of an introspection process.

## Read/Write (RW) Property Objects

These property objects define a typed property and required a get and set method be provided to enable reading/writing of the property value.

By calling methods on read and write of the property, the Read/Write property objects allow for the immediate, dynamic response to a property operation.

### Declaring Read/Write the Property

Within the definition of a class the property is for, the property is defined using the following pattern:

```cpp
flxPropertyRWType<ClassName, &ClassName::Getter, &ClassName::Setter>  property_name;
```

Where:

* flxPropertyRWType - the type of the property class being used.
* ClassName - the class name that the property is for. The name of the class type the property is being defined in.
* Getter - the name of the _get_ method the property should call when it's value is requested. **NOTE**: A reference, `& operator`, to the getter is provided
* Setter - the name of the _set_ method the property should call when it's value is set.  **NOTE**: A reference, `& operator`, to the getter is provided

#### Available Property Types

* #flxPropertyRWBool - bool property
* #flxPropertyRWInt8  - integer 8 property
* #flxPropertyRWInt16  - integer 16 property
* #flxPropertyRWInt32  - integer32 property
* #flxPropertyRWUInt8 - unsigned 8 integer
* #flxPropertyRWUInt16 - unsigned 16 integer
* #flxPropertyRWUInt32 - unsigned 32 integer
* #flxPropertyRWFloat - float
* #flxPropertyRWDouble - double
* #flxPropertyRWString - string -> std::string

#### Getter Methods

These methods are implemented on the containing class and are called when the value of a property is requested. These methods have the following signature:

```cpp
property_type ClassName::get_Name(void);
```

Where

* property_type - the type of the property (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
>By convention, getters method names are prefixed by ```get_```

#### Setter Methods

These methods are implemented on the containing class and are called when the value of a property is set. These methods have the following signature:

```cpp
void ClassName::set_Name(property_type value);
```

Where

* property_type - the type of the property (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
> By convention, getters method names are prefixed by ```set_```

#### Example {#rw-props-example}

```cpp
class MyClass2 : public flxObject
{
private:
    int  get_myProp(void);
    void set_myProp(int value);

public:
   // Define my property
   flxPropertyRWInt<MyClass2, &MyClass2::get_myProp, &MyClass2::set_myProp>  my_rwproperty;

}
```

> * By convention the getters and setters are declared as private. This can be optional
> * The getter and setter methods must be declared before defining the property
> * The use of `set_` and `get_` prefixes on the setter and getter methods help identify the methods as supporting a property.
> * If an initial value is set for a RW property it it's declaration statement, the _setter_ method called with the initial value when the property is registered via _flxRegister()_.

### RW Property Runtime Registration

When an instance of the object that contains the property is created, the property is registered with that object using the ```flxRegister()``` function. This step connects the object instance with the property.

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
MyClass2()     
{
    // Register our property
    flxRegister(my_rwproperty, "my read-write property" );
}
```

Note: If an initial value was set for the property, the value is passed to the _setter_ method as part of the registration process.

## Property Data Limits

### Data Limit Values

Data limits define restrictions on the values the input parameter accepts. There are two types of data limits: range and valid value sets.

_Data Range_
This represents the minimum and maximum values a input parameter will accept. The values can be specified at property definition (the preferred method) and also set at runtime.

To set the range at parameter definition, just set the declared parameter to the range using a C++ initializer list ```{ min, max}```

Additionally, the method `clearDataLimit()` can be called to delete the current limit.

A range examples for properties:

```cpp
    // System sleep properties
    flxPropertyUint<sfeDataLogger> sleepInterval = {5, 86400};

    // Serial Baud rate setting
    flxPropertyRWUint<sfeDataLogger, &sfeDataLogger::get_termBaudRate, &sfeDataLogger::set_termBaudRate>
        serialBaudRate = {1200, 500000};
```

If providing an initial value, the declaration has the form ```{initial value, min, max}```.

```cpp
    // Define a Property with an initial value of
    // 1, and a range of -29 to 144
    flxPropertyInInt<MyClass, &MyClass::write_MyInput>  my_input = { 01, -28, 144 };
```

To set/change the range value at runtime, the method ```setDataLimitRange(min, max)``` is called on the object.

For Example:

```cpp
    // change the data range
    my_input.setDataLimitRange(100, 198);
```

This changes the data range accepted by the object and deletes any existing data limit.

_Data Valid Value Set_

This represents data limit provides a defined set of valid values for the property. The limit is defined by a set of _name, value_ pairs that enable a human readable presentation for the values a property will accept. The values can be specified at property definition and also set at runtime.

To set the valid values at property definition, just set the declared property to the range using a C++ initializer list of name value pairs:  

```cpp
    {
        { NAME0, value0},
        { NAME1, value1},
        ...
        };
```

An example of a Valid Value Set - note an initial value is provided in this example:

```cpp

    // Define our read-write properties
    // binaryGas is STC3X_binary_gas_type_e. Default is STC3X_BINARY_GAS_CO2_AIR_25
    flxPropertyRWUint8<flxDevSTC31, &flxDevSTC31::get_binary_gas, &flxDevSTC31::set_binary_gas> binaryGas
        = { STC3X_BINARY_GAS_CO2_AIR_25, { { "CO2 in N2 (100% max)", STC3X_BINARY_GAS_CO2_N2_100 },
                                           { "CO2 in Air (100% max)", STC3X_BINARY_GAS_CO2_AIR_100 },
                                           { "CO2 in N2 (25% max)", STC3X_BINARY_GAS_CO2_N2_25 },
                                           { "CO2 in Air (25% max)", STC3X_BINARY_GAS_CO2_AIR_25 } } };

```

To set/change the range value at runtime, the method ```addDataLimitValidValue()``` is called on the property object. This object has two calling sequences:

* Called with a name and a value
* Called with a name value list, similar to the above initializer list.

Additionally, the method `clearDataLimit()` can be called to delete the current limit

Simple Example:

```cpp
    // Add valid values ...
    my_property.addDataLimitValidValue("ONE K", 100.);
    my_property.addDataLimitValidValue("TWO K", 200.);    
```

Or for an entire parameter list:

```cpp
    my_property.addDataLimitValidValue( {
                                        {"Value One", 22},
                                        {"Value Two", 44},
                                        {"Value Three", 66},        
                                        {"Value Four", 88},      
                                        {"Value Five", 10}                       
                                        });
```

The values are added to the current valid value list. If a _ValidValue_ data limit was not in place when called, any current limit (i.e. range limit) is deleted and a valid value limit is put in place.
