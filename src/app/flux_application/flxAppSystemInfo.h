/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */
#pragma once

#include "flxApplication.h"

#include <Flux/flxCoreLog.h>
#include <Flux/flxCoreParam.h>
#include <Flux/flxPlatform.h>
#include <Flux/flxUtils.h>

class flxAppSystemInfo : public flxOperation
{
  private:
#if defined(CONFIG_FLUX_WIFI)
    std::string get_wifi_ssid(void)
    {
        std::string sTmp;

        if (_pApplication && _pApplication->_wifiConnection.enabled())
            sTmp = _pApplication->_wifiConnection.connectedSSID().c_str();

        return sTmp;
    }

    uint8_t get_wifi_rssi(void)
    {
        if (!_pApplication || !_pApplication->_wifiConnection.enabled())
            return 0;

        return _pApplication->_wifiConnection.RSSI();
    }
#endif
    uint32_t get_uptime(void)
    {
        return millis();
    }

#if defined(CONFIG_FLUX_SDMMCARD)
    uint32_t get_sdfree(void)
    {
        if (!_pApplication || !_pApplication->_theSDCard.enabled())
            return 0;

        return _pApplication->_theSDCard.total() - _pApplication->_theSDCard.used();
    }
#endif
    uint32_t get_heap(void)
    {
        return flxPlatform::heap_free();
    }

  public:
    flxAppSystemInfo() : _pApplication{nullptr}
    {
        setName("System Info", "Operating information for the DataLogger");

#if defined(CONFIG_FLUX_WIFI)
        flxRegister(wifiSSID, "SSID", "Current WiFi SSID");
        flxRegister(wifiRSSI, "RSSI", "Current WiFi RSSI");
#endif
        flxRegister(systemUptime, "Uptime", "System Uptime in MS");
        flxRegister(systemHeap, "Heap", "Heap free size");
#if defined(CONFIG_FLUX_SDMMCARD)
        flxRegister(systemSDFree, "SD Free", "SD Card free space");
#endif
    }

    flxAppSystemInfo(flxApplication *dlApp) : flxAppSystemInfo()
    {
        setApplication(dlApp);
    }

    void setApplication(flxApplication *dlApp)
    {
        _pApplication = dlApp;
    }

    flxParameterOutString<flxAppSystemInfo, &flxAppSystemInfo::get_wifi_ssid> wifiSSID;

    flxParameterOutUInt8<flxAppSystemInfo, &flxAppSystemInfo::get_wifi_rssi> wifiRSSI;

    flxParameterOutUInt32<flxAppSystemInfo, &flxAppSystemInfo::get_uptime> systemUptime;

    // flxParameterOutUInt32<flxAppSystemInfo, &flxAppSystemInfo::get_sdfree> systemSDFree;

    flxParameterOutUInt32<flxAppSystemInfo, &flxAppSystemInfo::get_heap> systemHeap;

  private:
    flxApplication *_pApplication;
};
