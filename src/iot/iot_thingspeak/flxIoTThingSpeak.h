/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include "flxMQTTESP32.h"

#include "flxFlux.h"
#include "flxFmtJSON.h"
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
        std::string newValue;

        while (s_stream.good())
        {
            getline(s_stream, keyvalue, ','); // get first string delimited by comma

            // Get the key, value pair

            sz = keyvalue.find('=');

            if (sz == std::string::npos)
            {
                flxLogM_W(kMsgErrValueError, name(), "Device=Channel ID");
                continue;
            }
            newValue = flx_utils::strtrim(keyvalue.substr(sz + 1));
            // no dups for values (channel ids)
            for (auto it = _deviceToChannel.begin(); it != _deviceToChannel.end(); it++)
            {
                if (it->second == newValue)
                {
                    flxLogM_W(kMsgErrValueError, name(), "Duplicate ThingStream channel ID [%s], skipping",
                              keyvalue.c_str());
                    continue;
                }
            }
            _deviceToChannel[flx_utils::strtrim(keyvalue.substr(0, sz))] = newValue;
        }
    }

  public:
    flxIoTThingSpeak()
    {
        setName("ThingSpeak MQTT", "Connection to ThingSpeak");

        flxRegister(deviceList, "Channels", "Comma separated list of <device name>=<channel ID>");

        // The topic is auto-generated -- it needs/uses the channel ID. So, let's hide the topic property it from the
        // user.
        hideProperty(topic);

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
            {
                flxLog_W(F("ThingSpeak - no channel id found for device: %s. Check the Channel setting"), it->first);
                continue;
            }

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
                    flxLogM_W(kMsgErrValueError, name(), "Unknown type");
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
            flxMQTTESP32SecureCore::write(buffer.c_str(), false, flxLineTypeData);
        }
    }

    // TODO VISIT
    bool initialize(void)
    {

        return flxMQTTESP32SecureCore::initialize();
    }

    // Device=Channel ID property
    flxPropertyRWString<flxIoTThingSpeak, &flxIoTThingSpeak::get_channelList, &flxIoTThingSpeak::set_channelList>
        deviceList;

  private:
    std::map<std::string, std::string> _deviceToChannel;
};