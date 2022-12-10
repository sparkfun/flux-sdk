


#pragma once

#include "spMQTTESP32.h"

#include "spFmtJSON.h"

//#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
//#define kAWSUpdateShadowTopic "$aws/things/%s/shadow/update"


// simple class to support ThingSpeak output

class spThingSpeak : public spMQTTESP32SecureCore<spThingSpeak>, public spIWriterJSON
{

public:
    spThingSpeak()
    {
        setName("ThingSpeak", "Connection to ThingSpeak");

        spark.add(this);

    }

    void write(JsonDocument &jsonDoc)
    {

    }

    // TODO VISIT
    bool initialize(void)
    {

        return spMQTTESP32SecureCore::initialize();

        // char szBuffer[clientName().length() + sizeof(kAWSUpdateShadowTopic)];
        // snprintf(szBuffer, sizeof(szBuffer), kAWSUpdateShadowTopic, clientName().c_str());

        // topic = szBuffer;
        
        // return true;
    }

};