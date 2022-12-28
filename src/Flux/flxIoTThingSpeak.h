

#pragma once

#include "flxMQTTESP32.h"

#include "flxFmtJSON.h"
#include "spSpark.h"
#include "flxUtils.h"

#include <map>
#include <sstream>

#define kThingSpeakUpdateTopic "channels/%s/publish"

// simple class to support ThingSpeak output

class flxIoTThingSpeak : public flxMQTTESP32SecureCore<flxIoTThingSpeak>, public flxIWriterJSON
{
  private:
    //---------------------------------------------------------
    std::string get_channelList(void)
    {
        std::string theList = "";

        for (auto it = _deviceToChannel.begin(); it != _deviceToChannel.end(); it++)
        {
            if (theList.length() > 0)
                theList += ", ";

            theList += it->first + "=" + it->second;
        }
        return theList;
    }

    //---------------------------------------------------------
    void set_channelList(std::string theList)
    {
        _deviceToChannel.clear();

        std::stringstream s_stream(theList); // create string stream from the string
        std::string keyvalue;
        std::string::size_type sz;

        while (s_stream.good())
        {
            getline(s_stream, keyvalue, ','); // get first string delimited by comma

            // Get the key, value pair

            sz = keyvalue.find('=');

            if (sz == std::string::npos)
            {
                flxLog_W(F("%s:Invalid Device=Channel ID : %s"), name(), keyvalue.c_str());
                continue;
            }
            _deviceToChannel[flx_utils::strtrim(keyvalue.substr(0, sz))] = flx_utils::strtrim(keyvalue.substr(sz + 1));
        }
    }

  public:
    flxIoTThingSpeak()
    {
        setName("ThingSpeak MQTT", "Connection to ThingSpeak");

        spRegister(deviceList, "Channels", "Comma separated list of <device name>=<thingspeak channel ID");

        flux.add(this);
    }

    void write(JsonDocument &jsonDoc)
    {
        // loop over our channels and see if they are in the document
        JsonObject jObj;
        std::string buffer;
        char szBuffer[64];
        char szTopicBuffer[24 + sizeof(kThingSpeakUpdateTopic)];

        int i;
        for (auto it = _deviceToChannel.begin(); it != _deviceToChannel.end(); it++)
        {
            jObj = jsonDoc[it->first];
            if (jObj.isNull())
                continue;

            // Thingspeak channel values are sent as "field<n>=<value>&field<n+1>=<value>..."
            buffer = "";
            i = 1;
            std::string value;
            for (JsonPair kv : jObj)
            {
                // we need to convert the JSON value to string for transport
                if (kv.value().is<float>())
                    value = flx_utils::to_string(kv.value().as<float>());
                else if (kv.value().is<const char *>())
                    value = kv.value().as<const char *>();
                else if (kv.value().is<signed int>())
                    value = flx_utils::to_string(kv.value().as<signed int>());
                else if (kv.value().is<unsigned int>())
                    value = flx_utils::to_string(kv.value().as<unsigned int>());
                else if (kv.value().is<unsigned int>())
                    value = flx_utils::to_string(kv.value().as<unsigned int>());
                else
                {
                    flxLog_W(F("%s: Unknown data field type"), name());
                    continue;
                }

                snprintf(szBuffer, sizeof(szBuffer), "field%d=%s", i, value.c_str());
                if (i > 1)
                    buffer += "&";
                buffer = buffer + szBuffer;
                i++;
            }
            snprintf(szTopicBuffer, sizeof(szTopicBuffer), kThingSpeakUpdateTopic, it->second.c_str());
            topic = szTopicBuffer;

            // Topic is based on the channel...

            // send this payload to thingspeak
            flxMQTTESP32SecureCore::write(buffer.c_str(), false);
        }
    }

    // TODO VISIT
    bool initialize(void)
    {

        return flxMQTTESP32SecureCore::initialize();
    }

    // Device=Channel ID property
    flxPropertyRWString<flxIoTThingSpeak, &flxIoTThingSpeak::get_channelList, &flxIoTThingSpeak::set_channelList> deviceList;

  private:
    std::map<std::string, std::string> _deviceToChannel;
};