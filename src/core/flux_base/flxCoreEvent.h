/*
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "flxCoreEventID.h"
#include "flxCoreLog.h"
#include "flxCoreTypes.h"

#include <functional>
#include <map>
#include <memory>
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
typedef flxSignal<int32_t, int32_t> flxSignalInt32;
typedef flxSignal<uint8_t, uint8_t> flxSignalUInt8;
typedef flxSignal<uint16_t, uint16_t> flxSignalUInt16;
typedef flxSignal<uint32_t, uint32_t> flxSignalUInt32;
typedef flxSignal<float, float> flxSignalFloat;
typedef flxSignal<double, double> flxSignalDouble;
typedef flxSignal<const char *, const char *> flxSignalString;
typedef flxSignal<void> flxSignalVoid;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//
// Event Hub work
//
// Purpose:
//    The original event method - modeled after the signal and slot pattern - relied on the event receiver /observer
//    having a direct connection/access to the object being observed. This doesn't scale well, and fails with *general*
//    events ( a system *busy* event fired ... etc). So added an event hub
//
//    The event hub decouples the source from the sink of the event model. Events are posted, and if someone is
//    interested in it, they event is sent to a registered callback. In practice, the hub automatically creates the
//    above signal objects and maps them to an ID. This ID is used to register callbacks and post messages. The event
//    source and sink don't know about each other, but they use the same event ID to pass information via the Event Hub.
//
// Implementation:
//    The event hub is singleton that is built around a std::map. The event ID is mapped to an event signal object
//    (one of the above). When an event is registered, the provided callback is passed to the signal object. If
//    a signal object doesn't exist for the provided event ID, one is created.
//
//    When an event is *sent*, the event signal object is retrieved from the map, and the emit() method  called.
//
// Potential Issues:
//    Type matching of the event callback parameters might cause an issue. But since everything is tightly controlled
//    at this point in the implementation, this is not a major concern.
//

//----------------------------------------------------------------------------------------------------
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

    //----------------------------------------------------------------------------------------------------
    // Connects a member function to an Event ID using a Signal object. Template parameters are used to
    // determine types of the underlying signal object created.
    //
    template <typename T, typename TP>
    void registerEventCallback(flxEvent::flxEventID_t id, T *inst, void (T::*func)(TP var))
    {
        // do we have this event already registered

        flxSignal<TP, TP> *theSignal = nullptr;

        auto mpSig = _eventSignals.find(id());

        if (mpSig == _eventSignals.end())
        {
            // not setup, create it
            theSignal = new flxSignal<TP, TP>;
            if (!theSignal)
            {
                flxLogM_E(kMsgErrAllocErrorN, "Event Hub", "callback");
                return;
            }
            _eventSignals[id()] = theSignal;
        }
        else
            theSignal = reinterpret_cast<flxSignal<TP, TP> *>(mpSig->second); // exists

        theSignal->call(inst, func);
    }

    //----------------------------------------------------------------------------------------------------
    // Connects a member function to an Event ID using a void Signal object.
    //
    template <typename T> void registerEventCallback(flxEvent::flxEventID_t id, T *inst, void (T::*func)(void))
    {
        // do we have this event already registered

        flxSignal<void> *theSignal = nullptr;

        auto mpSig = _eventSignals.find(id());

        if (mpSig == _eventSignals.end())
        {
            // not setup, create it
            theSignal = new flxSignal<void>;
            if (!theSignal)
            {
                flxLogM_E(kMsgErrAllocErrorN, "Event Hub", "callback");
                return;
            }
            _eventSignals[id()] = theSignal;
        }
        else
            theSignal = reinterpret_cast<flxSignal<void> *>(mpSig->second); // exists

        theSignal->call(inst, func);
    }

    //----------------------------------------------------------------------------------------------------
    // Send and event with the given value.
    //
    template <typename T> void sendEvent(flxEvent::flxEventID_t id, T value)
    {
        // do the version that uses id numbers
        sendEvent(id(), value);
    }
    //----------------------------------------------------------------------------------------------------
    // Send a void event
    //
    void sendEvent(flxEvent::flxEventID_t id)
    {
        // do the version that uses id numbers
        sendEvent(id());
    }

    // send using the event number
    //----------------------------------------------------------------------------------------------------
    // Send and event with the given value.
    //
    template <typename T> void sendEvent(flxEvent::flxEventIDNum_t num, T value)
    {
        // does this event exist/registered?
        auto mpSig = _eventSignals.find(num);

        // send it
        if (mpSig != _eventSignals.end())
            reinterpret_cast<flxSignal<T, T> *>(mpSig->second)->emit(value);

        // now check if there are any aliases for this event
        // does this event exist/registered?
        auto mpAlias = _eventAlias.find(num);

        // None?
        if (mpAlias == _eventAlias.end())
            return;

        // send the alias events
        for (auto &alias : mpAlias->second)
            alias->dispatch(value);
    }
    //----------------------------------------------------------------------------------------------------
    // Send a void event
    //
    void sendEvent(flxEvent::flxEventIDNum_t num)
    {
        // first process this event.
        auto mpSig = _eventSignals.find(num);

        // send it if it exists
        if (mpSig != _eventSignals.end())
            reinterpret_cast<flxSignal<void> *>(mpSig->second)->emit();

        // now check if there are any aliases for this event
        // does this event exist/registered?
        auto mpAlias = _eventAlias.find(num);

        // None?
        if (mpAlias == _eventAlias.end())
            return;

        // send the alias events
        for (auto &alias : mpAlias->second)
            alias->dispatch();
    }

    //----------------------------------------------------------------------------------------------------
    // add an alias for an event ID

    void addEventAlias(flxEvent::flxEventID_t id, flxEvent::flxEventID_t alias)
    {
        // add the alias
        _eventAlias[id()].push_back(std::make_unique<_flxEventAlias>(alias));
    }
    // now an alias that includes a value that is always sent.
    template <typename T> void addEventAliasWithValue(flxEvent::flxEventID_t id, flxEvent::flxEventID_t alias, T value)
    {
        // add the alias with a default value
        _eventAlias[id()].push_back(std::make_unique<_flxEventAliasWithValue<T>>(alias, value));
    }

  private:
    //----------------------------------------------------------------------------------------------------
    // Alias work
    //
    // Purpose:
    //    Map one event ID to another - easy. But also include a default payload to send if desired.

    class _flxEventAlias
    {
      public:
        _flxEventAlias(flxEvent::flxEventID_t id) : _id{id()}
        {
        }
        _flxEventAlias(flxEvent::flxEventIDNum_t num) : _id{num}
        {
        }

        // Dispatch an event
        template <typename T> void dispatch(T value)
        {
            _flxEventHub::get().sendEvent(_id, value);
        }
        // Dispatch a void event
        virtual void dispatch(void)
        {
            _flxEventHub::get().sendEvent(_id);
        }

      protected:
        flxEvent::flxEventIDNum_t _id; // the event ID number
    };

    // now a alias that includes a default value to send
    template <typename T> class _flxEventAliasWithValue : public _flxEventAlias
    {
      public:
        _flxEventAliasWithValue(flxEvent::flxEventID_t id, T value) : _flxEventAlias(id), _value(value)
        {
        }
        _flxEventAliasWithValue(flxEvent::flxEventIDNum_t num, T value) : _flxEventAlias(num), _value(value)
        {
        }
        // Dispatch a void event
        virtual void dispatch(void)
        {
            _flxEventHub::get().sendEvent(_id, _value);
        }

      private:
        T _value; // default value to send
    };

    _flxEventHub() {};

    // map event ID to event signal
    std::map<flxEvent::flxEventIDNum_t, flxSignalBase *> _eventSignals;

    // map to store event alias - one event ID can have multiple aliases
    // If a event is called, the alias list is also checked.
    std::map<flxEvent::flxEventIDNum_t, std::vector<std::unique_ptr<_flxEventAlias>>> _eventAlias;
};
//
extern _flxEventHub &flxEventHub;

//----------------------------------------------------------------------------------------------------
// User exposed convenience function to register a value based callback
//
template <typename T, typename TP> void flxRegisterEventCB(flxEvent::flxEventID_t id, T *inst, void (T::*func)(TP var))
{
    flxEventHub.registerEventCallback(id, inst, func);
}
//----------------------------------------------------------------------------------------------------
// User exposed convenience function to register a void callback
//
template <typename T> void flxRegisterEventCB(flxEvent::flxEventID_t id, T *inst, void (T::*func)(void))
{
    flxEventHub.registerEventCallback(id, inst, func);
}
//----------------------------------------------------------------------------------------------------
// User exposed convenience function to send a void /empty event
//
void flxSendEvent(flxEvent::flxEventID_t id);

//----------------------------------------------------------------------------------------------------
// User exposed convenience function to send a void /empty event
//
void flxSendEvent(flxEvent::flxEventIDNum_t id);

//----------------------------------------------------------------------------------------------------
// User exposed convenience function to sent an event that takes a value
//
template <typename T> void flxSendEvent(flxEvent::flxEventID_t id, T value)
{
    flxEventHub.sendEvent(id, value);
}

//----------------------------------------------------------------------------------------------------
// User exposed convenience function to sent an event that takes a value
//
template <typename T> void flxSendEvent(flxEvent::flxEventIDNum_t num, T value)
{
    flxEventHub.sendEvent(num, value);
}

//----------------------------------------------------------------------------------------------------
// simple alias
void flxAddEventAlias(flxEvent::flxEventID_t id, flxEvent::flxEventID_t alias);

//----------------------------------------------------------------------------------------------------
// Alias with a value that is always sent with the event
template <typename T> void flxAddEventAliasWithValue(flxEvent::flxEventID_t id, flxEvent::flxEventID_t alias, T value)
{
    flxEventHub.addEventAliasWithValue(id, alias, value);
}
