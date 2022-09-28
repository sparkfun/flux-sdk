
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
    virtual spDataType_t type(void) = 0;

    virtual bool getBool() = 0;
    virtual int getInt() = 0;
    virtual uint getUint() = 0;
    virtual float getFloat() = 0;
    virtual double getDouble() = 0;
    virtual std::string getString() = 0;

    bool get_value(bool)
    {
        return getBool();
    }
    int get_value(int)
    {
        return getInt();
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
    spDataType_t type(void)
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

// End - spCoreTypes.h

