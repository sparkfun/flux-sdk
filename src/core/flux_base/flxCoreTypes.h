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
#include <stdexcept>

#include "flxCoreLog.h"
#include "flxUtils.h"

#include "Arduino.h"
//----------------------------------------------------------------------------------------
// flxDescriptor
//
// Simple class that can be mixed-in to add a common name and description string
// to user "exposed" objects in the framework..

/**
 * @class flxDescriptor
 * @brief Represents a descriptor object with name, description, and title.
 *
 * The `flxDescriptor` class provides methods to set and retrieve the name, description, and title of a descriptor
 * object. It also supports allocating memory for the name, description, and title strings.
 */
class flxDescriptor
{
  public:
    flxDescriptor()
        : _name{nullptr}, _nameAlloc{false}, _desc{nullptr}, _descAlloc{false}, _title(nullptr), _titleAlloc{false}
    {
    }

    /**
     * @brief Set the Name object  - storing the pointer to the provided string No allocation is
     * performed. If the previous name was allocated, it is freed.
     *
     * @param[in] new_name The new name for the object
     */

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

    /**
     * @brief Set the Name object and the Description for the object. Both strings are constants
     *
     * @param[in] new_name The new name for the object - passed to the setName() method
     * @param[in] new_desc The new description for the object - passed to the setDescription() method
     */
    void setName(const char *new_name, const char *new_desc)
    {
        setName(new_name);
        setDescription(new_desc);
    }

    /**
     * @brief Set the Name object, but makes a copy of the input string since the input is not const.
     * If the previous name was allocated, it is freed.
     *
     * @param[in] new_name The new name for the object
     */
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

    /**
     * @brief Return a const char pointer to the name of the object.
     *
     * @return const char*
     */
    const char *name()
    {
        return _name == nullptr ? "" : _name;
    }

    /**
     * @brief Returns a std::string of the name of the object.
     *
     * @return std::string
     */
    std::string name_(void)
    {
        return std::string(_name);
    }

    /**
     * @brief Set the Description object - the input value is constant and not copied. If the previous
     * description was allocated, it is freed.
     *
     * @param[in] new_desc A C string constant for the description
     */
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
    /**
     * @brief Set the Description object - the input value is not constant and is copied.
     * If the previous description was allocated, it is freed.
     *
     * @param[in] new_desc
     */
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

    /**
     * @brief Return the constant C string pointer to the description of the object.
     *
     * @return const char*
     */
    const char *description()
    {
        return _desc == nullptr ? "" : _desc;
    }

    /**
     * @brief Return the description of the object as a std::string.
     *
     * @return std::string
     */
    std::string description_(void)
    {
        return std::string(_desc);
    }

    /**
     * @brief Set the Title object - the title is optional and is used mostly for UX or organizational purposes.
     * The input value is constant and not copied. If the previous title was allocated, it is freed.
     *
     * @param[in] title  - A C string constant for the title
     */
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
    /**
     * @brief Set the Title of the object , but make an explict copy of the provided string
     *
     * @param[in] new_title
     */
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

