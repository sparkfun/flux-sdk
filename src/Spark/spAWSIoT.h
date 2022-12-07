


#pragma once

#include "spMQTTESP32.h"

#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
// simple class to support AWS IoT

class spAWSIoT : public spMQTTESP32Secure
{
public:
    virtual void write(const char * value, bool newline)
    {
    	if (!value)
    		return;

    	// Wrap the value with the structure required to update the device shadow
        char szBuffer[strlen(value) + sizeof(kAWSUpdateShadowTemplate)];
        snprintf(szBuffer, sizeof(szBuffer),  kAWSUpdateShadowTemplate, value);
        spMQTTESP32Secure::write(szBuffer, false);
    }
};