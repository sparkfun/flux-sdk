/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// Messaging/logging system for the framework
#include <map>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message block testing
//

// Define our message block interface

class flxMessageBlockCore
{
  public:
    virtual const char *get(int idMessage) = 0;
    virtual const char *get(int idMessage) const = 0;
};

// and our block type template

template <typename T> class flxMessageBlock : public flxMessageBlockCore
{
  public:
    flxMessageBlock()
    {
    }
    flxMessageBlock(std::initializer_list<std::pair<const T, const char *>> msgSet) : _msgMap{msgSet}
    {
    }

    const char *get(int idMessage)
    {
        auto msg = _msgMap.find(static_cast<T>(idMessage));
        return (msg != _msgMap.end() ? msg->second : nullptr);
    }

    const char *get(int idMessage) const
    {
        return (const_cast<flxMessageBlock<T> *>(this))->get(idMessage);
    }

  private:
    std::map<T, const char *> _msgMap;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message block codes (enums) for the core flux block. These are mapped to format strings in the block
//
typedef enum
{
    kMsgParentObjNotSet = 100,
    kMsgErrAllocError,
    kMsgErrAllocErrorN,
    kMsgErrConnectionFailure,
    kMsgErrConnectionFailureD,
    kMsgNotAddDupDev,
    kMsgErrSavingProperty,
    kMsgErrSaveResState,
    kMsgErrDeviceInit,
    kMsgErrFileOpen,
    kMsgErrSizeExceeded,
    kMsgErrInitialization,
    kMsgErrValueNotProvided,
    kMsgErrValueError,
    kMsgErrResourceNotAvail,
    kMsgErrCreateFailure,
    kMsgErrInvalidState
} flxMessageCoreID_t;

// Extern ref to the core message block for Flux
extern const flxMessageBlock<flxMessageCoreID_t> msgBlockFluxCore;
