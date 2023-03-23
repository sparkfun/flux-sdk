/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */

// flxCoreProps.h
//
// Defines the objects that make up the property system in the framework.
//
// Also includes the core object definition (flxObject and flxContainer)

#pragma once

#include <string>
#include <vector>

#include "flxCoreInterface.h"
#include "flxCoreTypes.h"
#include "flxStorage.h"
#include "flxUtils.h"

#define kMaxPropertyString 256

typedef enum
{
    flxEditSuccess = 0,
    flxEditFailure,
    flxEditOutOfRange
} flxEditResult_t;
//----------------------------------------------------------------------------------------
// flxProperty
//
// Base/Core Property Class
//
// From an abstract sense, a basic property - nothing more

class flxProperty : public flxDescriptor
{

  public:
    virtual flxDataType_t type(void) = 0;

    // Editor interface method - called to have the value of the property
    // displayed/set/managed in an editor

    virtual flxEditResult_t editValue(flxDataEditor &) = 0;
    virtual flxDataLimit *dataLimit(void) = 0;
    virtual bool setValue(flxDataVariable &) = 0;

    virtual bool hidden(void) = 0;
    virtual bool secure(void) = 0;
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
    virtual std::string to_string()
    {
        std::string s = "";
        return s;
    }
    //---------------------------------------------------------------------------------
    // continue to cascade down persistance interface (maybe do this later??)
    virtual bool save(flxStorageBlock *) = 0;
    virtual bool restore(flxStorageBlock *) = 0;
};

// simple def - list of flxProperty objects (it's a vector)
using flxPropertyList = std::vector<flxProperty *>;

//----------------------------------------------------------------------------------------
// flxPropertyContainer
//
// Define interface/class to manage a list of property
//
// The intent is to add this into other classes that want to expose properties.
//
class _flxPropertyContainer
{

  public:
    _flxPropertyContainer() : _nProperties{0}{}
    //---------------------------------------------------------------------------------
    void addProperty(flxProperty *newProperty)
    {

        if ( !newProperty)
            return;

        // We store hidden properties at the end of the list, all others at the
        // head.
        if (newProperty->hidden())
           _properties.push_back(newProperty);
        else
        {
            // find the location to insert the property...
            auto itProp = _properties.begin(); // get the iterator

            while (itProp != _properties.end())
            {
                // is the current prop hidden? If so, we will insert here, which
                // pushes the hidden values just past this value
                if ((*itProp)->hidden()) 
                    break;
                itProp++;
            }
            _nProperties++;
            _properties.insert(itProp, newProperty);
        }
    };

    //---------------------------------------------------------------------------------
    void addProperty(flxProperty &newProperty)
    {
        addProperty(&newProperty);
    };

    //---------------------------------------------------------------------------------
    void removeProperty(flxProperty *rmProp)
    {
        auto iter = std::find(_properties.begin(), _properties.end(), rmProp);

        if (iter != _properties.end())
            _properties.erase(iter);
    }

    //---------------------------------------------------------------------------------    
    void removeProperty(flxProperty &rmProp)
    {
        removeProperty(&rmProp);
    } 
    //---------------------------------------------------------------------------------
    flxPropertyList &getProperties(void)
    {
        return _properties;
    };

    uint nProperties(void)
    {

        return _nProperties;
    }
    //---------------------------------------------------------------------------------
    // save/restore for properties in this container. Note, since we
    // expect this to be a "mix-in" class, we use a different interface
    // for the save/restore routines

    bool saveProperties(flxStorageBlock *stBlk)
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
    bool restoreProperties(flxStorageBlock *stBlk)
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
        size_t totalSize = 0;

        for (auto property : _properties)
            totalSize += property->save_size();

        return totalSize;
    };

  private:
    flxPropertyList _properties;

    // The number of "visible" (not hidden) properties
    uint  _nProperties;  
};

//----------------------------------------------------------------------------------------
// _flxPropertyBase
//
// Template for a property object that implements typed operations for the property value
//
// Note - operator overloading isn't included. Easier to add in the actual property object
//        templates.  Although some "using" magic might work ...
//

