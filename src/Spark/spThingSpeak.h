


#pragma once

#include "spMQTTESP32.h"

#include "spFmtJSON.h"

//#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
//#define kAWSUpdateShadowTopic "$aws/things/%s/shadow/update"


// simple class to support ThingSpeak output

// Note - we contain an instance of a mqtt client. It does all the work
class spThingSpeak : public spActionType<spThingSpeak>, public spIWriterJSON
{

public:
    spThingSpeak()
    {
        setName("ThingSpeak", "Connection to ThingSpeak");

        spark.add(this);

        // promote properties up from the underlying mqtt object.

        addProperty(_mqttConnection.port);
        addProperty(_mqttConnection.username);
        addProperty(_mqttConnection.password);
        addProperty(_mqttConnection.caCertFilename);
        addProperty(_mqttConnection.clientCertFilename);
        addProperty(_mqttConnection.clientKeyFilename);
    }

    void write(JsonDocument &jsonDoc)
    {

    }

    // TODO VISIT
    bool initialize(void)
    {

        return _mqttConnection.initialize();

        // char szBuffer[clientName().length() + sizeof(kAWSUpdateShadowTopic)];
        // snprintf(szBuffer, sizeof(szBuffer), kAWSUpdateShadowTopic, clientName().c_str());

        // topic = szBuffer;
        
        // return true;
    }

private:
    spMQTTESP32Secure  _mqttConnection;

};