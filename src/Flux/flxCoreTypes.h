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
// flxCoreType.h
//
// Defines the core types and interfaces used in the framework.

#pragma once

#include <functional>
#include <map>
#include <string.h>
#include <string>
#include <type_traits>
#include <vector>

#include "flxCoreLog.h"
#include "flxUtils.h"

#include "Arduino.h"
//----------------------------------------------------------------------------------------
// flxDescriptor
//
// Simple class that can be mixed-in to add a common name and description string
// to user "exposed" objects in the framework..

class flxDescriptor
{
  public:
    flxDescriptor()
        : _name{nullptr}, _nameAlloc{false}, _desc{nullptr}, _descAlloc{false}, _title(nullptr), _titleAlloc{false}
    {
    }

    //-----------------------------------------------------------
    void setName(const char *new_name)
    {

        if (_nameAlloc)
        {
            if (_name != nullptr)
                delete _name;

            _nameAlloc = false;
        }
        _name = new_name;
    }

    //-----------------------------------------------------------
    void setName(const char *new_name, const char *new_desc)
    {
        setName(new_name);
        setDescription(new_desc);
    }

    //-----------------------------------------------------------
    // Set name, but make a copy of the input name
    void setName(char *new_name)
    {
        // clear out anything we currently have
        setName("");

        if (!new_name)
            return;

        int len = strlen(new_name) + 1;

        char *pTmp = new char[len];
        if (!pTmp)
        {
            flxLogM_E(kMsgErrAllocError, "name");
            return;
        }
        strncpy(pTmp, new_name, len);

        _name = (const char *)pTmp;
        _nameAlloc = true;
    }

    //-----------------------------------------------------------
    const char *name()
    {
        return _name == nullptr ? "" : _name;
    }

    //-----------------------------------------------------------
    std::string name_(void)
    {
        return std::string(_name);
    }

    //-----------------------------------------------------------
    void setDescription(const char *new_desc)
    {
        if (_descAlloc)
        {
            if (_desc != nullptr)
                delete _desc;

            _descAlloc = false;
        }
        _desc = new_desc;
    }
    //-----------------------------------------------------------
    // non const version - needs to allocate space for the value.
    // Note - non-const => allocate
    //
    void setDescription(char *new_desc)
    {
        // clear out anything we currently have
        setDescription("");

        if (!new_desc)
            return;

        int len = strlen(new_desc) + 1;

        char *pTmp = new char[len];
        if (!pTmp)
        {
            flxLogM_E(kMsgErrAllocError, "description");
            return;
        }
        strncpy(pTmp, new_desc, len);

        _desc = (const char *)pTmp;
        _descAlloc = true;
    }

    //-----------------------------------------------------------
    const char *description()
    {
        return _desc == nullptr ? "" : _desc;
    }

    //-----------------------------------------------------------
    std::string description_(void)
    {
        return std::string(_desc);
    }

    //-----------------------------------------------------------
    // Title - mostly used for UX/Org structure
    void setTitle(const char *title)
    {
        if (_titleAlloc)
        {
            if (_title != nullptr)
                delete _title;

            _titleAlloc = false;
        }
        _title = title;
    }
    //-----------------------------------------------------------
    // Set description, but make a copy of the input title
    void setTitleAlloc(char *new_title)
    {
        // clear out anything we currently have
        setTitle("");

        if (!new_title)
            return;

        int len = strlen(new_title) + 1;

        char *pTmp = new char[len];
        if (!pTmp)
        {
            flxLogM_E(kMsgErrAllocError, "title");
            return;
        }
        strncpy(pTmp, new_title, len);

        _title = (const char *)pTmp;
        _titleAlloc = true;
    }
    //-----------------------------------------------------------
    const char *title(void)
    {
        return _title;
    }

  protected:
    const char *_name;
    bool _nameAlloc;

    const char *_desc;
    bool _descAlloc;

    const char *_title;
    bool _titleAlloc;
};

