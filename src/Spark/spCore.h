

#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

using std::string;

// Testing
#include <ArduinoJson.h>

#include "spStorage.h"

// Handy macro
#define variable_name(A) #A

#define debug_message(_prompt_, _value_)                                                                               \
    {                                                                                                                  \
        Serial.print(_prompt_);                                                                                        \
        Serial.println(_value_);                                                                                       \
    }

// TODO clean up messaging
#define error_message(_message_) debug_message("[Error] - ", _message_);

#define warning_message(_message_) debug_message("[Warning] - ", _message_);

//-------------------------------------------------------------------------
// Storage interface
//-------------------------------------------------------------------------
struct spIPersist
{

    virtual bool save(spStorageBlock *stBlk) = 0;
    virtual bool restore(spStorageBlock *stBlk) = 0;
};

//-------------------------------------------------------------------------
// Managed Properties -  definitions
//-------------------------------------------------------------------------
//
// The goal is to create a simple property system that supports a level of introspection
// and automates the setting/getting of simple scalar properties.
//
// MANAGED PROPERTIES
// ------------------
//
// Each managed property is peformed using a simple *proxy* object, which enables
// simple property introspection for the object. The object provides the
// following:
//
//     - Stores name of the property
//     - Implements operator overloading to allow assignment (set) or access (get)
//       of the underlying property value. This allows simplified property access
//			i.e.    object.property = value    (set)
//					value = object.property    (get)
//     - Implements a save/restore methodology - to save property state to EEPROM
//     - Call back the owner object when a property is changed.
//	   - Introspection - Enumerate object names and types
//
// DEFINITION AND SETUP
// --------------------
//
// The system consists of a property object, which contains the property storage, set/get
// methods, and save/restore logic.
//
// Example of definition:
//
//    For the following desired properties:
//
//          bool      humidity
//          bool      pressure
//          int8_t    temperature
//          bool      celsius
//
//    The following property objects are declared in the class definition:
//
//          spPropertyBool humidity;
//          spPropertyBool pressure;
//          spPropertyInt8 temperature;
//          spPropertyBool celsius;
////
//    Initialization:
//
//    In the main object's constructor, attributes and default values for the
//    property objects are setup. Additionally, the property is registered with
//    the superclass - this enables simple introspection as well as state serialization.
//
//    Example setup calls for the humidity property:
//
//    MyObject::MyObject(){
//
//         // set target object (this), name and storage in the property object.
//         humidty.initWithDefault(this, "humidty", &_settings.name, 20);
//
//         // Add this property object to the list of managed properties
//		   this->addProperty(humidty);
//    }
//
//    These steps are encapsulated in a macro, so the actual setup call is:
//
//    Example - simplified setup call:
//
//    MyObject::MyObject(){
//
//        // Register property with system, set default value
//        spRegisterPropertyWithDefault(humidity, 20);
//    }
//
// Enums of data types - This enum is ussed to determine a type of an object using polymorphism -
//                       a virtual method is called for an objects type - subclasses override this method and
//                        return the type enum for the property implemented.
//
enum DataTypes
{
    TypeNone,
    TypeBool,
    TypeInt,
    TypeFloat,
    TypeDouble,
    TypeString
};

//#################################################################################
// Refactor
typedef enum
{
    spTypeNone,
    spTypeBool,
    spTypeInt,
    spTypeUInt,
    spTypeFloat,
    spTypeDouble,
    spTypeString
} spDataType_t;

class spDataTyper
{
    // some method overloading to determine types
    static spDataType_t type(std::nullptr_t *t)
    {
        return spTypeNone;
    };
    static spDataType_t type(bool *t)
    {
        return spTypeBool;
    };
    static spDataType_t type(int *t)
    {
        return spTypeInt;
    };
    static spDataType_t type(uint *t)
    {
        return spTypeUInt;
    };
    static spDataType_t type(float *t)
    {
        return spTypeFloat;
    };
    static spDataType_t type(double *t)
    {
        return spTypeDouble;
    };
    static spDataType_t type(std::string *t)
    {
        return spTypeString;
    };

    // non pointer
    static spDataType_t type(bool &t)
    {
        return type(&t);
    };
    static spDataType_t type(int &t)
    {
        return type(&t);
    };
    static spDataType_t type(uint &t)
    {
        return type(&t);
    };
    static spDataType_t type(float &t)
    {
        return type(&t);
    };
    static spDataType_t type(double &t)
    {
        return type(&t);
    };
    static spDataType_t type(std::string &t)
    {
        return type(&t);
    };
};

//#################################################################################
// these are used in the object property intrface.
class spPropertyBase;
using spPropertyList = std::vector<spPropertyBase *>;

//////////////////////////////////////////////////////////////////////
// Object Property Interface
//////////////////////////////////////////////////////////////////////
class spIProperty : public spIPersist
{

  public:
    // get/set property

    template <typename T> bool getProperty(const char *name, T &value);
    template <typename T> bool setProperty(const char *name, T &value);

    // Method called when a managed property is updated.
    virtual void onPropertyUpdate(const char *){};

  protected:
    friend spPropertyBase; // when a prop is registered, it adds itself to our list.

    // persitence methods
    virtual bool save(spStorageBlock *sBLK);
    virtual bool restore(spStorageBlock *sBLK);
    size_t save_size(void);

    // Method for a sub-class to add a managed property
    void addProperty(spPropertyBase *newProperty)
    {
        // TODO: Check for dups.
        _myProps.push_back(newProperty);
    };
    // reference version...
    void addProperty(spPropertyBase &newProperty)
    {
        addProperty(&newProperty);
    };

    spPropertyList _myProps;
};

//------------------------------------------------------------------------
// core class to define a typed value

class spDataCore
{

  public:
    const char *name; // TODO - IS THIS NEEDED - KDB

    // type method - how a property's type is determined at runtime.
    virtual DataTypes type(void)
    {
        return TypeNone;
    }

    // methods to get the value of a data item. These are all virtual,
    // with the expectation that the sub-class will fill in with the
    // correct methods that they support...

    virtual bool getBool() = 0;
    virtual int getInt() = 0;
    virtual float getFloat() = 0;
    virtual std::string getString() = 0;
    const char *getCString()
    {
        return getString().c_str();
    };

  protected:
    // some method overloading to determine types
    inline DataTypes _getType(std::nullptr_t *t)
    {
        return TypeNone;
    };
    inline DataTypes _getType(bool *t)
    {
        return TypeBool;
    };
    inline DataTypes _getType(int *t)
    {
        return TypeInt;
    };
    inline DataTypes _getType(float *t)
    {
        return TypeFloat;
    };
    inline DataTypes _getType(double *t)
    {
        return TypeDouble;
    };
    inline DataTypes _getType(std::string *t)
    {
        return TypeString;
    };

