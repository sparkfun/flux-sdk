


#pragma once

#include "spMQTTESP32.h"

#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
#define kAWSUpdateShadowTopic "$aws/things/%s/shadow/update"


// simple class to support AWS IoT

class flxIoTAWS : public spMQTTESP32SecureCore<flxIoTAWS>, public flxWriter
{
public:
    flxIoTAWS()
    {
        setName("AWS IoT", "Connection to AWs IoT");
        spark.add(this);
    }
    
    // for the Writer interface
    void write(int data)
    {
        // noop
    }
    void write(float data)
    {
        // noop
    }
    virtual void write(const char * value, bool newline)
    {
    	if (!value)
    		return;

    	// Wrap the value with the structure required to update the device shadow
        char szBuffer[strlen(value) + sizeof(kAWSUpdateShadowTemplate)];
        snprintf(szBuffer, sizeof(szBuffer),  kAWSUpdateShadowTemplate, value);
        spMQTTESP32SecureCore::write(szBuffer, false);
    }

    bool initialize(void)
    {

        spMQTTESP32SecureCore::initialize();

        char szBuffer[clientName().length() + sizeof(kAWSUpdateShadowTopic)];
        snprintf(szBuffer, sizeof(szBuffer), kAWSUpdateShadowTopic, clientName().c_str());

        topic = szBuffer;
        
        return true;
    }
};