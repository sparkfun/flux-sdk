

// spCoreProps.h
//
// Defines the objects that make up the property system in the framework.
//
// Also includes the core object definition (spObject and spContainer)

#pragma once

#include <string>
#include <vector>

#include "spCoreTypes.h"
#include "spStorage.h"
#include "spUtils.h"


#define kMaxPropertyString 256
//----------------------------------------------------------------------------------------
// spProperty
//
// Base/Core Property Class
//
// From an abstract sense, a basic property - nothing more

class spProperty : public spDescriptor
{

  public:
    virtual spDataType_t type(void) = 0;

    // Editor interface method - called to have the value of the property
    // displayed/set/managed in an editor

    virtual bool editValue(spDataEditor &) = 0;

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

    // Expect subclasses will override this
    virtual std::string getString()
    {
        std::string s = "";
        return s;
    }
    //---------------------------------------------------------------------------------
    // continue to cascade down persistance interface (maybe do this later??)
    virtual bool save(spStorageBlock2 *) = 0;
    virtual bool restore(spStorageBlock2 *) = 0;
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

    uint nProperties(void)
    {

        return _properties.size();
    }
    //---------------------------------------------------------------------------------
    // save/restore for properties in this container. Note, since we
    // expect this to be a "mix-in" class, we use a different interface
    // for the save/restore routines

    bool saveProperties(spStorageBlock2 *stBlk)
    {
        bool rc = true;
        bool status; 
        for (auto property : _properties)
        {
            status = property->save(stBlk);
            rc = rc && status;
        }
        return rc;
    };

    //---------------------------------------------------------------------------------
    bool restoreProperties(spStorageBlock2 *stBlk)
    {
        bool rc = true;
        bool status;

        for (auto property : _properties)
        {
            status = property->restore(stBlk);
            rc = rc && status;
        }
        return rc;
    };

    size_t propertySaveSize()
    {
        size_t totalSize =0;

        for (auto property : _properties)
            totalSize += property->save_size();

        return totalSize;
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
    spDataType_t type()
    {
        return _spDataOut<T>::type();
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
    // Virtual functions to get and set the value - these are filled in
    // by the sub-class

    virtual T get(void) const = 0;
    virtual void set(T const &value) = 0;

    //---------------------------------------------------------------------------------
    // serialization methods
    bool save(spStorageBlock2 *stBlk)
    {
        T c = get();
        bool status = stBlk->write(name(), c);

        if( !status )
            spLog_E("Error when saving property %s", name());

        return status;
    };

    //---------------------------------------------------------------------------------
    bool restore(spStorageBlock2 *stBlk)
    {
        T c;

        bool status = stBlk->read(name(), c);

        if (status )
            set(c);

        return status;
    };

    // use this to route the call to our dataOut baseclass
    virtual std::string getString(void)
    {
        return _spDataOut<T>::getString();
    }
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    bool editValue(spDataEditor &theEditor)
    {

        T value = get();

        bool bSuccess = theEditor.editField(value);

        if (bSuccess) // success
            set(value);

        return bSuccess;
    }
};

//----------------------------------------------------------------------------------------
// _spPropertyBaseString
//
// Strings are special ...
//
// There is some code duplication here - not happy about this - but strings
// are different, so they require a unique implementation. I'm sure there's some
// magic that could reduce the code duplication - but this isn't happening today ...
//
class _spPropertyBaseString : public spProperty, _spDataInString, _spDataOutString
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
    // Virtual functions to get and set the value - these are filled in
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
    bool save(spStorageBlock2 *stBlk)
    {
        // strings ... len, data
        std::string c = get();

        bool status = stBlk->writeString(name(), c.c_str()) == c.length();
        if( !status )
            spLog_E("Error saving string for property: %s", name());
        return status;
    }

    //---------------------------------------------------------------------------------
    bool restore(spStorageBlock2 *stBlk)
    {
        char szBuffer[kMaxPropertyString];

        size_t len = stBlk->readString(name(), szBuffer, sizeof(szBuffer));

        if ( len > 0)
            set(szBuffer);

        return true;
    };
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    bool editValue(spDataEditor &theEditor)
    {

        std::string value = get();

        bool bSuccess = theEditor.editField(value);

        if (bSuccess) // success
            set(value);

        return bSuccess;
    }
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
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T)>
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
    // Also the containing object is needed to call the getter/setter methods on that object
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
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this property.");
            return (T)0;
        }

        return (my_object->*_getter)();
    }
    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        if (!my_object)
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this property.");
            return; // would normally throw an exception, but not very Arduino like!
        }

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

