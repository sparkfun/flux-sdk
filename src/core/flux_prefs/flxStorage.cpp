

/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxStorage.h"
#include "flxCoreLog.h"
#include "flxFlux.h"
#include "flxUtils.h"

//--------------------------------------------------------------------------
// saveSecureString()
//
// Takes in a string, AES encodes it and writes out as a binary blob.
//
// Notes:
//    Uses the device ID and the application token array to create a unique key for
//    the process.
//
//    Encryption works on 16 byte blocks - if there is trailing/empty bytes in the
//    last block, we fill them with nulls (\0), so in decrypt the result is a c string
//    with extra nulls.

bool flxStorageBlock::saveSecureString(const char *tag, const char *data)
{

    if (!data || strlen(data) == 0)
        return true;

    // token/key
    uint8_t token[32];
    if (!flux.getAppToken(token))
    {
        flxLogM_E(kMsgErrValueNotProvided, "Secure Storage", "App Token");
        return false;
    }
    unsigned char IV[16];
    memcpy(IV, flux.deviceId(), sizeof(IV));

    int len = strlen(data);

    // Buffer needs to be a multiple of 16 - AES uses 16 byte blocks
    int remainder = len % 16;

    int buffer_size = (len / 16 + (remainder > 0 ? 1 : 0)) * 16;

    char input_buffer[buffer_size + 1];
    memset(input_buffer, '\0', sizeof(input_buffer));
    memcpy(input_buffer, data, len);

    char encoded_buffer[buffer_size + 1];
    memset(encoded_buffer, '\0', sizeof(encoded_buffer));

    if (!flx_utils::encode_data_aes(token, IV, input_buffer, encoded_buffer, buffer_size))
    {
        flxLog_W(F("Error encoding secure string"));
        return false;
    }

    bool status = this->writeBytes(tag, (const uint8_t *)encoded_buffer, buffer_size);
    if (!status)
        flxLogM_E(kMsgErrSavingProperty, tag);

    return status;
}

//--------------------------------------------------------------------------
// restoreSecureString()
//
// The inverse of the above method - decrypts a given string.
//
// If the provided buffer isn't long enough, the value is truncated.

bool flxStorageBlock::restoreSecureString(const char *tag, char *data, size_t len)
{

    if (!data || len == 0)
        return false;

    // token/key
    uint8_t token[32];
    if (!flux.getAppToken(token))
    {
        flxLogM_E(kMsgErrValueNotProvided, "Settings", "Application Token");
        return false;
    }
    char data_buffer[512];

    size_t read_size = this->readBytes(tag, (uint8_t *)data_buffer, sizeof(data_buffer));
    if (read_size == 0)
        return false;

    int remainder = read_size % 16;
    int buffer_size = (read_size / 16 + (remainder > 0 ? 1 : 0)) * 16;

    char input_buffer[buffer_size + 1];
    memset(input_buffer, '\0', sizeof(input_buffer));
    memcpy(input_buffer, data_buffer, read_size);

    char decoded_buffer[buffer_size + 1];
    memset(decoded_buffer, '\0', sizeof(decoded_buffer));

    unsigned char IV[16];
    memcpy(IV, flux.deviceId(), sizeof(IV));

    if (!flx_utils::decode_data_aes(token, IV, input_buffer, decoded_buffer, buffer_size))
    {
        flxLogM_W(kMsgErrInvalidState, "string decode");
        return false;
    }

    memset(data, '\0', len);

    memcpy(data, decoded_buffer, (len - 1 < buffer_size ? len - 1 : buffer_size));

    return true;
}
