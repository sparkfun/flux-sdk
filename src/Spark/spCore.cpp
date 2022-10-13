

#include "spCore.h"

static const char *typeNames[] = {"none",  "bool",   "int8", "int16", "integer", "unsigned int8", "unsigned int16", "unsigned integer",
                                  "float", "double", "string"};

//-------------------------------------------------------------------------
// spTypeName()
//
// Return a human type give the framework type
const char *spGetTypeName(spDataType_t type)
{
    if (type < sizeof(typeNames))
        return typeNames[type];

    return "Invalid Type";
}