// int8
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using spPropertyRWInt8 = _spPropertyTypedRW<int8_t, Object, _getter, _setter>;

// int16
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using spPropertyRWInt16 = _spPropertyTypedRW<int16_t, Object, _getter, _setter>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using spPropertyRWInt = _spPropertyTypedRW<int, Object, _getter, _setter>;

// unsigned int 8
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using spPropertyRWUint8 = _spPropertyTypedRW<uint8_t, Object, _getter, _setter>;

// unsigned int 16
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using spPropertyRWUint16 = _spPropertyTypedRW<uint16_t, Object, _getter, _setter>;

// unsigned int
template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using spPropertyRWUint = _spPropertyTypedRW<uint, Object, _getter, _setter>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using spPropertyRWFloat = _spPropertyTypedRW<float, Object, _getter, _setter>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using spPropertyRWDouble = _spPropertyTypedRW<double, Object, _getter, _setter>;

//---------------------------------------------------------------------------------
// spPropertyRWString
//
// "strings are special"
//
// A read/write property string class that takes a getter and a setter method and the target object
//
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
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
    // Also the containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // Make sure the container type has spPropContainer as it's base class or it's a spObject
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
    // String - needed to overload the equality operator
    bool operator!=(const std::string &rhs)
    {
        return get() != rhs;
    }
    // String - needed to overload the equality operator
    bool operator==( const char * rhs)
    {
        return strcmp(get().c_str(), rhs) == 0;
    }
    // String - needed to overload the equality operator
    bool operator!=( const char * rhs)
    {
        return strcmp(get().c_str(), rhs) != 0;
    }
    //---------------------------------------------------------------------------------
    // get/set syntax
    std::string get() const
    {
        if (!my_object)
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this property.");            
            return "";
        }

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    void set(std::string const &value)
    {
        if (!my_object){
            spLog_E("Containing object not set. Verify spRegister() was called on this property.");            
            return;
        }

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
        // Make sure the container type has spPropContainer as it's base class or it's a spObject
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

    //---------------------------------------------------------------------------------
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
template <class Object> using spPropertyInt8 = _spPropertyTyped<Object, int8_t>;
template <class Object> using spPropertyInt16 = _spPropertyTyped<Object, int16_t>;
template <class Object> using spPropertyInt = _spPropertyTyped<Object, int>;
template <class Object> using spPropertyUint8 = _spPropertyTyped<Object, uint8_t>;
template <class Object> using spPropertyUint16 = _spPropertyTyped<Object, uint16_t>;
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
        // Make sure the container type has spPropContainer as it's base class or it's a spObject
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
    // String - needed to overload the equality operator
    bool operator!=(const std::string &rhs)
    {
        return get() != rhs;
    }
    // String - needed to overload the equality operator
    bool operator==( const char * rhs)
    {
        return strcmp(get().c_str(), rhs) == 0;
    }
    // String - needed to overload the equality operator
    bool operator!=( const char * rhs)
    {
        return strcmp(get().c_str(), rhs) != 0;
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
// TODO:
//   - Add instance ID counter

class spObject : public spPersist, public _spPropertyContainer, public spDescriptor
{

private:

    spObject *_parent;

    //---------------------------------------------------------------------------------
    static uint16_t getNextNameNumber(void)
    {
        static uint16_t _nextNumber=0;

        _nextNumber++;
        return _nextNumber;
    }

  public:
    spObject()
    {
        // setup a default name for this device.
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "spObject%04u", getNextNameNumber() );
        setName(szBuffer);
    }
    virtual ~spObject()
    {
    }

    void setParent(spObject *parent)
    {
        _parent = parent;
    }
    void setParent(spObject &parent)
    {
        setParent(&parent);
    }

    spObject *parent()
    {
        return _parent;
    }


    //---------------------------------------------------------------------------------
    virtual bool save(spStorage2 *pStorage)
    {

        spStorageBlock2 * stBlk = pStorage->beginBlock( name() );
        if ( !stBlk )
            return false;

        bool status = saveProperties(stBlk);
        if (!status)
            spLog_W("Error Saving a property for %s", name());


        pStorage->endBlock(stBlk);

        return status;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(spStorage2 *pStorage)
    {
        // Do we have this block in storage?
        spStorageBlock2 * stBlk = pStorage->getBlock( name() );


        if ( !stBlk )
        {
            spLog_E("Object Restore - error getting storage block");
            return true;  // nothing to restore
        }

        // restore props
        bool status = restoreProperties(stBlk);
        if (!status) 
            spLog_W("Error restoring a property for %s", name());           

        pStorage->endBlock(stBlk);

        return status;
    };
    //---------------------------------------------------------------------------------
    // Return the type ID of this
    virtual spTypeID getType(void)
    {
        return type();
    }
    //---------------------------------------------------------------------------------
    // A static type class for spObject
    static spTypeID type(void)
    {
        static spTypeID _myTypeID = sp_utils::getClassTypeID<spObject>();

        return _myTypeID;
    }
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
    auto size() -> decltype(_vector.size())
    {
        return _vector.size();
    }

    void push_back(T &value)
    {
        _vector.push_back(value);
        value->setParent(this);
    }
    void push_back(T *value)
    {
        _vector.push_back(value);
        value->setParent(this);
    }
    void pop_back(void)
    {
        _vector.pop_back();
    }

    auto back(void) -> decltype(_vector.back())
    {
        return _vector.back();
    }
    auto front() -> decltype(_vector.front())
    {
        return _vector.front();
    }
    T &at(size_t pos)
    {
        return _vector.at(pos);
    }
    auto cbegin() -> decltype(_vector.cbegin())
    {
        return _vector.cbegin();
    }
    auto cend() -> decltype(_vector.cend())
    {
        return _vector.cend();
    }

    auto begin() -> decltype(_vector.begin())
    {
        return _vector.begin();
    }
    auto end() -> decltype(_vector.end())
    {
        return _vector.end();
    }
    auto rbegin() -> decltype(_vector.rbegin())
    {
        return _vector.rbegin();
    }
    auto rend() -> decltype(_vector.rend())
    {
        return _vector.rend();
    }
    auto empty() -> decltype(_vector.empty())
    {
        return _vector.empty();
    }

    typedef typename std::vector<T>::iterator iterator;

    iterator erase(iterator pos)
    {
        return _vector.erase(pos);
    }

    // Defines a type specific static method - so can be called outside
    // of an instance.
    //
    // The typeID is determined by hashing the name of the class.
    // This way the type ID is consistant across invocations

    static spTypeID type(void)
    {
        static spTypeID _myTypeID = sp_utils::getClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    spTypeID getType(void)
    {
        return type();
    }

    //---------------------------------------------------------------------------------
    virtual bool save(spStorage2 *pStorage)
    {
        for( auto pObj: _vector)
        pObj->save(pStorage);
     
        return true;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(spStorage2 *pStorage)
    {
        for( auto pObj: _vector)
            pObj->restore(pStorage);

        return true;
    };
};
using spObjectContainer = spContainer<spObject *>;

//----------------------------------------------------------------------
// Handy template to test if an object is of a specific type
template <class T> bool spIsType(spObject *pObj)
{
    return (T::type() == pObj->getType());
};

// End - spCoreProps.h