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

#include "flxCore.h"

static const char *typeNames[] = {"none",  "bool",   "int8", "int16", "integer", "unsigned int8", "unsigned int16", "unsigned integer",
                                  "float", "double", "string"};

//-------------------------------------------------------------------------
// flxTypeName()
//
// Return a human type give the framework type
const char *flxGetTypeName(flxDataType_t type)
{
    if (type < sizeof(typeNames))
        return typeNames[type];

    return "Invalid Type";
}
