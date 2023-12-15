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
    {kMsgErrInvalidState, "%s: Invalid internal state"},
    {kMsgErrDeviceInit, "Device %s failed to %s"},
    {kMsgErrAllocError, "Allocation error for %s"},
    {kMsgErrAllocErrorN, "%s: Allocation error for %s"},
    {kMsgErrFileOpen, "%s: Unable to open file %s"},
    {kMsgErrSizeExceeded, "%s size exceeded limit"},
    {kMsgErrInitialization, "%s: Initialization Error - %s"},
    {kMsgErrValueNotProvided, "%s: Value not provided - %s"},
    {kMsgErrConnectionFailure, "%s: Connection Error: %s"},
    {kMsgErrValueError, "%s: Value Error - %s"},
    {kMsgErrCreateFailure, "%s failure to create %s"},
    {kMsgErrResourceNotAvail, "%s resource not available"},
    {kMsgErrSaveResState, "Error %s state for %s"}};