    std::string &to_string(std::string &data)
    {
        return data;
    }

    std::string to_string(int data)
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%d", data);
        std::string stmp = szBuffer;
        return stmp;
    }

    std::string to_string(float data)
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%f", data);
        std::string stmp = szBuffer;
        return stmp;
    }
    std::string to_string(bool data)
    {
        std::string stmp;
        stmp = data ? "true" : "false";
        return stmp;
    }
};

using spDataCoreList = std::vector<spDataCore *>;

//------------------------------------------------------------------------

// Base class for our manage properites. This allows for easy storage of a list
// of property objects, implementation of a name instance variable and definition of
// the type method.

class spPropertyBase : public spIPersist, public spDataCore
{

  public:
    virtual size_t size(void)
    {
        return 0;
    };
    virtual size_t save_size(void)
    {
        return 0;
    }; // number of bytes used to persist value
    virtual bool save(spStorageBlock *stBlk) = 0;
    virtual bool restore(spStorageBlock *stBlk) = 0;

    // initialize the property
    // pass in callback object, and name of the the property
    void initialize(spIProperty *pTarget, const char *name)
    {
        _pTarget = pTarget;
        this->name = name;

        // Add this property to the target
        pTarget->addProperty(this);
    }

    // get the value of a property, stash in JSON
    virtual void getValue(const JsonVariant &) = 0;

  protected:
    // sub-classes call this to dispatch message to target object
    void onPropertyUpdate(void)
    {
        if (_pTarget)
            _pTarget->onPropertyUpdate(this->name);
    }

  protected:
    spIProperty *_pTarget;
};

//##############################################################################################################################
//##############################################################################################################################
// Sept 22 Refactor work
//##############################################################################################################################
//##############################################################################################################################
//
// Note: '2' used during dev to prevent symbol collisions
//
//----------------------------------------------------------------------------------------
// spDescriptor
//
// Simple class that can be mixed-in to add a common name and description string
// to user "exposed" objects in the framework..

class spDescriptor
{
  public:
    spDescriptor() : name{""}, description{""}
    {
    }

    std::string name;
    std::string description;
};

//----------------------------------------------------------------------------------------
// spDataOut
//
// Interface to get outputs from an object.
class spDataOut
{

  public:
    virtual operator bool() const = 0;
    virtual operator int() const = 0;
    virtual operator uint() const = 0;
    virtual operator float() const = 0;
    virtual operator double() const = 0;
    virtual operator std::string() const = 0;
    virtual operator char *() const = 0;

    std::string &to_string(std::string &data) const
    {
        return data;
    }

    const std::string &to_string(std::string const &data) const
    {
        return data;
    }

    std::string to_string(int const data) const
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%d", data);
        std::string stmp = szBuffer;
        return stmp;
    }
    std::string to_string(uint const data) const
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%u", data);
        std::string stmp = szBuffer;
        return stmp;
    }
    std::string to_string(float const data) const
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%f", data);
        std::string stmp = szBuffer;
        return stmp;
    }
    std::string to_string(double const data) const
    {
        char szBuffer[20];
        snprintf(szBuffer, sizeof(szBuffer), "%f", data);
        std::string stmp = szBuffer;
        return stmp;
    }
    std::string to_string(bool const data) const
    {
        std::string stmp;
        stmp = data ? "true" : "false";
        return stmp;
    }
};

template <typename T> class _spDataOut : public spDataOut
{

  public:
    virtual T get(void) const = 0;
    operator bool() const override
    {
        return (bool)get();
    }
    operator int() const override
    {
        return (int)get();
    }
    operator uint() const override
    {
        return (uint)get();
    }
    operator float() const override
    {
        return (float)get();
    }
    operator double() const override
    {
        return (double)get();
    }
    operator std::string() const override
    {
        T c = get();
        return to_string(c);
    }
    operator char *() const override
    {
        return (char *)to_string(get()).c_str();
    };
};

class _spDataOutString : public spDataOut
{

  public:
    virtual std::string get(void) const = 0;

    operator bool() const override
    {
        return get() == "true";
    }
    operator int() const override
    {
        return std::stoi(get());
    };
    operator uint() const override
    {
        return std::stoul(get());
    };
    operator float() const override
    {
        return std::stof(get());
    }
    operator double() const override
    {
        return std::stod(get());
    }
    operator std::string() const override
    {
        return get();
    }
    operator char *() const override
    {
        return (char *)get().c_str();
    };
};

template <typename T> class _spDataIn
{

  public:
    virtual void set(T const &value) = 0;
};
//----------------------------------------------------------------------------------------
// spProperty
//
// Base/Core Property Class
//
// From an abstract sense, a basic property - nothing more

class spProperty2 : public spIPersist, public spDescriptor
{

  public:
    //---------------------------------------------------------------------------------
    virtual size_t size(void)
    {
        return 0;
    };

    //---------------------------------------------------------------------------------
    virtual size_t save_size(void)
    {
        return 0; // number of bytes used to persist value
    };

    //---------------------------------------------------------------------------------
    // continue to cascade down persistance interface (maybe do this later??)
    virtual bool save(spStorageBlock *stBlk) = 0;
    virtual bool restore(spStorageBlock *stBlk) = 0;
};

// simple def - list of spProperty objects (it's a vector)
using spProperty2List = std::vector<spProperty2 *>;

//----------------------------------------------------------------------------------------
// spPropertyContainer
//
// Define interface/class to manage a list of property
//
// The intent is to add this into other classes that want to expose properties.
//
class _spProperty2Container
{

  public:
    //---------------------------------------------------------------------------------
    void addProperty(spProperty2 *newProperty)
    {
        _properties.push_back(newProperty);
    };

    //---------------------------------------------------------------------------------
    void addProperty(spProperty2 &newProperty)
    {
        addProperty(&newProperty);
    };

    //---------------------------------------------------------------------------------
    spProperty2List &getProperties(void)
    {
        return _properties;
    };

    //---------------------------------------------------------------------------------
    // save/restore for properties in this container. Note, since we
    // expect this to be a "mix-in" class, we use a different interface
    // for the save/restore routines

    bool saveProperties(spStorageBlock *stBlk)
    {
        bool rc = true;
        for (auto property : _properties)
            rc = rc && property->save(stBlk);
        return rc;
    };

    //---------------------------------------------------------------------------------
    bool restoreProperties(spStorageBlock *stBlk)
    {
        bool rc = true;
        for (auto property : _properties)
            rc = rc && property->restore(stBlk);
        return rc;
    };

