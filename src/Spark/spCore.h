

#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <string>
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

#define warning_message(_message_)                                                                                     \
    debug_message("[Warning] - ", _message_);                                                                          \
    \

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
    const char *name;

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

class spDescriptor
{
public:
	spDescriptor() : name{""}, description{""}{}

	std::string   name;
	std::string   description;

};
// Base/Core Property Class
//
// From an abstract sense, a basic property - nothing more

class spProperty2 : public spIPersist, public spDataCore, public spDescriptor
{

public:
    virtual size_t size(void)
    {
        return 0;
    };
    virtual size_t save_size(void)
    {
        return 0;  // number of bytes used to persist value
    };

    // continue to cascade down persistance interface (maybe do this later??)
    virtual bool save(spStorageBlock *stBlk) = 0;
    virtual bool restore(spStorageBlock *stBlk) = 0;

};
using spProperty2List = std::vector<spProperty2 *>;

//------------------------------------------------------------------------------
// Define interface/class to manage a list of property

class _spProperty2Container{

public:
	void addProperty(spProperty2 *newProperty)
	{
		_properties.push_back(newProperty);
	};
	void addProperty(spProperty2& newProperty)
	{
		addProperty(&newProperty);
	};

	spProperty2List& getProperties(void)
	{
		return _properties;
	};

private:
	spProperty2List  _properties;
};

template<class T>
class _spProperty2TypedBase : public spProperty2
{

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

    // Virutal functions to get and set the value - these are filled in
    // by the sub-class

    virtual T get(void)=0;
    virtual void set(T &value)=0;

	// function call syntax
    T operator()() const
    {
        return get();
    };
    void operator()(T const &value)
    {
        return set(value);
    };

    // access with '=' sign
    operator T() const
    {
        return get();
    };

    void operator=(T const &value)
    {
        return set(value);
    };
 
    // cover our type values - can't template this b/c super methods are virtual
    bool getBool()
    {
        return 0 != (int)get();
    };
    int getInt()
    {
        return (int)get();
    };
    float getFloat()
    {
        return (float)get();
    };
    std::string getString()
    {
        return to_string(get());
    };

    //----------------------------------------
    // serialization methods
    bool save(spStorageBlock *stBlk)
    {

    	T c = get();
        return stBlk->writeBytes(save_size(), (char *)&c);
    };

    //----------------------------------------
    bool restore(spStorageBlock *stBlk)
    {
    	T c;
        return stBlk->readBytes(save_size(), (char *)&c);
        set(c);
    };
};


// A read/write property base class that takes a getter and a setter method and the target object
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T const &)> 
class _spPropertyTypedRW : public _spProperty2TypedBase<T>
{
    Object *my_object;

  public:
    _spPropertyTypedRW() : my_object(0)
    {
    }
    _spPropertyTypedRW(Object *me) : my_object(me)
    {
    	// my_object must be derived from _spProperty2Container
    	my_object->addProperty(this);
    }

    // get/set syntax
    T get() const
    {
        return (my_object->*_getter)();
    }
    T set(T const &value)
    {
        (my_object->*_setter)(value);
    }
    typedef T value_type;
    // might be useful for template
    // deductions
};

// Create type values of this Read/Write property
template<class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool const &)>
using spPropertyRWBool = _spPropertyTypedRW<bool, Object, _getter, _setter>;

template<class Object, int (Object::*_getter)(), void (Object::*_setter)(int const &)>
using spPropertyRWInt = _spPropertyTypedRW<int, Object, _getter, _setter>;

template<class Object, float (Object::*_getter)(), void (Object::*_setter)(float const &)>
using spPropertyRWFloat = _spPropertyTypedRW<float, Object, _getter, _setter>;

template<class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string const &)>
using spPropertyRWString = _spPropertyTypedRW<std::string, Object, _getter, _setter>;


// Template class for a property object that contains storage for the property. 
template <class T>
class _spPropertyTyped : public _spProperty2TypedBase<T> 
{

public:

  	// access with function call syntax
  	_spPropertyTyped() { }

  	// access with get()/set() syntax
  	T get() const {
    	return data;
  	}
  	void set(T const & value) {
    	data = value;
  	}

  	typedef T value_type;
            // might be useful for template
            // deductions

private:
	T data;
};

// Define typed properties
using spPropertyBool2 = _spPropertyTyped<bool>;
using spPropertyInt2 = _spPropertyTyped<int>;
using spPropertyFloat2 = _spPropertyTyped<float>;
using spPropertyString2 = _spPropertyTyped<std::string>;


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



//---------------------------------------------------------
// Core Object Definition for framework objects
//
// Object can:
//    - serialize
//    - have properties 
//    - name and descriptor
//    - typed. 
     
class spObject : public spIPersist, _spProperty2Container, public spDescriptor
{

  public:
  	spObject(){}

    virtual bool save(void)
    {
    	// TODO implement -
    	return true;
    };
    
    virtual bool restore(void)
    {
    	// TODO implement -
    	return true;    	
    };

    // TODO:
    //   - Add type?
    //   - Add instance ID counter
};
//---------------------------------------------------------
// Container class - containts objects. Mimics some aspects of a vector interface.
//

class spObjectContainer : public spObject, public std::vector<spObject*>
{


  public:
    // State things -- entry for save/restore
    bool save(void)
    {
        // save ourselfs
        this->spObject::save();
        for(auto it = begin(); it != end(); it++)
        	(*it)->save();

        return true;
    };

    bool restore(void)
    {
        // restore ourselfs
        this->spObject::restore();
        for(auto it = begin(); it != end(); it++)
        	(*it)->restore();

        return true;
    };

};

// END rework of props/object
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

    	if(_setter != nullptr)
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

#define spPropertySetSetter(_property_, _function_)  _property_.set_setter(this, &_function_)

#define spPropertySetGetter(_property_, _function_)  _property_.set_getter(this, &_function_)

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
// a read-write property which invokes
// user-defined functions
template <class T, class Object, T (Object::*real_getter)(), T (Object::*real_setter)(T const &)> 
class RWProperty
{
    Object *my_object;

  public:
    RWProperty() : my_object(0)
    {
    }
    RWProperty(Object *me) : my_object(me)
    {
    }

    // this function must be called by the
    // containing class, normally in a
    // constructor, to initialize the
    // ROProperty so it knows where its
    // real implementation code can be
    // found
    void operator()(Object *obj)
    {
        my_object = obj;
    }

    // function call syntax
    T operator()() const
    {
        return (my_object->*real_getter)();
    }
    T operator()(T const &value)
    {
        return (my_object->*real_setter)(value);
    }

    // get/set syntax
    T get() const
    {
        return (my_object->*real_getter)();
    }
    T set(T const &value)
    {
        return (my_object->*real_setter)(value);
    }
    // access with '=' sign
    operator T() const
    {
        return (my_object->*real_getter)();
    }
    T operator=(T const &value)
    {
        return (my_object->*real_setter)(value);
    }

    typedef T value_type;
    // might be useful for template
    // deductions
};

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

    typename std::vector<T*>::iterator erase( typename std::vector<T*>::iterator it)
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
