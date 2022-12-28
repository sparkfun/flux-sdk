

// spCoreParam.h
//
// Defines the objects that make up the "parameter" system of the framework

#pragma once

#include <string>
#include <vector>

#include "spCoreInterface.h"
#include "spCoreProps.h"
#include "flxCoreTypes.h"
#include "spUtils.h"

//----------------------------------------------------------------------------------------
// spParameter
//
// Base/Core Parameter Class
//
// From an abstract sense, a basic parameter   - nothing more

class spParameter : public flxDescriptor
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
    virtual flxDataType_t type(void) = 0;
};

// We want to bin parameters as input and output for storing different
// arguments lists per object type via overloading. So define some simple classes

class spParameterIn : public spParameter
{
  public:
    virtual spEditResult_t editValue(flxDataEditor &) = 0;
    virtual bool setValue(flxDataVariable &) = 0;
    virtual flxDataLimit *dataLimit(void) = 0;
    std::string to_string()
    {
        return std::string(name());
    }; // for consistancy
};

#define kParameterOutFlagArray 0x01

// class spParameterOut : public spParameter, public flxDataOut
class spParameterOut : public spParameter
{
  public:
    spParameterOut() : _flags{0}
    {
    }
    spParameterOut(uint8_t flags) : _flags{flags}
    {
    }

    virtual flxDataType_t type(void) = 0;
    // Some types need precision - just make it generic
    virtual uint16_t precision(void)
    {
        return 0;
    };

    // This is used with covariant returns values of the sub-classes.
    // Returns the property pointer for a given class
    virtual spParameterOut *accessor() = 0;

    // flags -- used to highlight attributes of the output

    uint8_t flags()
    {
        return _flags;
    }

  protected:
    void setFlag(uint8_t flag)
    {
        _flags |= flag;
    }

  private:
    uint8_t _flags;
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
    void addParameter(spParameterIn *newParam, bool head=false)
    {
        // Insert at the head?
        if (head)
            _input_parameters.insert(_input_parameters.begin(), newParam);
        else 
            _input_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(spParameterIn &newParam, bool head=false)
    {
        addParameter(&newParam, head);
    };
    //---------------------------------------------------------------------------------
    void removeParameter(spParameterIn *rmParam)
    {
        auto iter = std::find(_input_parameters.begin(), _input_parameters.end(), rmParam);

        if (iter != _input_parameters.end())
            _input_parameters.erase(iter);
    }

    //---------------------------------------------------------------------------------    
    void removeParameter(spParameterIn &rmParam)
    {
        removeParameter(&rmParam);
    } 

    //---------------------------------------------------------------------------------
    size_t nInputParameters()
    {
        return _input_parameters.size();
    }

    //---------------------------------------------------------------------------------
    void addParameter(spParameterOut *newParam, bool head=false)
    {
        // Insert at the head?
        if (head)
            _output_parameters.insert(_output_parameters.begin(), newParam);
        else
            _output_parameters.push_back(newParam);
    };

    //---------------------------------------------------------------------------------
    void addParameter(spParameterOut &newParam, bool head=false)
    {
        addParameter(&newParam, head);
    };

    //---------------------------------------------------------------------------------
    void removeParameter(spParameterOut *rmParam)
    {
        auto iter = std::find(_output_parameters.begin(), _output_parameters.end(), rmParam);

        if (iter != _output_parameters.end())
            _output_parameters.erase(iter);
    }

    //---------------------------------------------------------------------------------    
    void removeParameter(spParameterOut &rmParam)
    {
        removeParameter(&rmParam);
    }    
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
// spParameterOutScalar

class spParameterOutScalar : public spParameterOut, public flxDataOut
{
  public:
    // mostly a
    spParameterOutScalar *accessor()
    {
        return this;
    }
    virtual flxDataType_t type(void) = 0;
};
//----------------------------------------------------------------------------------------------------
// spParameterOut
//
// Output Parameter Template
//
//

template <class T, class Object, T (Object::*_getter)()>
class _spParameterOut : public _flxDataOut<T>, public spParameterOutScalar
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
    // return our data type
    flxDataType_t type()
    {
        return _flxDataOut<T>::type();
    };

    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_object(this);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also the containing object is needed to call the getter/setter methods on that object
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
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this output parameter ");
            return (T)0;
        }
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
        return _flxDataOut<T>::getBool();
    };
    int8_t getInt8()
    {
        return _flxDataOut<T>::getInt8();
    };
    int16_t getInt16()
    {
        return _flxDataOut<T>::getInt16();
    };
    int getInt()
    {
        return _flxDataOut<T>::getInt();
    };
    uint8_t getUint8()
    {
        return _flxDataOut<T>::getUint8();
    };
    uint16_t getUint16()
    {
        return _flxDataOut<T>::getUint16();
    };
    uint getUint()
    {
        return _flxDataOut<T>::getUint();
    };
    float getFloat()
    {
        return _flxDataOut<T>::getFloat();
    };
    double getDouble()
    {
        return _flxDataOut<T>::getDouble();
    };
    std::string getString()
    {
        return _flxDataOut<T>::getString();
    };
};

