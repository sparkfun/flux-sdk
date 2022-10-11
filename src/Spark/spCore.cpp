

#include "spCore.h"

static const char *typeNames[] = {"none",  "bool",   "int8",  "integer", "unsigned int8", "unsigned integer",
                                  "float", "double", "string"};

//-------------------------------------------------------------------------
// spTypeName()
//
// Return a human type give the framework type
const char *spGetTypeName(spDataType_t type)
{
    return typeNames[type];
}
