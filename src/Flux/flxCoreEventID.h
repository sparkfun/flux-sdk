
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
// Our core Event ID registry...etc

typedef const uint32_t flxEventID_t;

flxEventID_t kFlxEventLogErrWarn = 0x1000;
flxEventID_t kFlxEventOnEdit = 0x1001;
flxEventID_t kFlxEventOnEditFinished = 0x1002;
flxEventID_t kFlxEventOnNewFile = 0x1003;
flxEventID_t kFlxEventOnConnectionChange = 0x1004;
flxEventID_t kFlxEventOnFirmwareLoad = 0x1005;
flxEventID_t kFlxEventOnSystemActivity = 0x1006;