    /**
     * @brief Return the C constant string pointer to the title of the object.
     *
     * @return const char*
     */
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

/**
 * @brief Enumeration of data types - used throughout the framework.
 *
 * @note: The number used for the datatype codes is based on the sizeof() value and other attributes determined at
compile
// time.
 *
 * This is something picked up from the NVS system of esp32 - solid hack.
 */
enum flxDataType_t : std::uint8_t
{
    flxTypeNone = 0x00,
    flxTypeBool = 0x0A,
    flxTypeInt8 = 0x11,
    flxTypeUInt8 = 0x01,
    flxTypeInt16 = 0x12,
    flxTypeUInt16 = 0x02,
    flxTypeInt32 = 0x14,
    flxTypeUInt32 = 0x04,
    flxTypeFloat = 0x24,
    flxTypeDouble = 0x28,
    flxTypeString = 0x21
};

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version focuses on integral types (non floating point).
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_integral<T>::value, void *>::type
 * @return constexpr flxDataType_t
 */
template <typename T, typename std::enable_if<std::is_integral<T>::value, void *>::type = nullptr>
constexpr flxDataType_t flxGetTypeOf()
{
    return std::is_same<T, bool>::value
               ? flxTypeBool
               : (static_cast<flxDataType_t>(((std::is_signed<T>::value) ? 0x10 : 0x00) | sizeof(T)));
}

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version focuses on floating types.
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_floating_point<T>::value, void *>::type
 * @return constexpr flxDataType_t
 */

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, void *>::type = nullptr>
constexpr flxDataType_t flxGetTypeOf()
{
    return static_cast<flxDataType_t>(0x20 | sizeof(T));
}

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version focuses on string types.
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_same<char *, T>::value, void *>::type
 * @return constexpr flxDataType_t
 */
template <typename T, typename std::enable_if<std::is_same<char *, T>::value, void *>::type = nullptr>
constexpr flxDataType_t flxGetTypeOf()
{
    return flxTypeString;
}

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version focuses on const string types.
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_same<const char *, T>::value, void *>::type
 * @return constexpr flxDataType_t
 */
template <typename T, typename std::enable_if<std::is_same<const char *, T>::value, void *>::type = nullptr>
constexpr flxDataType_t flxGetTypeOf()
{
    return flxTypeString;
}

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version focuses on std::string types.
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_same<std::string, T>::value, void *>::type
 * @return constexpr flxDataType_t
 */
template <typename T, typename std::enable_if<std::is_same<std::string, T>::value, void *>::type = nullptr>
constexpr flxDataType_t flxGetTypeOf()
{
    return flxTypeString;
}

/*******************************************************************************
 * @brief A constexpr function that returns the flxDataType_t value for a given type.
 * This version handles all types, calling a templated version of the function based on the input type.
 *
 * As a constexpr function, this function can be used at compile time to determine the flxDataType_t
 * value for a given type.
 *
 * @tparam T
 * @tparam std::enable_if<std::is_same<const std::string, T>::value, void *>::type
 * @return constexpr flxDataType_t
 */
template <typename T> constexpr flxDataType_t flxGetTypeOf(const T &)
{
    return flxGetTypeOf<T>();
}
/**
 * @brief A union that can hold any of the basic data types used in the framework.
 *
 */
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
/**
 * @brief A class that can hold any of the basic data types used in the framework. It supports the
 * getting and setting of values based on type. Additionally it can check quality (based on type and value) and
 * convert the value to a string.
 *
 * @note This class does not provide any type of type conversion.
 */
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
        type = flxTypeInt32;
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
        type = flxTypeUInt32;
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
    int32_t get(int32_t v)
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
    uint32_t get(uint32_t v)
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
        return (type == flxTypeInt32 && value.i32 == v);
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
        return (type == flxTypeUInt32 && value.ui32 == v);
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
        case flxTypeInt32:
            return flx_utils::to_string(value.i32);
        case flxTypeUInt8:
            return flx_utils::to_string(value.ui8);
        case flxTypeUInt16:
            return flx_utils::to_string(value.ui16);
        case flxTypeUInt32:
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

/**
 * @brief Return the name of the data type as a constant string.
 *
 * @param type The datatype to get the name of.
 * @return const char*
 */
const char *flxGetTypeName(flxDataType_t type);

// Basic interface
/**
 * @brief This class is used as a base class for the templated array classes.
 *
 * @note Currently the interface only supports up to 3 dimensions.
 */
class flxDataArray
{

  public:
    flxDataArray() : _n_dims{0}, _dimensions{0} {};

    virtual ~flxDataArray()
    {
    }

    virtual flxDataType_t type() = 0;

    /**
     * @brief Return the number of dimensions in the array.
     *
     * @return uint8_t
     */
    uint8_t n_dimensions()
    {
        return _n_dims;
    };

    /**
     * @brief Return a pointer to the array's dimensions array .
     *
     * @return uint16_t*
     */
    uint16_t *dimensions()
    {
        return (uint16_t *)&_dimensions;
    }

    /**
     * @brief Return the total number of elements in the array.
     *
     * @return size_t
     */
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

/**
 * @brief A templated class to define an array class of a specific type.
 */
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

    /**
     * @brief Return the data type of the array.
     *
     * @return flxDataType_t
     */
    flxDataType_t type(void)
    {
        return flxGetTypeOf<T>();
    }

    /**
     * @brief Set the array data and pass in dimensions for the array
     *
     * @param data The array data pointer
     * @param d0 The first dimension of the array
     * @param no_copy If set, the data is not copied.
     */
    void set(T *data, uint16_t d0, bool no_copy = false)
    {
        setDimensions(d0);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };

    /**
     * @brief Set the array data and pass in dimensions for the array
     *
     * @param data The array data pointer
     * @param d0 The first dimension of the array
     * @param d1 The second dimension of the array
     * @param no_copy If set, the data is not copied.
     */
    void set(T *data, uint16_t d0, uint16_t d1, bool no_copy = false)
    {
        setDimensions(d0, d1);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };

    /**
     * @brief Set the array data and pass in dimensions for the array
     *
     * @param data The array data pointer
     * @param d0 The first dimension of the array
     * @param d1 The second dimension of the array
     * @param d2 The third dimension of the array
     * @param no_copy If set, the data is not copied.
     */
    void set(T *data, uint16_t d0, uint16_t d1, uint16_t d2, bool no_copy = false)
    {
        setDimensions(d0, d1, d2);
        if (!setDataPtr(data, size(), no_copy))
        {
            reset();
        }
    };