  private:
    spProperty2List _properties;
};

//----------------------------------------------------------------------------------------
// _spPropertyBase
//
// Template for a property object that implements typed operations for the property value
//
// Note - operator overloading isn't included. Easier to add in the actual property object
//        templates.  Although some "using" magic might work ...
//

template <class T> class _spProperty2Base : public spProperty2, public _spDataIn<T>, public _spDataOut<T>
{

  public:
    //---------------------------------------------------------------------------------
    // Type of property
    spDataType_t type2(void)
    {
        T c;
        return spDataTyper::type(c);
    };

    //---------------------------------------------------------------------------------
    // size in bytes of this property
    virtual size_t size()
    {
        return sizeof(T);
    };

    //---------------------------------------------------------------------------------
    virtual size_t save_size()
    {
        return size();
    }; // sometimes save size is different than size

    //---------------------------------------------------------------------------------
    // Virutal functions to get and set the value - these are filled in
    // by the sub-class

    virtual T get(void) const = 0;
    virtual void set(T const &value) = 0;

    //---------------------------------------------------------------------------------
    // serialization methods
    bool save(spStorageBlock *stBlk)
    {
        T c = get();
        return stBlk->writeBytes(save_size(), (char *)&c);
    };

    //---------------------------------------------------------------------------------
    bool restore(spStorageBlock *stBlk)
    {
        T c;
        return stBlk->readBytes(save_size(), (char *)&c);
        set(c);
    };

    typedef T value_type; // might be handy in future
};

//----------------------------------------------------------------------------------------
// _spPropertyBaseString
//
// Strings are special ...
//
// There is some code duplication here - not happy about this - but strings
// are differnet, so they require a unique implementation. I'm sure there's some
// magic that could reduce the code duplication - but this isn't happening today ...
//
class _spProperty2BaseString : public spProperty2, _spDataIn<std::string>, _spDataOutString
{

  public:
    spDataType_t type2()
    {
        return spTypeString;
    };

    //---------------------------------------------------------------------------------
    // size in bytes of this property
    size_t size()
    {
        std::string c = get();
        return c.size();
    };

    //---------------------------------------------------------------------------------
    size_t save_size()
    {
        return this->size() + sizeof(uint8_t);
    };

    //---------------------------------------------------------------------------------
    // Virutal functions to get and set the value - these are filled in
    // by the sub-class

    virtual std::string get(void) const = 0;
    virtual void set(const std::string &value) = 0;

    //---------------------------------------------------------------------------------
    // set for a c string...
    void set(const char *value)
    {
        std::string c = value;
        set(c);
    }

    //---------------------------------------------------------------------------------
    // serialization methods
    bool save(spStorageBlock *stBlk)
    {
        // strings ... len, data
        std::string c = get();
        uint8_t len = c.size(); // yes, this limits str len of a property to 256.
        stBlk->writeBytes(sizeof(uint8_t), (char *)&len);
        return stBlk->writeBytes(len, (char *)c.c_str());
    }

    //---------------------------------------------------------------------------------
    bool restore(spStorageBlock *stBlk)
    {

        uint8_t len = 0;
        stBlk->readBytes(sizeof(uint8_t), (char *)&len);
        char szBuffer[len + 1];
        bool rc = stBlk->readBytes(len, (char *)szBuffer);
        if (rc)
        {
            szBuffer[len] = '\0';
            set(szBuffer);
        }
        return rc;
    };

    typedef std::string value_type; // might be handy in future/templates
};

//----------------------------------------------------------------------------------------------------
// Now to define different methods on how the value of a property is get/set and stored
// These methods are:
//    - Read/Write property that calls user provided getter/setter methods
//    - Property object that provides storage for the property value
//
//----------------------------------------------------------------------------------------------------
// RW Property templated class: _spPropertyTypedRW
//
//
// A read/write property base class that takes a getter and a setter method and the target object
//
//
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T const &)>
class _spPropertyTypedRW : public _spProperty2Base<T>
{
    Object *my_object; // Pointer to the containing object

  public:
    _spPropertyTypedRW() : my_object(0)
    {
    }

    _spPropertyTypedRW(Object *me) : my_object(me)
    {
    }

    //---------------------------------------------------------------------------------
    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.
    //
    // Also thie containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spProperty2Container
        static_assert(std::is_base_of<_spProperty2Container, Object>::value,
                      "_spPropertyTypedRW: type parameter of this class must derive from spPropertyContainer");

        my_object = obj;
        assert(my_object);

        if (my_object)
            my_object->addProperty(this);
    }
    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }

    //---------------------------------------------------------------------------------
    // get/set syntax
    T get() const
    {
        assert(my_object);
        if (!my_object) // would normally throw an exception, but not very Arduino like!
            return (T)0;

        return (my_object->*_getter)();
    }
    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        assert(my_object);
        if (!my_object)
            return; // would normally throw an exception, but not very Arduino like!

        (my_object->*_setter)(value);
    }

    //---------------------------------------------------------------------------------
    //  need to overload the equality operator
    bool operator==(const T &rhs)
    {
        return get() == rhs;
    }
    //---------------------------------------------------------------------------------
    // get -> property()
    T operator()() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property(value)
    void operator()(T const &value)
    {
        set(value);
    };

    //---------------------------------------------------------------------------------
    // access with '=' sign

    // get -> value = property
    operator T() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property = value  (note: had to add class here to get beyond the copy constructor/op)

    _spPropertyTypedRW<T, Object, _getter, _setter> &operator=(T const &value)
    {
        set(value);
        return *this;
    };
};

// Create typed read/writer property objects - type and RW objects as super classes

// bool
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool const &)>
using spPropertyRWBool = _spPropertyTypedRW<bool, Object, _getter, _setter>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int const &)>
using spPropertyRWInt = _spPropertyTypedRW<int, Object, _getter, _setter>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float const &)>
using spPropertyRWFloat = _spPropertyTypedRW<float, Object, _getter, _setter>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double const &)>
using spPropertyRWDouble = _spPropertyTypedRW<double, Object, _getter, _setter>;

//---------------------------------------------------------------------------------
// spPropertyRWString
//
// "strings are special"
//
// A read/write property string class that takes a getter and a setter method and the target object
//
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string const &)>
class spPropertyRWString : public _spProperty2BaseString
{
    Object *my_object;

  public:
    spPropertyRWString() : my_object(0)
    {
    }