// Define by type
template <class Object, bool (Object::*_getter)()> using spParameterOutBool = _spParameterOut<bool, Object, _getter>;

template <class Object, int8_t (Object::*_getter)()>
using spParameterOutInt8 = _spParameterOut<int8_t, Object, _getter>;

template <class Object, int16_t (Object::*_getter)()>
using spParameterOutInt16 = _spParameterOut<int16_t, Object, _getter>;

template <class Object, int (Object::*_getter)()> using spParameterOutInt = _spParameterOut<int, Object, _getter>;

template <class Object, uint8_t (Object::*_getter)()>
using spParameterOutUint8 = _spParameterOut<uint8_t, Object, _getter>;

template <class Object, uint16_t (Object::*_getter)()>
using spParameterOutUint16 = _spParameterOut<uint16_t, Object, _getter>;

template <class Object, uint (Object::*_getter)()> using spParameterOutUint = _spParameterOut<uint, Object, _getter>;

template <class Object, float (Object::*_getter)()>
class spParameterOutFloat : public _spParameterOut<float, Object, _getter>
{
  public:
    spParameterOutFloat() : _precision(3)
    {
    }
    void setPrecision(uint16_t prec)
    {
        _precision = prec;
    }
    uint16_t precision(void)
    {
        return _precision;
    }

  private:
    uint16_t _precision;
};

template <class Object, double (Object::*_getter)()>
class spParameterOutDouble : public _spParameterOut<double, Object, _getter>
{
  public:
    spParameterOutDouble() : _precision(3)
    {
    }
    void setPrecision(uint16_t prec)
    {
        _precision = prec;
    }
    uint16_t precision(void)
    {
        return _precision;
    }

  private:
    uint16_t _precision;
};
//----------------------------------------------------------------------------------------------------
// spParameterOutString
//
// Strings are special
// Output Parameter Template
//
//
template <class Object, std::string (Object::*_getter)()>
class spParameterOutString : public spParameterOutScalar, public _flxDataOutString
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterOutString() : my_object(0)
    {
    }

    // type
    flxDataType_t type(void)
    {
        return _flxDataOutString::type();
    };
    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_object(this);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also the containing object is needed to call the getter/setter methods on that object
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
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this output parameter ");
            return std::string("");
        }

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
        return _flxDataOutString::getBool();
    };
    int8_t getInt8()
    {
        return _flxDataOutString::getInt8();
    };
    int16_t getInt16()
    {
        return _flxDataOutString::getInt16();
    };
    int getInt()
    {
        return _flxDataOutString::getInt();
    };
    uint8_t getUint8()
    {
        return _flxDataOutString::getUint8();
    };
    uint16_t getUint16()
    {
        return _flxDataOutString::getUint16();
    };
    uint getUint()
    {
        return _flxDataOutString::getUint();
    };
    float getFloat()
    {
        return _flxDataOutString::getFloat();
    };
    double getDouble()
    {
        return _flxDataOutString::getDouble();
    };
    std::string getString()
    {
        return _flxDataOutString::getString();
    };
};

