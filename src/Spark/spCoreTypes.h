
// spCoreType.h
//
// Defines the core types and interfaces used in the framework.

#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <vector>
#include <map>
#include <string.h>

#include "spCoreLog.h"
#include "spStorage.h"
#include "spUtils.h"
//----------------------------------------------------------------------------------------
// spDescriptor
//
// Simple class that can be mixed-in to add a common name and description string
// to user "exposed" objects in the framework..

class spDescriptor
{
  public:
    spDescriptor() : _name{""}, _description{""}
    {
    }

    void setName(const char *new_name)
    {
        _name = new_name;
    }
    void setName(const char *new_name, const char *new_desc)
    {
        _name = new_name;
        setDescription(new_desc);
    }
    const char *name()
    {
        return _name.c_str();
    }
    void setDescription(const char *new_desc)
    {
        _description = new_desc;
    }
    const char *description()
    {
        return _description.c_str();
    }

  protected:
    std::string _name;
    std::string _description;
};

typedef enum
{
    spTypeNone = 0,
    spTypeBool,
    spTypeInt8,
    spTypeInt16,
    spTypeInt,
    spTypeUInt8,
    spTypeUInt16,
    spTypeUInt,
    spTypeFloat,
    spTypeDouble,
    spTypeString
} spDataType_t;

// helpful data types 
typedef union {
    bool        b;
    int8_t      i8;
    int16_t     i16;
    int32_t     i32;
    uint8_t     ui8;
    uint16_t    ui16;
    uint32_t    ui32;
    float       f;
    double      d;
    const char *      str;
} spDataAllType_t;

class spDataVariable {
public:
    spDataType_t     type;
    spDataAllType_t  value;

    spDataVariable() : type{spTypeNone}{}
    void set(bool v)
    {
        type = spTypeBool;
        value.b = v;
    };
    void set(int8_t v)
    {
        type = spTypeInt8;
        value.i8 = v;
    };
    void set(int16_t v)
    {
        type = spTypeInt16;
        value.i16 = v;
    };
    void set(int32_t v)
    {
        type = spTypeInt;
        value.i32 = v;
    };
    void set(uint8_t v)
    {
        type = spTypeUInt8;
        value.ui8 = v;
    };
    void set(uint16_t v)
    {
        type = spTypeUInt16;
        value.ui16 = v;
    };
    void set(uint32_t v)
    {
        type = spTypeUInt;
        value.ui32 = v;
    };  
    void set(float v)
    {
        type = spTypeFloat;
        value.f = v;
    };
    void set(double v)
    {
        type = spTypeDouble;
        value.d = v;
    };
    void set(const char * v)
    {
        type = spTypeString;
        value.str = v;
    };
    // gets
    bool        get(bool v) {return value.b;}
    int8_t      get(int8_t v) {return value.i8;}
    int16_t     get(int16_t v) {return value.i16;}
    int         get(int  v) {return value.i32;}
    uint8_t     get(uint8_t v) {return value.ui8;}
    uint16_t    get(uint16_t v) {return value.ui16;}
    uint        get(uint  v) {return value.ui32;}    
    float       get(float  v) {return value.f;}    
    double      get(double  v) {return value.d;}        
    char *      get(char *) {return (char*) value.str;}
    char *      get(std::string&) {return (char*) value.str;}
    

    // is equal?
    bool isEqual(bool v)  { return ( type == spTypeBool && v == value.b); }
    bool isEqual(int8_t v)  { return (type == spTypeInt8 && value.i8 == v); }
    bool isEqual(int16_t v)  { return (type == spTypeInt16 && value.i16 == v); }
    bool isEqual(int32_t v)  { return (type == spTypeInt && value.i32 == v); }
    bool isEqual(uint8_t v)  { return (type == spTypeUInt8 && value.ui8 == v); }
    bool isEqual(uint16_t v) { return (type == spTypeUInt16 && value.ui16 == v); }
    bool isEqual(uint32_t v) { return ( type == spTypeUInt && value.ui32 == v); }  
    bool isEqual(float v) { return (type == spTypeFloat && value.f == v); }
    bool isEqual(double v) { return (type == spTypeDouble && value.d == v); }
    bool isEqual(const char * v) { return (type == spTypeString && strcmp(value.str, v) == 0); }

