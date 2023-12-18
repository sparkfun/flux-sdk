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

#include "flxCoreEventID.h"
#include "flxCoreLog.h"
#include "flxCoreTypes.h"

#include <functional>
#include <vector>
// spEvent.h
//
// Define the concept of signals/slots to enable events within the framework
//-----------------------------------------------------------------------
// 5/20 - Impl based on https://schneegans.github.io/tutorials/2015/09/20/signal-slot
// flxSignals - sort of  - support for our simple observer pattern for events
//
//
class flxSignalBase
{
    // empty base class
};

template <typename TB, typename... ArgT> class flxSignal : public flxSignalBase
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

///////////////////////////////////////////////////////////////////////////////////////
//
// event Hub testing
//

class _flxEventHub
{

  public:
    // _flxEventHub is a singleton
    static _flxEventHub &get(void)
    {

        static _flxEventHub instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _flxEventHub(_flxEventHub const &) = delete;
    void operator=(_flxEventHub const &) = delete;

    // connects a member function to this flxSignal
    template <typename T, typename TP> void registerEventCallback(flxEventID_t id, T *inst, void (T::*func)(TP var))
    {
        // do we have this

        flxSignal<TP, TP> *theSignal = nullptr;

        auto mpSig = _eventSignals.find(id);
        if (mpSig == _eventSignals.end())
        {
            theSignal = new flxSignal<TP, TP>;
            if (!theSignal)
            {
                flxLogM_E(kMsgErrAllocErrorN, "Event Hub", "callback");
                return;
            }
            _eventSignals[id] = theSignal;
        }
        else
            theSignal = reinterpret_cast<flxSignal<TP, TP> *>(mpSig->second);

        theSignal->call(inst, func);
    }
    // connects a member function to this flxSignal
    template <typename T> void registerEventCallback(flxEventID_t id, T *inst, void (T::*func)(void))
    {
        // do we have this

        flxSignal<void> *theSignal = nullptr;

        auto mpSig = _eventSignals.find(id);
        if (mpSig == _eventSignals.end())
        {
            theSignal = new flxSignal<void>;
            if (!theSignal)
            {
                flxLogM_E(kMsgErrAllocErrorN, "Event Hub", "callback");
                return;
            }
            _eventSignals[id] = theSignal;
        }
        else
            theSignal = reinterpret_cast<flxSignal<void> *>(mpSig->second);

        theSignal->call(inst, func);
    }

    // template <typename T> void makeCBCall(std::pair<flxEventID_t, flxSignal<T, T>> *theSignal, T &value)
    // {
    //     theSignal->second->emit(value);
    // }

    template <typename T> void postEvent(flxEventID_t id, T value)
    {
        auto mpSig = _eventSignals.find(id);
        if (mpSig == _eventSignals.end())
        {
            flxLog_E("Event handler not found: %u", id);
            return;
        }

        reinterpret_cast<flxSignal<T, T> *>(mpSig->second)->emit(value);
    }

    void postEvent(flxEventID_t id)
    {
        auto mpSig = _eventSignals.find(id);
        if (mpSig == _eventSignals.end())
        {
            flxLog_E("Event handler not found: %u", id);
            return;
        }

        reinterpret_cast<flxSignal<void> *>(mpSig->second)->emit();
    }

  private:
    _flxEventHub(){};

    // map event ID to event signal
    std::map<flxEventID_t, flxSignalBase *> _eventSignals;
};
extern _flxEventHub &flxEventHub;

template <typename T, typename TP> void flxRegisterEventCB(flxEventID_t id, T *inst, void (T::*func)(TP var))
{
    flxEventHub.registerEventCallback(id, inst, func);
}

template <typename T> void flxRegisterEventCB(flxEventID_t id, T *inst, void (T::*func)(void))
{
    flxEventHub.registerEventCallback(id, inst, func);
}

void flxEventPost(flxEventID_t id);

template <typename T> void flxEventPost(flxEventID_t id, T value)
{
    flxEventHub.postEvent(id, value);
}