    spPropertyRWString(Object *me) : my_object(me)
    {
    }
    //---------------------------------------------------------------------------------
    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.
    //
    // Also thie containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // Make sure the container type has spPropContainer as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<_spProperty2Container, Object>::value,
                      "_spPropertyTypedRWString: type parameter of this class must derive from spPropertyContainer");

        my_object = obj;
        assert(my_object);
        if (my_object)
            my_object->addProperty(this);
    }

    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }
    //---------------------------------------------------------------------------------
    // String - needed to overload the equality operator
    bool operator==(const std::string &rhs)
    {
        return get() == rhs;
    }

    //---------------------------------------------------------------------------------
    // get/set syntax
    std::string get() const
    {
        assert(my_object);
        if (!my_object)
            return "";

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    void set(std::string const &value)
    {
        assert(my_object);
        if (!my_object)
            return;

        (my_object->*_setter)(value);
    }

    //---------------------------------------------------------------------------------
    // get -> property()
    std::string operator()() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property(value)
    void operator()(std::string const &value)
    {
        set(value);
    };

    //---------------------------------------------------------------------------------
    // access with '=' sign
    // get -> value = property
    operator std::string() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property = value  (note: had to add class here to get beyond the copy constructor/op)
    spPropertyRWString<Object, _getter, _setter> &operator=(std::string const &value)
    {
        set(value);
        return *this;
    };
};
//----------------------------------------------------------------------------------------------------
// spPropertyTyped
//
// Template class for a property object that contains storage for the property.
//
template <class Object, class T> class _spPropertyTyped : public _spProperty2Base<T>
{

  public:
    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.
    void operator()(Object *me)
    {
        // Make sure the container type has spPropContainer as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<_spProperty2Container, Object>::value,
                      "_spPropertyTyped: type parameter of this class must derive from spPropertyContainer");

        // my_object must be derived from _spProperty2Container
        assert(me);
        if (me)
            me->addProperty(this);
    }
    void operator()(Object *obj, const char *name)
    {

        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }
    //---------------------------------------------------------------------------------
    // access with get()/set() syntax
    T get() const
    {
        return data;
    }

    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        data = value;
    }

    //---------------------------------------------------------------------------------
    //  need to overload the equality operator
    bool operator==(const T &rhs)
    {
        return get() == rhs;
    }
    //---------------------------------------------------------------------------------
    // function call syntax
    // get -> property()
    T operator()() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property(value)
    void operator()(T const &value)
    {
        set(value);
    };

    //---------------------------------------------------------------------------------
    // access with '=' sign
    // get -> value = property
    operator T() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------    0
    // set -> property = value  (note: had to add class here to get beyond the copy constructor/op)
    _spPropertyTyped<Object, T> &operator=(T const &value)
    {
        set(value);
        return *this;
    };

  private:
    T data; // actual storage for the property
};

// Define typed properties
template <class Object> using spPropertyBool2 = _spPropertyTyped<Object, bool>;
template <class Object> using spPropertyInt2 = _spPropertyTyped<Object, int>;
template <class Object> using spPropertyFloat2 = _spPropertyTyped<Object, float>;
template <class Object> using spPropertyDouble2 = _spPropertyTyped<Object, double>;

//----------------------------------------------------------------------------------------------------
// spPropertyString
//
// "Strings are special"
//
// Implements the property, but uses string specific logic

template <class Object> class spPropertyString2 : public _spProperty2BaseString
{

  public:
    //---------------------------------------------------------------------------------
    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.

    void operator()(Object *me)
    {
        // Make sure the container type has spPropContainer as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<_spProperty2Container, Object>::value,
                      "_spPropertyString: type parameter of this class must derive from spPropertyContainer");

        assert(me);
        if (me)
            me->addProperty(this);
    }
    // set the name of the property on init
    void operator()(Object *obj, const char *name)
    {
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    // allow the passing in of a name and description
    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }
    //---------------------------------------------------------------------------------
    // access with get()/set() syntax
    std::string get() const
    {
        return data;
    }

    //---------------------------------------------------------------------------------
    void set(std::string const &value)
    {
        data = value;
    }

    //---------------------------------------------------------------------------------
    // String - needed to overload the equality operator
    bool operator==(const std::string &rhs)
    {
        return get() == rhs;
    }

    //---------------------------------------------------------------------------------
    // function call syntax
    // get -> property()
    std::string operator()() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property(value)
    void operator()(std::string const &value)
    {
        set(value);
    };

    //---------------------------------------------------------------------------------
    // access with '=' sign
    operator std::string() const
    {
        return get();
    };

    //---------------------------------------------------------------------------------
    // set -> property = value  (note: had to add class here to get beyond the copy constructor/op)
    spPropertyString2<Object> &operator=(std::string const &value)
    {
        set(value);
        return *this;
    };

  private:
    std::string data; // storage for the property
};

//---------------------------------------------------------
// Class/device typing. use an empty class to define a type. Each typed
// object adds a spType object as a class instance varable - one per class definition.
// Since there is only one instance per  object definition, the address to that sptype
// instance forms a "type" ID for the class that contains it.
//
// So, to find something of a specific type, see if the address of the spType object
// matches that of the target Class.
//
// Simple Example:
//
//  Define a class with a static spType variable, called Type
//       class cow {
//	          static spType Type;
//            ...
//        };
//
// And in the class implementation ,init this static variable (this creates the actual insance)
//        spType cow::Type;
//
// Later:
//
//     pThing = nextItem();
//
//     // Is this a cow?
//
//     if ( pTying->Type == cow::Type) Serial.print("THIS IS A COW");
//
// Define the class and a few operators for quick compairison.

struct spType
{
    spType(){};
    // copy and assign constructors - delete them to prevent extra copys being
    // made. We only want a singletype objects to be part of the class definiton.
    // Basically: One spType object pre defined type.
    spType(spType const &) = delete;
    void operator=(spType const &) = delete;
};

inline bool operator==(const spType &lhs, const spType &rhs)
{
    return &lhs == &rhs;
}
inline bool operator==(const spType &lhs, const spType *rhs)
{
    return &lhs == rhs;
}
inline bool operator==(const spType *lhs, const spType &rhs)
{
    return lhs == &rhs;
}

//----------------------------------------------------------------------------------------------------
// spObject
//
// Core Object Definition for framework objects
//
// Object can:
//    - serialize
//    - have properties
//    - name and descriptor
//    - typed.

class spObject : public spIPersist, public _spProperty2Container, public spDescriptor
{

  public:
    spObject()
    {
    }

