

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