template <class T, bool HIDDEN, bool SECURE> class _flxPropertyBase : public flxProperty, public _flxDataIn<T>, public _flxDataOut<T>
{

  public:
    _flxPropertyBase() : _isHidden{HIDDEN}, _isSecure{SECURE}
    {

    }

    bool hidden()
    {
        return _isHidden;
    }
    bool secure()
    {
        return _isSecure;
    }
    //---------------------------------------------------------------------------------
    flxDataType_t type()
    {
        return _flxDataOut<T>::type();
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
    bool save(flxStorageBlock *stBlk)
    {
        bool status = true;

        // We don't save hidden or secure properties if this is an external source
        if ( stBlk->kind() == flxStorage::flxStorageKindInternal || (!_isHidden && !_isSecure))
        {
            T c = get();
            bool status = stBlk->write(name(), c);

            if (!status)
                flxLog_E("Error saving property %s", name());
        }
        return status;
    };

    //---------------------------------------------------------------------------------
    bool restore(flxStorageBlock *stBlk)
    {
        T c;

        bool status = stBlk->read(name(), c);

        if (status)
            set(c);

        // If the value wasn't there, this is not a failure. So always return true
        return true;
    };

    // use this to route the call to our dataOut base class
    virtual std::string to_string(void)
    {
        return _flxDataOut<T>::getString();
    }
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    flxEditResult_t editValue(flxDataEditor &theEditor)
    {

        T value = get();

        bool bSuccess = theEditor.editField(value, secure());

        if (bSuccess) // success
        {
            // do we have a dataLimit set, and if so are we in limits?
            if (!_flxDataIn<T>::isValueValid(value))
                return flxEditOutOfRange;

            set(value);
        }

        return bSuccess ? flxEditSuccess : flxEditFailure;
    }
    //---------------------------------------------------------------------------------
    bool setValue(flxDataVariable &value)
    {

        if (value.type == type())
        {
            T c;
            set(value.get(c));
            return true;
        }
        return false;
    };

    flxDataLimit *dataLimit(void)
    {
        return _flxDataIn<T>::dataLimit();
    }
private:
    bool _isHidden;
    bool _isSecure;
};

//----------------------------------------------------------------------------------------
// _flxPropertyBaseString
//
// Strings are special ...
//
// There is some code duplication here - not happy about this - but strings
// are different, so they require a unique implementation. I'm sure there's some
// magic that could reduce the code duplication - but this isn't happening today ...
//
template <bool HIDDEN, bool SECURE>
class _flxPropertyBaseString : public flxProperty, _flxDataInString, _flxDataOutString
{
  protected:
    flxDataLimitType<std::string> *_dataLimit;

  public:
    _flxPropertyBaseString() : _dataLimit{nullptr}, _isHidden{HIDDEN}, _isSecure{SECURE}
    {
    }

    bool hidden()
    {
        return _isHidden;
    }
    bool secure()
    {
        return _isSecure;
    }

    flxDataType_t type()
    {
        return flxTypeString;
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

    std::string to_string()
    {
        return _flxDataOutString::getString();
    }
    //---------------------------------------------------------------------------------
    // serialization methods
    bool save(flxStorageBlock *stBlk)
    {
        bool status = true;

        // If this is a secure string and storage is internal, the strings are stored 
        // encrypted
        if ( stBlk->kind() == flxStorage::flxStorageKindInternal && _isSecure)
            return stBlk->saveSecureString(name(), get().c_str() );

        // If we are saving to an external source, we don't save hidden values or secure values.
        // But, for secure props, we to write the key and a blank string (makes it easier to enter values)

        // We don't save hidden or secure properties if this is an external source
        if ( stBlk->kind() == flxStorage::flxStorageKindInternal || !_isHidden)
        {
            // if a secure property and external storage, set value to an empty string
            std::string c = ( stBlk->kind() == flxStorage::flxStorageKindExternal && _isSecure) ? "" : get();

            status = stBlk->writeString(name(), c.c_str());
            if (!status)
                flxLog_E("Error saving string for property: %s", name());
        }
        return status;
    }

    //---------------------------------------------------------------------------------
    bool restore(flxStorageBlock *stBlk)
    {
        size_t len;

        // Secure string? 
        if ( stBlk->kind() == flxStorage::flxStorageKindInternal && _isSecure)
        {
            // get buffer length 
            len = stBlk->getBytesLength(name());
            if (!len)
                return false;

            char szBuffer[len];
            if (!stBlk->restoreSecureString(name(), szBuffer, len) ) 
                return false;

            set(szBuffer);
        }
        else
        {
            len = stBlk->getStringLength(name());
            if (!len)
                return false; 

            char szBuffer[len + 1]= {'\0'};
            len = stBlk->readString(name(), szBuffer, sizeof(szBuffer));

            set(szBuffer);

        }


        return true;
    };
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    flxEditResult_t editValue(flxDataEditor &theEditor)
    {

        std::string value = get();

        bool bSuccess = theEditor.editField(value, secure());

        if (bSuccess) // success
            set(value);

        return bSuccess ? flxEditSuccess : flxEditFailure;
        ;
    }
    // Data Limit things
    void setDataLimit(flxDataLimitType<std::string> &dataLimit)
    {
        _dataLimit = &dataLimit;
    }
    flxDataLimit *dataLimit(void)
    {
        return _dataLimit;
    }
    bool setValue(flxDataVariable &value)
    {

        if (value.type == type())
        {
            set(value.value.str);
            return true;
        }
        return false;
    };
private:

    bool _isHidden;
    bool _isSecure;
};

//----------------------------------------------------------------------------------------------------
// Now to define different methods on how the value of a property is get/set and stored
// These methods are:
//    - Read/Write property that calls user provided getter/setter methods
//    - Property object that provides storage for the property value
//
//----------------------------------------------------------------------------------------------------
// RW Property templated class: _flxPropertyTypedRW
//
//
// A read/write property base class that takes a getter and a setter method and the target object
//
//
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T), bool HIDDEN=false, bool SECURE=false>
class _flxPropertyTypedRW : public _flxPropertyBase<T, HIDDEN, SECURE>
{
    Object *my_object; // Pointer to the containing object

    //  member vars to cache an initial value until this object is connected to it's containing obj
    T _initialValue;
    bool _hasInitial;

  public:
    _flxPropertyTypedRW() : my_object(nullptr), _hasInitial{false}
    {
    }
    // Initial Value
    _flxPropertyTypedRW(T value) : my_object{nullptr}, _initialValue{value}, _hasInitial{true}
    {
    }

    // set min and max range
    _flxPropertyTypedRW(T min, T max)
    {
        _flxDataIn<T>::setDataLimitRange(min, max);
    }
    // initial value, min, max range
    _flxPropertyTypedRW(T value, T min, T max) : _flxPropertyTypedRW(value)
    {
        _flxDataIn<T>::setDataLimitRange(min, max);
    }

    // Limit data set
    _flxPropertyTypedRW(std::initializer_list<std::pair<const std::string, T>> limitSet)
    {
        _flxDataIn<T>::addDataLimitValidValue(limitSet);
    }
    // Initial value and limit data set.
    _flxPropertyTypedRW(T value, std::initializer_list<std::pair<const std::string, T>> limitSet)
        : _flxPropertyTypedRW(value)
    {
        _flxDataIn<T>::addDataLimitValidValue(limitSet);
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
        // my_object must be derived from _flxPropertyContainer
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value,
                      "_flxPropertyTypedRW: type parameter of this class must derive from flxPropertyContainer");

        my_object = obj;
        assert(my_object);

        if (my_object)
            my_object->addProperty(this);

        // This is basically the "registration" & init step of this object.
        // do we have an initial value? Now that we are "wired up" to the containing
        // class, we can pass on the initial value
        if (_hasInitial)
            set(_initialValue);
    }
    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            flxDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            flxDescriptor::setDescription(desc);

        // cascade to other version of method
        (*this)(obj, name);
    }

    //---------------------------------------------------------------------------------
    // get/set syntax
    T get() const
    {
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            flxLog_E("Containing object not set. Verify flxRegister() was called on this property.");
            return (T)0;
        }

        return (my_object->*_getter)();
    }
    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        if (!my_object)
        {
            flxLog_E("Containing object not set. Verify flxRegister() was called on this property.");
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

    _flxPropertyTypedRW<T, Object, _getter, _setter, HIDDEN, SECURE> &operator=(T const &value)
    {
        set(value);
        return *this;
    };
};

