
// spCoreType.h
//
// Defines the core types and interfaces used in the framework.

#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <vector>



// Handy macro
#define variable_name(A) #A

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
    const char *name()
    {
        return _name;
    }
    void setDescription(const char *new_desc)
    {
        _description = new_desc;
    }
    const char *description()
    {
        return _description;
    }

  protected:
    const char *_name;
    const char *_description;
};

typedef enum
{
    spTypeNone,
    spTypeBool,
    spTypeInt8,
    spTypeInt,
    spTypeUInt8,
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
    static spDataType_t type(int8_t *t)
    {
        return spTypeInt8;
    };
    static spDataType_t type(int *t)
    {
        return spTypeInt;
    };
    static spDataType_t type(uint8_t *t)
    {
        return spTypeUInt8;
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
    static spDataType_t type(int &t)
    {
        return type(&t);
    };
    static spDataType_t type(uint8_t &t)
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
    virtual spDataType_t type(void) = 0;

    virtual bool getBool() = 0;
    virtual int8_t getInt8() = 0;
    virtual int getInt() = 0;
    virtual uint8_t getUint8() = 0;
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
    int get_value(int)
    {
        return getInt();
    }
    uint8_t get_value(uint8_t)
    {
        return getUint8();
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
    int8_t getInt8()
    {
        return (int8_t)get();
    }
    int getInt()
    {
        return (int)get();
    }
    uint8_t getUint8()
    {
        return (uint8_t)get();
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
    int getInt()
    {
        return std::stoi(get());
    };
    uint8_t getUint8()
    {
        return (uint8_t)std::stoul(get());
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
//            static spType Type;
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

typedef uint32_t spTypeID;

struct spType
{
    spType(){};
    // copy and assign constructors - delete them to prevent extra copys being
    // made. We only want a singletype objects to be part of the class definiton.
    // Basically: One spType object pre defined type.
    spType(spType const &) = delete;
    void operator=(spType const &) = delete;

public:
    inline spTypeID typeID(void)
    {
        return (spTypeID)this; 
    }

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



// End - spCoreTypes.h

