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

#include <string.h>
#include <time.h>

#define kOutputBufferSize 1600

// simple class to support MachineChat output

class flxIoTMachineChat : public flxIoTHTTPBase<flxIoTMachineChat>, public flxIWriterJSON
{
  public:
    flxIoTMachineChat() : _isInitalized{false}, _szLocalIP{""}
    {
        setName("Machinechat", "Connection to Machinechat IoT Server");

        flux.add(this);
    }

    void setupTargetIP(void)
    {
        if (!_theNetwork)
            return;

        IPAddress myIP = _theNetwork->localIP();
        snprintf(_szLocalIP, sizeof(_szLocalIP), "%u.%u.%u.%u", myIP[0], myIP[1], myIP[2], myIP[3]);
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

        if (!_isInitalized)
        {
            setupTargetIP();
            _isInitalized = true;
        }

        char szTime[64];
        memset(szTime, '\0', sizeof(szTime));

        time_t t_now;
        time(&t_now);
        flx_utils::timestampISO8601(t_now, szTime, sizeof(szTime), true);

        DynamicJsonDocument jsonOutput(kOutputBufferSize);

        JsonObject jsonContext;
        JsonObject jRoot = jsonDoc.as<JsonObject>();

        std::string strOutput;

        char szName[64];
        char szData[64];

        // loop over top level objects in the output document - these are devices
        for (JsonPair kvObj : jRoot)
        {
            if (!kvObj.value().is<JsonObject>())
                continue;

            // get a proper context name/ID --
            if (!flx_utils::createVariableName(kvObj.key().c_str(), szName))
            {
                flxLog_E(F("%s: Unable to create context id for: %s"), name(), kvObj.key().c_str());
                continue;
            }

            jsonOutput.clear();
            jsonOutput["context"]["target_id"] = szName;
            jsonOutput["context"]["target_ip"] = _szLocalIP;
            jsonOutput["context"]["timestamp"] = szTime;

            // loop over data
            for (JsonPair kvParam : kvObj.value().as<JsonObject>())
            {
                // cleanup the name
                if (!flx_utils::createVariableName(kvParam.key().c_str(), szData))
                {
                    flxLog_E(F("%s: Unable to create data name for: %s"), name(), kvParam.key().c_str());
                    continue;
                }
                jsonOutput["data"][szData] = kvParam.value();
            }

            strOutput.clear();
            serializeJson(jsonOutput, strOutput);

            // post to machine chat.
            flxIoTHTTPBase<flxIoTMachineChat>::write(strOutput.c_str(), false, flxLineTypeData);
        }
    }

  private:
    bool _isInitalized;
    char _szLocalIP[16];
};
