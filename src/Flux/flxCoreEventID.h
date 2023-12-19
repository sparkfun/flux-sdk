
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

#include <cstdint>

///////////////////////////////////////////////////////////////////////////////
// Our core Event ID registry
//
// Goals:
//    - Each Event ID is unique
//    - Each ID is a constant and not mutable
//    - Event definition is simple
//
// Implementation
//    - The namespace provides an unique symbol (flxEvent::) for the IDs and ID Type
//    - Use the pre-processor macro __COUNTER__ to get unique IDs for each event, so no bookkeeping needed
//    - 'inline constexpr' is the modern method to define constants that share the same memory location
//    - Using {} for initialization - recommended these days, but basically same as `=`
//
//    The pattern used for the event IDs
//        flxEvent::k[Verb][Noun/Action]
//
namespace flxEvent
{
typedef uint32_t flxEventID_t;

inline constexpr flxEventID_t kLogErrWarn{__COUNTER__};
inline constexpr flxEventID_t kOnNewFile{__COUNTER__};
inline constexpr flxEventID_t kOnConnectionChange{__COUNTER__};
inline constexpr flxEventID_t kOnEdit{__COUNTER__};
inline constexpr flxEventID_t kOnEditFinished{__COUNTER__};
inline constexpr flxEventID_t kOnFirmwareLoad{__COUNTER__};
inline constexpr flxEventID_t kOnSystemActivity{__COUNTER__};
} // namespace flxEvent