    /**
     * @brief Return a pointer to the array data.
     *
     * @return T* Templated type pointer
     */
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

        flxDataArray::reset(); // call super class
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

/**
 * @brief A boolean array class.
 */
using flxDataArrayBool = flxDataArrayType<bool>;
/**
 * @brief An int8_t array class.
 */
using flxDataArrayInt8 = flxDataArrayType<int8_t>;
/**
 * @brief An int16_t array class.
 */
using flxDataArrayInt16 = flxDataArrayType<int16_t>;
/**
 * @brief An int32_t array class.
 */
using flxDataArrayInt32 = flxDataArrayType<int32_t>;
/**
 * @brief A uint8_t array class.
 */
using flxDataArrayUInt8 = flxDataArrayType<uint8_t>;
/**
 * @brief A uint16_t array class.
 */
using flxDataArrayUInt16 = flxDataArrayType<uint16_t>;
/**
 * @brief A uint32_t array class.
 */
using flxDataArrayUInt32 = flxDataArrayType<uint32_t>;
/**
 * @brief A float array class.
 */
using flxDataArrayFloat = flxDataArrayType<float>;
/**
 * @brief A double array class.
 */
using flxDataArrayDouble = flxDataArrayType<double>;

// strings are special ..
/**
 * @brief A string array class.
 */
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
/**
 * @brief Defines a basic persistance interface for the framework.
 *
 */
struct flxPersist
{

    virtual bool save(flxStorage *) = 0;
    virtual bool restore(flxStorage *) = 0;
};

/**
 * @brief Defines the interface for getting data from an object.
 *
 */
class flxDataOut
{

  public:
    /**
     * @brief Return the type of the data.
     *
     * @return flxDataType_t
     */
    virtual flxDataType_t type(void) = 0;

    /**
     * @brief Get the data as a boolean.
     *
     * @return bool
     */
    virtual bool getBool() = 0;
    /**
     * @brief Get the data as an int8_t.
     *
     * @return int8_t
     */
    virtual int8_t getInt8() = 0;
    /**
     * @brief Get the data as an int16_t.
     *
     * @return int16_t
     */
    virtual int16_t getInt16() = 0;
    /**
     * @brief Get the data as an int32_t.
     *
     * @return int32_t
     */
    virtual int32_t getInt32() = 0;
    /**
     * @brief Get the data as a uint8_t.
     *
     * @return uint8_t
     */
    virtual uint8_t getUInt8() = 0;
    /**
     * @brief Get the data as a uint16_t.
     *
     * @return uint16_t
     */
    virtual uint16_t getUInt16() = 0;
    /**
     * @brief Get the data as a uint32_t.
     *
     * @return uint32_t
     */
    virtual uint32_t getUInt32() = 0;
    /**
     * @brief Get the data as a float.
     *
     * @return float
     */
    virtual float getFloat() = 0;
    /**
     * @brief Get the data as a double.
     *
     * @return double
     */
    virtual double getDouble() = 0;
    /**
     * @brief Get the data as a string.
     *
     * @return std::string
     */
    virtual std::string getString() = 0;

