

#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

//using std::string;

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

//----------------------------------------------------------------------------------------
// spDescriptor
//
// Simple class that can be mixed-in to add a common name and description string
// to user "exposed" objects in the framework..

class spDescriptor
{
  public:
    spDescriptor() : _name{NULL}, _description{NULL}
    {
    }

    void setName( const char *new_name)
    {
        _name = new_name;
    }
    const char * name()
    {
        return _name;
    }
    void setDescription( const char *new_desc)
    {
        _description = new_desc;
    }
    const char * description()
    {
        return _description;
    }
protected:
     const char * _name;
     const char * _description;

};

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
public:
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

struct spPersist
{

    virtual bool save(spStorageBlock *stBlk) = 0;
    virtual bool restore(spStorageBlock *stBlk) = 0;
};
//----------------------------------------------------------------------------------------
// spDataOut
//
// Interface to get outputs from an object.
class spDataOut
{

  public:

    virtual spDataType_t type(void)=0;

    virtual bool getBool()  = 0;
    virtual int getInt()  = 0;
    virtual uint getUint()  = 0;
    virtual float getFloat()  = 0;
    virtual double getDouble()  = 0;
    virtual std::string getString()  = 0;

    bool get_value(bool){
        return getBool();
    }
    int get_value(int){
        return getInt();
    }
    uint get_value(uint){
        return getUint();
    }
    float get_value(float){
        return getFloat();
    }       
    double get_value(double){
        return getDouble();
    }       
    std::string get_value(std::string){
        return getString();
    }               



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

    // Type of property
    spDataType_t type(void)
    {
        T c;
        return spDataTyper::type(c);
    };


    virtual T get(void) const = 0;

    bool getBool() 
    {
        return (bool)get();
    }
    int getInt()  
    {
        return (int)get();
    }
    uint getUint() 
    {
        return (uint)get();
    }
    float getFloat() 
    {
        return (float)get();
    }
    double getDouble() 
    {
        return (double)get();
    }
    std::string getString() 
    {
        T c = get();
        return to_string(c);
    }

    typedef T value_type; // might be handy in future
};

class _spDataOutString : public spDataOut
{

  public:

    // Type of property
    spDataType_t  type(void)
    {
        return spTypeString;
    };

    virtual std::string get(void) const = 0;

    bool getBool() 
    {
        return get() == "true";
    }
    int getInt() 
    {
        return std::stoi(get());
    };
    uint getUint() 
    {
        return std::stoul(get());
    };
    float getFloat() 
    {
        return std::stof(get());
    }
    double getDouble() 
    {
        return std::stod(get());
    }
    std::string getString() 
    {
        return get();
    }

    typedef std::string value_type; // might be handy in future
};

class spDataIn{
public:
    virtual spDataType_t type(void)=0;
};

template <typename T> class _spDataIn : public spDataIn
{

  public:
    spDataType_t type(void)
    {
        T c;
        return spDataTyper::type(c);
    };
    virtual void set(T const &value) = 0;
};
//----------------------------------------------------------------------------------------
// spProperty
//
// Base/Core Property Class
//
// From an abstract sense, a basic property - nothing more

class spProperty : public spPersist, public spDescriptor
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
using spPropertyList = std::vector<spProperty *>;

//----------------------------------------------------------------------------------------
// spPropertyContainer
//
// Define interface/class to manage a list of property
//
// The intent is to add this into other classes that want to expose properties.
//
class _spPropertyContainer
{

  public:
    //---------------------------------------------------------------------------------
    void addProperty(spProperty *newProperty)
    {
        _properties.push_back(newProperty);
    };

    //---------------------------------------------------------------------------------
    void addProperty(spProperty &newProperty)
    {
        addProperty(&newProperty);
    };

    //---------------------------------------------------------------------------------
    spPropertyList &getProperties(void)
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
    spPropertyList _properties;
};

