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

#include "flxUtils.h"

#include <math.h>
#include <string.h>
#include <time.h>

#include <Arduino.h>

#include "flxCoreLog.h"

#include "mbedtls/aes.h"

//-------------------------------------------------------------------------
// dtostr()
//
// Function to take a double and convert to a string.
//
// Logic from the quad project
//
size_t flx_utils::dtostr(double value, char *szBuffer, size_t nBuffer, uint8_t precision)
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

    // keep track of chars in output buffer we use that are not digits
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

uint32_t flx_utils::id_hash_string(const char *str)
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
//-------------------------------------------------------------------
// returns a string version of the hash.
bool flx_utils::id_hash_string_to_string(const char *instr, char *outstr, size_t len)
{

    if (!instr || !outstr || !len)
    {
        flxLog_E("String hash - invalid parameters");
        return false;
    }

    // hash the input string - returns 32 bits of hash-ness
    uint32_t hash = id_hash_string(instr);

    // Now print this has into a string -- forms a unique tag
    snprintf(outstr, len, "%X", hash);

    return true;
}
//-------------------------------------------------------------------
std::string &flx_utils::to_string(std::string &data)
{
    return data;
}
//-------------------------------------------------------------------
const std::string &flx_utils::to_string(std::string const &data)
{
    return data;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(char *const data)
{
    std::string stmp = data;
    return stmp;
}

//-------------------------------------------------------------------
std::string flx_utils::to_string(int const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%d", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(int8_t const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%d", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(int16_t const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%d", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(uint const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%u", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(uint8_t const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%u", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(uint16_t const data)
{
    char szBuffer[20];
    snprintf(szBuffer, sizeof(szBuffer), "%u", data);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(float const data, uint8_t precision)
{
    return flx_utils::to_string((double)data, precision);
    char szBuffer[20];
    flx_utils::dtostr((double)data, szBuffer, sizeof(szBuffer), precision);
    std::string stmp = szBuffer;
    return stmp;
}
std::string flx_utils::to_string(float const data)
{
    return flx_utils::to_string(data, 3);
}

//-------------------------------------------------------------------
std::string flx_utils::to_string(double const data, uint8_t precision)
{
    char szBuffer[20];
    flx_utils::dtostr((double)data, szBuffer, sizeof(szBuffer), precision);
    std::string stmp = szBuffer;
    return stmp;
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(double const data)
{
    return flx_utils::to_string(data, 4);
}
//-------------------------------------------------------------------
std::string flx_utils::to_string(bool const data)
{
    std::string stmp;
    stmp = data ? "true" : "false";
    return stmp;
}

//-------------------------------------------------------------------
std::string flx_utils::strtrim(const std::string &str, const std::string &whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
// Simple data encoder - using a random number generator and XOR
// because this uses xor - encode/decode are the same

static void simple_encode(uint8_t *source, uint8_t *dest, size_t len, uint32_t key)
{
    randomSeed(key);

    for (int i = 0; i < len; i++)
        *dest++ = *source++ ^ random(1, 255);
}
void flx_utils::encode_data(uint8_t *source, uint8_t *dest, size_t len, uint32_t key)
{
    simple_encode(source, dest, len, key);
}

void flx_utils::decode_data(uint8_t *source, uint8_t *dest, size_t len, uint32_t key)
{
    simple_encode(source, dest, len, key);
}

bool flx_utils::encode_data_aes(uint8_t key[32], unsigned char iv[16], char *source, char *output, size_t len)
{
    if (!source || !output)
        return false;

    // Expect inputs to be in 16 byte blocks.
    if (len % 16 != 0)
    {
        flxLog_E(F("Invalid data encryption block size."));
        return false;
    }

    mbedtls_aes_context ctxAES;
    int rc = mbedtls_aes_setkey_enc(&ctxAES, key, 256);
    if (rc != 0)
    {
        flxLog_E(F("Invalid encryption key length"));
        return false;
    }
    rc = mbedtls_aes_crypt_cbc(&ctxAES, MBEDTLS_AES_ENCRYPT, len, iv, (unsigned char *)source, (unsigned char *)output);

    if (rc != 0)
    {
        flxLog_E(F("Invalid encryption key length"));
        return false;
    }

    mbedtls_aes_free(&ctxAES);

    return true;
}
bool flx_utils::decode_data_aes(uint8_t *key, unsigned char iv[16], char *source, char *output, size_t len)
{
    if (!source || !output)
        return false;

    // Expect inputs to be in 16 byte blocks.
    if (len % 16 != 0)
    {
        flxLog_E(F("Invalid data encryption block size."));
        return false;
    }

    mbedtls_aes_context ctxAES;
    int rc = mbedtls_aes_setkey_dec(&ctxAES, key, 256);
    if (rc != 0)
    {
        flxLog_E(F("Invalid decryption key length"));
        return false;
    }
    rc = mbedtls_aes_crypt_cbc(&ctxAES, MBEDTLS_AES_DECRYPT, len, iv, (unsigned char *)source, (unsigned char *)output);

    if (rc != 0)
    {
        flxLog_E(F("Invalid decryption key length"));
        return false;
    }

    mbedtls_aes_free(&ctxAES);

    return true;
}
//---------------------------------------------------------------------------------------------------
// calculate uptime for the device ... since last boot
void flx_utils::uptime(uint32_t &days, uint32_t &hours, uint32_t &minutes, uint32_t &secs, uint32_t &mills)
{
    mills = millis();
    secs = mills / 1000;
    minutes = secs / 60;
    hours = minutes / 60;
    days = hours / 24;
    mills %= 1000;
    secs %= 60;
    minutes %= 60;
    hours %= 24;
}
//---------------------------------------------------------------------------------------------------
// Return a ISO8601 timestamp
void flx_utils::timestampISO8601(time_t &t_time, char *buffer, size_t length, bool bTZ)
{

    struct tm *tmLocal = localtime(&t_time);
    strftime(buffer, length, "%G-%m-%dT%T", tmLocal);

    if (!bTZ)
        return;

    time_t t_gmt = mktime(gmtime(&t_time));
    int deltaT = t_time - t_gmt;

    char chSign;
    if (deltaT < 0)
    {
        chSign = '-';
        deltaT *= -1;
    }
    else
        chSign = '+';

    char szTmp[24] = {0};

    int tz_hrs = deltaT / 3600;
    int tz_min = (deltaT % 3600) / 60;

    snprintf(szTmp, sizeof(szTmp), "%c%02d:%02d", chSign, tz_hrs, tz_min);

    strlcat(buffer, szTmp, length);
}

//---------------------------------------------------------------------------------------------------
// formatBytes()
//
// Return a formatted byte string. This returns values of B (1000), not iB (1024)

void flx_utils::formatByteString(uint64_t nBytes, uint prec, char *szBuffer, size_t len)
{

    char *sizeNames[] = {"Bytes", "KB", "MB", "GB", "TB"};

    if (nBytes < 0)
        nBytes = 0;

    if (prec < 0)
        prec = 0;

    double tmp1 = floor((nBytes ? log(nBytes) : 0) / log(1000.));

    // overflow our name array?
    if (tmp1 > (sizeof(sizeNames) / sizeof(char *)) - 1)
        tmp1 = (sizeof(sizeNames) / sizeof(char *)) - 1;

    char szFormat[32];
    snprintf(szFormat, sizeof(szFormat), "%%.%df%%s", prec);
    snprintf(szBuffer, len, szFormat, nBytes / pow(1000., tmp1), sizeNames[(int)tmp1]);
}

//
//---------------------------------------------------------------------------------------
/// createVariableName()
///
/// @brief     Creates a valid arduino variable name
/// @param[In] szInVariable the name to convert
/// @param[Out] szOutVariable the converted name - assumed to same len as in variable.
///
/// @return true on success, false on failure
///
bool flx_utils::createVariableName(const char *szInVariable, char *szOutVariable)
{
    if (!szInVariable || !szOutVariable)
        return false;

    size_t nChar = strlen(szInVariable);

    int idst = 0;
    int isrc = 0;
    for (; isrc < nChar; isrc++)
    {
        // pass through alphanumeric and underscores
        if (std::isalnum(szInVariable[isrc]) || szInVariable[isrc] == '_')
            szOutVariable[idst++] = szInVariable[isrc];
        else if (szInVariable[isrc] == ' ') // spaces to underlines
            szOutVariable[idst++] = '_';
    }
    szOutVariable[idst] = '\0';

    return (strlen(szOutVariable) > 1);
}