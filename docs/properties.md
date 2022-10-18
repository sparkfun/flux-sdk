
# Properties

Properties represent the "_settings_" for a particular object within the system. This property values describe their object, as well as how the object behaves/operates within the system. 

It's worth noting that properties don't reflect the input or output data from an object within the framework - this managed by *parameter* objects.

### Property Attributes
The following are key attributes of properties within the framework
* Properties can be added to any classed derived from the spObject class
* Properties can be read (get) and written (set) to
* Property objects are typed
* Property objects can act like a variable
* Property objects allow introspection - they can be discovered and manipulated at runtime via software

#### Property Types
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

#### Property Use
Setting an value:
```c++
    anObject.property = value;
    anObject.property(value); 
    anObject.property.set(value);
```
Getting a value:
```C++
    value = anObject.property;
    value = anObject.property();
    value = anObject.property.get();
```

### Defining a Property
For the framework, two types of property classes exist. 
* Standard Property - Defines a property that acts like a variable
* Read/Write Property - Defines a property that calls a ```get()``` method on a value request and calls a ```set()``` method when it's value is set. 

### Standard Property Objects
These property objects define a typed property and provided storage for this property. As such, they act like a instance variable for their containment class. 

#### Declaring the Property
Within the definition of the class the property is for, the property is defined using the following pattern:
```C++
    spPropertyType<ClassName>  property_name = {optional initial value, optional data limit};
```
Where:
* spPropertyType - the type of the property class being used. 
* ClassName - the class name that the property is for. The name of the class type the property is being defined in. 

Available Property Types:

* spPropertyBool
* spPropertyInt8
* spPropertyInt16
* spPropertyInt
* spPropertyUint8
* spPropertyUint16
* spPropertyUint
* spPropertyFloat
* spPropertyDouble
* spPropertyString

##### Example

```C++
class MyClass : public spObject
{
public:
   // Define my property
   spPropertyInt<MyClass>  my_property;

}
```

#### Setting an Initial Value

The initial value for a property can be set in it's declaration statement by assigning the desired value to the declared variable. The value is set using a standard C++ initialization list syntax - aka `{}` braces.

In the above example, setting an initial value of 42 to *my_property* looks like:

```C++
class MyClass : public spObject
{
public:
   // Define my property
   spPropertyInt<MyClass>  my_property = {42};

}
```

#### Runtime Registration

When an instance of the object that contains the property is created, the property is registered with that object using the ```spRegister()``` function. This step connects the object instance with the property. 

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
    // Register our property
    spRegister(my_property);
}
```

### Read/Write (RW) Property Objects
These property objects define a typed property and required a get and set method be provided to enable reading/writing of the property value. 

By calling methods on read and write of the property, the Read/Write property objects allow for the immediate response to a property operation. 

#### Declaring the Property
Within the definition of a class the property is for, the property is defined using the following pattern:

```C++
spPropertyRWType<ClassName, &ClassName::Getter, &ClassName::Setter>  property_name;
```
Where:
* spPropertyRWType - the type of the property class being used. 
* ClassName - the class name that the property is for. The name of the class type the property is being defined in. 
* Getter - the name of the _get_ method the property should call when it's value is requested. **NOTE**: A reference, `& operator`, to the getter is provided
* Setter - the name of the _set_ method the property should call when it's value is set.  **NOTE**: A reference, `& operator`, to the getter is provided

##### Available Property Types:

* spPropertyRWBool - bool property
* spPropertyRWInt8  - integer 8 property
* spPropertyRWInt16  - integer 16 property
* spPropertyRWInt  - integer property
* spPropertyRWUint8 - unsigned 8 integer
* spPropertyRWUint16 - unsigned 16 integer
* spPropertyRWUint - unsigned integer
* spPropertyRWFloat - float
* spPropertyRWDouble - double
* spPropertyRWString - string -> std::string

##### Getter Methods

These methods are implemented on the containing class and are called when the value of a property is requested. These methods have the following signature:

```C++
property_type ClassName::get_Name(void);
```
Where

* property_type - the type of the property (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
>By convention, getters method names are prefixed by ```get_```

##### Setter Methods

These methods are implemented on the containing class and are called when the value of a property is set. These methods have the following signature:

```C++
void ClassName::set_Name(property_type value);
```
Where

* property_type - the type of the property (bool, int, uint, float, double, std::string)
* ClassName - the name of the containing class for the property

Note
> By convention, getters method names are prefixed by ```set_```

##### Example

```C++
class MyClass2 : public spObject
{
private:
    int  get_myProp(void);
    void set_myProp(int value);

public:
   // Define my property
   spPropertyRWInt<MyClass2, &MyClass2::get_myProp, &MyClass2::set_myProp>  my_rwproperty;

}
```
Note
> * By convention declaring the getters and setters as private. This can be optional
> * The getter and setter methods must be declared before defining the property
> * The use of `set_` and `get_` prefixes on the setter and getter methods help identify the methods as supporting a property.
> * If an initial value is set for a RW property it it's declaration statement, the *setter* method called with the initial value when the property is registered via *spRegister()*.


#### Runtime Registration

When an instance of the object that contains the property is created, the property is registered with that object using the ```spRegister()``` function. This step connects the object instance with the property. 

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
MyClass2()     
{
    // Register our property
    spRegister(my_rwproperty);
}
```
Note: If an initial value was set for the property, the value is passed to the *setter* method as part of the registration process.

### Property Data Limits 

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

If providing an initial value, the declaration has the form ```{initial value, min, max}```.

```C++
    // Define an input parameter with an initial value of
    // 1, and a range of -29 to 144
    spParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = { 01, -28, 144 };
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

If providing an initial value, the declaration has the form ```{initial value, valid value set}```.

```C++
// Define an initial value of 22, and a valid value set. 
spParameterInInt<MyClass, &MyClass::write_MyInput>  my_input = {22, {
                                        {"Value One", 22},
                                        {"Value Two", 44},
                                        {"Value Three", 66},        
                                        {"Value Four", 88},      
                                        {"Value Five", 110}
                                    } };
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