//----------------------------------------------------------------------------------------
// _spPropertyBase
//
// Template for a property object that implements typed operations for the property value
//
// Note - operator overloading isn't included. Easier to add in the actual property object
//        templates.  Although some "using" magic might work ...
//

template <class T> class _spPropertyBase : public spProperty, public _spDataIn<T>, public _spDataOut<T>
{

  public:
    //---------------------------------------------------------------------------------
    
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
class _spPropertyBaseString : public spProperty, _spDataIn<std::string>, _spDataOutString
{

  public:
    spDataType_t type()
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
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T )>
class _spPropertyTypedRW : public _spPropertyBase<T>
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
        // my_object must be derived from _spPropertyContainer
        static_assert(std::is_base_of<_spPropertyContainer, Object>::value,
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
            spDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::setDescription(desc);

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
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using spPropertyRWBool = _spPropertyTypedRW<bool, Object, _getter, _setter>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using spPropertyRWInt = _spPropertyTypedRW<int, Object, _getter, _setter>;

template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using spPropertyRWUint = _spPropertyTypedRW<uint, Object, _getter, _setter>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using spPropertyRWFloat = _spPropertyTypedRW<float, Object, _getter, _setter>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double )>
using spPropertyRWDouble = _spPropertyTypedRW<double, Object, _getter, _setter>;

//---------------------------------------------------------------------------------
// spPropertyRWString
//
// "strings are special"
//
// A read/write property string class that takes a getter and a setter method and the target object
//
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string )>
class spPropertyRWString : public _spPropertyBaseString
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
        static_assert(std::is_base_of<_spPropertyContainer, Object>::value,
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
            setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            setDescription(desc);

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
template <class Object, class T> class _spPropertyTyped : public _spPropertyBase<T>
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
        static_assert(std::is_base_of<_spPropertyContainer, Object>::value,
                      "_spPropertyTyped: type parameter of this class must derive from spPropertyContainer");

        // my_object must be derived from _spPropertyContainer
        assert(me);
        if (me)
            me->addProperty(this);
    }
    void operator()(Object *obj, const char *name)
    {

        // set the name of the property on init
        if (name)
            spDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            spDescriptor::setDescription(desc);

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

    bool operator>(int rhs)
    {
        return (get() > rhs);
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
template <class Object> using spPropertyBool = _spPropertyTyped<Object, bool>;
template <class Object> using spPropertyInt = _spPropertyTyped<Object, int>;
template <class Object> using spPropertyUint = _spPropertyTyped<Object, uint>;
template <class Object> using spPropertyFloat = _spPropertyTyped<Object, float>;
template <class Object> using spPropertyDouble = _spPropertyTyped<Object, double>;

//----------------------------------------------------------------------------------------------------
// spPropertyString
//
// "Strings are special"
//
// Implements the property, but uses string specific logic

template <class Object> class spPropertyString : public _spPropertyBaseString
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
        static_assert(std::is_base_of<_spPropertyContainer, Object>::value,
                      "_spPropertyString: type parameter of this class must derive from spPropertyContainer");

        assert(me);
        if (me)
            me->addProperty(this);
    }
    // set the name of the property on init
    void operator()(Object *obj, const char *name)
    {
        if (name)
            setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    // allow the passing in of a name and description
    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            setDescription(desc);

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
    spPropertyString<Object> &operator=(std::string const &value)
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

class spObject : public spPersist, public _spPropertyContainer, public spDescriptor
{

    spObject * _parent;
  public:
    spObject()
    {
    }

    void setParent(spObject * parent)
    {
        _parent = parent;
    }
    void setParent(spObject &parent)
    {
        setParent(&parent);
    }

    spObject * parent()
    {
        return _parent;
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
//####################################################################
// Container update
//
// spContainer
//
// A list/container that holds spObjects and supports serialization. The 
// container itself is a object

template <class T> class spContainer : public spObject
{
  protected:
    // Use a vector to store data
    std::vector<T> _vector;

  public:

    spContainer() : _vector()
    {
    }
    // we're compositing the std::vector inteface - just bridge it up to
    // our interface
    auto size()  -> decltype(_vector.size())
    {
        return _vector.size();
    }

    void push_back( T& value)
    {
        _vector.push_back(value);
        value.setParent(this);
    }
    void push_back( T* value)
    {
        _vector.push_back(value);
        value->setParent(this);
    }
    void pop_back(void)  
    {
        _vector.pop_back();
    }

    auto back(void)  -> decltype(_vector.back())
    {
        return _vector.back();
    }
    auto front()  -> decltype(_vector.front())
    {
        return _vector.front();
    }
    T& at(size_t pos )  
    {
        return _vector.at(pos);
    }    
    auto cbegin()  -> decltype(_vector.cbegin())
    {
        return _vector.cbegin();
    }
    auto cend()  -> decltype(_vector.cend())
    {
        return _vector.cend();
    }

    auto begin()  -> decltype(_vector.begin())
    {
        return _vector.begin();
    }
    auto end()  -> decltype(_vector.end())
    {
        return _vector.end();
    }
    auto rbegin()  -> decltype(_vector.rbegin())
    {
        return _vector.rbegin();
    }
    auto rend()  -> decltype(_vector.rend())
    {
        return _vector.rend();
    }
    auto empty() -> decltype(_vector.empty())
    {
        return _vector.empty();
    }

    typedef typename std::vector<T>::iterator iterator;

    iterator erase( iterator pos)
    {
        return _vector.erase(pos);
    }
};

//####################################################################


//----------------------------------------------------------------------------------------------------
// _spObjectContainer
//
// Container class - containts objects. Mimics some aspects of a vector interface.
//
// It's a template, so it can be typed to subclasses.
//
// Note, it ensures that the class is a spObject

template <typename T> class _spObjectContainer : public T, public std::vector<T *>
{

  public:
    _spObjectContainer()
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

    void add( T *value ){
        this->std::vector<T*>::push_back(value);
        value->setParent(this);
    }
    void add( T &value ){
        add(&value);
    }
};
using spObjectContainer = _spObjectContainer<spObject>;

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



// We want to bin parameters as input and output for storing different
// arguments lists per object type via overloading. So define some simple classes

class spParameterIn : public spParameter
{
};
class spParameterOut : public spParameter, public spDataOut
{
};
// simple def - list of parameters
using spParameterInList = std::vector<spParameterIn *>;
using spParameterOutList = std::vector<spParameterOut *>;

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
    void addParameter(spParameterIn *newParam)
    {
        _input_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(spParameterIn &newParam)
    {
        addParameter(&newParam);
    };

    //---------------------------------------------------------------------------------
    size_t nInputParameters()
    {
        return _input_parameters.size();
    }

    //---------------------------------------------------------------------------------
    void addParameter(spParameterOut *newParam)
    {
        _output_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(spParameterOut &newParam)
    {
        addParameter(&newParam);
    };

    //---------------------------------------------------------------------------------
    size_t nOutputParameters()
    {
        return _output_parameters.size();
    }

    //---------------------------------------------------------------------------------
    spParameterOutList &getOutputParameters(void)
    {
        return _output_parameters;
    };

    //---------------------------------------------------------------------------------
    spParameterInList &getInputParameters(void)
    {
        return _input_parameters;
    };

  private:
    spParameterInList _input_parameters;
    spParameterOutList _output_parameters;
};

//----------------------------------------------------------------------------------------------------
// spParameterOut
//
// Output Parameter Template
//
//

template <class T, class Object, T (Object::*_getter)()>
class _spParameterOut : public _spDataOut<T>, public spParameterOut
{
    Object *my_object; // Pointer to the containing object

  public:
    _spParameterOut() : my_object(0)
    {
    }

    _spParameterOut(Object *me) : my_object(me)
    {
    }

    //---------------------------------------------------------------------------------
    // Type of property
    spDataType_t type(void)
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
            setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            setDescription(desc);

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

    bool getBool()
    {
       return _spDataOut<T>::getBool();
    };
    int getInt()
    {
        return _spDataOut<T>::getInt();
    };
    uint getUint()
    {
       return _spDataOut<T>::getUint();
    };
    float getFloat()
    {
        return _spDataOut<T>::getFloat();
    };
    double getDouble()
    {
        return _spDataOut<T>::getDouble();
    };
    std::string getString()
    {
        return _spDataOut<T>::getString();
    };
};

// Define by type
template <class Object, bool (Object::*_getter)()> using spParameterOutBool = _spParameterOut<bool, Object, _getter>;

template <class Object, int (Object::*_getter)()> using spParameterOutInt = _spParameterOut<int, Object, _getter>;

template <class Object, uint (Object::*_getter)()> using spParameterOutUint = _spParameterOut<uint, Object, _getter>;

template <class Object, float (Object::*_getter)()> using spParameterOutFloat = _spParameterOut<float, Object, _getter>;

template <class Object, float (Object::*_getter)()> using spParameterOutDouble = _spParameterOut<double, Object, _getter>;


//----------------------------------------------------------------------------------------------------
// spParameterOutString
//
// Strings are special
// Output Parameter Template
//
//
template <class Object, std::string (Object::*_getter)()>
class spParameterOutString : public spParameterOut, public _spDataOutString
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
    spDataType_t type(void)
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
            setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            setDescription(desc);

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

    bool getBool()
    {
       return _spDataOutString::getBool();
    };
    int getInt()
    {
        return _spDataOutString::getInt();
    };
    uint getUint()
    {
       return _spDataOutString::getUint();
    };
    float getFloat()
    {
        return _spDataOutString::getFloat();
    };
    double getDouble()
    {
        return _spDataOutString::getDouble();
    };
    std::string getString()
    {
        return _spDataOutString::getString();
    };
};




template <class T, class Object, void (Object::*_setter)(T const &)>
class _spParameterIn : public spParameterIn, _spDataIn<T>
{
    Object *my_object; // Pointer to the containing object

  public:
    _spParameterIn() : my_object(0)
    {
    }

    _spParameterIn(Object *me) : my_object(me)
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
            setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            setDescription(desc);

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
using spParameterInBool = _spParameterIn<bool, Object, _setter>;

template <class Object, void (Object::*_setter)(int const &)>
using spParameterInInt = _spParameterIn<int, Object, _setter>;

template <class Object, void (Object::*_setter)(uint const &)>
using spParameterInUint = _spParameterIn<uint, Object, _setter>;

template <class Object, void (Object::*_setter)(float const &)>
using spParameterInFloat = _spParameterIn<float, Object, _setter>;

template <class Object, void (Object::*_setter)(double const &)>
using spParameterInDouble = _spParameterIn<double, Object, _setter>;

template <class Object, void (Object::*_setter)(std::string const &)>
using spParameterInString = _spParameterIn<std::string, Object, _setter>;

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
public:
    virtual spType *getType(void)
    {
        return (spType *)nullptr;
    }

    virtual bool loop(void)
    {
        return false;
    }
};

// and a conainer for this thing

template <typename T> class _spOperationContainer : public _spObjectContainer<T>
{
  public:
    _spOperationContainer()
    {
        // Make sure the container type has sp object as it's baseclass or it's a spObject
        // Compile-time check
        static_assert(std::is_base_of<spOperation, T>::value,
                      "spOperationContainer: type parameter of this class must derive from spOperation");
    }
};

using spOperationContainer = spContainer<spOperation>;
//-----------------------------------------
// Spark Actions

class spAction : public spOperation
{
};

using spActionContainer = _spOperationContainer<spAction>;

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
