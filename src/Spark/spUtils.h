

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>

#include "spCoreTypes.h"
// use a utils namespace

namespace sp_utils
{

const char *spTypeName(spDataType_t type);

size_t dtostr(double value, char *szBuffer, size_t nBuffer, uint8_t precision = 3);

uint32_t id_hash_string(const char *str);

// Type strings in ordinary C++ syntax
template <typename T> char const *classname_helper()
{
    return __PRETTY_FUNCTION__ +
           sizeof(
#ifdef __clang__
               "const char* sp_utils::classname_helper() [T = "
#else
               "const char* sp_utils::classname_helper() [with T = "
#endif
               ) -
           1;
}

template <typename T> std::string getClassName()
{

    char const *const p = classname_helper<T>();
    return std::string(p, strlen(p) - sizeof(']'));
}

} // namespace sp_utils

/*
 * Example of how to use the stack based allocator with a string:
 *
 * Declaration
/-------------------------------------------------------------------------------------
// Use a stack based string allocator ... it basically replaces the allocator
// used with std::string with one that uses a stack memory area, and if that
// size is exceeded, uses a heap allocation scheme.
//
// See spUtil.h for more info.
#define kCSVStackMemoryBufferSize 500

template <class T, std::size_t buffSize = kCSVStackMemoryBufferSize>
using stackString = std::basic_string<T, std::char_traits<T>, short_alloc<T, buffSize, alignof(T)>>;

*
* Use/implementation
*
// For the stack based allocator  - First you create a area on the stack
    stackString<char>::allocator_type::arena_type static_allocator;

    // Create strings that use this stack areas
    stackString<char> _header_buffer{static_allocator};
    stackString<char> _data_buffer{static_allocator};

*/
// include the stack based allocator for std::
#include "utils/short_alloc.h"