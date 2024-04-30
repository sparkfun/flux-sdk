/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
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
    virtual flxDataVariable getValue(void) = 0;

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
    // continue to cascade down persistence interface (maybe do this later??)
    virtual bool save(flxStorageBlock *) = 0;
    virtual bool restore(flxStorageBlock *) = 0;
};

/**
 * @brief Define a property list type - vector of properties
 *
 */
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
    _flxPropertyContainer() : _nProperties{0}
    {
    }
    //---------------------------------------------------------------------------------
    void addProperty(flxProperty *newProperty)
    {

        if (!newProperty)
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
    uint _nProperties;
};

//----------------------------------------------------------------------------------------
// _flxPropertyBase
//
// Template for a property object that implements typed operations for the property value
//
// Note - operator overloading isn't included. Easier to add in the actual property object
//        templates.  Although some "using" magic might work ...
//
/**
 * @brief A core template to define a property object that implements a typed value that
 * sets/gets an attribute value of a objects *property*
 *
 * This class implements the core behavior that is used by all property objects
 *
 * @tparam T  The type of the property value
 * @tparam HIDDEN If true, this a hidden property - not shown to users - for internal object use
 * @tparam SECURE If true, this is a secure property - the value is encrypted when stored on device
 */
template <class T, bool HIDDEN, bool SECURE>
class _flxPropertyBase : public flxProperty, public _flxDataIn<T>, public _flxDataOut<T>
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
        if (stBlk->kind() == flxStorage::flxStorageKindInternal || (!_isHidden && !_isSecure))
        {
            T c = get();
            bool status = stBlk->write(name(), c);

            if (!status)
                flxLogM_E(kMsgErrSavingProperty, name());
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

    //---------------------------------------------------------------------------------
    // method to get the value of a prop - as a variable
    flxDataVariable getValue(void)
    {
        flxDataVariable value;
        value.type = type();
        T c = get();
        value.set(c);
        return value;
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
        if (stBlk->kind() == flxStorage::flxStorageKindInternal && _isSecure)
            return stBlk->saveSecureString(name(), get().c_str());

        // If we are saving to an external source, we don't save hidden values or secure values.
        // But, for secure props, we to write the key and a blank string (makes it easier to enter values)

        // We don't save hidden or secure properties if this is an external source
        if (stBlk->kind() == flxStorage::flxStorageKindInternal || !_isHidden)
        {
            // if a secure property and external storage, set value to an empty string
            std::string c = (stBlk->kind() == flxStorage::flxStorageKindExternal && _isSecure) ? "" : get();

            status = stBlk->writeString(name(), c.c_str());
            if (!status)
                flxLogM_E(kMsgErrSavingProperty, name());
        }
        return status;
    }

    //---------------------------------------------------------------------------------
    bool restore(flxStorageBlock *stBlk)
    {
        size_t len;

        // Secure string?
        if (stBlk->kind() == flxStorage::flxStorageKindInternal && _isSecure)
        {
            // get buffer length. Note, add one to make sure we have room for line termination
            len = stBlk->getBytesLength(name()) + 1;
            if (!len)
                return false;

            char szBuffer[len];
            if (!stBlk->restoreSecureString(name(), szBuffer, len))
                return false;

            set(szBuffer);
        }
        else
        {
            len = stBlk->getStringLength(name());
            if (!len)
                return false;

            char szBuffer[len + 1] = {'\0'};
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
            set(value.to_string().c_str());
            return true;
        }
        return false;
    };
    //---------------------------------------------------------------------------------
    // method to get the value of a prop - as a variable
    flxDataVariable getValue(void)
    {
        flxDataVariable value;
        value.type = type();
        std::string c = get();
        value.set(c);
        return value;
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
template <class T, class Object, T (Object::*_getter)(), void (Object::*_setter)(T), bool HIDDEN = false,
          bool SECURE = false>
class _flxPropertyTypedRW : public _flxPropertyBase<T, HIDDEN, SECURE>
{
    Object *my_object; // Pointer to the containing object

    //  member vars to cache an initial value until this object is connected to it's containing obj
    T _initialValue;
    bool _hasInitial;

  public:
    _flxPropertyTypedRW() : my_object{nullptr}, _hasInitial{false}
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
    void operator()(Object *obj, bool skipAdd = false)
    {
        // my_object must be derived from _flxPropertyContainer
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value, "TypedRW: invalid object");

        my_object = obj;
        assert(my_object);

        if (my_object && !skipAdd)
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
            flxLogM_E(kMsgParentObjNotSet, "property");
            return (T)0;
        }

        return (my_object->*_getter)();
    }
    //---------------------------------------------------------------------------------
    void set(T const &value)
    {
        if (!my_object)
        {
            flxLogM_E(kMsgParentObjNotSet, "property");
            return; // would normally throw an exception, but not very Arduino like!
        }

        (my_object->*_setter)(value);
        my_object->setIsDirty();
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

/**
 * @brief A boolean read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWBool = _flxPropertyTypedRW<bool, Object, _getter, _setter>;

/**
 * @brief A int8_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(int8_t) The method of Object to call when the property is set
 */
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter>;

/**
 * @brief A int16_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter>;

/**
 * @brief A int32_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int32_t (Object::*_getter)(), void (Object::*_setter)(int32_t)>
using flxPropertyRWInt32 = _flxPropertyTypedRW<int32_t, Object, _getter, _setter>;

/**
 * @brief A uint8_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWUInt8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter>;

/**
 * @brief A uint16_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWUInt16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter>;

/**
 * @brief A uint32_t read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint32_t (Object::*_getter)(), void (Object::*_setter)(uint32_t)>
using flxPropertyRWUInt32 = _flxPropertyTypedRW<uint32_t, Object, _getter, _setter>;

/**
 * @brief A float read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWFloat = _flxPropertyTypedRW<float, Object, _getter, _setter>;

/**
 * @brief A double read/write property object that takes a getter and a setter method and the target object.
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWDouble = _flxPropertyTypedRW<double, Object, _getter, _setter>;

// HIDDEN

/**
 * @brief A HIDDEN boolean read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWHiddenBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN int8_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWHiddenInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN int16_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWHiddenInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN int32_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int32_t (Object::*_getter)(), void (Object::*_setter)(int32_t)>
using flxPropertyRWHiddenInt32 = _flxPropertyTypedRW<int32_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN uint8_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWHiddenUInt8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN uint16_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWHiddenUInt16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN uint32_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint32_t (Object::*_getter)(), void (Object::*_setter)(uint32_t)>
using flxPropertyRWHiddenUInt32 = _flxPropertyTypedRW<uint32_t, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN float read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWHiddenFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, true>;

/**
 * @brief A HIDDEN double read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWHiddenDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, true>;

// Secure
/**
 * @brief A SECURE bool read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWSecureBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE int8_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWSecureInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE int16_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWSecureInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE int32_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int32_t (Object::*_getter)(), void (Object::*_setter)(int32_t)>
using flxPropertyRWSecureInt32 = _flxPropertyTypedRW<int32_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE uint8_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */

template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWSecureUInt8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE uint16_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWSecureUInt16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE uint32_t read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint32_t (Object::*_getter)(), void (Object::*_setter)(uint32_t)>
using flxPropertyRWSecureUInt32 = _flxPropertyTypedRW<uint32_t, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE float read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWSecureFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, false, true>;

/**
 * @brief A SECURE double read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWSecureDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, false, true>;

// Hidden Secure
/**
 * @brief A HIDDEN and SECURE bool read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, bool (Object::*_getter)(), void (Object::*_setter)(bool)>
using flxPropertyRWSecretBool = _flxPropertyTypedRW<bool, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE int8_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int8_t (Object::*_getter)(), void (Object::*_setter)(int8_t)>
using flxPropertyRWSecretInt8 = _flxPropertyTypedRW<int8_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE int16_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int16_t (Object::*_getter)(), void (Object::*_setter)(int16_t)>
using flxPropertyRWSecretInt16 = _flxPropertyTypedRW<int16_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE int32_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, int32_t (Object::*_getter)(), void (Object::*_setter)(int32_t)>
using flxPropertyRWSecretInt32 = _flxPropertyTypedRW<int32_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE uint8_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint8_t (Object::*_getter)(), void (Object::*_setter)(uint8_t)>
using flxPropertyRWSecretUInt8 = _flxPropertyTypedRW<uint8_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE uint16_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint16_t (Object::*_getter)(), void (Object::*_setter)(uint16_t)>
using flxPropertyRWSecretUInt16 = _flxPropertyTypedRW<uint16_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE uint32_t read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, uint32_t (Object::*_getter)(), void (Object::*_setter)(uint32_t)>
using flxPropertyRWSecretUInt32 = _flxPropertyTypedRW<uint32_t, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE float read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, float (Object::*_getter)(), void (Object::*_setter)(float)>
using flxPropertyRWSecretFloat = _flxPropertyTypedRW<float, Object, _getter, _setter, true, true>;

/**
 * @brief A HIDDEN and SECURE double read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, double (Object::*_getter)(), void (Object::*_setter)(double)>
using flxPropertyRWSecretDouble = _flxPropertyTypedRW<double, Object, _getter, _setter, true, true>;

//---------------------------------------------------------------------------------
// flxPropertyRWString
//
// "strings are special"
//
// A read/write property string class that takes a getter and a setter method and the target object
//
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string), bool HIDDEN = false,
          bool SECURE = false>
class flxPropertyRWString : public _flxPropertyBaseString<HIDDEN, SECURE>
{
    Object *my_object;

    //  member vars to cache an initial value until this object is connected to it's containing obj
    std::string _initialValue;
    bool _hasInitial;

  public:
    flxPropertyRWString() : my_object{nullptr}, _hasInitial{false}
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
    void operator()(Object *obj, bool skipAdd = false)
    {
        // Make sure the container type has spPropContainer as it's base class or it's a flxObject
        // Compile-time check
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value, "RWString: invalid object");

        my_object = obj;
        assert(my_object);

        if (my_object && !skipAdd)
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
            flxLogM_E(kMsgParentObjNotSet, "property");
            return "";
        }

        return (my_object->*_getter)();
    }

    //---------------------------------------------------------------------------------
    void set(std::string const &value)
    {
        if (!my_object)
        {
            flxLogM_E(kMsgParentObjNotSet, "property");
            return;
        }

        (my_object->*_setter)(value);
        my_object->setIsDirty();
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

/**
 * @brief A HIDDEN uint8_t read/write property object that takes a getter and a setter method and the target object.
 * This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWHiddenString = flxPropertyRWString<Object, _getter, _setter, true, false>;

/**
 * @brief A SECURE string read/write property object that takes a getter and a setter method and the target
 * object. This property is secure - the value is encrypted when stored on device and not exported from the device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWSecureString = flxPropertyRWString<Object, _getter, _setter, false, true>;

/**
 * @brief A HIDDEN and SECURE string read/write property object that takes a getter and a setter method and the target
 * object. This property is hidden from the user - not shown in editors, property sheets or in exported setting lists
 * and this property is secure - the value is encrypted when stored on device
 *
 * @tparam Object The containing object of this property
 * @tparam (Object::*_getter)() The method of Object to call when the property is read
 * @tparam (Object::*_setter)(bool) The method of Object to call when the property is set
 */
template <class Object, std::string (Object::*_getter)(), void (Object::*_setter)(std::string)>
using flxPropertyRWSecretString = flxPropertyRWString<Object, _getter, _setter, true, true>;
//----------------------------------------------------------------------------------------------------
// flxPropertyTyped
//
// Template class for a property object that contains storage for the property.
//
template <class Object, class T, bool HIDDEN = false, bool SECURE = false>
class _flxPropertyTyped : public _flxPropertyBase<T, HIDDEN, SECURE>
{
    Object *my_object; // Pointer to the containing object

  public:
    _flxPropertyTyped() : my_object{nullptr}
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
    _flxPropertyTyped(T value, std::initializer_list<std::pair<const std::string, T>> limitSet)
        : _flxPropertyTyped(value)
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
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value, "PropTyped: invalid object");

        // my_object must be derived from _flxPropertyContainer
        assert(me);
        if (me)
        {
            me->addProperty(this);
            my_object = me;
        }
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

        if (my_object)
            my_object->setIsDirty();
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
template <class Object> using flxPropertyInt32 = _flxPropertyTyped<Object, int32_t>;
template <class Object> using flxPropertyUInt8 = _flxPropertyTyped<Object, uint8_t>;
template <class Object> using flxPropertyUInt16 = _flxPropertyTyped<Object, uint16_t>;
template <class Object> using flxPropertyUInt32 = _flxPropertyTyped<Object, uint32_t>;
template <class Object> using flxPropertyFloat = _flxPropertyTyped<Object, float>;
template <class Object> using flxPropertyDouble = _flxPropertyTyped<Object, double>;

// Define typed properties - HIDDEN
template <class Object> using flxPropertyHiddenBool = _flxPropertyTyped<Object, bool, true>;
template <class Object> using flxPropertyHiddenInt8 = _flxPropertyTyped<Object, int8_t, true>;
template <class Object> using flxPropertyHiddenInt16 = _flxPropertyTyped<Object, int16_t, true>;
template <class Object> using flxPropertyHiddenInt32 = _flxPropertyTyped<Object, int32_t, true>;
template <class Object> using flxPropertyHiddenUInt8 = _flxPropertyTyped<Object, uint8_t, true>;
template <class Object> using flxPropertyHiddenUInt16 = _flxPropertyTyped<Object, uint16_t, true>;
template <class Object> using flxPropertyHiddenUInt32 = _flxPropertyTyped<Object, uint32_t, true>;
template <class Object> using flxPropertyHiddenFloat = _flxPropertyTyped<Object, float, true>;
template <class Object> using flxPropertyHiddenDouble = _flxPropertyTyped<Object, double, true>;

// Define typed properties - SECURE
template <class Object> using flxPropertySecureBool = _flxPropertyTyped<Object, bool, false, true>;
template <class Object> using flxPropertySecureInt8 = _flxPropertyTyped<Object, int8_t, false, true>;
template <class Object> using flxPropertySecureInt16 = _flxPropertyTyped<Object, int16_t, false, true>;
template <class Object> using flxPropertySecureInt32 = _flxPropertyTyped<Object, int32_t, false, true>;
template <class Object> using flxPropertySecureUInt8 = _flxPropertyTyped<Object, uint8_t, false, true>;
template <class Object> using flxPropertySecureUInt16 = _flxPropertyTyped<Object, uint16_t, false, true>;
template <class Object> using flxPropertySecureUInt32 = _flxPropertyTyped<Object, uint32_t, false, true>;
template <class Object> using flxPropertySecureFloat = _flxPropertyTyped<Object, float, false, true>;
template <class Object> using flxPropertySecureDouble = _flxPropertyTyped<Object, double, false, true>;

// Define typed properties - SECURE
template <class Object> using flxPropertySecretBool = _flxPropertyTyped<Object, bool, true, true>;
template <class Object> using flxPropertySecretInt8 = _flxPropertyTyped<Object, int8_t, true, true>;
template <class Object> using flxPropertySecretInt16 = _flxPropertyTyped<Object, int16_t, true, true>;
template <class Object> using flxPropertySecretInt32 = _flxPropertyTyped<Object, int32_t, true, true>;
template <class Object> using flxPropertySecretUInt8 = _flxPropertyTyped<Object, uint8_t, true, true>;
template <class Object> using flxPropertySecretUInt16 = _flxPropertyTyped<Object, uint16_t, true, true>;
template <class Object> using flxPropertySecretUInt32 = _flxPropertyTyped<Object, uint32_t, true, true>;
template <class Object> using flxPropertySecretFloat = _flxPropertyTyped<Object, float, true, true>;
template <class Object> using flxPropertySecretDouble = _flxPropertyTyped<Object, double, true, true>;
//----------------------------------------------------------------------------------------------------
// flxPropertyString
//
// "Strings are special"
//
// Implements the property, but uses string specific logic

template <class Object, bool HIDDEN = false, bool SECURE = false>
class flxPropertyString : public _flxPropertyBaseString<HIDDEN, SECURE>
{
    Object *my_object;

  public:
    flxPropertyString() : my_object{nullptr}
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
        static_assert(std::is_base_of<_flxPropertyContainer, Object>::value, "PropString: invalid object");

        assert(me);
        if (me)
            me->addProperty(this);
        my_object = me;
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
        if (my_object)
            my_object->setIsDirty();
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
template <class Object> using flxPropertyHiddenString = flxPropertyString<Object, true, false>;

// SECURE
template <class Object> using flxPropertySecureString = flxPropertyString<Object, false, true>;

// Hidden/SECURE
template <class Object> using flxPropertySecretString = flxPropertyString<Object, true, true>;
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
    bool _hidden;
    bool _isDirty; // needs saving of props/data

    //---------------------------------------------------------------------------------
    static uint16_t getNextNameNumber(void)
    {
        static uint16_t _nextNumber = 0;

        _nextNumber++;
        return _nextNumber;
    }

  public:
    flxObject() : _hidden{false}, _parent(nullptr), _isDirty{false}
    {
        // setup a default name for this device.
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "flxObject%04u", getNextNameNumber());
        setName(szBuffer); // make sure we create a copy - this name is stack based
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

    void setHidden(bool bHide = true)
    {
        _hidden = bHide;
    }
    bool hidden()
    {
        return _hidden;
    }

    void setIsDirty(bool bDirty = true)
    {
        _isDirty = bDirty;

        // if we have a parent, set it's dirty flag - if we are dirty now, so is our parent
        if (_parent != nullptr && bDirty)
            _parent->setIsDirty();
    }

    bool isDirty(void)
    {
        return _isDirty;
    }
    //---------------------------------------------------------------------------------
    virtual bool onSave(flxStorageBlock *stBlk)
    {
        bool status = saveProperties(stBlk);

        // clear our dirty flag
        if (status)
            setIsDirty(false);

        return status;
    }
    //---------------------------------------------------------------------------------
    virtual bool save(flxStorage *pStorage)
    {

        flxStorageBlock *stBlk = pStorage->beginBlock(name());
        if (!stBlk)
            return false;

        bool status = onSave(stBlk);

        if (!status)
            flxLogM_W(kMsgErrSaveResState, "saving", name());

        pStorage->endBlock(stBlk);

        return status;
    };

    //---------------------------------------------------------------------------------
    virtual bool onRestore(flxStorageBlock *stBlk)
    {
        return restoreProperties(stBlk);
    }
    //---------------------------------------------------------------------------------
    virtual bool restore(flxStorage *pStorage)
    {
        // Do we have this block in storage?
        flxStorageBlock *stBlk = pStorage->getBlock(name());

        if (!stBlk)
        {
            flxLogM_I(kMsgErrSaveResState, "restoring", name());
            return true; // nothing to restore
        }

        // restore the object
        bool status = onRestore(stBlk);

        if (!status)
            flxLogM_D(kMsgErrSaveResState, "restoring", name());

        pStorage->endBlock(stBlk);

        // when you restore and set new values to this object, the dirty flag is set
        // But since the values were from storage, we are not dirty
        setIsDirty(false);

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
// ####################################################################
//  Container update
//
//  flxContainer
//
//  A list/container that holds flxObjects and supports serialization. The
//  container itself is a object

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

    void push_back(T value)
    {
        // make sure the value isn't already in the list...
        if (std::find(_vector.begin(), _vector.end(), value) != _vector.end())
        {
            flxLogM_I(kMsgNotAddDupDev, name());
            return;
        }
        _vector.push_back(value);

        // DONT overwrite a parent
        if (!value->parent())
            value->setParent(this);
    }

    void pop_back(void)
    {
        _vector.pop_back();
    }

    void insert(typename std::vector<T>::iterator it, T value)
    {
        // make sure the value isn't already in the list...
        if (std::find(_vector.begin(), _vector.end(), value) != _vector.end())
        {
            flxLogM_I(kMsgNotAddDupDev, name());
            return;
        }
        _vector.insert(it, value);

        // DONT overwrite a parent
        if (!value->parent())
            value->setParent(this);
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

    // simple remove based on value
    void remove(T value)
    {
        // in the vector?
        iterator it = std::find(_vector.begin(), _vector.end(), value);

        if (it == _vector.end())
            return;

        _vector.erase(it);
    }

    // Defines a type specific static method - so can be called outside
    // of an instance.
    //
    // The typeID is determined by hashing the name of the class.
    // This way the type ID is consistent across invocations

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
        // save ourselves
        flxObject::save(pStorage);

        // Save the children
        for (auto pObj : _vector)
            pObj->save(pStorage);

        return true;
    };

    //---------------------------------------------------------------------------------
    virtual bool restore(flxStorage *pStorage)
    {
        // restore our children
        for (auto pObj : _vector)
            pObj->restore(pStorage);

        // restore ourselves - after our children to manage our dirty flag
        flxObject::restore(pStorage);

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