


#pragma once

#include "spMQTTESP32.h"

#include "spFmtJSON.h"
#include "spUtils.h"

#include <vector>
#include <sstream>
//#define kAWSUpdateShadowTemplate "{\"state\":{\"reported\":%s}}"
//#define kAWSUpdateShadowTopic "$aws/things/%s/shadow/update"


// simple class to support ThingSpeak output

class spThingSpeak : public spMQTTESP32SecureCore<spThingSpeak>, public spIWriterJSON
{
private:
    //---------------------------------------------------------
    std::string get_channelList(void)
    {
        std::string theList = "";

        for ( auto channel : _channelList)
        {
            if (theList.length() > 0)
                theList += ", ";

            theList += channel;
        }
        return theList;
    }

    //---------------------------------------------------------
    void set_channelList(std::string theList)
    {
        _channelList.clear();

        std::stringstream s_stream(theList); //create string stream from the string
        std::string token;
        while(s_stream.good()) 
        {
            getline(s_stream, token, ','); //get first string delimited by comma
            _channelList.push_back(sp_utils::strtrim(token));
        }
    }
public:
    spThingSpeak()
    {
        setName("ThingSpeak", "Connection to ThingSpeak");

        spark.add(this);

    }

    void write(JsonDocument &jsonDoc)
    {
        // loop over our channels and see if they are in the document
        JsonObject jObj;
        std::string buffer;
        char szBuffer[64];
        int i;
        for (auto channel : _channelList)
        {
            jObj = jsonDoc[channel];
            if (jObj.isNull())
                continue;

            // Thingspeak channel values are sent as "field<n>=<value>&field<n+1>=<value>..."
            buffer="";
            i=1;
            for (JsonPair kv : jObj) 
            {
                snprintf(szBuffer, sizeof(szBuffer), "field%d=%s", i, kv.value().as<char*>());
                if ( i > 1)
                    buffer += "&";
                buffer = buffer + szBuffer;
                i++;
            }   

            // send this payload to thingspeak
        }

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

private:
    std::vector<std::string> _channelList;

};