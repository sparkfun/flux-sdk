


#pragma once

#include "spMQTTESP32.h"

#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
#define kAWSUpdateShadowTopic "$aws/things/%s/shadow/update"


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

    bool initialize(void)
    {

        spMQTTESP32Secure::initialize();

        char szBuffer[clientName().length() + sizeof(kAWSUpdateShadowTopic)];
        snprintf(szBuffer, sizeof(szBuffer), kAWSUpdateShadowTopic, clientName().c_str());

        topic = szBuffer;
        
        return true;
    }
};