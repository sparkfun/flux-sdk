
/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include <cstdint>

///////////////////////////////////////////////////////////////////////////////
// Our core Event ID registry
//
// Goals:
//    - ID's can be defined anywhere in the system
//    - Each Event ID is unique
//    - Each ID is a constant and immutable
//    - Event definition is simple
//
// Implementation
//    - using empty objects provides a unique ID, and allows definition anywhere - not in a single file
//           * when combined with using a namespace - which can span multiple files.
//    - The namespace provides an unique symbol (flxEvent::) for the IDs and ID Type
//    - 'inline const' is the modern method to define constants that share the same memory location
//
//    The pattern used for the event IDs
//        flxEvent::k[Verb][Noun/Action]
//
namespace flxEvent
{

// define a simple object that creates our event ID. We use the pointer "this" as the ID number
// Note:
//    - While this seems like a lot, it uses very little space, gives a unique ID.
//    - The copy and assignment ops are deleted - to prevent any copy creations
//    - We use a ref type to pass these around - so it's always the same object for the type
//    - These can be created in different files, but still have a unique value
//    - inline const declaration makes sure only one copy is created,
//
class flxEventIDTypeDef
{
  public:
    // default constructor  - noop
    flxEventIDTypeDef()
    {
    }
    // delete copy and assignment constructors
    flxEventIDTypeDef(flxEventIDTypeDef const &) = delete;
    void operator=(flxEventIDTypeDef const &) = delete;

    // handy way to get an ID - the "()" operator
    uint32_t operator()(void) const
    {
        return (uint32_t)this;
    }
};

// define the type used to pass these around - via refs
using flxEventID_t = const flxEventIDTypeDef &;

} // namespace flxEvent

// define a handy macro to define an event ID type
#define flxDefineEventID(__event__)                                                                                    \
    namespace flxEvent                                                                                                 \
    {                                                                                                                  \
    inline flxEventIDTypeDef const __event__;                                                                          \
    }

// just one event here -- system activity. Other events defined across the framework
flxDefineEventID(kOnSystemActivity);

// Low notice system activity;
flxDefineEventID(kOnSystemActivityLow);
