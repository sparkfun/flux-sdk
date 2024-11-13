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

#ifdef ESP32

#include "flxCoreInterface.h"
#include "flxFS.h"
#include "flxFlux.h"
#include "flxNetwork.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// A General HTTP/HTTPS output writer for the framework
//
// Create a template for the HTTP operation. This is then
// used by classes implementing specific HTTP output functionality

// Object -- the name of the class

template <class Object> class flxIoTHTTPBase : public flxActionType<Object>
{
  private:
    bool createWiFiClient(void)
    {
        if (_wifiClient)
            delete _wifiClient;

        _wifiClient = _isSecure ? new WiFiClientSecure : new WiFiClient;

        checkConnectionCert();

        return _wifiClient != nullptr;
    }

    void checkConnectionCert()
    {
        if (_wifiClient != nullptr && _isSecure && _pCACert != nullptr)
            ((WiFiClientSecure *)_wifiClient)->setCACert(_pCACert);
    }

    // Enabled Property setter/getters
    void set_isEnabled(bool bEnabled)
    {
        // Any changes?
        if (_isEnabled == bEnabled)
            return;

        _isEnabled = bEnabled;
    }

    //----------------------------------------------------------------
    bool get_isEnabled(void)
    {
        return _isEnabled;
    }

    //---------------------------------------------------------
    std::string get_caCert(void)
    {

        std::string tmp = _pCACert ? _pCACert : "";
        return tmp;
    }

    //---------------------------------------------------------
    void set_caCert(std::string theCert)
    {
        if (_pCACert != nullptr)
        {
            delete _pCACert;
            _pCACert = nullptr;
        }

        if (theCert.length() > 0)
            _pCACert = strdup(theCert.c_str());
    }

    //---------------------------------------------------------
    std::string get_URL(void)
    {
        return _url;
    }

    //---------------------------------------------------------
    void set_URL(std::string theURL)
    {
        if (theURL.length() < 10)
        {
            flxLogM_E(kMsgErrValueError, this->name(), theURL.c_str());
            return;
        }

        _url = theURL;

        _isSecure = theURL.find("https") != std::string::npos;

        if (!createWiFiClient())
        {
            flxLogM_E(kMsgErrInitialization, this->name(), "WiFi Client");
        }
    }

    //---------------------------------------------------------
    char *loadCertFile(std::string &theFile)
    {

        if (!_fileSystem || theFile.length() < 1)
            return nullptr;

        if (!_fileSystem->exists(theFile.c_str()))
        {
            flxLogM_E(kMsgErrFileOpen, this->name(), theFile.c_str());
            return nullptr;
        }

        flxFSFile certFile = _fileSystem->open(theFile.c_str(), flxIFileSystem::kFileRead);
        if (!certFile)
        {
            flxLogM_E(kMsgErrFileOpen, this->name(), theFile.c_str());
            return nullptr;
        }

        size_t szFile = certFile.size();
        if (szFile < 1)
        {
            certFile.close();
            flxLogM_E(kMsgErrFileOpen, this->name(), theFile.c_str());
            return nullptr;
        }
        uint8_t *pCert = new uint8_t[szFile + 1];

        if (!pCert)
        {
            certFile.close();
            flxLogM_E(kMsgErrAllocErrorN, this->name(), theFile.c_str());
            return nullptr;
        }

        size_t szRead = certFile.read(pCert, szFile);

        certFile.close(); // we are done with the file.

        if (szFile != szRead)
        {
            flxLogM_E(kMsgErrInitialization, this->name(), theFile.c_str());
            delete pCert;
            return nullptr;
        }
        // null terminate the string
        pCert[szFile] = '\0';

        return (char *)pCert;
    }
    //---------------------------------------------------------
    std::string get_caCertFilename(void)
    {
        return _caFilename;
    }

    //---------------------------------------------------------
    void set_caCertFilename(std::string theFile)
    {
        char *pCert = loadCertFile(theFile);

        if (!pCert)
            return;

        if (_pCACert != nullptr)
            delete _pCACert;

        _pCACert = pCert;

        checkConnectionCert();

        _caFilename = theFile;
    }
    // Event callback
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected)
    {

        _canConnect = bConnected;

        // Are we enabled ...
        if (!_isEnabled)
            return;
    }

  public:
    flxIoTHTTPBase()
        : _theNetwork{nullptr}, _isEnabled{false}, _canConnect{false}, _isSecure{false}, _pCACert{nullptr},
          _fileSystem{nullptr}, _wifiClient{nullptr}
    {
        flxRegister(enabled, "Enabled", "Enable or Disable the HTTP Client");

        flxRegister(URL, "URL", "URL to call with log information");

        flxRegister(caCertificate, "CA Certificate", "Certificate Authority certificate. Set to secure connection");

        flxRegister(caCertFilename, "CA Cert Filename", "File to load the certificate from");
    };

    ~flxIoTHTTPBase()
    {
        if (_pCACert != nullptr)
            delete _pCACert;

        if (_wifiClient != nullptr)
            delete _wifiClient;
    }

    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;
        flxRegisterEventCB(flxEvent::kOnConnectionChange, this, &flxIoTHTTPBase::onConnectionChange);
    }

    bool connected()
    {
        return (_isEnabled && _canConnect);
    }

    //----------------------------------------------------------------------------
    // flxWriter interface method
    virtual void write(const char *value, bool newline, flxLineType_t type)
    {
        // if we are not connected, ignore, bad url skip, we want json, so no headers
        if (!_isEnabled || !_canConnect || !value || _url.length() < 10 || type != flxLineTypeData)
            return;

        if (!_wifiClient)
        {
            if (!createWiFiClient())
            {
                flxLogM_E(kMsgErrInitialization, this->name(), "Network Connection");
                return;
            }
        }

        // Connect to server, post data, disconnect

        HTTPClient http;

        if (!http.begin(*_wifiClient, _url.c_str()))
        {
            flxLogM_E(kMsgErrConnectionFailure, this->name(), _url.c_str());
            return;
        }

        http.addHeader("Content-Type", "application/json");

        int rc = http.POST((uint8_t *)value, strlen(value));

        if (rc != 200)
            flxLogM_W(kMsgErrConnectionFailure, this->name(), http.errorToString(rc).c_str());

        http.end();
    }
    //---------------------------------------------------------
    void setFileSystem(flxIFileSystem *fs)
    {
        _fileSystem = fs;
    }

    // Properties

    // Enabled/Disabled
    flxPropertyRWBool<flxIoTHTTPBase, &flxIoTHTTPBase::get_isEnabled, &flxIoTHTTPBase::set_isEnabled> enabled;

    flxPropertyRWString<flxIoTHTTPBase, &flxIoTHTTPBase::get_URL, &flxIoTHTTPBase::set_URL> URL;

    // Security certs/keys
    flxPropertyRWSecretString<flxIoTHTTPBase, &flxIoTHTTPBase::get_caCert, &flxIoTHTTPBase::set_caCert> caCertificate;

    flxPropertyRWString<flxIoTHTTPBase, &flxIoTHTTPBase::get_caCertFilename, &flxIoTHTTPBase::set_caCertFilename>
        caCertFilename;

  protected:
    flxNetwork *_theNetwork;

  private:
    // WiFiClientSecure _wifiClient;

    std::string _caFilename;

    std::string _url;

    bool _isEnabled;
    bool _canConnect;
    bool _isSecure;

    // We need perm version of the keys for the secure connection, so the values are stashed in allocated
    // strings
    char *_pCACert;

    // Filesystem to load a file from
    flxIFileSystem *_fileSystem;

    WiFiClient *_wifiClient;
};

class flxIoTHTTP : public flxIoTHTTPBase<flxIoTHTTP>, public flxWriter
{
  public:
    flxIoTHTTP()
    {
        setName("HTTP IoT", "An HTTP IoT data connector");

        flux.add(this);
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
    //---------------------------------------------------------------------
    virtual void write(const char *value, bool newline, flxLineType_t type)
    {

        flxIoTHTTPBase<flxIoTHTTP>::write(value, false, type);
    }
};
#endif