typedef enum
{
    flxTypeNone = 0,
    flxTypeBool,
    flxTypeInt8,
    flxTypeInt16,
    flxTypeInt,
    flxTypeUInt8,
    flxTypeUInt16,
    flxTypeUInt,
    flxTypeFloat,
    flxTypeDouble,
    flxTypeString
} flxDataType_t;

// helpful data types
typedef union {
    bool b;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    float f;
    double d;
    const char *str;
} flxDataAllType_t;

class flxDataVariable
{
  public:
    flxDataType_t type;
    flxDataAllType_t value;

  private:
    std::string _sValue;

  public:
    flxDataVariable() : type{flxTypeNone}
    {
    }
    void set(bool v)
    {
        type = flxTypeBool;
        value.b = v;
    };
    void set(int8_t v)
    {
        type = flxTypeInt8;
        value.i8 = v;
    };
    void set(int16_t v)
    {
        type = flxTypeInt16;
        value.i16 = v;
    };
    void set(int32_t v)
    {
        type = flxTypeInt;
        value.i32 = v;
    };
    void set(uint8_t v)
    {
        type = flxTypeUInt8;
        value.ui8 = v;
    };
    void set(uint16_t v)
    {
        type = flxTypeUInt16;
        value.ui16 = v;
    };
    void set(uint32_t v)
    {
        type = flxTypeUInt;
        value.ui32 = v;
    };
    void set(float v)
    {
        type = flxTypeFloat;
        value.f = v;
    };
    void set(double v)
    {
        type = flxTypeDouble;
        value.d = v;
    };
    void set(const char *v)
    {
        type = flxTypeString;
        _sValue = v;
        value.str = _sValue.c_str();
    };
    void set(std::string &v)
    {
        set(v.c_str());
    }
    // gets
    bool get(bool v)
    {
        return value.b;
    }
    int8_t get(int8_t v)
    {
        return value.i8;
    }
    int16_t get(int16_t v)
    {
        return value.i16;
    }
    int get(int v)
    {
        return value.i32;
    }
    uint8_t get(uint8_t v)
    {
        return value.ui8;
    }
    uint16_t get(uint16_t v)
    {
        return value.ui16;
    }
    uint get(uint v)
    {
        return value.ui32;
    }
    float get(float v)
    {
        return value.f;
    }
    double get(double v)
    {
        return value.d;
    }
    char *get(char *)
    {
        return (char *)_sValue.c_str();
    }
    std::string get(std::string &)
    {
        return _sValue;
    }

    // is equal?
    bool isEqual(bool v)
    {
        return (type == flxTypeBool && v == value.b);
    }
    bool isEqual(int8_t v)
    {
        return (type == flxTypeInt8 && value.i8 == v);
    }
    bool isEqual(int16_t v)
    {
        return (type == flxTypeInt16 && value.i16 == v);
    }
    bool isEqual(int32_t v)
    {
        return (type == flxTypeInt && value.i32 == v);
    }
    bool isEqual(uint8_t v)
    {
        return (type == flxTypeUInt8 && value.ui8 == v);
    }
    bool isEqual(uint16_t v)
    {
        return (type == flxTypeUInt16 && value.ui16 == v);
    }
    bool isEqual(uint32_t v)
    {
        return (type == flxTypeUInt && value.ui32 == v);
    }
    bool isEqual(float v)
    {
        return (type == flxTypeFloat && value.f == v);
    }
    bool isEqual(double v)
    {
        return (type == flxTypeDouble && value.d == v);
    }
    bool isEqual(const char *v)
    {
        return (type == flxTypeString && strcmp(_sValue.c_str(), v) == 0);
    }

    bool isEqual(std::string &v)
    {
        return (type == flxTypeString && _sValue == v);
    }

