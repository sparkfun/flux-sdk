
# Properties

Properties represent the "_settings_" for a particular object withing the system. This property values describe their object, as well as how the object behaves/operates within the system. 

It's worth noting that properties don't reflect the input or output data from an object within the framework - this managed by *parameter* objects.

### Property Attributes
The following are key attributes of properties within the framework
* Properties can be added to any classed derived from the spObject class.
* Properties can be read (get) and written (set) to
* Property objects are typed
* Property objects can act like a variable
* Property objects allow introspection - they can be discovered and manipulated at runtime via software

#### Property Types
The following types are available for properties
* bool
* int
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
Within the definition of a class the property is for, the property is define using the following pattern:
```C++
    spPropertyType<ClassName>  property_name;
```
Where:
* spPropertyType - the type of the property class being used. 
* ClassName - the class name that the property is for. The name of the class type the property is being defined in. 

Available Property Types:

* spPropertyBool
* spPropertyInt
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
Within the definition of a class the property is for, the property is define using the following pattern:

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
* spPropertyRWInt  - integer property
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
 