//----------------------------------------------------------------------------------------------------
// spParameterOutArray

class spParameterOutArray : public spParameterOut
{
  public:
    spParameterOutArray() : spParameterOut{kParameterOutFlagArray}
    {
    }

    spParameterOutArray *accessor()
    {
        return this;
    }

    virtual flxDataArray *get(void) = 0;
};

//---------------------------------------------------------------------------------------

template <class T, class Object, bool (Object::*_getter)(flxDataArrayType<T> *)>
class spParameterOutArrayType : public spParameterOutArray
{

    Object *my_object; // Pointer to the containing object

  public:
    spParameterOutArrayType() : my_object(0)
    {
    }

    //---------------------------------------------------------------------------------
    // return our data type
    flxDataType_t type()
    {
        T c;
        return flxDataTyper::type(c);
    };

    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_object(this);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also the containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spParameterContainer
        static_assert(std::is_base_of<_spParameterContainer, Object>::value,
                      "spParameterOutArray: type parameter of this class must derive from _spParameterContainer");

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
    // get the value of the parameter.
    //
    // NOTE - using smart pointer/shared pointer for the return value. This will automatically
    //        free memory when the pointer goes out of scope.

    flxDataArrayType<T> *get(void)
    {
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this output parameter ");
            return nullptr;
        }
        flxDataArrayType<T> *data = new flxDataArrayType<T>;
        bool bstatus = (my_object->*_getter)(data);

        if (!bstatus)
        {
            delete data;
            return nullptr;
        }

        return data;
    }

    // //---------------------------------------------------------------------------------
    // // get -> parameter()
    // bool operator()(flxDataArrayType<T> & data)
    // {
    //     return get(data);
    // };
};

// Define by type
template <class Object, bool (Object::*_getter)(flxDataArrayType<bool> *)>
using spParameterOutArrayBool = spParameterOutArrayType<bool, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<int8_t> *)>
using spParameterOutArrayInt8 = spParameterOutArrayType<int8_t, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<int16_t> *)>
using spParameterOutArrayInt16 = spParameterOutArrayType<int16_t, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<int> *)>
using spParameterOutArrayInt = spParameterOutArrayType<int, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<uint8_t> *)>
using spParameterOutArrayUint8 = spParameterOutArrayType<uint8_t, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<uint16_t> *)>
using spParameterOutArrayUint16 = spParameterOutArrayType<uint16_t, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<uint> *)>
using spParameterOutArrayUint = spParameterOutArrayType<uint, Object, _getter>;

template <class Object, bool (Object::*_getter)(flxDataArrayType<float> *)>
class spParameterOutArrayFloat : public spParameterOutArrayType<float, Object, _getter>
{
  public:
    spParameterOutArrayFloat() : _precision(3)
    {
    }
    void setPrecision(uint16_t prec)
    {
        _precision = prec;
    }
    uint16_t precision(void)
    {
        return _precision;
    }

  private:
    uint16_t _precision;
};

template <class Object, bool (Object::*_getter)(flxDataArrayType<double> *)>
class spParameterOutArrayDouble : public spParameterOutArrayType<double, Object, _getter>
{
  public:
    spParameterOutArrayDouble() : _precision(3)
    {
    }
    void setPrecision(uint16_t prec)
    {
        _precision = prec;
    }
    uint16_t precision(void)
    {
        return _precision;
    }

  private:
    uint16_t _precision;
};

template <class Object, bool (Object::*_getter)(flxDataArrayString *)>
class spParameterOutArrayString : public spParameterOutArray
{

    Object *my_object; // Pointer to the containing object

  public:
    spParameterOutArrayString() : my_object(0)
    {
    }

