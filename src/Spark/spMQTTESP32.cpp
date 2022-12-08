

#ifdef ESP32

#include "spMQTTESP32.h"


//-----------------------------------------------------------
spMQTTESP32Secure::spMQTTESP32Secure() : _pCACert{nullptr}, _pClientCert{nullptr}, 
            _pClientKey{nullptr}, _fileSystem{nullptr}
{
    spRegister(caCertificate, "CA Certificate",
               "The Certificate Authority certificate. If set, the connection is secure");
    spRegister(clientCertificate, "Client Certificate", "The certificate for the client connection");
    spRegister(clientKey, "Client Key", "The secure key used for client verification");


    spRegister(caCertFilename, "CA Cert Filename", "The File to load the certificate from");
    spRegister(clientCertFilename, "Client Cert Filename", "The File to load the client certificate from");
    spRegister(clientKeyFilename, "Client Key Filename", "The File to load the client key from");

    this->setName("MQTT Secure Client", "A MQTT Secure Client for ESP32 Systems");
}

//-----------------------------------------------------------
spMQTTESP32Secure::~spMQTTESP32Secure() 
{
    if (_pCACert != nullptr)
        delete _pCACert;

    if (_pClientCert != nullptr)
        delete _pClientCert;

    if (_pClientKey != nullptr)
        delete _pClientKey;
}
//-----------------------------------------------------------

std::string spMQTTESP32Secure::get_caCert(void)
{

    std::string tmp = _pCACert ? _pCACert : "";
    return tmp;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_caCert(std::string theCert)
{
    if (_pCACert != nullptr)
    {
        delete _pCACert;
        _pCACert = nullptr;
    }

    if (theCert.length() > 0)
        _pCACert = strdup(theCert.c_str());
}
//-----------------------------------------------------------
std::string spMQTTESP32Secure::get_clientCert(void)
{
    std::string tmp = _pClientCert ? _pClientCert : "";
    return tmp;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_clientCert(std::string theCert)
{
    if (_pClientCert != nullptr)
    {
        delete _pClientCert;
        _pClientCert = nullptr;
    }

    if (theCert.length() > 0)
        _pClientCert = strdup(theCert.c_str());
}
//-----------------------------------------------------------
std::string spMQTTESP32Secure::get_clientKey(void)
{
    std::string tmp = _pClientKey ? _pClientKey : "";
    return tmp;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_clientKey(std::string theCert)
{
    if (_pClientKey != nullptr)
    {
        delete _pClientKey;
        _pClientKey = nullptr;
    }

    if (theCert.length() > 0)
        _pClientKey = strdup(theCert.c_str());
}

//----------------------------------------------------------------------------
bool spMQTTESP32Secure::connect(void)
{
    // Already connected?
    if (connected())
        return true;

    if (_pCACert != nullptr)
        _wifiClient.setCACert(_pCACert);

    if (_pClientCert != nullptr)
        _wifiClient.setCertificate(_pClientCert);

    if (_pClientKey != nullptr)
        _wifiClient.setPrivateKey(_pClientKey);

    return spMQTTESP32Base::connect();
}

//-----------------------------------------------------------
char * spMQTTESP32Secure::loadCertFile(std::string &theFile)
{

    if (!_fileSystem || theFile.length() < 1)
        return nullptr;

    if (!_fileSystem->exists(theFile.c_str()) )
    {
        spLog_E(F("Certificate file does not exist: %s"), theFile.c_str());
        return nullptr;
    }

    spFSFile certFile =  _fileSystem->open(theFile.c_str(), spIFileSystem::kFileRead);
    if (!certFile)
    {
        spLog_E(F("Unable to load certificate file: %s"), theFile.c_str());
        return nullptr;
    }

    size_t szFile = certFile.size();
    if (szFile < 1)
    {
        certFile.close();
        spLog_E(F("Unable to load certificate file: %s"), theFile.c_str());
        return nullptr;
    }
    uint8_t * pCert = new uint8_t[szFile+1];

    if (!pCert)
    {
        certFile.close();        
        spLog_E(F("Unable to allocate certificate memory: %s"), theFile.c_str());
        return nullptr;
    }

    size_t szRead = certFile.read(pCert, szFile);

    certFile.close(); // we are done with the file.
    
    if (szFile != szRead) 
    {
        spLog_W(F("Error reading certificate file - size mismatch: %s"), theFile.c_str());
        delete pCert;
        return nullptr;
    }
    // null terminate the string
    pCert[szFile]='\0';

    return (char*)pCert;
}

//-----------------------------------------------------------

std::string spMQTTESP32Secure::get_caCertFilename(void)
{
    return _caFilename;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_caCertFilename(std::string theFile)
{
    char * pCert = loadCertFile(theFile);

    if (!pCert)
        return;

    if ( _pCACert != nullptr )
        delete _pCACert;

    _pCACert = pCert;

    _caFilename = theFile;
}
//-----------------------------------------------------------
std::string spMQTTESP32Secure::get_clientCertFilename(void)
{
    return _clientFilename;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_clientCertFilename(std::string theFile)
{
    char * pCert = loadCertFile(theFile);

    if (!pCert)
        return;

    if ( _pClientCert != nullptr)
        delete _pClientCert;

    _pClientCert = pCert;

    _clientFilename = theFile;
}
//-----------------------------------------------------------
std::string spMQTTESP32Secure::get_clientKeyFilename(void)
{
    return _keyFilename;
}
//-----------------------------------------------------------
void spMQTTESP32Secure::set_clientKeyFilename(std::string theFile)
{
    char * pCert = loadCertFile(theFile);

    if (!pCert)
        return;

    if ( _pClientKey != nullptr)
        delete _pClientKey;

    _pClientKey = pCert;

    _keyFilename = theFile;
}
#endif