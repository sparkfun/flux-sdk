

#pragma once

#ifdef ESP32

#include "flxCoreInterface.h"
#include "flxFS.h"
#include "flxNetwork.h"
#include "flxFlux.h"

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

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

        _wifiClient = _isSecure ?  new WiFiClientSecure : new WiFiClient;

        checkConnectionCert();

        return _wifiClient != nullptr;
    }

    void checkConnectionCert()
    {
        if (_wifiClient != nullptr && _isSecure && _pCACert != nullptr)
            ((WiFiClientSecure*)_wifiClient)->setCACert(_pCACert);
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
        if (theURL.length() < 10){
            flxLog_E(F("%s: Invalid URL - failed to parse protocol: %s"), this->name(), theURL.c_str());
            return;
        }

        _url = theURL;

        _isSecure = theURL.find("https") != std::string::npos;

        if (!createWiFiClient())
        {
            flxLog_E(F("%s : Error creating a wifi network client connection"), this->name());
        }
    }

    //---------------------------------------------------------
    char *loadCertFile(std::string &theFile)
    {

        if (!_fileSystem || theFile.length() < 1)
            return nullptr;

        if (!_fileSystem->exists(theFile.c_str()))
        {
            flxLog_E(F("Certificate file does not exist: %s"), theFile.c_str());
            return nullptr;
        }

        flxFSFile certFile = _fileSystem->open(theFile.c_str(), flxIFileSystem::kFileRead);
        if (!certFile)
        {
            flxLog_E(F("Unable to load certificate file: %s"), theFile.c_str());
            return nullptr;
        }

        size_t szFile = certFile.size();
        if (szFile < 1)
        {
            certFile.close();
            flxLog_E(F("Unable to load certificate file: %s"), theFile.c_str());
            return nullptr;
        }
        uint8_t *pCert = new uint8_t[szFile + 1];

        if (!pCert)
        {
            certFile.close();
            flxLog_E(F("Unable to allocate certificate memory: %s"), theFile.c_str());
            return nullptr;
        }

        size_t szRead = certFile.read(pCert, szFile);

        certFile.close(); // we are done with the file.

        if (szFile != szRead)
        {
            flxLog_W(F("Error reading certificate file - size mismatch: %s"), theFile.c_str());
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
    flxIoTHTTPBase() : _isEnabled{false}, _canConnect{false}, _isSecure{false},
                     _theNetwork{nullptr}, _pCACert{nullptr}, _fileSystem{nullptr},
                     _wifiClient{nullptr}
    {
        spRegister(enabled, "Enabled", "Enable or Disable the MQTT Client");

        spRegister(URL, "URL", "The URL to call with log information");

        spRegister(caCertificate, "CA Certificate",
                   "The Certificate Authority certificate. If set, the connection is secure");

        spRegister(caCertFilename, "CA Cert Filename", "The File to load the certificate from");
    };

    ~flxIoTHTTPBase()
    {
        if (_pCACert != nullptr)
            delete _pCACert;

        if (_wifiClient != nullptr)
            delete _wifiClient;
    }
    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(flxSignalBool &theEvent)
    {
        // Register to get notified on connection changes
        theEvent.call(this, &flxIoTHTTPBase::onConnectionChange);
    }

    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    bool connected()
    {
        return (_isEnabled && _canConnect);
    }
    
    //----------------------------------------------------------------------------
    // flxWriter interface method
    virtual void write(const char *value, bool newline)
    {
        // if we are not connected, ignore
        if (!_isEnabled || !_canConnect || !value || _url.length() < 10)
            return;

        if (!_wifiClient)
        {
            if (!createWiFiClient())
            {
                flxLog_E(F("%s: Error creating network connection."), this->name());
                return;
            }
        }

        // Connect to server, post data, disconnnect

        HTTPClient http; 

        if (!http.begin(*_wifiClient, _url.c_str()))
        {
            flxLog_E(F("%s: Error reaching URL: %s"), this->name(), _url.c_str());
            return;
        }

        http.addHeader("Content-Type", "application/json");

        int rc = http.POST(value);

        if (rc != 200)
            flxLog_W(F("%s: Error [%s] posting to: %s"), this->name(),
                        http.errorToString(rc).c_str(), _url.c_str());

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

  private:
    // WiFiClientSecure _wifiClient;

    std::string _caFilename;

    std::string _url;

    bool _isEnabled;
    bool _canConnect;
    bool _isSecure;

    flxNetwork *_theNetwork;

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
    virtual void write(const char *value, bool newline)
    {

        flxIoTHTTPBase<flxIoTHTTP>::write(value, false);
    }
};
#endif