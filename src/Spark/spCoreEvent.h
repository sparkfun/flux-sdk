

#pragma once

#include <vector>
// spEvent.h
//
// Define the concept of signals/slots to enable events within the framework
//-----------------------------------------------------------------------
// 5/20 - Impl based on https://schneegans.github.io/tutorials/2015/09/20/signal-slot
// spSignals - sort of  - support for our simple observer pattern for events
//

template <typename ArgT> class spSignal
{

  public:
    // connects a member function to this spSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT var))
    {
        connect([=](ArgT var) { // uses a lambda for the callback
            (inst->*func)(var);
        });
    }

    // connects a const member function to this spSignal
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
    // connects a std::function to the spSignal.
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

  private:
    mutable std::vector<std::function<void(ArgT &var)>> slots_;
};

typedef spSignal<bool> spSignalBool;
typedef spSignal<int8_t> spSignalInt8;
typedef spSignal<int> spSignalInt;
typedef spSignal<uint8_t> spSignalUInt8;
typedef spSignal<uint> spSignalUInt;
typedef spSignal<float> spSignalFloat;
typedef spSignal<double> spSignalDouble;
typedef spSignal<std::string &> spSignalString;

// Signal - zero arg - aka void function callback type. Unable to template this, so
// just brute force the impl.
class spSignalVoid
{

  public:
    // connects a member function to this spSignal
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

    // connects a const member function to this spSignal
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