    std::string to_string(void)
    {
        switch (type)
        {
        case flxTypeBool:
            return flx_utils::to_string(value.b);
        case flxTypeInt8:
            return flx_utils::to_string(value.i8);
        case flxTypeInt16:
            return flx_utils::to_string(value.i16);
        case flxTypeInt:
            return flx_utils::to_string(value.i32);
        case flxTypeUInt8:
            return flx_utils::to_string(value.ui8);
        case flxTypeUInt16:
            return flx_utils::to_string(value.ui16);
        case flxTypeUInt:
            return flx_utils::to_string(value.ui32);
        case flxTypeFloat:
            return flx_utils::to_string(value.f);
        case flxTypeDouble:
            return flx_utils::to_string(value.d);
        case flxTypeString:
            return _sValue;
        default:
            break;
        }
        return std::string("");
    };
};

class flxDataTyper
{
  public:
    // some method overloading to determine types
    static flxDataType_t type(std::nullptr_t *t)
    {
        return flxTypeNone;
    };
    static flxDataType_t type(bool *t)
    {
        return flxTypeBool;
    };
    static flxDataType_t type(int8_t *t)
    {
        return flxTypeInt8;
    };
    static flxDataType_t type(int16_t *t)
    {
        return flxTypeInt16;
    };
    static flxDataType_t type(int *t)
    {
        return flxTypeInt;
    };
    static flxDataType_t type(uint8_t *t)
    {
        return flxTypeUInt8;
    };
    static flxDataType_t type(uint16_t *t)
    {
        return flxTypeUInt16;
    };
    static flxDataType_t type(uint *t)
    {
        return flxTypeUInt;
    };
    static flxDataType_t type(float *t)
    {
        return flxTypeFloat;
    };
    static flxDataType_t type(double *t)
    {
        return flxTypeDouble;
    };
    static flxDataType_t type(std::string *t)
    {
        return flxTypeString;
    };
    static flxDataType_t type(char *t)
    {
        return flxTypeString;
    };

    // non pointer
    static flxDataType_t type(bool &t)
    {
        return type(&t);
    };
    static flxDataType_t type(int8_t &t)
    {
        return type(&t);
    };
    static flxDataType_t type(int16_t &t)
    {
        return type(&t);
    };
    static flxDataType_t type(int &t)
    {
        return type(&t);
    };
    static flxDataType_t type(uint8_t &t)
    {
        return type(&t);
    };
    static flxDataType_t type(uint16_t &t)
    {
        return type(&t);
    };
    static flxDataType_t type(uint &t)
    {
        return type(&t);
    };
    static flxDataType_t type(float &t)
    {
        return type(&t);
    };
    static flxDataType_t type(double &t)
    {
        return type(&t);
    };
    static flxDataType_t type(std::string &t)
    {
        return type(&t);
    };
};
const char *flxGetTypeName(flxDataType_t type);

//----------------------------------------------------------------------------------------
// Array variable/data type.

// Basic interface
class flxDataArray
{

  public:
    flxDataArray() : _n_dims{0}, _dimensions{0} {};

    virtual ~flxDataArray()
    {
    }

    virtual flxDataType_t type() = 0;

    //--------------------------------------------------------------------
    // number of dimensions

    uint8_t n_dimensions()
    {
        return _n_dims;
    };
    //--------------------------------------------------------------------
    // dimensions()
    //
    // Returns a pointer to the array's dimension array.
    uint16_t *dimensions()
    {
        return (uint16_t *)&_dimensions;
    }
    //--------------------------------------------------------------------
    // size()
    //
    // Total number of elements in the array
    size_t size(void)
    {
        uint sum = 0;
        for (int i = 0; i < _n_dims; i++)
            sum += _dimensions[i];
        return sum;
    }

  protected:
    static constexpr uint16_t kMaxArrayDims = 3;

    void setDimensions(uint16_t d0)
    {
        _n_dims = 1;
        _dimensions[0] = d0;
    }

    void setDimensions(uint16_t d0, uint16_t d1)
    {
        _n_dims = 2;
        _dimensions[0] = d0;
        _dimensions[1] = d1;
    }
    void setDimensions(uint16_t d0, uint16_t d1, uint16_t d2)
    {
        _n_dims = 3;
        _dimensions[0] = d0;
        _dimensions[1] = d1;
        _dimensions[2] = d2;
    }