    std::string to_string(void)
    {
        switch(type){
            case spTypeBool:
                return sp_utils::to_string(value.b);
            case spTypeInt8:
                return sp_utils::to_string(value.i8);
            case spTypeInt16:
                return sp_utils::to_string(value.i16);
            case spTypeInt:
                return sp_utils::to_string(value.i32);            
            case spTypeUInt8:
                return sp_utils::to_string(value.ui8);
            case spTypeUInt16:
                return sp_utils::to_string(value.ui16);
            case spTypeUInt:
                return sp_utils::to_string(value.ui32);            
            case spTypeFloat:
                return sp_utils::to_string(value.f);
            case spTypeDouble:
                return sp_utils::to_string(value.d);                                
            case spTypeString:
                return sp_utils::to_string(value.str);                                
            default:
                break;
        }
        return std::string("");
    };
};

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
    static spDataType_t type(int8_t *t)
    {
        return spTypeInt8;
    };
    static spDataType_t type(int16_t *t)
    {
        return spTypeInt16;
    };
    static spDataType_t type(int *t)
    {
        return spTypeInt;
    };
    static spDataType_t type(uint8_t *t)
    {
        return spTypeUInt8;
    };
    static spDataType_t type(uint16_t *t)
    {
        return spTypeUInt16;
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
    static spDataType_t type(int8_t &t)
    {
        return type(&t);
    };
    static spDataType_t type(int16_t &t)
    {
        return type(&t);
    };
    static spDataType_t type(int &t)
    {
        return type(&t);
    };
    static spDataType_t type(uint8_t &t)
    {
        return type(&t);
    };
    static spDataType_t type(uint16_t &t)
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
const char *spGetTypeName(spDataType_t type);

struct spPersist
{

    virtual bool save(spStorage2 *) = 0;
    virtual bool restore(spStorage2 *) = 0;
};
//----------------------------------------------------------------------------------------
// spDataOut
//
// Interface to get outputs from an object.
class spDataOut
{

  public:
    virtual spDataType_t type(void) = 0;

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
        return sp_utils::to_string(c);
    }

    typedef T value_type; // might be handy in future
};

class _spDataOutString : public spDataOut
{

