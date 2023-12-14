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

typedef enum
{
    kMsgParentObjNotSet = 100,
    kMsgNotAddDupDev,
    kMsgErrSavingProperty,
    kMsgErrSaveResState
} flxMessageCoreID_t;

// extern const flxMessageBlock<flxMessageCoreID_t> msgBlockFluxCore = {
//     {kMsgParentObjNotSet, "Containing object not set. Verify flxRegister() was called on this %s"},
//     {kMsgNotAddDupDev, "Not adding duplicate device item to container: %s"}};
//
//
extern const flxMessageBlock<flxMessageCoreID_t> msgBlockFluxCore;
