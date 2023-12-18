/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include <functional>
#include <vector>
// spEvent.h
//
// Define the concept of signals/slots to enable events within the framework
//-----------------------------------------------------------------------
// 5/20 - Impl based on https://schneegans.github.io/tutorials/2015/09/20/signal-slot
// flxSignals - sort of  - support for our simple observer pattern for events
//

template <typename TB, typename... ArgT> class flxSignal
{

  public:
    // connects a member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT...))
    {
        connect([=](ArgT... args) { // uses a lambda for the callback
            (inst->*func)(args...);
        });
    }

    // connects a const member function to this flxSignal
    template <typename T> void call(T *inst, void (T::*func)(ArgT...) const)
    {
        connect([=](ArgT... args) { // users a lambda for the callback
            (inst->*func)(args...);
        });
    }

    // Just call a user supplied function - no object
    void call(void (*func)(ArgT...))
    {
        connect([=](ArgT... args) { // users a lambda for the callback
            (*func)(args...);
        });
    }
    // Just call a user supplied function - no object - but with a User Defined value
    template <typename T> void call(void (*func)(T uval, ArgT...), T uValue)
    {
        connect([=](ArgT... args) { // users a lambda for the callback
            (*func)(uValue, args...);
        });
    }

    template <typename T, typename U> void call(T *inst, void (T::*func)(U uVal, ArgT...), U uValue)
    {
        connect([=](ArgT... args) { // users a lambda for the callback
            (inst->*func)(uValue, args...);
        });
    }
    // connects a std::function to the flxSignal.
    void connect(std::function<void(ArgT &...Values)> const &slot) const
    {
        slots_.push_back(slot);
    }

    // calls all connected functions
    void emit(ArgT... args)
    {
        for (auto const &it : slots_)
        {
            it(args...);
        }
    }

    typedef TB value_type;

  private:
    mutable std::vector<std::function<void(ArgT &...Values)>> slots_;
};

typedef flxSignal<bool, bool> flxSignalBool;
typedef flxSignal<int8_t, int8_t> flxSignalInt8;
typedef flxSignal<int16_t, int16_t> flxSignalInt16;
typedef flxSignal<int, int> flxSignalInt;
typedef flxSignal<uint8_t, uint8_t> flxSignalUInt8;
typedef flxSignal<uint16_t, uint16_t> flxSignalUInt16;
typedef flxSignal<uint, uint> flxSignalUInt;
typedef flxSignal<float, float> flxSignalFloat;
typedef flxSignal<double, double> flxSignalDouble;
typedef flxSignal<const char *, const char *> flxSignalString;
typedef flxSignal<void> flxSignalVoid;