  public:
    // Type of property
    spDataType_t type(void)
    {
        return spTypeString;
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

class spDataIn
{
  public:
    virtual spDataType_t type(void) = 0;

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

template <typename T> class _spDataIn : public spDataIn
{

  public:
    spDataType_t type(void)
    {
        T c;
        return spDataTyper::type(c);
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
        case spTypeBool:
            set(value.length() > 0 ? (value != "false") : false);
            break;
        case spTypeInt:
            set(std::stoi(value));
            break;
        case spTypeInt8:
            set((int8_t)std::stoi(value));
            break;
        case spTypeInt16:
            set((int16_t)std::stoi(value));
            break;
        case spTypeUInt:
            set(std::stoul(value));
            break;
        case spTypeUInt8:
            set((uint8_t)std::stoul(value));
            break;
        case spTypeUInt16:
            set((uint16_t)std::stoul(value));
            break;
        case spTypeFloat:
            set(std::stof(value));
            break;
        case spTypeDouble:
            set(std::stof(value));
            break;
        case spTypeString:
            break;
        default:
            spLog_W("Unknown property type set");
            break;
        }
    };
};

class _spDataInString : public spDataIn
{

  public:
    spDataType_t type(void)
    {
        return spTypeString;
    };
    virtual void set(const std::string &value) = 0;

    virtual void set(const char *value)
    {
        std::string stmp = value;
        set(value);
    }

    void setBool(bool value)
    {
        set(sp_utils::to_string(value));
    }
    void setInt8(int8_t value)
    {
        set(sp_utils::to_string(value));
    }
    void setInt16(int16_t value)
    {
        set(sp_utils::to_string(value));
    }
    void setInt(int value)
    {
        set(sp_utils::to_string(value));
    }
    void setUint8(uint8_t value)
    {
        set(sp_utils::to_string(value));
    }
    void setUint16(uint16_t value)
    {
        set(sp_utils::to_string(value));
    }
    void setUint(uint value)
    {
        set(sp_utils::to_string(value));
    }
    void setFloat(float value)
    {
        set(sp_utils::to_string(value));
    }
    void setDouble(double value)
    {
        set(sp_utils::to_string(value));   
    }
    void setString(std::string &value)
    {
        set(value);
    };
};


//---------------------------------------------------------
// Testing data limits

typedef enum 
{
    spDataLimitTypeNone = 0,
    spDataLimitTypeRange,
    spDataLimitTypeSet
} spDataLimit_t;

class spDataLimitDesc {
public:
    std::string      name;
    spDataVariable   data;
}; 
using spDataLimitList = std::vector<spDataLimitDesc>;

//-----------------------------------------------------------------
class spDataLimit 
{
public:
    virtual ~spDataLimit(){
        
    }
    virtual spDataLimit_t type(void)
    {
        return spDataLimitTypeNone;
    }
    spDataLimitList  _dataLimits;

     spDataLimitList & limits(void)
    {
        return _dataLimits;
    }
    void addLimit(spDataLimitDesc & item)
    {
        _dataLimits.push_back(item);
    }
    void clearLimits(void ){
        _dataLimits.clear();
    };
};

template <typename T>
class spDataLimitType : public spDataLimit
{
public:
    virtual bool isValid(T value) = 0;
};

template <typename T>
class spDataLimitRange : public spDataLimitType<T> 
{
public:
    spDataLimitRange() : _isSet{false} {}
    spDataLimitRange( T min, T max )
    {
        setRange( min, max);
    }

    void setRange(T min, T max)
    {
        if (min < max )
        {
            _min = min;
            _max = max;
        }
        else
        {
            _min = max;
            _max = min;
        }

        _isSet=true;

        spDataLimit::clearLimits();
        // build our limit descriptors
        spDataLimitDesc  limit;
        limit.name = sp_utils::to_string(_min);
        limit.data.set(_min);
        spDataLimit::addLimit(limit);

        limit.name = sp_utils::to_string(_max);
        limit.data.set(_max);
        spDataLimit::addLimit(limit);

    };


    bool isValid(T value)
    {
        if ( !_isSet )
            return false;

        return ( value >= _min && value <= _max);
    }
    spDataLimit_t type(void)
    {
        return spDataLimitTypeRange;
    };

private:
    T _min;
    T _max;
    bool _isSet;
};

using spDataLimitRangeInt8 = spDataLimitRange<int8_t>;
using spDataLimitRangeInt16 = spDataLimitRange<int16_t>;
using spDataLimitRangeInt = spDataLimitRange<int>;
using spDataLimitRangeUint8 = spDataLimitRange<uint8_t>;
using spDataLimitRangeUint16 = spDataLimitRange<uint16_t>;
using spDataLimitRangeUnt = spDataLimitRange<uint>;
using spDataLimitRangeFloat = spDataLimitRange<float>;
using spDataLimitRangeDouble = spDataLimitRange<double>;


//----------------------------------------------------------------------------
// spDataLimitSetType
//
// Used to contain a set of valid values. The values are stored as Name, Value pairs, 
// where Name is a human readable string for display/UX
//
// This is the base class for this type of limit

template <typename T>
class spDataLimitSetType :  public spDataLimitType<T>
{
public:
    spDataLimitSetType()
    {
    }
    
    spDataLimitSetType( std::initializer_list<std::pair<const std::string, T>> list)
    {
        if (list.size() < 1)
            throw std::length_error("invalid number of arguments");
        
        spDataLimitDesc  limit;

        for (auto item : list)
        {
            limit.name = item.first.c_str();
            limit.data.set(item.second);
            spDataLimit::addLimit(limit);            
        }
    }

    bool isValid(T value)
    {
        for ( auto item : spDataLimit::limits())
        {
            if ( item.data.isEqual(value))
                return true;
        }
        return false;
    }

    void addItem(std::string &name, T value)
    {
        spDataLimitDesc  limit;
        limit.name = name.c_str();
        limit.data.set(value);
        spDataLimit::addLimit(limit);     
    }
    spDataLimit_t type(void)
    {
        return spDataLimitTypeSet;
    };  
};

using spDataLimitSetInt8 = spDataLimitSetType<int8_t>;
using spDataLimitSetInt16 = spDataLimitSetType<int16_t>;
using spDataLimitSetInt = spDataLimitSetType<int>;
using spDataLimitSetUint8 = spDataLimitSetType<uint8_t>;
using spDataLimitSetUint16 = spDataLimitSetType<uint16_t>;
using spDataLimitSetUnt = spDataLimitSetType<uint>;
using spDataLimitSetFloat = spDataLimitSetType<float>;
using spDataLimitSetDouble = spDataLimitSetType<double>;
using spDataLimitSetString = spDataLimitSetType<std::string>;

//---------------------------------------------------------
// Define simple type ID "types" - used for class IDs

typedef uint32_t spTypeID;
#define kspTypeIDNone 0


template <typename T> spTypeID spGetClassTypeID()
{
    // Use the name of this method via the __PRETTY_FUNCTION__ macro
    // to create our ID. The macro gives us a unique name for
    // each class b/c it uses the template parameter.

    // Hash the name, make that our type ID.
    return sp_utils::id_hash_string(__PRETTY_FUNCTION__);
    
};

//----------------------------------------------------------------------
// spDataEditor()
//
// define a data editor interface - called to have some value edited.
//
//
// TODO: Factor to a different header in the future...

class spDataEditor
{

  public:
    virtual bool editField(char *value, size_t lenValue, uint32_t timeout = 60) = 0;
    virtual bool editField(std::string &value, uint32_t timeout = 60) = 0;
    virtual bool editField(bool &value, uint32_t timeout = 60) = 0;
    virtual bool editField(int8_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(int16_t &value, uint32_t timeout = 60) = 0;    
    virtual bool editField(int32_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(uint8_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(uint16_t &value, uint32_t timeout = 60) = 0;    
    virtual bool editField(uint32_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(float &value, uint32_t timeout = 60) = 0;
    virtual bool editField(double &value, uint32_t timeout = 60) = 0;
    virtual void beep() = 0;
};
// End - spCoreTypes.h