    //---------------------------------------------------------------------------------
    virtual bool save(spStorageBlock *stBlk)
    {

        if (!saveProperties(stBlk))
            return false;
        // TODO implement - finish

        return true;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(spStorageBlock *stBlk)
    {
        if (!restoreProperties(stBlk))
            return false;
        // TODO implement - finish
        return true;
    };

    // TODO:
    //   - Add type?
    //   - Add instance ID counter
};

//----------------------------------------------------------------------------------------------------
// spObjectContainer
//
// Container class - containts objects. Mimics some aspects of a vector interface.
//
// It's a template, so it can be typed to subclasses.
//
// Note, it ensures that the class is a spObject

template <typename T> class spObjectContainer : public T, public std::vector<T *>
{

  public:
    spObjectContainer()
    {
        // Make sure the container type has sp object as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<spObject, T>::value,
                      "spObjectContainer: type parameter of this class must derive from spObject");
    }

    //---------------------------------------------------------------------------------
    // State things -- entry for save/restore
    bool save(spStorageBlock *stBlk)
    {
        // save ourselfs
        this->T::save(stBlk);
        for (auto it = this->std::vector<T *>::begin(); it != this->std::vector<T *>::end(); it++)
            (*it)->save(stBlk);

        return true;
    };

    //---------------------------------------------------------------------------------
    bool restore(spStorageBlock *stBlk)
    {
        // restore ourselfs
        this->T::restore(stBlk);
        for (auto it = this->std::vector<T *>::begin(); it != std::vector<T *>::end(); it++)
            (*it)->restore(stBlk);

        return true;
    };
};

//##############################################################################################################################
//##############################################################################################################################
//----------------------------------------------------------------------------------------
// spParameter
//
// Base/Core Parameter Class
//
// From an abstract sense, a basic parameter   - nothing more

class spParameter : public spDescriptor
{
    bool _isEnabled;

  public:
    spParameter() : _isEnabled{true} {};

    bool enabled(void)
    {
        return _isEnabled;
    }

    void setEnabled(bool enabled)
    {
        _isEnabled = enabled;
    };
};

// simple def - list of parameters
using spParameterList = std::vector<spParameter *>;

// We want to bin parameters as input and output for storing different
// arguments lists per object type via overloading. So define some simple classes

class _spParameterIn : public spParameter
{
};
class _spParameterOut : public spParameter
{
};
//----------------------------------------------------------------------------------------
// spParameterContainer
//
// Define interface/class to manage a list of input and output parameters
//
//
// The intent is to add this into other classes that want to expose parameters.
//
class _spParameterContainer
{

  public:
    //---------------------------------------------------------------------------------
    void addParameter(_spParameterIn *newParam)
    {
        _input_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(_spParameterIn &newParam)
    {
        addParameter(&newParam);
    };

    //---------------------------------------------------------------------------------
    size_t nInputParameters()
    {
        return _input_parameters.size();
    }

    //---------------------------------------------------------------------------------
    void addParameter(_spParameterOut *newParam)
    {
        _output_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(_spParameterOut &newParam)
    {
        addParameter(&newParam);
    };

    //---------------------------------------------------------------------------------
    size_t nOutputParameters()
    {
        return _input_parameters.size();
    }

    //---------------------------------------------------------------------------------
    spParameterList &getOutputParameters(void)
    {
        return _output_parameters;
    };

    //---------------------------------------------------------------------------------
    spParameterList &getInputParameters(void)
    {
        return _input_parameters;
    };

  private:
    spParameterList _input_parameters;
    spParameterList _output_parameters;
};

//----------------------------------------------------------------------------------------------------
// spParameterOut
//
// Output Parameter Template
//
//
template <class T, class Object, T (Object::*_getter)()>
class spParameterOut : public _spParameterOut, public _spDataOut<T>
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterOut() : my_object(0)
    {
    }

    spParameterOut(Object *me) : my_object(me)
    {
    }

    //---------------------------------------------------------------------------------
    // Type of property
    spDataType_t type2(void)
    {
        T c;
        return spDataTyper::type(c);
    };
    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_objectthis);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also thie containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spParameterContainer
        static_assert(std::is_base_of<_spParameterContainer, Object>::value,
                      "spParameterOut: type parameter of this class must derive from _spParameterContainer");

        my_object = obj;
        assert(my_object);

        if (my_object)
            my_object->addParameter(this);
    }
    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }

    //---------------------------------------------------------------------------------
    // get/set syntax
    T get() const
    {
        assert(my_object);
        if (!my_object) // would normally throw an exception, but not very Arduino like!
            return (T)0;

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    // get -> parameter()
    T operator()() const
    {
        return get();
    };
};

// Define by type
template <class Object, bool (Object::*_getter)()> using spParameterOutBool = spParameterOut<bool, Object, _getter>;

template <class Object, int (Object::*_getter)()> using spParameterOutInt = spParameterOut<int, Object, _getter>;

template <class Object, uint (Object::*_getter)()> using spParameterOutUint = spParameterOut<uint, Object, _getter>;

template <class Object, float (Object::*_getter)()> using spParameterOutFloat = spParameterOut<float, Object, _getter>;


//----------------------------------------------------------------------------------------------------
// spParameterOutString
//
// Strings are special
// Output Parameter Template
//
//
template <class Object, std::string (Object::*_getter)()>
class spParameterOutString : public _spParameterOut, public _spDataOutString
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterOutString() : my_object(0)
    {
    }

    spParameterOutString(Object *me) : my_object(me)
    {
    }

    //---------------------------------------------------------------------------------
    // Type of property
    spDataType_t type2(void)
    {
        std::string c;
        return spDataTyper::type(c);
    };
    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_objectthis);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also thie containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spParameterContainer
        static_assert(std::is_base_of<_spParameterContainer, Object>::value,
                      "spParameterOutString: type parameter of this class must derive from _spParameterContainer");

        my_object = obj;
        assert(my_object);

        if (my_object)
            my_object->addParameter(this);
    }
    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }

    //---------------------------------------------------------------------------------
    // get/set syntax
    std::string get() const
    {
        assert(my_object);
        if (!my_object) // would normally throw an exception, but not very Arduino like!
            return std::string("");

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    // get -> parameter()
    std::string operator()() const
    {
        return get();
    };
};




template <class T, class Object, void (Object::*_setter)(T const &)>
class spParameterIn : public _spParameterIn, _spDataIn<T>
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterIn() : my_object(0)
    {
    }

    spParameterIn(Object *me) : my_object(me)
    {
    }

    //---------------------------------------------------------------------------------
    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.
    //
    // Also thie containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spParameterContainer
        static_assert(std::is_base_of<_spParameterContainer, Object>::value,
                      "spParameterIn: type parameter of this class must derive from _spParameterContainer");

        my_object = obj;
        assert(my_object);

        if (my_object)
            my_object->addParameter(this);
    }
    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            spDescriptor::name = name;

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::description = desc;

        // cascade to other version of method
        (*this)(obj, name);
    }

    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        assert(my_object);
        if (!my_object)
            return; // would normally throw an exception, but not very Arduino like!

        (my_object->*_setter)(value);
    }

    //---------------------------------------------------------------------------------
    // set -> parameter(value)
    void operator()(T const &value)
    {
        set(value);
    };
};