    //---------------------------------------------------------------------------------
    // return our data type
    flxDataType_t type()
    {
        return flxTypeString;
    };

    //---------------------------------------------------------------------------------
    // to register the parameter - set the containing object instance
    // Normally done in the containing objects constructor.
    // i.e.
    //     parameter_object(this);
    //
    // This allows the parameter to add itself to the containing objects list of
    // parameter.
    //
    // Also the containing object is needed to call the getter/setter methods on that object
    void operator()(Object *obj)
    {
        // my_object must be derived from _spParameterContainer
        static_assert(std::is_base_of<_spParameterContainer, Object>::value,
                      "spParameterOutArray: type parameter of this class must derive from _spParameterContainer");

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
    // get the value of the parameter.
    //
    // NOTE - using smart pointer/shared pointer for the return value. This will automatically
    //        free memory when the pointer goes out of scope.

    flxDataArrayString *get(void)
    {
        if (!my_object) // would normally throw an exception, but not very Arduino like!
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this output parameter ");
            return nullptr;
        }
        flxDataArrayString *data = new flxDataArrayString;
        bool bstatus = (my_object->*_getter)(data);

        if (!bstatus)
        {
            delete data;
            return nullptr;
        }

        return data;
    }
};

//-----------------------------------------------------------------------------------

template <class T, class Object, void (Object::*_setter)(T const &)>
class _spParameterIn : public spParameterIn, public _flxDataIn<T>
{
    Object *my_object; // Pointer to the containing object

  public:
    _spParameterIn() : my_object(0)
    {
    }

    // Limit data range
    _spParameterIn(T min, T max)
    {
        _flxDataIn<T>::setDataLimitRange(min, max);
    }
    // Limit data set
    _spParameterIn(std::initializer_list<std::pair<const std::string, T>> limitSet)
    {
        _flxDataIn<T>::addDataLimitValidValue(limitSet);
    }

    //---------------------------------------------------------------------------------
    flxDataType_t type()
    {
        return _flxDataIn<T>::type();
    };
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

        if (!my_object)
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this input parameter ");
            return;
        }

        (my_object->*_setter)(value);
    }

    //---------------------------------------------------------------------------------
    // set -> parameter(value)
    void operator()(T const &value)
    {
        set(value);
    };
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    spEditResult_t editValue(flxDataEditor &theEditor)
    {

        T value = 0;

        bool bSuccess = theEditor.editField(value);

        if (bSuccess) // success
        {
            // do we have a dataLimit set, and if so are we in limits?
            if (!_flxDataIn<T>::isValueValid(value))
                return spEditOutOfRange;

            set(value);
        }

        return bSuccess ? spEditSuccess : spEditFailure;
    }
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
};

// Define by type

// bool
template <class Object, void (Object::*_setter)(bool const &)>
using spParameterInBool = _spParameterIn<bool, Object, _setter>;

template <class Object, void (Object::*_setter)(int8_t const &)>
using spParameterInInt8 = _spParameterIn<int8_t, Object, _setter>;

template <class Object, void (Object::*_setter)(int16_t const &)>
using spParameterInInt16 = _spParameterIn<int16_t, Object, _setter>;

template <class Object, void (Object::*_setter)(int const &)>
using spParameterInInt = _spParameterIn<int, Object, _setter>;

template <class Object, void (Object::*_setter)(uint8_t const &)>
using spParameterInUint8 = _spParameterIn<uint8_t, Object, _setter>;

template <class Object, void (Object::*_setter)(uint16_t const &)>
using spParameterInUint16 = _spParameterIn<uint16_t, Object, _setter>;

template <class Object, void (Object::*_setter)(uint const &)>
using spParameterInUint = _spParameterIn<uint, Object, _setter>;

template <class Object, void (Object::*_setter)(float const &)>
using spParameterInFloat = _spParameterIn<float, Object, _setter>;

