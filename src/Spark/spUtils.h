

#pragma once

#include <stddef.h>
#include <stdint.h>
// use a utils namespace

namespace sp_utils
{

size_t dtostr(double value, char *szBuffer, size_t nBuffer, uint8_t precision = 3);

}

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