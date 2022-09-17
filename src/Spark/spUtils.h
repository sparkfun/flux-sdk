

#pragma once


#include <stddef.h>
#include <stdint.h>
// use a utils namespace

namespace sp_utils{

size_t dtostr(double value, char * szBuffer, size_t nBuffer, uint8_t precision=3);

}