// Create typed read/writer property objects - type and RW objects as super classes

// bool
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWBool = _flxPropertyTypedRW<bool, Object, _getter, _setter>;

// int8
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter>;

// int16
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using flxPropertyRWInt = _flxPropertyTypedRW<int, Object, _getter, _setter>;

// unsigned int 8
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWUint8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter>;

// unsigned int 16
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWUint16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter>;

// unsigned int
template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using flxPropertyRWUint = _flxPropertyTypedRW<uint, Object, _getter, _setter>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWFloat = _flxPropertyTypedRW<float, Object, _getter, _setter>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWDouble = _flxPropertyTypedRW<double, Object, _getter, _setter>;


// HIDDEN
// bool
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWHiddenBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, true>;

// int8
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWHiddenInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, true>;

// int16
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWHiddenInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, true>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using flxPropertyRWHiddenInt = _flxPropertyTypedRW<int, Object, _getter, _setter, true>;

// unsigned int 8
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWHiddenUint8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, true>;

// unsigned int 16
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWHiddenUint16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, true>;

// unsigned int
template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using flxPropertyRWHiddenUint = _flxPropertyTypedRW<uint, Object, _getter, _setter, true>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWHiddenFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, true>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWHiddenDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, true>;


// Secure
// bool
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWSecureBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, false, true>;