// Define by type

// bool
template <class Object, void (Object::*_setter)(bool const &)>
using spParameterInBool = spParameterIn<bool, Object, _setter>;

template <class Object, void (Object::*_setter)(int const &)>
using spParameterInInt = spParameterIn<int, Object, _setter>;

template <class Object, void (Object::*_setter)(uint const &)>
using spParameterInUint = spParameterIn<uint, Object, _setter>;

template <class Object, void (Object::*_setter)(float const &)>
using spParameterInFloat = spParameterIn<float, Object, _setter>;

template <class Object, void (Object::*_setter)(double const &)>
using spParameterInDouble = spParameterIn<double, Object, _setter>;

template <class Object, void (Object::*_setter)(std::string const &)>
using spParameterInString = spParameterIn<std::string, Object, _setter>;

// Handy macros to "register attributes (props/params)"

// If the user doesn't supply a unique name or desc - use the object name/prop var name for the name

// Use some macro magic to determine which actual call to make based on the number of passed in
// parameters..
#define _spGetRegAttributeMacro(_1, _2, _3, _NAME_, ...) _NAME_
#define spRegister(...) _spGetRegAttributeMacro(__VA_ARGS__, spRegisterDesc, spRegisterName, spRegisterObj)(__VA_ARGS__)

#define spRegisterObj(_obj_name_) _obj_name_(this, #_obj_name_)

// User provided Name
#define spRegisterName(_obj_name_, _name_) _obj_name_(this, _name_)

// User provided Name and description
#define spRegisterDesc(_obj_name_, _name_, _desc_) _obj_name_(this, _name_, _desc_)

// Define a object type that suppoarts parameter lists (input and output)
class spOperation : public spObject, public _spParameterContainer
{
};

// and a conainer for this thing

template <typename T> class spOperationContainer : public T, public spObjectContainer<T>
{
  public:
    spOperationContainer()
    {
        // Make sure the container type has sp object as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<spOperation, T>::value,
                      "spOperationContainer: type parameter of this class must derive from spOperation");
    }
};

//##############################################################################################################################
//##############################################################################################################################
// END rework of props/object
//##############################################################################################################################
//##############################################################################################################################

// The property object template used to define a type of the object.
//
// Template arg: T - underlying data type of the object - (int, float, string),
//

template <class T> class spProperty : public spPropertyBase
{

  private:
    T _Data;
    spIProperty *_pTarget;

    std::function<T()> _getter;
    std::function<void(T)> _setter;

  public:
    //----------------------------------------
    // Type of property
    DataTypes type(void)
    {
        T c;
        return _getType(&c);
    };

    //----------------------------------------
    // size in bytes of this property
    size_t size()
    {
        return sizeof(T);
    };
    size_t save_size()
    {
        return size();
    }; // sometimes save size is different than size

    //----------------------------------------
    // promote superclasses initialize method to our interface - so overloading works
    using spPropertyBase::initialize;

    void initialize(spIProperty *pTarget, const char *name, const T &value)
    {
        _getter = nullptr;
        _setter = nullptr;

        _Data = value;
        this->spPropertyBase::initialize(pTarget, name);
    }
    // Template for our callback and target.
    // It is expected that these functions have no params.
    template <typename tTarget, typename tCallback> void set_setter(tTarget target, tCallback callback)
    {
        using namespace std::placeholders;
        _setter = std::bind(callback, target, _1);
    }
    // It is expected that these functions have no params.
    template <typename tTarget, typename tCallback> void set_getter(tTarget target, tCallback callback)
    {
        _getter = std::bind(callback, target);
    }

    //----------------------------------------
    // property get
    operator const T() const
    {
        return this->get();
    }

    const T get(void) const
    {
        return (_getter != nullptr ? _getter() : _Data);
    };
    //----------------------------------------
    // property set
    spProperty<T> &operator=(const T &value)
    {

        if (_setter != nullptr)
            _setter(value);
        else
            _Data = value;

        // call the device objects property update method ..
        onPropertyUpdate();

        return *this;
    }

    // cover our type values - can't template this b/c super methods are virtual
    bool getBool()
    {
        return 0 != (int)get();
    }
    int getInt()
    {
        return (int)get();
    }
    float getFloat()
    {
        return (float)get();
    }
    std::string getString()
    {
        return to_string(get());
    }

    // Get value, stash in JSON Variant
    void getValue(const JsonVariant &var)
    {
        var.set(_Data);
    }
    //----------------------------------------
    // serialization methods
    bool save(spStorageBlock *stBlk)
    {
        return stBlk->writeBytes(sizeof(_Data), (char *)&_Data);
    }

    //----------------------------------------
    bool restore(spStorageBlock *stBlk)
    {
        return stBlk->readBytes(sizeof(_Data), (char *)&_Data);
    }
};

#define spPropertySetSetter(_property_, _function_) _property_.set_setter(this, &_function_)

#define spPropertySetGetter(_property_, _function_) _property_.set_getter(this, &_function_)

// Define the typed properties based on the above template
typedef spProperty<std::nullptr_t> spPropertyNone;
typedef spProperty<bool> spPropertyBool;
typedef spProperty<int> spPropertyInt;
typedef spProperty<float> spPropertyFloat;
typedef spProperty<double> spPropertyDouble;

// HACK
// Strings are special - we use std:string for storage, but from the outside
// it looks like a char *. There is some code duplication here, but for a
// proto this is fine.
//
class spPropertyString : public spPropertyBase
{

  private:
    std::string _Data;

  public:
    DataTypes type(void)
    {
        return TypeString;
    };

    //----------------------------------------
    // size in bytes of this property
    size_t size()
    {
        return _Data.size();
    }
    size_t save_size()
    {
        return _Data.size() + sizeof(uint8_t);
    } // string add len

    //----------------------------------------
    // promote superclasses initialize method to our interface - so overloading works
    using spPropertyBase::initialize;

    void initialize(spIProperty *pTarget, const char *name, const char *value)
    {
        _Data = value;
        this->spPropertyBase::initialize(pTarget, name);
    }

    //----------------------------------------
    // property get
    operator const char *() const
    {
        return _Data.c_str();
    }
    operator char *() const
    {
        return (char *)_Data.c_str();
    }

    //----------------------------------------
    // property set
    spPropertyString &operator=(const char *value)
    {
        _Data = value;

        // call the  objects property update method ..
        onPropertyUpdate();

        return *this;
    }

