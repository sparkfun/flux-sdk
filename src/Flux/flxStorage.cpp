


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


#include "flxStorage.h"
#include "flxUtils.h"
#include "flxCoreLog.h"
#include "flxFlux.h"

// TODO FIX
char * key = "1234567kjueswabcoiu223smzx2he1as";

bool flxStorageBlock::saveSecureString(flxStorageBlock *stBlk, const char * tag, char * data)
{
    unsigned char IV[16];

    memcpy(IV, flux.deviceId(), sizeof(IV));

    int len = strlen(data);

    int remander = len % 16;

    int buffer_size = (len/16 + (remander > 0 ? 1 : 0))*16;

    char input_buffer[buffer_size+1];
    memset(input_buffer, '\0', sizeof(input_buffer));
    memcpy(input_buffer, data, len);

    char encoded_buffer[buffer_size+1];
    memset(encoded_buffer, '\0', sizeof(encoded_buffer));

    if (!flx_utils::encode_data_aes((uint8_t*)key, IV, input_buffer, encoded_buffer, buffer_size) )
    {
        flxLog_W("Error encoding secure string");
        return false;
    }

   	bool status = stBlk->writeBytes(tag, (const uint8_t*)encoded_buffer, buffer_size);
    if (!status)
       flxLog_E("Error saving secure string for property: %s",  tag);

   return status;
}

bool flxStorageBlock::restoreSecureString(flxStorageBlock *stBlk, const char * tag, char * data, size_t len)
{

	char data_buffer[512];

	size_t read_size = stBlk->readBytes(tag, (uint8_t*)data_buffer, sizeof(data_buffer));
	if (read_size == 0)
		return false;

	int remander = read_size % 16;
    int buffer_size = (read_size/16 + (remander > 0 ? 1 : 0))*16;

    char input_buffer[buffer_size+1];
    memset(input_buffer, '\0', sizeof(input_buffer));
    memcpy(input_buffer, data_buffer, read_size);

    char decoded_buffer[buffer_size+1];
    memset(decoded_buffer, '\0', sizeof(decoded_buffer));

    unsigned char IV[16];
    memcpy(IV, flux.deviceId(), sizeof(IV));

    if (!flx_utils::decode_data_aes((uint8_t*)key, IV, input_buffer, decoded_buffer, buffer_size) )
    {
        flxLog_W("Error decoding secure string");
        return false;
    }

    memset(data, '\0', len);

    memcpy(data, decoded_buffer, (len-1 < buffer_size ? len-1 : buffer_size));

  	return true;
}