    virtual void reset()
    {
        _n_dims = 0;
        memset(_dimensions, 0, sizeof(uint16_t) * kMaxArrayDims);
    };

    uint8_t _n_dims;
    uint16_t _dimensions[kMaxArrayDims];
};
// ----------------------------------------------------------------------
// Type templated array class...
//
template <typename T> class flxDataArrayType : public flxDataArray
{

  public:
    flxDataArrayType() : _bAlloc{false}, _data{nullptr}
    {
    }

    ~flxDataArrayType()
    {
        reset();
    }

    //--------------------------------------------------------------------
    // return the type of this array

    flxDataType_t type(void)
    {
        T c = {0};
        return flxDataTyper::type(c); // use the typer object - leverage overloading
    }

    //--------------------------------------------------------------------
    // set the array data and pass in dimensions - several variations of this method
    //
    // note - by default the no_copy flag is false, so a copy of the set data is made.

    void set(T *data, uint16_t d0, bool no_copy = false)
    {
        setDimensions(d0);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };

    //--------------------------------------------------------------------

    void set(T *data, uint16_t d0, uint16_t d1, bool no_copy = false)
    {
        setDimensions(d0, d1);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };
    //--------------------------------------------------------------------

    void set(T *data, uint16_t d0, uint16_t d1, uint16_t d2, bool no_copy = false)
    {
        setDimensions(d0, d1, d2);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };

    //--------------------------------------------------------------------
    // Return a pointer to the array data

    T *get()
    {
        return _data;
    };

  protected:
    //--------------------------------------------------------------------
    // Reset the array object.

    virtual void reset()
    {
        // free any alloc first - this might require array dims ...
        freeAlloc();

        flxDataArray::reset(); // call superclass
    }

    virtual void freeAlloc(void)
    {
        // free any existing alloc'd memory
        if (_data != nullptr && _bAlloc)
        {
            delete _data;
            _data = nullptr;
            _bAlloc = false;
        }
    }

    bool _bAlloc;

    //--------------------------------------------------------------------
    virtual bool setDataPtr(T *data, size_t length, bool no_copy)
    {
        // valid?
        if (!data || length == 0)
            return false;

        // free any alloc'd memory
        freeAlloc();

        // copy data or not?
        if (no_copy)
            _data = data;
        else
        {
            // create a copy of the passed in data.
            _data = new T[length];
            memcpy(_data, data, length * sizeof(T));
            _bAlloc = true;
        }

        return true;
    };

  private:
    T *_data;
};

using flxDataArrayBool = flxDataArrayType<bool>;
using flxDataArrayInt8 = flxDataArrayType<int8_t>;
using flxDataArrayInt16 = flxDataArrayType<int16_t>;
using flxDataArrayInt = flxDataArrayType<int>;
using flxDataArrayUint8 = flxDataArrayType<uint8_t>;
using flxDataArrayUint16 = flxDataArrayType<uint16_t>;
using flxDataArrayUint = flxDataArrayType<uint>;
using flxDataArrayFloat = flxDataArrayType<float>;
using flxDataArrayDouble = flxDataArrayType<double>;

// strings are special ..
class flxDataArrayString : public flxDataArrayType<char *>
{
    //--------------------------------------------------------------------
    void freeAlloc(void)
    {

        char **pData = get();
        // free any existing alloc'd memory
        if (pData != nullptr && _bAlloc)
        {
            // need to delete the strings

            uint16_t nStr = size();

            for (int i = 0; i < nStr; i++)
                delete *pData++;
        }

        // call super
        flxDataArrayType<char *>::freeAlloc();
    }
    //--------------------------------------------------------------------
    bool setDataPtr(char **data, size_t length, bool no_copy)
    {
        if (!flxDataArrayType<char *>::setDataPtr(data, length, no_copy))
            return false;

        if (no_copy) // done
            return true;

        // at this point the array storage area is a new allocated char * array,
        // with the original pointers values in place. The strings need to be copied.

        char **pData = get();
        if (!pData) // this is an issue
            return false;

        uint16_t nStr = size();
        size_t slen;
        for (int i = 0; i < nStr; i++, pData++, data++)
        {
            if (!*data) // no string?
                continue;

            // alloc and copy over string data...
            slen = strlen(*data) + 1;
            *pData = new char[slen];
            strlcpy(*pData, *data, slen);
        }
        return true;
    };
};

