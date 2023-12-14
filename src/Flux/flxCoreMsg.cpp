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

#include "flxCoreMsg.h"

// Just declare the implementation of the string table/message block.
//
// Note: This needs to be in a impl file, not the header. In the header, I believe the
// strings are duplicated.
//
const flxMessageBlock<flxMessageCoreID_t> msgBlockFluxCore = {
    {kMsgParentObjNotSet, "Containing object not set. Verify flxRegister() was called on this %s"},
    {kMsgNotAddDupDev, "Not adding duplicate device item to container: %s"},
    {kMsgErrSavingProperty, "Error saving property %s"},
    {kMsgErrSaveResState, "Error %s state for %s"}};
