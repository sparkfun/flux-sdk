/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
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
std::string flx_utils::to_string(int32_t const data)
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
std::string flx_utils::to_string(uint32_t const data)
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

    const char *sizeNames[] = {"B", "KB", "MB", "GB", "TB"};

    if (nBytes < 0)
        nBytes = 0;

    if (prec < 0)
        prec = 0;

    double tmp1 = floor((nBytes ? log(nBytes) : 0) / log(1000.));

    // overflow our name array?
    if (tmp1 > (sizeof(sizeNames) / sizeof(char *)) - 1)
        tmp1 = (sizeof(sizeNames) / sizeof(char *)) - 1;

    if (tmp1 < .1)
        prec = 0;

    char szFormat[32];
    snprintf(szFormat, sizeof(szFormat), "%%.%df%%s", prec);
    snprintf(szBuffer, len, szFormat, nBytes / pow(1000., tmp1), sizeNames[(int)tmp1]);
}

//
//---------------------------------------------------------------------------------------
/// createVariableName()
///
/// @brief     Creates a valid arduino variable name
/// @param[in] szInVariable the name to convert
/// @param[out] szOutVariable the converted name - assumed to same len as in variable.
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
//---------------------------------------------------------------------------------------
// CRC32 Calculation
// Implementation of CRC32 from https://web.mit.edu/freebsd/head/sys/libkern/crc32.c

static const uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832,
    0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a,
    0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
    0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
    0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4,
    0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074,
    0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525,
    0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
    0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76,
    0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6,
    0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
    0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7,
    0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
    0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330,
    0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

//---------------------------------------------------------------------------------------
/// calc_crc32()
///
/// @brief     Calculate a CRC32 value for a buffer
/// @param[in] crc input CRC value
/// @param[in] buf buffer to calculate CRC on
/// @param[in] size size of buffer
///
/// @return The calculated CRC32 value
///
uint32_t flx_utils::calc_crc32(uint32_t crc, const uint8_t *buf, uint32_t size)
{

    crc = ~crc;
    while (size--)
        crc = crc32_tab[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    return ~crc;
}