/*
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "flxCore.h"

static const struct
{
    flxDataType_t type;
    const char *name;
} typeNames[] = {
    {flxTypeNone, "none"},   {flxTypeBool, "bool"},     {flxTypeInt8, "int8"},     {flxTypeInt16, "int16"},
    {flxTypeInt32, "int32"}, {flxTypeUInt8, "uint8"},   {flxTypeUInt16, "uint16"}, {flxTypeUInt32, "uint32"},
    {flxTypeFloat, "float"}, {flxTypeDouble, "double"}, {flxTypeString, "string"},
};

//-------------------------------------------------------------------------
// flxTypeName()
//
// Return a human type give the framework type
const char *flxGetTypeName(flxDataType_t type)
{
    for (size_t i = 0; i < sizeof(typeNames) / sizeof(typeNames[0]); i++)
    {
        if (typeNames[i].type == type)
            return typeNames[i].name;
    }

    return "Invalid Type";
}