//----------------------------------------------------------------------------------------
class flxStorage;
//----------------------------------------------------------------------------------------
struct flxPersist
{

    virtual bool save(flxStorage *) = 0;
    virtual bool restore(flxStorage *) = 0;
};
//----------------------------------------------------------------------------------------
// flxDataOut
//
// Interface to get outputs from an object.
class flxDataOut
{

  public:
    virtual flxDataType_t type(void) = 0;

    virtual bool getBool() = 0;
    virtual int8_t getInt8() = 0;
    virtual int16_t getInt16() = 0;
    virtual int getInt() = 0;
    virtual uint8_t getUint8() = 0;
    virtual uint16_t getUint16() = 0;
    virtual uint getUint() = 0;
    virtual float getFloat() = 0;
    virtual double getDouble() = 0;
    virtual std::string getString() = 0;

    bool get_value(bool)
    {
        return getBool();
    }
    int8_t get_value(int8_t)
    {
        return getInt8();
    }
    int16_t get_value(int16_t)
    {
        return getInt16();
    }
    int get_value(int)
    {
        return getInt();
    }
    uint8_t get_value(uint8_t)
    {
        return getUint8();
    }
    uint16_t get_value(uint16_t)
    {
        return getUint16();
    }
    uint get_value(uint)
    {
        return getUint();
    }
    float get_value(float)
    {
        return getFloat();
    }
    double get_value(double)
    {
        return getDouble();
    }
    std::string get_value(std::string)
    {
        return getString();
    }
};

template <typename T> class _flxDataOut : public flxDataOut
{

  public:
    // Type of property
    flxDataType_t type(void)
    {
        T c;
        return flxDataTyper::type(c);
    };

    virtual T get(void) const = 0;

    bool getBool()
    {
        return (bool)get();
    }
    int8_t getInt8()
    {
        return (int8_t)get();
    }
    int16_t getInt16()
    {
        return (int16_t)get();
    }
    int getInt()
    {
        return (int)get();
    }
    uint8_t getUint8()
    {
        return (uint8_t)get();
    }
    uint16_t getUint16()
    {
        return (uint16_t)get();
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
        return flx_utils::to_string(c);
    }

    typedef T value_type; // might be handy in future
};

class _flxDataOutString : public flxDataOut
{

  public:
    // Type of property
    flxDataType_t type(void)
    {
        return flxTypeString;
    };

    virtual std::string get(void) const = 0;