template <class Object, void (Object::*_setter)(double const &)>
using spParameterInDouble = _spParameterIn<double, Object, _setter>;

// strings are special.
template <class Object, void (Object::*_setter)(std::string const &)>
class spParameterInString : public spParameterIn, _flxDataInString
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterInString() : my_object(0)
    {
    }

    spParameterInString(Object *me) : my_object(me)
    {
    }
    //---------------------------------------------------------------------------------
    flxDataType_t type()
    {
        return _flxDataInString::type();
    };

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
    void set(std::string const &value)
    {

        if (!my_object)
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this input parameter ");
            return;
        }

        (my_object->*_setter)(value);
    }

    //---------------------------------------------------------------------------------
    // set -> parameter(value)
    void operator()(std::string const &value)
    {
        set(value);
    };
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // Send the property value to the passed in editor for -- well -- editing
    spEditResult_t editValue(flxDataEditor &theEditor)
    {

        std::string value = "";

        bool bSuccess = theEditor.editField(value);

        if (bSuccess) // success
            set(value);

        return bSuccess ? spEditSuccess : spEditFailure;
    }
    flxDataLimit *dataLimit(void)
    {
        return nullptr;
    }

    bool setValue(flxDataVariable &value)
    {

        if (value.type == type())
        {
            std::string c;
            set(value.get(c));
            return true;
        }
        return false;
    };
};

// Need a wedge class to make it easy to cast to a void outside of the template

class spParameterInVoidType : public spParameterIn
{
  public:
    virtual void set(void) = 0;
};

// VOID input parameter -- function call, no params
template <class Object, void (Object::*_setter)()> class spParameterInVoid : public spParameterInVoidType
{
    Object *my_object; // Pointer to the containing object

  public:
    spParameterInVoid() : my_object(0)
    {
    }

    spParameterInVoid(Object *me) : my_object(me)
    {
    }
    //---------------------------------------------------------------------------------
    flxDataType_t type()
    {
        return flxTypeNone;
    };
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
    void set()
    {
        if (!my_object)
        {
            spLog_E("Containing object not set. Verify spRegister() was called on this input parameter ");
            return;
        }

        (my_object->*_setter)();
    }

    //---------------------------------------------------------------------------------
    // set -> parameter(value)
    void operator()()
    {
        set();
    };
    //---------------------------------------------------------------------------------
    // editValue()
    //
    // there is nothing to edit - this method just supports the interface
    spEditResult_t editValue(flxDataEditor &theEditor)
    {
        return spEditSuccess;
    };
    flxDataLimit *dataLimit(void)
    {
        return nullptr;
    };
    bool setValue(flxDataVariable &value)
    {
        return true;
    };
};
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

// Define a object type that supports parameter lists (input and output)
class spOperation : public spObject, public _spParameterContainer
{
  public:
    virtual flxTypeID getType(void)
    {
        return (flxTypeID) nullptr;
    }

    virtual bool loop(void)
    {
        return false;
    }
};

using spOperationContainer = spContainer<spOperation *>;
//-----------------------------------------
// Spark Actions
//
// spAction - just to enable grouping of actions
class spAction : public spOperation
{
public:
    virtual bool initialize(void)
    {
        return true;
    }

};
// Container for actions
using spActionContainer = spContainer<spAction *>;

// For subclasses
template <typename T> class spActionType : public spAction
{
  public:
    // ---------------------------------------------------------------
    // Typing system for actions
    //
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

    // ---------------------------------------------------------------
    // Return the type ID of this
    flxTypeID getType(void)
    {
        return type();
    }
};

// KDB - Temporary ...

template <typename T>
class spSystemType : public spActionType<T> {

public:
    // setup and lifecycle of the file system interface
    virtual bool initialize()=0;

    // Power interface
    virtual void setPower(bool powerOn)=0;
    void powerOn(void)
    {
        setPower(true);
    }
    void powerOff()
    {
        setPower(false);
    }
    virtual bool power(void) = 0;
};
// End - spCoreParam.h
