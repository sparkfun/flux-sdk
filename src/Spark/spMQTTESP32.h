

#pragma once

#ifdef ESP32

#include "spCoreInterface.h"
#include "spFS.h"
#include "spNetwork.h"
#include "spSpark.h"

#include <ArduinoMqttClient.h>
#include <WiFiClientSecure.h>

// A General MQTT client for the framework - for use on the ESP32
template <class Object, typename CLIENT> class spMQTTESP32Base : public spActionType<Object>
{
  private:
    // Enabled Property setter/getters
    void set_isEnabled(bool bEnabled)
    {
        // Any changes?
        if (_isEnabled == bEnabled)
            return;

        _isEnabled = bEnabled;

        // TODO chech network connection availablity ...
        if (_isEnabled)
            (void)connect();
        else
            disconnect();
    }

    //----------------------------------------------------------------
    bool get_isEnabled(void)
    {
        return _isEnabled;
    }
    //----------------------------------------------------------------------------

    void set_bufferSize(uint16_t buffSize)
    {
        if (buffSize > 0)
        {
            _mqttClient.setTxPayloadSize(buffSize);
            _dynamicBufferSize = buffSize;
        }
        _txBufferSize = buffSize;
    }
    //----------------------------------------------------------------------------
    uint16_t get_bufferSize(void)
    {
        return _txBufferSize;
    }

    // Event callback
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected)
    {
        // Are we enabled ...
        if (!_isEnabled)
            return;

        // Anything change?
        if (bConnected == connected())
            return;

        if (bConnected)
        {
            spLog_I_(F("%s: connecting to MQTT endpoint %s:%u ..."), this->name(), server().c_str(), port());
            if (connect())
                spLog_N(F("connected"));
            // the connect method will print out sufficient error messages
        }
        else
        {
            spLog_I(F("Disconnecting from MQTT endpoint %s"), clientName().c_str());
            disconnect();
        }
    }

  public:
    spMQTTESP32Base()
        : _isEnabled{true}, _theNetwork{nullptr}, _mqttClient(_wifiClient), _txBufferSize{0}, _dynamicBufferSize{0}
    {
        spRegister(enabled, "Enabled", "Enable or Disable the MQTT Client");

        spRegister(port, "Port", "The MQTT broker port to connect to.");
        spRegister(server, "Server", "The MQTT server to connect to.");
        spRegister(topic, "MQTT Topic", "The MQTT topic to publish to.");
        spRegister(clientName, "Client Name", "Name of this device used for MQTT Communications");

        spRegister(username, "Username", "Username to connect to an MQTT broker, if required");
        spRegister(password, "Password", "Password to connect to an MQTT broker, if required");

        spRegister(bufferSize, "Buffer Size", "MQTT payload buffer size. If 0, the buffer size is dynamic");
    };

    ~spMQTTESP32Base()
    {
        disconnect();
    }
    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(spSignalBool &theEvent)
    {
        // Register to get notified on connection changes
        theEvent.call(this, &spMQTTESP32Base::onConnectionChange);
    }

    void setNetwork(spNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    bool connected()
    {
        return (_isEnabled && _wifiClient.connected() != 0 && _mqttClient.connected() != 0);
    }

    //----------------------------------------------------------------------------
    void disconnect(void)
    {
        if (_mqttClient.connected() != 0)
            _mqttClient.stop();

        if (_wifiClient.connected() != 0)
            _wifiClient.stop();
    }
    //----------------------------------------------------------------------------
    virtual bool connect(void)
    {
        // Already connected?
        if (connected())
            return true;

        // do we have all the parameters we need?
        if (clientName().length() == 0)
        {
            spLog_E(F("%s : No Thing/Client name set. Unable to connect"), this->name());
            return false;
        }
        if (server().length() == 0)
        {
            spLog_E(F("%s : No server/endpoint set. Unable to connect"), this->name());
            return false;
        }
        if (port() < 1024)
        {
            spLog_E(F("%s : A valid port is not set %d. Unable to connect"), this->name(), port());
            return false;
        }
        // mqtt time

        _mqttClient.setId(clientName().c_str());
        _mqttClient.setKeepAliveInterval(60 * 1000);
        _mqttClient.setConnectionTimeout(20 * 1000);

        // Username/password provided?
        if (username().length() > 0 && password().length() > 0)
            _mqttClient.setUsernamePassword(username().c_str(), password().c_str());

        // Connect
        for (int i = 0; !_mqttClient.connect(server().c_str(), port()); i++)
        {
            if (i > 3)
            {
                spLog_E(F("%s: MQTT connection failed. Error Code: %d"), this->name(), _mqttClient.connectError());
                return false;
            }
            spLog_N_(".");
            delay(400);
        }

        // we're connected
        return true;
    }

    //----------------------------------------------------------------------------
    // spWriter interface method
    virtual void write(const char *value, bool newline)
    {
        // if we are not connected, ignore
        if (!connected() || !value)
            return;

        // do we have a topic?
        if (topic().length() == 0)
        {
            spLog_E(F("%s : No MQTT topic provided."), this->name());
            return;
        }

        // the mqtt object has a limited transmitt buffer size (256) that doesn't adapt,
        // but you can set the size (which performs a malloc and free)
        //
        // Openlog payloads can be large, so if in dynamic mode we keep track of the
        // allocated size and increase when needed if in dynamic buffer size mode ..

        if (_txBufferSize == 0 && _dynamicBufferSize < strlen(value))
        {
            _dynamicBufferSize = strlen(value);
            _mqttClient.setTxPayloadSize(_dynamicBufferSize);
        }

        // send the message
        _mqttClient.beginMessage(topic().c_str());
        _mqttClient.print(value);
        _mqttClient.endMessage();
    }
    // Properties

    // Enabled/Disabled
    spPropertyRWBool<spMQTTESP32Base, &spMQTTESP32Base::get_isEnabled, &spMQTTESP32Base::set_isEnabled> enabled;

    spPropertyUint<spMQTTESP32Base> port = {1883}; // default mqtt port
    spPropertyString<spMQTTESP32Base> server;
    spPropertyString<spMQTTESP32Base> topic;
    spPropertyString<spMQTTESP32Base> clientName;

    // Buffer size property
    spPropertyRWUint16<spMQTTESP32Base, &spMQTTESP32Base::get_bufferSize, &spMQTTESP32Base::set_bufferSize> bufferSize =
        {0};

    // username and password properties - some brokers requires this
    spPropertyString<spMQTTESP32Base> username;
    spPropertySecureString<spMQTTESP32Base> password;

  protected:
    CLIENT _wifiClient;

  private:
    bool _isEnabled;

    spNetwork *_theNetwork;

    MqttClient _mqttClient;

    uint16_t _txBufferSize;
    uint16_t _dynamicBufferSize;
};