    bool getBool()
    {
        return get() == "true";
    }
    int8_t getInt8()
    {
        return (int8_t)std::stoi(get());
    };
    int16_t getInt16()
    {
        return (int16_t)std::stoi(get());
    };
    int getInt()
    {
        return std::stoi(get());
    };
    uint8_t getUint8()
    {
        return (uint8_t)std::stoul(get());
    };
    uint16_t getUint16()
    {
        return (uint16_t)std::stoul(get());
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

//---------------------------------------------------------
// Data input limits

typedef enum
{
    flxDataLimitTypeNone = 0,
    flxDataLimitTypeRange,
    flxDataLimitTypeSet
} flxDataLimit_t;

class flxDataLimitDesc
{
  public:
    std::string name;
    flxDataVariable data;
};
using flxDataLimitList = std::vector<flxDataLimitDesc>;

//-----------------------------------------------------------------
class flxDataLimit
{
  public:
    virtual ~flxDataLimit()
    {
    }
    virtual flxDataLimit_t type(void)
    {
        return flxDataLimitTypeNone;
    }
    flxDataLimitList _dataLimits;

    flxDataLimitList &limits(void)
    {
        return _dataLimits;
    }
    void addLimit(flxDataLimitDesc &item)
    {
        _dataLimits.push_back(item);
    }
    void clearLimits(void)
    {
        _dataLimits.clear();
    };

    // method to get the name of a limit based on value -
    // sub-classes specialize this
    virtual std::string getName(flxDataVariable &var)
    {
        return std::string("");
    }

    typedef enum
    {
        dataLimitNone = 0,
        dataLimitRange,
        dataLimitSet
    } dataLimitType_t;
};

template <typename T> class flxDataLimitType : public flxDataLimit
{
  public:
    virtual bool isValid(T value) = 0;
};

template <typename T> class flxDataLimitRange : public flxDataLimitType<T>
{
  public:
    flxDataLimitRange() : _isSet{false}
    {
    }
    flxDataLimitRange(T min, T max)
    {
        setRange(min, max);
    }

    void setRange(T min, T max)
    {
        if (min < max)
        {
            _min = min;
            _max = max;
        }
        else
        {
            _min = max;
            _max = min;
        }

        _isSet = true;

        flxDataLimit::clearLimits();
        // build our limit descriptors
        flxDataLimitDesc limit;
        limit.name = flx_utils::to_string(_min);
        limit.data.set(_min);
        flxDataLimit::addLimit(limit);

        limit.name = flx_utils::to_string(_max);
        limit.data.set(_max);
        flxDataLimit::addLimit(limit);
    };

    bool isValid(T value)
    {
        if (!_isSet)
            return false;

        return (value >= _min && value <= _max);
    }
    flxDataLimit_t type(void)
    {
        return flxDataLimitTypeRange;
    };

  private:
    T _min;
    T _max;
    bool _isSet;
};

using flxDataLimitRangeInt8 = flxDataLimitRange<int8_t>;
using flxDataLimitRangeInt16 = flxDataLimitRange<int16_t>;
using flxDataLimitRangeInt = flxDataLimitRange<int>;
using flxDataLimitRangeUint8 = flxDataLimitRange<uint8_t>;
using flxDataLimitRangeUint16 = flxDataLimitRange<uint16_t>;
using flxDataLimitRangeUnt = flxDataLimitRange<uint>;
using flxDataLimitRangeFloat = flxDataLimitRange<float>;
using flxDataLimitRangeDouble = flxDataLimitRange<double>;

//----------------------------------------------------------------------------
// flxDataLimitSetType
//
// Used to contain a set of valid values. The values are stored as Name, Value pairs,
// where Name is a human readable string for display/UX
//
// This is the base class for this type of limit

template <typename T> class flxDataLimitSetType : public flxDataLimitType<T>
{
  public:
    flxDataLimitSetType()
    {
    }

    flxDataLimitSetType(std::initializer_list<std::pair<const std::string, T>> list)
    {
        if (list.size() < 1)
            throw std::length_error("invalid number of arguments");

        flxDataLimitDesc limit;

        for (auto item : list)
        {
            limit.name = item.first.c_str();
            limit.data.set(item.second);
            flxDataLimit::addLimit(limit);
        }
    }

    bool isValid(T value)
    {
        for (auto item : flxDataLimit::limits())
        {
            if (item.data.isEqual(value))
                return true;
        }
        return false;
    }

    // method to get the name of the limit based on a passed in value
    std::string getName(flxDataVariable &var)
    {
        // get the typed value
        T value;
        value = var.get(value);

        // do we have a match - if so return name value
        for (auto item : flxDataLimit::limits())
        {
            if (item.data.isEqual(value))
                return item.name;
        }
        return std::string("");
    }

    void addItem(std::string name, T value)
    {
        addItem(name.c_str(), value);
    }

    void addItem(const char *name, T value)
    {
        flxDataLimitDesc limit;
        limit.name = name;
        limit.data.set(value);
        flxDataLimit::addLimit(limit);
    }
    flxDataLimit_t type(void)
    {
        return flxDataLimitTypeSet;
    };
};

using flxDataLimitSetInt8 = flxDataLimitSetType<int8_t>;
using flxDataLimitSetInt16 = flxDataLimitSetType<int16_t>;
using flxDataLimitSetInt = flxDataLimitSetType<int>;
using flxDataLimitSetUint8 = flxDataLimitSetType<uint8_t>;
using flxDataLimitSetUint16 = flxDataLimitSetType<uint16_t>;
using flxDataLimitSetUint = flxDataLimitSetType<uint>;
using flxDataLimitSetFloat = flxDataLimitSetType<float>;
using flxDataLimitSetDouble = flxDataLimitSetType<double>;
using flxDataLimitSetString = flxDataLimitSetType<std::string>;
//-----------------------------------------------------------------------------
// Data input interface.
class flxDataIn
{
  public:
    virtual flxDataType_t type(void) = 0;

    virtual void setBool(bool) = 0;
    virtual void setInt8(int8_t) = 0;
    virtual void setInt16(int16_t) = 0;
    virtual void setInt(int) = 0;
    virtual void setUint8(uint8_t) = 0;
    virtual void setUint16(uint16_t) = 0;
    virtual void setUint(uint) = 0;
    virtual void setFloat(float) = 0;
    virtual void setDouble(double) = 0;
    virtual void setString(std::string &) = 0;

    void set_value(bool v)
    {
        setBool(v);
    }
    void set_value(int8_t v)
    {
        setInt8(v);
    }
    void set_value(int16_t v)
    {
        setInt16(v);
    }
    void set_value(int v)
    {
        setInt(v);
    }
    void set_value(uint8_t v)
    {
        setUint8(v);
    }
    void set_value(uint16_t v)
    {
        setUint16(v);
    }
    void set_value(uint v)
    {
        setUint(v);
    }
    void set_value(float v)
    {
        setFloat(v);
    }
    void set_value(double v)
    {
        setDouble(v);
    }
    void set_value(std::string &v)
    {
        setString(v);
    }
};

template <typename T> class _flxDataIn : public flxDataIn
{

  public:
    _flxDataIn() : _dataLimit{nullptr}, _limitIsAlloc{false}, _dataLimitType{flxDataLimit::dataLimitNone} {};

    flxDataType_t type(void)
    {
        T c;
        return flxDataTyper::type(c);
    };
    virtual void set(T const &value) = 0;

    void setBool(bool value)
    {
        set((T)value);
    }
    void setInt8(int8_t value)
    {
        set((T)value);
    }
    void setInt16(int16_t value)
    {
        set((T)value);
    }
    void setInt(int value)
    {
        set((T)value);
    }
    void setUint8(uint8_t value)
    {
        set((T)value);
    }
    void setUint16(uint16_t value)
    {
        set((T)value);
    }
    void setUint(uint value)
    {
        set((T)value);
    }
    void setFloat(float value)
    {
        set((T)value);
    }
    void setDouble(double value)
    {
        set((T)value);
    }
    void setString(std::string &value)
    {
        // Convert string to native type..
        switch (this->type())
        {
        case flxTypeBool:
            set(value.length() > 0 ? (value != "false") : false);
            break;
        case flxTypeInt:
            set(std::stoi(value));
            break;
        case flxTypeInt8:
            set((int8_t)std::stoi(value));
            break;
        case flxTypeInt16:
            set((int16_t)std::stoi(value));
            break;
        case flxTypeUInt:
            set(std::stoul(value));
            break;
        case flxTypeUInt8:
            set((uint8_t)std::stoul(value));
            break;
        case flxTypeUInt16:
            set((uint16_t)std::stoul(value));
            break;
        case flxTypeFloat:
            set(std::stof(value));
            break;
        case flxTypeDouble:
            set(std::stof(value));
            break;
        case flxTypeString:
            break;
        default:
            flxLog_W("Unknown property type set");
            break;
        }
    };
    //---------------------------------------------------------------------------------
    // Data Limit things
    //---------------------------------------------------------------------------------
    void setDataLimit(flxDataLimitType<T> &dataLimit)
    {
        if (_dataLimit && _limitIsAlloc)
            delete _dataLimit;

        _limitIsAlloc = false;
        _dataLimit = &dataLimit;
    }
    void setDataLimit(flxDataLimitType<T> *dataLimit)
    {
        if (_dataLimit && _limitIsAlloc)
            delete _dataLimit;

        _limitIsAlloc = true;
        _dataLimit = dataLimit;
    }
    // -------------------------------------------
    flxDataLimit *dataLimit(void)
    {
        return _dataLimit;
    }

    void setDataLimitRange(T min, T max)
    {
        if (_dataLimitType != flxDataLimit::dataLimitRange)
        {
            if (_dataLimit != nullptr && _limitIsAlloc)
                delete _dataLimit;
            _dataLimit = new flxDataLimitRange<T>();
            _dataLimitType = flxDataLimit::dataLimitRange;
            _limitIsAlloc = true;
        }
        ((flxDataLimitRange<T> *)_dataLimit)->setRange(min, max);
    }

    void setDataLimitRange(std::pair<T, T> range)
    {
        setDataLimitRange(range.first, range.second);
    }

    void addDataLimitValidValue(std::string name, T value)
    {
        if (_dataLimitType != flxDataLimit::dataLimitSet)
        {
            if (_dataLimit != nullptr && _limitIsAlloc)
                delete _dataLimit;
            _dataLimit = new flxDataLimitSetType<T>();
            _dataLimitType = flxDataLimit::dataLimitSet;
            _limitIsAlloc = true;
        }
        ((flxDataLimitSetType<T> *)_dataLimit)->addItem(name, value);
    }
    void addDataLimitValidValue(std::pair<const std::string, T> value)
    {
        addDataLimitValidValue((std::string)value.first, value.second);
    }

    void addDataLimitValidValue(std::initializer_list<std::pair<const std::string, T>> limitSet)
    {
        for (auto item : limitSet)
            addDataLimitValidValue(item);
    }

    void clearDataLimit(void)
    {
        if (_dataLimit)
        {
            delete _dataLimit;
            _dataLimitType = flxDataLimit::dataLimitNone;
        }
    }
    // Check value against the data limits
    bool isValueValid(T value)
    {
        // if we have a limit, check value, else return true.
        return _dataLimit != nullptr ? _dataLimit->isValid(value) : true;
    }

  private:
    flxDataLimitType<T> *_dataLimit;
    bool _limitIsAlloc;
    flxDataLimit::dataLimitType_t _dataLimitType;
};

class _flxDataInString : public flxDataIn
{

  public:
    flxDataType_t type(void)
    {
        return flxTypeString;
    };
    virtual void set(const std::string &value) = 0;

    virtual void set(const char *value)
    {
        std::string stmp = value;
        set(value);
    }

    void setBool(bool value)
    {
        set(flx_utils::to_string(value));
    }
    void setInt8(int8_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setInt16(int16_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setInt(int value)
    {
        set(flx_utils::to_string(value));
    }
    void setUint8(uint8_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setUint16(uint16_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setUint(uint value)
    {
        set(flx_utils::to_string(value));
    }
    void setFloat(float value)
    {
        set(flx_utils::to_string(value));
    }
    void setDouble(double value)
    {
        set(flx_utils::to_string(value));
    }
    void setString(std::string &value)
    {
        set(value);
    };
};

//---------------------------------------------------------
// Define simple type ID "types" - used for class IDs

typedef uint32_t flxTypeID;
#define kflxTypeIDNone 0

template <typename T> flxTypeID flxGetClassTypeID()
{
    // Use the name of this method via the __PRETTY_FUNCTION__ macro
    // to create our ID. The macro gives us a unique name for
    // each class b/c it uses the template parameter.

    // Hash the name, make that our type ID.
    return flx_utils::id_hash_string(__PRETTY_FUNCTION__);
};

// End - flxCoreTypes.h