    bool getBool()
    {
        return std::atoi(_Data.c_str()) != 0;
    }
    int getInt()
    {
        return std::atoi(_Data.c_str());
    }
    float getFloat()
    {
        return std::atof(_Data.c_str());
    }
    std::string getString()
    {
        return _Data;
    }

    // Get value, stash in JSON Variant
    void getValue(const JsonVariant &jVar)
    {
        jVar.set(_Data);
    }

    //----------------------------------------
    // serialization methods
    bool save(spStorageBlock *stBlk)
    {

        // strings ... len, data
        uint8_t len = _Data.size(); // yes, this limits str len of a property to 256.
        stBlk->writeBytes(sizeof(uint8_t), (char *)&len);
        return stBlk->writeBytes(_Data.size(), (char *)_Data.c_str());
    }

    //----------------------------------------
    bool restore(spStorageBlock *stBlk)
    {

        uint8_t len;
        stBlk->readBytes(sizeof(uint8_t), (char *)&len);
        char szBuffer[len];
        bool rc = stBlk->readBytes(len, (char *)szBuffer);
        _Data = szBuffer;

        return rc;
    }
};

// Macro use to register the property with the system/base class. Use the CPP # to expand name to
// a string const. Varargs used to pass along a default value if one is passed in.
#define spRegisterProperty(_name_, ...) _name_.initialize(this, #_name_, ##__VA_ARGS__);

//////////////////////////////////////////////////////////////////////////////
// End of property definitions
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Parameter Definitions
//////////////////////////////////////////////////////////////////////////////
//  What this implements:
//
//  Object based input and output parameters that enable "easy"
//  association between objects. Connect outputs from one framework
//  object to the inputs of another.
//
//  The goal is to make it easy for the end-user, but also enable
//  automated code generation via other tools.
//
//  Example:  Connect Joystick output to a motor control driver -
//            to drive the robot
//
//     spJoystick     joystick;
//     spMotorDriver  robot;
//
//     // connect joystick output to robot - assume a controller interface type
//     //
//	   //  Connects the "controller" input parameter of the robot, to the
//     //  controller output of the joystick.
//
//     robot.controller(joy.controller);
//
//  Example:
//      Logic that uses position and temp to open a window (relay)
//
//      spUbloxGNSS  theGNSS;
//      spBME280     theBME;
//      spRelay      theRelay;
//
//      myController winCtl ;  // user implemented
//
//      winCtl.temperature(theBME.temperature);
//      winCtl.position(theGNSS.position);
//      winCtl.time(theGNS.time);
//
//      // relay listens to output from the controller to open
//
//      theRelay.listen(winCtl.on_open);

//////////////////////////////////////////////////////////////////////
// Output Parameter object defs
//////////////////////////////////////////////////////////////////////
//
// Base class for output parameters. This is basically a functor pattern
//
// We template this and define based on base types. The input parameter
// objects will use the resultant base classes to type output parameters
// when the two are bound together.
//
// Typedefs are used to create "typed" output params based on the base template
//
// How this works:
//
//    In class def:
//
//    class myClass{
//    	spParamOutFlt  outParam;
//
//    }
//
//    In the constructor or init routine - register callback
//    function - what the paramter calls to get the actual value.
//
//    a macro is provided to make this *nice*. Registers the value method
//    and sets the name of the output parameter (based on object name)
//
//	  myClass::myClass{
//
//			spSetOutParamSource(outParam, myClass::getValue);
//
//	  }
//
//    The object is a functor - so you can get the value it represents by
//    treating it like a function.
//
//        float thevalue = outParam();
//
//	  Get its name:
//         Serial.println(outParam.name);
//

template <typename T> class spParamOut : public spDataCore
{

    // std::function<T (void)> _handler_func;
    std::function<T()> _handler_func;

  public:
    //----------------------------------------
    // Type of parameter
    DataTypes type(void)
    {
        T c;
        return _getType(&c);
    };

    // Template for our callback and target.
    // It is expected that these functions have no params.
    template <typename tCallback, typename tTarget> void set_callback(tCallback callback, tTarget target)
    {
        _handler_func = std::bind(callback, target);
    }

    // Override (), so you can call this object like a function to
    // get the value - this just calls the bound callback method.
    T operator()()
    {
        return _handler_func();
    }

    // cover our type values - can't template this b/c super methods are virtual
    bool getBool()
    {
        return 0 != (int)_handler_func();
    }
    int getInt()
    {
        return (int)_handler_func();
    }
    float getFloat()
    {
        return (float)_handler_func();
    }
    std::string getString()
    {
        return to_string(_handler_func());
    }
};

// typedef some standard types
typedef spParamOut<bool> spParamOutBool;
typedef spParamOut<int> spParamOutInt;
typedef spParamOut<float> spParamOutFlt;
typedef spParamOut<std::string &> spParamOutStr;

// Simple macro that can be used to setup the parameter -- sets the name to the instance name.

#define spSetupParameter(_param_) _param_.name = #_param_

// Macro to simplify wiring up a parameter to a source funct, and also set the name of
// the parameter. Note old do{}while(false) trick for multi-line macros.

#define spSetupOutParameter(_param_, _function_)                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        _param_.set_callback(&_function_, this);                                                                       \
        _param_.name = #_param_;                                                                                       \
        this->addOutputParameter(_param_);                                                                             \
    } while (false)

///////////////////////////////////////////////////////////////////
// Input Parameter object definition.
///////////////////////////////////////////////////////////////////
//
// This object as an input destination - used to connect an output
// param object to the input of another object.
//
// A template is used to derive types used for the parameters.
//
// The type defines the "type" of the parameter, as well as the type
// of the associated output parameter - note - typing of output is based
// on output param superclass : _spParamOut;
//
// This object overfloads the cast operator for the derived type, SO, the
// object just looks like a variable. So:
//
//	if defined:
//
//     spParamInFlt   param1;
//
//  Output params are "bound" to the input param by just calling it as an function
//
//     param1(anotherObject.outputparam); // The type of outputparam needs to match
//
//	   //FUTURE: option: could make this a method
//		param1.[link|connect|bind](anotherObject.outputparam);
//
//  Use :
//     Just treat as a variable.
//
//     float myobj::getInputValue(){
//
//        return param1;
//      }
//

template <typename T> class spParamIn : public spDataCore
{

  public:
    //----------------------------------------
    // Type of parameter
    DataTypes type(void)
    {
        T *c;
        return _getType(c);
    };

    spParamOut<T> *_inputParameter;

    spParamIn() : _inputParameter(nullptr){};

    void operator()(spParamOut<T> &inputParam)
    {
        _inputParameter = &inputParam;
    }

    const T get(void) const
    {
        return _inputParameter == nullptr ? (T)0 : (*_inputParameter)();
    }
    operator const T() const
    {
        return get();
    }

    // cover our type values - can't template this b/c super methods are virtual
    bool getBool()
    {
        return 0 != (int)get();
    }
    int getInt()
    {
        return (int)get();
    }
    float getFloat()
    {
        return (float)get();
    }
    std::string getString()
    {
        return to_string(get());
    }
};