    /**
     * @brief Get the value object as a bool
     *
     * @param bool value
     * @return value as a bool
     */
    bool get_value(bool b)
    {
        return getBool();
    }
    /**
     * @brief Get the value object as an int8_t
     *
     * @param int8_t value
     * @return value as an int8_t
     */
    int8_t get_value(int8_t)
    {
        return getInt8();
    }
    /**
     * @brief Get the value object as an int16_t
     *
     * @param int16_t value
     * @return value as an int16_t
     */
    int16_t get_value(int16_t)
    {
        return getInt16();
    }
    /**
     * @brief Get the value object as an int32_t
     *
     * @param int32_t value
     * @return value as an int32_t
     */
    int32_t get_value(int32_t)
    {
        return getInt32();
    }
    /**
     * @brief Get the value object as a uint8_t
     *
     * @param uint8_t value
     * @return value as a uint8_t
     */
    uint8_t get_value(uint8_t)
    {
        return getUInt8();
    }
    /**
     * @brief Get the value object as a uint16_t
     *
     * @param uint16_t value
     * @return value as a uint16_t
     */
    uint16_t get_value(uint16_t)
    {
        return getUInt16();
    }
    /**
     * @brief Get the value object as a uint32_t
     *
     * @param uint32_t value
     * @return value as a uint32_t
     */
    uint32_t get_value(uint32_t)
    {
        return getUInt32();
    }
    /**
     * @brief Get the value object as a float
     *
     * @param float value
     * @return value as a float
     */
    float get_value(float)
    {
        return getFloat();
    }
    /**
     * @brief Get the value object as a double
     *
     * @param double value
     * @return value as a double
     */
    double get_value(double)
    {
        return getDouble();
    }
    /**
     * @brief Get the value object as a string
     *
     * @param std::string value
     * @return value as a std::string
     */
    std::string get_value(std::string)
    {
        return getString();
    }
};

/**
 * @brief A template DataOut class that returns values of the templated type.
 *
 * @tparam T
 */
template <typename T> class _flxDataOut : public flxDataOut
{

  public:
    /**
     * @brief Returns the data type of the object - based on the templated type.
     *
     * @return flxDataType_t
     */
    flxDataType_t type(void)
    {
        return flxGetTypeOf<T>();
    };

    /**
     * @brief Get the value of the object as a templated type. A pure virtual method.
     *
     * @return T
     */
    virtual T get(void) const = 0;

    /**
     * @brief Get the Bool value of the object
     *
     * @return the value of the object as a bool
     */
    bool getBool()
    {
        return (bool)get();
    }
    /**
     * @brief Get the int8 value of the object
     *
     * @return the value of the object as an int8_t
     */
    int8_t getInt8()
    {
        return (int8_t)get();
    }
    /**
     * @brief Get the int16 value of the object
     *
     * @return the value of the object as an int16_t
     */
    int16_t getInt16()
    {
        return (int16_t)get();
    }
    /**
     * @brief Get the int32 value of the object
     *
     * @return the value of the object as an int32_t
     */
    int32_t getInt32()
    {
        return (int32_t)get();
    }
    /**
     * @brief Get the uint8 value of the object
     *
     * @return the value of the object as a uint8_t
     */
    uint8_t getUInt8()
    {
        return (uint8_t)get();
    }
    /**
     * @brief Get the uint16 value of the object
     *
     * @return the value of the object as a uint16_t
     */
    uint16_t getUInt16()
    {
        return (uint16_t)get();
    }
    /**
     * @brief Get the uint32 value of the object
     *
     * @return the value of the object as a uint32_t
     */
    uint32_t getUInt32()
    {
        return (uint32_t)get();
    }
    /**
     * @brief Get the float value of the object
     *
     * @return the value of the object as a float
     */
    float getFloat()
    {
        return (float)get();
    }
    /**
     * @brief Get the double value of the object
     *
     * @return the value of the object as a double
     */
    double getDouble()
    {
        return (double)get();
    }
    /**
     * @brief Get the string value of the object
     *
     * @return the value of the object as a std::string
     */
    std::string getString()
    {
        T c = get();
        return flx_utils::to_string(c);
    }
    /**
     * @brief A public member that is of the type of this object. Handy for templated code.
     *
     */
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
    int32_t getInt32()
    {
        return std::stoi(get());
    };
    uint8_t getUInt8()
    {
        return (uint8_t)std::stoul(get());
    };
    uint16_t getUInt16()
    {
        return (uint16_t)std::stoul(get());
    };
    uint32_t getUInt32()
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
using flxDataLimitRangeInt32 = flxDataLimitRange<int32_t>;
using flxDataLimitRangeUInt8 = flxDataLimitRange<uint8_t>;
using flxDataLimitRangeUInt16 = flxDataLimitRange<uint16_t>;
using flxDataLimitRangeUInt32 = flxDataLimitRange<uint32_t>;
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
using flxDataLimitSetInt32 = flxDataLimitSetType<int32_t>;
using flxDataLimitSetUInt8 = flxDataLimitSetType<uint8_t>;
using flxDataLimitSetUInt16 = flxDataLimitSetType<uint16_t>;
using flxDataLimitSetUInt32 = flxDataLimitSetType<uint32_t>;
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
    virtual void setInt32(int32_t) = 0;
    virtual void setUInt8(uint8_t) = 0;
    virtual void setUInt16(uint16_t) = 0;
    virtual void setUInt32(uint32_t) = 0;
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
    void set_value(int32_t v)
    {
        setInt32(v);
    }
    void set_value(uint8_t v)
    {
        setUInt8(v);
    }
    void set_value(uint16_t v)
    {
        setUInt16(v);
    }
    void set_value(uint32_t v)
    {
        setUInt32(v);
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
        return flxGetTypeOf<T>();
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
    void setInt32(int32_t value)
    {
        set((T)value);
    }
    void setUInt8(uint8_t value)
    {
        set((T)value);
    }
    void setUInt16(uint16_t value)
    {
        set((T)value);
    }
    void setUInt32(uint32_t value)
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
        case flxTypeInt32:
            set(std::stoi(value));
            break;
        case flxTypeInt8:
            set((int8_t)std::stoi(value));
            break;
        case flxTypeInt16:
            set((int16_t)std::stoi(value));
            break;
        case flxTypeUInt32:
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
    void setInt32(int32_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setUInt8(uint8_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setUInt16(uint16_t value)
    {
        set(flx_utils::to_string(value));
    }
    void setUInt32(uint32_t value)
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
