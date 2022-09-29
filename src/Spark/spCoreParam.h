

// spCoreParam.h
//
// Defines the objects that make up the "parameter" system of the framework

#pragma once

#include <string>
#include <vector>

#include "spCoreTypes.h"

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

template <class Object, float (Object::*_getter)()>
using spParameterOutDouble = _spParameterOut<double, Object, _getter>;

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

using spOperationContainer = spContainer<spOperation *>;
//-----------------------------------------
// Spark Actions

class spAction : public spOperation
{
};

using spActionContainer = spContainer<spAction *>;


// End - spCoreParam.h