typedef spParamIn<float> spParamInFlt;
typedef spParamIn<int> spParamInInt;
typedef spParamIn<std::string> spParamInStr;

//////////////////////////////////////////////////////////////////////////////
// End of Parameter definitions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Define a simple class hierarchy interface definitions. Used to walk the hierarchy.
//////////////////////////////////////////////////////////////////////////////
//

//---------------------------------------------------------
// base class
class spBase : public spIProperty
{

  public:
    virtual bool save(void);
    virtual bool restore(void);

    bool serializeJSON(JsonObject &jObj);

    spPropertyString name;

    spBase()
    {
        // reg the name property on the device
        spRegisterProperty(name, "");
        _id = 0; // no id yet
    }

    virtual void onPropertyUpdate(const char *);

    void addOutputParameter(spDataCore &param)
    {
        _outParameters.push_back(&param);
    }

    spDataCoreList &getOutputParameters(void)
    {
        return _outParameters;
    }

    size_t nOutputParameters(void)
    {
        return _outParameters.size();
    }

  protected:
    uint16_t getID();

  private:
    uint16_t _id;

    static uint16_t _name_count; // used to build a unique name

    spDataCoreList _outParameters;
};

using spBaseList = std::vector<spBase *>;
//---------------------------------------------------------
// Container class. Mimics some aspects of a vector interface.
//
// Use template to set typing

template <typename T> class spContainer : public spBase
{

    std::vector<T *> _children;

  public:
    // child things
    int size(void)
    {
        return _children.size();
    }

    T *at(int i)
    {
        return _children.at(i);
    }

    void add(T *theChild)
    {
        _children.push_back(theChild);
    }

    // State things -- entry for save/restore
    bool save(void)
    {
        // save ourselfs
        this->spBase::save();
        for (auto pChild : _children)
            pChild->save();

        return true;
    }

    bool restore(void)
    {
        // restore ourselfs
        this->spBase::restore();
        for (auto pChild : _children)
            pChild->restore();
        return true;
    }

    bool serializeJSON(JsonDocument &jRoot)
    {
        JsonArray jArray = jRoot.createNestedArray(name);
        return serializeChildrenJSON(jArray);
    }

    bool serializeJSON(JsonObject &jRoot)
    {
        JsonArray jArray = jRoot.createNestedArray(name);
        return serializeChildrenJSON(jArray);
    }

    bool serializeJSON(JsonArray &jRoot)
    {
        JsonArray jArray = jRoot.createNestedArray();
        return serializeChildrenJSON(jArray);
    }

    // make this container interable ...
    typename std::vector<T *>::iterator begin()
    {
        return _children.begin();
    }
    typename std::vector<T *>::iterator end()
    {
        return _children.end();
    }
    typename std::vector<T *>::const_iterator cbegin() const
    {
        return _children.cbegin();
    }
    typename std::vector<T *>::const_iterator cend() const
    {
        return _children.cend();
    }
    typename std::vector<T *>::reverse_iterator rbegin()
    {
        return _children.rbegin();
    }
    typename std::vector<T *>::reverse_iterator rend()
    {
        return _children.rend();
    }

    typename std::vector<T *>::iterator erase(typename std::vector<T *>::iterator it)
    {
        return _children.erase(it);
    }

  private:
    bool serializeChildrenJSON(JsonArray &jArray)
    {

        for (auto pChild : _children)
        {
            JsonObject jChild = jArray.createNestedObject();
            pChild->serializeJSON(jChild);
        }
        return true;
    }
};

//-----------------------------------------
// Spark Activites

struct spAction : public spBase
{

    virtual bool loop(void)
    {
        return false;
    }; // default is a noop
};

using spActionContainer = spContainer<spAction>;

//-----------------------------------------------------------------------
// 5/20 - Impl based on https://schneegans.github.io/tutorials/2015/09/20/signal-slot
// spSignals - sort of  - support for our simple observer pattern for events
//

template <typename ArgT> class spSignal
{

  public:
    // connects a member function to this spSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT var))
    {
        connect([=](ArgT var) { // uses a lambda for the callback
            (inst->*func)(var);
        });
    }

    // connects a const member function to this spSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT var) const)
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (inst->*func)(var);
        });
    }

    // Just call a user supplied function - no object
    void call(void (*func)(ArgT var))
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (*func)(var);
        });
    }
    // Just call a user supplied function - no object - but with a User Defined value
    template <typename T> void call(void (*func)(T uval, ArgT var), T uValue)
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (*func)(uValue, var);
        });
    }
    // connects a std::function to the spSignal.
    void connect(std::function<void(ArgT var)> const &slot) const
    {
        slots_.push_back(slot);
    }

    // calls all connected functions
    void emit(ArgT p)
    {
        for (auto const &it : slots_)
        {
            it(p);
        }
    }

  private:
    mutable std::vector<std::function<void(ArgT &var)>> slots_;
};

typedef spSignal<bool> spSignalBool;
typedef spSignal<int> spSignalInt;
typedef spSignal<float> spSignalFloat;
typedef spSignal<std::string &> spSignalString;

// Signal - zero arg - aka void function callback type. Unable to template this, so
// just brute force the impl.
class spSignalVoid
{

  public:
    // connects a member function to this spSignal
    template <typename T> void call(T *inst, void (T::*func)())
    {
        connect([=]() { // uses a lambda for the callback
            (inst->*func)();
        });
    }

    // Just call a user supplied function - no object
    void call(void (*func)())
    {
        connect([=]() { // uses a lambda for the callback
            (*func)();
        });
    }
    // Just call a user supplied function - no object - but with a User Defined value
    template <typename T> void call(void (*func)(T uval), T uValue)
    {
        connect([=]() { // users a lambda for the callback
            (*func)(uValue);
        });
    }

    // connects a const member function to this spSignal
    template <typename T> void call(T *inst, void (T::*func)() const)
    {
        connect([=]() { // users a lambda for the callback
            (inst->*func)();
        });
    }

    void connect(std::function<void()> const &slot) const
    {
        slots_.push_back(slot);
    }

    // calls all connected functions
    void emit(void)
    {
        for (auto const &it : slots_)
        {
            it();
        }
    }

  private:
    mutable std::vector<std::function<void()>> slots_;
};
