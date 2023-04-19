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

#pragma once

#include "flxIoTHTTP.h"

#include "flxFlux.h"
#include "flxFmtJSON.h"
#include "flxUtils.h"

#include <time.h>
#include <string.h>

#define kOutputBufferSize 2500

// simple class to support MachineChat output

class flxIoTMachineChat : public flxIoTHTTPBase<flxIoTMachineChat>, public flxIWriterJSON
{
  public:
    flxIoTMachineChat()
    {
        setName("MachineChat", "Connection to MachineChat");

        flux.add(this);
    }

    void write(JsonDocument &jsonDoc)
    {

        if (!enabled())
            return;

        // no URL, no dice
        if (URL().length() == 0)
        {
            flxLog_E(F("No URL provided for the MachineChat Server"));
            return;
        }

        // We need time for the timestamp, TODO - make this a util function ...
        char szBuffer[64];
        memset(szBuffer, '\0', sizeof(szBuffer));

        time_t t_now;
        time(&t_now);
        struct tm *tmLocal = localtime(&t_now);

        strftime(szBuffer, sizeof(szBuffer), "%G-%m-%dT%T", tmLocal);


        time_t t_gmt = mktime(gmtime(&t_now));
        int deltaT = t_now - t_gmt;

        char chSign;
        if (deltaT < 0)
        {
            chSign = '-';
            deltaT *= -1;
        }else 
            chSign = '+';

        char szTmp[24] = {0};

        int tz_hrs = deltaT / 3600;
        int tz_min = (deltaT % 3600) / 60;

        snprintf(szTmp, sizeof(szTmp), "%c%02d:%02d", chSign, tz_hrs, tz_min);

        strlcat(szBuffer, szTmp, sizeof(szBuffer));


        StaticJsonDocument<kOutputBufferSize> jsonOutput;

        JsonObject jsonContext;
        JsonObject jObj = jsonDoc.as<JsonObject>();

        std::string strOutput;

        for (JsonPair kv : jObj)
        {
            if (!kv.value().is<JsonObject>())
                continue;

            // okay, we have a sensor reading.
            jsonOutput.clear();
            jsonContext = jsonOutput.createNestedObject("context");
            jsonContext["target_id"] = kv.key();
            jsonContext["timestamp"] = szBuffer;

            jsonOutput["data"] = kv.value();

            strOutput.clear();
            serializeJson(jsonOutput, strOutput);

            // post to machine chat.
            flxIoTHTTPBase<flxIoTMachineChat>::write(strOutput.c_str(), false);
        }
    }
};