class spMQTTESP32 : public spMQTTESP32Base<spMQTTESP32, WiFiClient>, public spWriter
{
public:
    spMQTTESP32()
    {
        this->setName("MQTT Client", "A generic MQTT Client");

        spark.add(this);
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
    void write(const char *value, bool newline)
    {
        spMQTTESP32Base::write(value, newline);
    }
};

template <class Object> class spMQTTESP32SecureCore : public spMQTTESP32Base<Object, WiFiClientSecure>
{

  private:
    // NOTE:
    //   Standard string props are stored in std::string variables.
    //   Because the c_str() method on std::strings returns a value that is temporary,
    ///  and the underlying secure connection is expecting const chars, we use a RW
    //   property for the cert/key strings and stash the values in allocated memory.

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
    std::string get_clientCert(void)
    {
        std::string tmp = _pClientCert ? _pClientCert : "";
        return tmp;
    }

    //---------------------------------------------------------
    void set_clientCert(std::string theCert)
    {
        if (_pClientCert != nullptr)
        {
            delete _pClientCert;
            _pClientCert = nullptr;
        }

        if (theCert.length() > 0)
            _pClientCert = strdup(theCert.c_str());
    }

    //---------------------------------------------------------
    std::string get_clientKey(void)
    {
        std::string tmp = _pClientKey ? _pClientKey : "";
        return tmp;
    }

    //---------------------------------------------------------
    void set_clientKey(std::string theCert)
    {
        if (_pClientKey != nullptr)
        {
            delete _pClientKey;
            _pClientKey = nullptr;
        }

        if (theCert.length() > 0)
            _pClientKey = strdup(theCert.c_str());
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

        _caFilename = theFile;
    }

    //---------------------------------------------------------
    std::string get_clientCertFilename(void)
    {
        return _clientFilename;
    }

    //---------------------------------------------------------
    void set_clientCertFilename(std::string theFile)
    {
        char *pCert = loadCertFile(theFile);

        if (!pCert)
            return;

        if (_pClientCert != nullptr)
            delete _pClientCert;

        _pClientCert = pCert;

        _clientFilename = theFile;
    }


    //---------------------------------------------------------
    std::string get_clientKeyFilename(void)
    {
        return _keyFilename;
    }

    //---------------------------------------------------------
    void set_clientKeyFilename(std::string theFile)
    {
        char *pCert = loadCertFile(theFile);

        if (!pCert)
            return;

        if (_pClientKey != nullptr)
            delete _pClientKey;

        _pClientKey = pCert;

        _keyFilename = theFile;
    }

    //---------------------------------------------------------
    char *loadCertFile(std::string &theFile)
    {

        if (!_fileSystem || theFile.length() < 1)
            return nullptr;

        if (!_fileSystem->exists(theFile.c_str()))
        {
            spLog_E(F("Certificate file does not exist: %s"), theFile.c_str());
            return nullptr;
        }

        spFSFile certFile = _fileSystem->open(theFile.c_str(), spIFileSystem::kFileRead);
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
        uint8_t *pCert = new uint8_t[szFile + 1];

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
        pCert[szFile] = '\0';

        return (char *)pCert;
    }

  public:
    spMQTTESP32SecureCore() : _pCACert{nullptr}, _pClientCert{nullptr}, _pClientKey{nullptr}, _fileSystem{nullptr}
    {
        spRegister(caCertificate, "CA Certificate",
                   "The Certificate Authority certificate. If set, the connection is secure");
        spRegister(clientCertificate, "Client Certificate", "The certificate for the client connection");
        spRegister(clientKey, "Client Key", "The secure key used for client verification");

        spRegister(caCertFilename, "CA Cert Filename", "The File to load the certificate from");
        spRegister(clientCertFilename, "Client Cert Filename", "The File to load the client certificate from");
        spRegister(clientKeyFilename, "Client Key Filename", "The File to load the client key from");
    }

    //---------------------------------------------------------
    ~spMQTTESP32SecureCore()
    {
        if (_pCACert != nullptr)
            delete _pCACert;

        if (_pClientCert != nullptr)
            delete _pClientCert;

        if (_pClientKey != nullptr)
            delete _pClientKey;
    }

    //---------------------------------------------------------
    bool connect(void)
    {
        // Already connected?
        if (spMQTTESP32Base<Object, WiFiClientSecure>::connected())
            return true;

        if (_pCACert != nullptr)
            spMQTTESP32Base<Object, WiFiClientSecure>::_wifiClient.setCACert(_pCACert);

        if (_pClientCert != nullptr)
            spMQTTESP32Base<Object, WiFiClientSecure>::_wifiClient.setCertificate(_pClientCert);

        if (_pClientKey != nullptr)
            spMQTTESP32Base<Object, WiFiClientSecure>::_wifiClient.setPrivateKey(_pClientKey);

        return spMQTTESP32Base<Object, WiFiClientSecure>::connect();
    }

    //---------------------------------------------------------
    void setFileSystem(spIFileSystem *fs)
    {
        _fileSystem = fs;
    }

    // Security certs/keys
    spPropertyRWSecureString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_caCert,
                             &spMQTTESP32SecureCore::set_caCert>
        caCertificate;

    spPropertyRWSecureString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_clientCert,
                             &spMQTTESP32SecureCore::set_clientCert>
        clientCertificate;

    spPropertyRWSecureString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_clientKey,
                             &spMQTTESP32SecureCore::set_clientKey>
        clientKey;

    // Define filename properties to access the secure keys. A filesystem must be provided to this object for it to read
    // the data.
    // Security certs/keys
    spPropertyRWString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_caCertFilename,
                       &spMQTTESP32SecureCore::set_caCertFilename>
        caCertFilename;
    spPropertyRWString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_clientCertFilename,
                       &spMQTTESP32SecureCore::set_clientCertFilename>
        clientCertFilename;
    spPropertyRWString<spMQTTESP32SecureCore, &spMQTTESP32SecureCore::get_clientKeyFilename,
                       &spMQTTESP32SecureCore::set_clientKeyFilename>
        clientKeyFilename;

    // We need perm version of the keys for the secure connection, so the values are stashed in allocated
    // strings
    char *_pCACert;
    char *_pClientCert;
    char *_pClientKey;

    // Filesystem to load a file from
    spIFileSystem *_fileSystem;

    std::string _caFilename;
    std::string _clientFilename;
    std::string _keyFilename;
};


class spMQTTESP32Secure : public spMQTTESP32SecureCore<spMQTTESP32Secure>, public spWriter
{
public:
    spMQTTESP32Secure()
    {
        this->setName("MQTT Secure Client", "A secure MQTT client");

        spark.add(this);
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
    void write(const char *value, bool newline)
    {
        spMQTTESP32Base::write(value, newline);
    }
};
#endif