// int8
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWSecureInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, false, true>;

// int16
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWSecureInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, false, true>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using flxPropertyRWSecureInt = _flxPropertyTypedRW<int, Object, _getter, _setter, false, true>;

// unsigned int 8
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWSecureUint8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, false, true>;

// unsigned int 16
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWSecureUint16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, false, true>;

// unsigned int
template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using flxPropertyRWSecureUint = _flxPropertyTypedRW<uint, Object, _getter, _setter, false, true>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWSecureFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, false, true>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWSecureDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, false, true>;


// Hidden Secure
// bool
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWSecretBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, true, true>;

// int8
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWSecretInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, true, true>;

// int16
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWSecretInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, true, true>;

// int
template <class Object, int (Object::*_getter)(), void (Object::*_setter)(int)>
using flxPropertyRWSecretInt = _flxPropertyTypedRW<int, Object, _getter, _setter, true, true>;

// unsigned int 8
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWSecretUint8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, true, true>;

// unsigned int 16
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWSecretUint16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, true, true>;

// unsigned int
template <class Object, uint (Object::*_getter)(), void (Object::*_setter)(uint)>
using flxPropertyRWSecretUint = _flxPropertyTypedRW<uint, Object, _getter, _setter, true, true>;

// float
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWSecretFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, true, true>;

// double
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWSecretDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, true, true>;

//---------------------------------------------------------------------------------
// flxPropertyRWString
//
// "strings are special"
//
// A read/write property string class that takes a getter and a setter method and the target object
//
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string), bool HIDDEN=false, bool SECURE=false>
class flxPropertyRWString : public _flxPropertyBaseString<HIDDEN, SECURE>
{
    Object *my_object;

    //  member vars to cache an initial value until this object is connected to it's containing obj
    std::string _initialValue;
    bool _hasInitial;

