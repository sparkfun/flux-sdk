

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>

// use a utils namespace

namespace sp_utils
{

size_t dtostr(double value, char *szBuffer, size_t nBuffer, uint8_t precision = 3);

uint32_t id_hash_string(const char *str);

bool id_hash_string_to_string(const char *instr, char *outstr, size_t len);

//-------------------------------------------------------------------
std::string &to_string(std::string &data);
const std::string &to_string(std::string const &data);
std::string to_string(int const data);
std::string to_string(int8_t const data);
std::string to_string(int16_t const data);
std::string to_string(uint const data);
std::string to_string(uint8_t const data);
std::string to_string(uint16_t const data);
std::string to_string(float const data, uint8_t precision);
std::string to_string(float const data);
std::string to_string(double const data, uint8_t precision);
std::string to_string(double const data);
std::string to_string(bool const data);

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