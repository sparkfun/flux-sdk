

#include "spUtils.h"

#include <math.h>
#include <string.h>

static const char *typeNames[] = {"none",  "bool",   "int8",  "integer", "unsigned int8", "unsigned integer",
                                  "float", "double", "string"};

//-------------------------------------------------------------------------
// spTypeName()
//
// Return a human type give the framework type
const char *sp_utils::spTypeName(spDataType_t type)
{
    return typeNames[type];
}

//-------------------------------------------------------------------------
// dtostr()
//
// Function to take a double and convert to a string.
//
// Logic from the quad project
//
size_t sp_utils::dtostr(double value, char *szBuffer, size_t nBuffer, uint8_t precision)
{
    if (!szBuffer || nBuffer == 0)
        return 0;

    memset(szBuffer, '\0', nBuffer);

    // check floating point math edge values
    // Not a Number?
    if (isnan(value))
    {
        strlcpy(szBuffer, "nan", nBuffer);
        return 3;
    }

    // Infinity?
    if (isinf(value))
    {
        strlcpy(szBuffer, "inf", nBuffer);
        return 3;
    }

    // keep track of chars in outupt buffer we use that are not digits
    size_t n_used = 1; // null char needed

    char *pCurr = szBuffer;
    // Handle negative numbers
    if (value < 0.0)
    {
        value = -value;
        *pCurr++ = '-';
        n_used++; // - sign
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    // I optimized out most of the divisions
    double rounding = 2.0;

    for (uint8_t i = 0; i < precision; ++i)
        rounding *= 10.0;

    rounding = 1.0 / rounding;

    value += rounding;

    // Figure out how big our number really is.
    double tenpow = 1.0;
    int digitcount = 1;
    while (value >= 10.0 * tenpow)
    {
        tenpow *= 10.0;
        digitcount++;
    }
    // take value to a sigle digit -
    value /= tenpow;

    // Print the digits, and if necessary, the decimal point
    digitcount += precision;

    // Will we have a decimal point? If we have precision then yes
    if (precision)
        n_used++;

    // will overflow our buffer? - reduce the number of digits
    if (n_used + digitcount > nBuffer)
        digitcount = nBuffer - n_used;

    int8_t digit;
    while (digitcount-- > 0)
    {
        digit = (int8_t)value;
        if (digit > 9)
            digit = 9; // insurance
        *pCurr++ = (char)('0' | digit);

        // Decimal point position?
        if ((digitcount == precision) && (precision > 0))
            *pCurr++ = '.';

        // next digit = subtract current, multiply up next digit.
        value = (value - digit) * 10.0;
    }

    // note: out string is null terminated b/c of entry memset to buffer.

    return strlen(szBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Internal hash function used to generate a unique ID based on a string
//    From: http://www.cse.yorku.ca/~oz/hash.html

uint32_t sp_utils::id_hash_string(const char *str)
{

    uint32_t hash = 5381;
    int c = *str;

    while (c != 0)
    {
        c = *str++;
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash; //& 0xFFFF; // NOTE - we're just using 16 bits
}

bool sp_utils::id_hash_string_to_string(const char * instr, char *outstr, size_t len){

    if ( !instr || !outstr || !len)
        return false;

    // has the input string - returns 32 bits of hashness
    uint32_t hash = id_hash_string(instr);

    // Now print this has into a string -- forms a unique tag
    snprintf(outstr, len, "%u", hash);

    return true;

}