  public:
    flxPropertyRWString() : my_object(0), _hasInitial{false}
    {
    }
    // Initial Value
    flxPropertyRWString(std::string value) : my_object{nullptr}, _initialValue{value}, _hasInitial{true}
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
        // Make sure the container type has spPropContainer as it's base class or it's a flxObject
        // Compile-time check
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value,
                      "_flxPropertyTypedRWString: type parameter of this class must derive from flxPropertyContainer");

        my_object = obj;
        assert(my_object);
        if (my_object)
            my_object->addProperty(this);

        // This is basically the "registration" & init step of this object.
        // do we have an initial value? Now that we are "wired up" to the containing
        // class, we can pass on the initial value
        if (_hasInitial)
            set(_initialValue);
    }

    void operator()(Object *obj, const char *name)
    {
        // set the name of the property on init
        if (name)
            flxDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            flxDescriptor::setDescription(desc);

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
    bool operator==(const char *rhs)
    {
        return strcmp(get().c_str(), rhs) == 0;
    }
    // String - needed to overload the equality operator
    bool operator!=(const char *rhs)
    {
        return strcmp(get().c_str(), rhs) != 0;
    }
    //---------------------------------------------------------------------------------
    // get/set syntax
    std::string get() const
    {
        if (!my_object)
        {
            flxLog_E("Containing object not set. Verify flxRegister() was called on this property.");
            return "";
        }

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    void set(std::string const &value)
    {
        if (!my_object)
        {
            flxLog_E("Containing object not set. Verify flxRegister() was called on this property.");
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
    flxPropertyRWString<Object, _getter, _setter, HIDDEN, SECURE> &operator=(std::string const &value)
    {
        set(value);
        return *this;
    };
};

// HIDDEN
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWHiddenString = flxPropertyRWString<Object, _getter, _setter, true, false>;

// SECURE
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWSecureString = flxPropertyRWString<Object, _getter, _setter, false, true>;

// Hidden and SECURE
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWSecretString = flxPropertyRWString<Object, _getter, _setter, true, true>;
//----------------------------------------------------------------------------------------------------
// flxPropertyTyped
//
// Template class for a property object that contains storage for the property.
//
template <class Object, class T, bool HIDDEN=false, bool SECURE=false> 
class _flxPropertyTyped : public _flxPropertyBase<T, HIDDEN, SECURE>
{
  public:
    _flxPropertyTyped()
    {
    }
    // Create property with an initial value
    _flxPropertyTyped(T value) : data{value}
    {
    }

    // Just a limit range
    _flxPropertyTyped(T min, T max)
    {
        _flxDataIn<T>::setDataLimitRange(min, max);
    }

    // Initial value and a limit range
    _flxPropertyTyped(T value, T min, T max) : _flxPropertyTyped(value)
    {
        _flxDataIn<T>::setDataLimitRange(min, max);
    }
    // Limit data set
    _flxPropertyTyped(std::initializer_list<std::pair<const std::string, T>> limitSet)
    {
        _flxDataIn<T>::addDataLimitValidValue(limitSet);
    }
    // Initial value and limit data set.
    _flxPropertyTyped(T value, std::initializer_list<std::pair<const std::string, T>> limitSet) : _flxPropertyTyped(value)
    {
        _flxDataIn<T>::addDataLimitValidValue(limitSet);
    }

    // to register the property - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     property_obj(this);
    //
    // This allows the property to add itself to the containing objects list of
    // properties.
    void operator()(Object *me)
    {
        // Make sure the container type has spPropContainer as it's base class or it's a flxObject
        // Compile-time check
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value,
                      "_flxPropertyTyped: type parameter of this class must derive from flxPropertyContainer");

        // my_object must be derived from _flxPropertyContainer
        assert(me);
        if (me)
            me->addProperty(this);
    }
    void operator()(Object *obj, const char *name)
    {

        // set the name of the property on init
        if (name)
            flxDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            flxDescriptor::setDescription(desc);

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
    _flxPropertyTyped<Object, T, HIDDEN, SECURE> &operator=(T const &value)
    {
        set(value);
        return *this;
    };

  private:
    T data; // actual storage for the property
};

// Define typed properties
template <class Object> using flxPropertyBool = _flxPropertyTyped<Object, bool>;
template <class Object> using flxPropertyInt8 = _flxPropertyTyped<Object, int8_t>;
template <class Object> using flxPropertyInt16 = _flxPropertyTyped<Object, int16_t>;
template <class Object> using flxPropertyInt = _flxPropertyTyped<Object, int>;
template <class Object> using flxPropertyUint8 = _flxPropertyTyped<Object, uint8_t>;
template <class Object> using flxPropertyUint16 = _flxPropertyTyped<Object, uint16_t>;
template <class Object> using flxPropertyUint = _flxPropertyTyped<Object, uint>;
template <class Object> using flxPropertyFloat = _flxPropertyTyped<Object, float>;
template <class Object> using flxPropertyDouble = _flxPropertyTyped<Object, double>;

// Define typed properties - HIDDEN
template <class Object> using flxPropertyHiddenBool = _flxPropertyTyped<Object, bool, true>;
template <class Object> using flxPropertyHiddenInt8 = _flxPropertyTyped<Object, int8_t, true>;
template <class Object> using flxPropertyHiddenInt16 = _flxPropertyTyped<Object, int16_t, true>;
template <class Object> using flxPropertyHiddenInt = _flxPropertyTyped<Object, int, true>;
template <class Object> using flxPropertyHiddenUint8 = _flxPropertyTyped<Object, uint8_t, true>;
template <class Object> using flxPropertyHiddenUint16 = _flxPropertyTyped<Object, uint16_t, true>;
template <class Object> using flxPropertyHiddenUint = _flxPropertyTyped<Object, uint, true>;
template <class Object> using flxPropertyHiddenFloat = _flxPropertyTyped<Object, float, true>;
template <class Object> using flxPropertyHiddenDouble = _flxPropertyTyped<Object, double, true>;

// Define typed properties - SECURE
template <class Object> using flxPropertySecureBool = _flxPropertyTyped<Object, bool, false, true>;
template <class Object> using flxPropertySecureInt8 = _flxPropertyTyped<Object, int8_t, false, true>;
template <class Object> using flxPropertySecureInt16 = _flxPropertyTyped<Object, int16_t, false, true>;
template <class Object> using flxPropertySecureInt = _flxPropertyTyped<Object, int, false, true>;
template <class Object> using flxPropertySecureUint8 = _flxPropertyTyped<Object, uint8_t, false, true>;
template <class Object> using flxPropertySecureUint16 = _flxPropertyTyped<Object, uint16_t, false, true>;
template <class Object> using flxPropertySecureUint = _flxPropertyTyped<Object, uint, false, true>;
template <class Object> using flxPropertySecureFloat = _flxPropertyTyped<Object, float, false, true>;
template <class Object> using flxPropertySecureDouble = _flxPropertyTyped<Object, double, false, true>;

// Define typed properties - SECURE
template <class Object> using flxPropertySecretBool = _flxPropertyTyped<Object, bool, true, true>;
template <class Object> using flxPropertySecretInt8 = _flxPropertyTyped<Object, int8_t, true, true>;
template <class Object> using flxPropertySecretInt16 = _flxPropertyTyped<Object, int16_t, true, true>;
template <class Object> using flxPropertySecretInt = _flxPropertyTyped<Object, int, true, true>;
template <class Object> using flxPropertySecretUint8 = _flxPropertyTyped<Object, uint8_t, true, true>;
template <class Object> using flxPropertySecretUint16 = _flxPropertyTyped<Object, uint16_t, true, true>;
template <class Object> using flxPropertySecretUint = _flxPropertyTyped<Object, uint, true, true>;
template <class Object> using flxPropertySecretFloat = _flxPropertyTyped<Object, float, true, true>;
template <class Object> using flxPropertySecretDouble = _flxPropertyTyped<Object, double, true, true>;
//----------------------------------------------------------------------------------------------------
// flxPropertyString
//
// "Strings are special"
//
// Implements the property, but uses string specific logic

template <class Object, bool HIDDEN=false, bool SECURE=false> 
class flxPropertyString : public _flxPropertyBaseString<HIDDEN, SECURE>
{

  public:
    flxPropertyString()
    {
    }

    // Create property with an initial value
    flxPropertyString(std::string value) : data{value}
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

    void operator()(Object *me)
    {
        // Make sure the container type has spPropContainer as it's base class or it's a flxObject
        // Compile-time check
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value,
                      "_flxPropertyString: type parameter of this class must derive from flxPropertyContainer");

        assert(me);
        if (me)
            me->addProperty(this);
    }
    // set the name of the property on init
    void operator()(Object *obj, const char *name)
    {
        if (name)
            flxDescriptor::setName(name);

        // cascade to other version of method
        (*this)(obj);
    }

    // allow the passing in of a name and description
    void operator()(Object *obj, const char *name, const char *desc)
    {
        // Description of the object
        if (desc)
            flxDescriptor::setDescription(desc);

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
    bool operator==(const char *rhs)
    {
        return strcmp(get().c_str(), rhs) == 0;
    }
    // String - needed to overload the equality operator
    bool operator!=(const char *rhs)
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
    flxPropertyString<Object, HIDDEN, SECURE> &operator=(std::string const &value)
    {
        set(value);
        return *this;
    };

  private:
    std::string data; // storage for the property
};

// HIDDEN
template <class Object>
using flxPropertyHiddenString = flxPropertyString<Object, true, false>;

// SECURE
template <class Object>
using flxPropertySecureString = flxPropertyString<Object, false, true>;


// Hidden/SECURE
template <class Object>
using flxPropertySecretString = flxPropertyString<Object, true, true>;
//----------------------------------------------------------------------------------------------------
// flxObject
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

class flxObject : public flxPersist, public _flxPropertyContainer, public flxDescriptor
{

  private:
    flxObject *_parent;

    //---------------------------------------------------------------------------------
    static uint16_t getNextNameNumber(void)
    {
        static uint16_t _nextNumber = 0;

        _nextNumber++;
        return _nextNumber;
    }

  public:
    flxObject()
    {
        // setup a default name for this device.
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "flxObject%04u", getNextNameNumber());
        setName(szBuffer);
    }
    virtual ~flxObject()
    {
    }

    void setParent(flxObject *parent)
    {
        _parent = parent;
    }
    void setParent(flxObject &parent)
    {
        setParent(&parent);
    }

    flxObject *parent()
    {
        return _parent;
    }

    //---------------------------------------------------------------------------------
    virtual bool save(flxStorage *pStorage)
    {

        flxStorageBlock *stBlk = pStorage->beginBlock(name());
        if (!stBlk)
            return false;

        bool status = saveProperties(stBlk);
        if (!status)
            flxLog_W("Error Saving a property for %s", name());

        pStorage->endBlock(stBlk);

        return status;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(flxStorage *pStorage)
    {
        // Do we have this block in storage?
        flxStorageBlock *stBlk = pStorage->getBlock(name());

        if (!stBlk)
        {
            flxLog_I("Object Restore - error getting storage block");
            return true; // nothing to restore
        }

        // restore props
        bool status = restoreProperties(stBlk);
        if (!status)
            flxLog_D("Error restoring a property for %s", name());

        pStorage->endBlock(stBlk);

        return true;
    };
    //---------------------------------------------------------------------------------
    // Return the type ID of this
    virtual flxTypeID getType(void)
    {
        return type();
    }
    //---------------------------------------------------------------------------------
    // A static type class for flxObject
    static flxTypeID type(void)
    {
        static flxTypeID _myTypeID = flxGetClassTypeID<flxObject>();

        return _myTypeID;
    }
};
//####################################################################
// Container update
//
// flxContainer
//
// A list/container that holds flxObjects and supports serialization. The
// container itself is a object

template <class T> class flxContainer : public flxObject
{
  protected:
    // Use a vector to store data
    std::vector<T> _vector;

  public:
    flxContainer() : _vector()
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
        // make sure the value isn't already in the list...
        if (std::find(_vector.begin(), _vector.end(), value) != _vector.end())
            return;

        _vector.push_back(value);
        value->setParent(this);
    }
    void pop_back(void)
    {
        _vector.pop_back();
    }

    void insert(typename std::vector<T>::iterator it, T value)
    {
       _vector.insert(it, value);
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

    static flxTypeID type(void)
    {
        static flxTypeID _myTypeID = flxGetClassTypeID<T>();

        return _myTypeID;
    }

    // Return the type ID of this
    flxTypeID getType(void)
    {
        return type();
    }

    //---------------------------------------------------------------------------------
    virtual bool save(flxStorage *pStorage)
    {
        for (auto pObj : _vector)
            pObj->save(pStorage);

        return true;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(flxStorage *pStorage)
    {
        for (auto pObj : _vector)
            pObj->restore(pStorage);

        return true;
    };
};
using flxObjectContainer = flxContainer<flxObject *>;

//----------------------------------------------------------------------
// Handy template to test if an object is of a specific type
template <class T> bool flxIsType(flxObject *pObj)
{
    return (T::type() == pObj->getType());
};

// End - flxCoreProps.h