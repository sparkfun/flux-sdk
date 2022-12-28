

#pragma once

#include <vector>
// spEvent.h
//
// Define the concept of signals/slots to enable events within the framework
//-----------------------------------------------------------------------
// 5/20 - Impl based on https://schneegans.github.io/tutorials/2015/09/20/signal-slot
// flxSignals - sort of  - support for our simple observer pattern for events
//

template <typename ArgT> class flxSignal
{

  public:
    // connects a member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT var))
    {
        connect([=](ArgT var) { // uses a lambda for the callback
            (inst->*func)(var);
        });
    }

    // connects a const member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT var) const)
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (inst->*func)(var);
        });
    }

    // Just call a user supplied function - no object
    void call(void (*func)(ArgT var))
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (*func)(var);
        });
    }
    // Just call a user supplied function - no object - but with a User Defined value
    template <typename T> void call(void (*func)(T uval, ArgT var), T uValue)
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (*func)(uValue, var);
        });
    }   

    template <typename T, typename U> void call(T *inst, void (T::*func)(U uVal, ArgT var), U uValue)
    {
        connect([=](ArgT var) { // users a lambda for the callback
            (inst->*func)(uValue, var);
        });
    }
    // connects a std::function to the flxSignal.
    void connect(std::function<void(ArgT var)> const &slot) const
    {
        slots_.push_back(slot);
    }

    // calls all connected functions
    void emit(ArgT p)
    {
        for (auto const &it : slots_)
        {
            it(p);
        }
    }

    typedef ArgT value_type;

  private:
    mutable std::vector<std::function<void(ArgT &var)>> slots_;
};

typedef flxSignal<bool> flxSignalBool;
typedef flxSignal<int8_t> flxSignalInt8;
typedef flxSignal<int16_t> flxSignalInt16;
typedef flxSignal<int> flxSignalInt;
typedef flxSignal<uint8_t> flxSignalUInt8;
typedef flxSignal<uint16_t> flxSignalUInt16;
typedef flxSignal<uint> flxSignalUInt;
typedef flxSignal<float> flxSignalFloat;
typedef flxSignal<double> flxSignalDouble;
typedef flxSignal<const char *> flxSignalString;

// Signal - zero arg - aka void function callback type. Unable to template this, so
// just brute force the impl.
class flxSignalVoid
{

  public:
    // connects a member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)())
    {
        connect([=]() { // uses a lambda for the callback
            (inst->*func)();
        });
    }

    // Just call a user supplied function - no object
    void call(void (*func)())
    {
        connect([=]() { // uses a lambda for the callback
            (*func)();
        });
    }
    // Just call a user supplied function - no object - but with a User Defined value
    template <typename T> void call(void (*func)(T uval), T uValue)
    {
        connect([=]() { // users a lambda for the callback
            (*func)(uValue);
        });
    }
    // Just call a user supplied function and object but with a User Defined value
    template <typename T, typename U> void call(T *inst, void (T::*func)(U uval), U uValue)
    {
        connect([=]() { // users a lambda for the callback
            (inst->*func)(uValue);
        });
    }

    // connects a const member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)() const)
    {
        connect([=]() { // users a lambda for the callback
            (inst->*func)();
        });
    }

    void connect(std::function<void()> const &slot) const
    {
        slots_.push_back(slot);
    }

    // calls all connected functions
    void emit(void)
    {
        for (auto const &it : slots_)
        {
            it();
        }
    }

  private:
    mutable std::vector<std::function<void()>> slots_;
};
