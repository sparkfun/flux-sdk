

#pragma once

#ifdef ESP32


#include "spSpark.h"
#include "spCoreInterface.h"
#include "spNetwork.h"

#include <ArduinoMqttClient.h>
#include <WiFiClientSecure.h>

// A General MQTT client for the framework - for use on the ESP32
template <class Object, typename CLIENT>
class spMQTTESP32Base : public spActionType<Object>, public spWriter
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
            spLog_I_(F("Connecting to MQTT endpoint %s:%u ..."), server().c_str(), port());
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
    spMQTTESP32Base() : _isEnabled{true}, _theNetwork{nullptr}, _mqttClient(_wifiClient), 
        _txBufferSize{0}, _dynamicBufferSize{0}
    {
        spRegister(enabled, "Enabled", "Enable or Disable the MQTT Client");

        spRegister(port, "Port", "The MQTT broker port to connect to.");
        spRegister(server, "Server", "The MQTT server to connect to.");
        spRegister(topic, "MQTT Topic", "The MQTT topic to publish to.");
        spRegister(clientName, "Client Name", "Name of this device used for MQTT Communications");

        spRegister(username, "Username", "Username to connect to an MQTT broker, if required");
        spRegister(password, "Password", "Password to connect to an MQTT broker, if required");

        spRegister(bufferSize, "Buffer Size", "MQTT payload buffer size. If 0, the buffer size is dynamic");

        this->setName("MQTT Client", "A MQTT Client for ESP32 Systems");

        spark.add(this);
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
        _mqttClient.setConnectionTimeout(5 * 1000);

        // Username/password provided?

        if (username().length() > 0 && password().length() > 0)
            _mqttClient.setUsernamePassword(username().c_str(), password().c_str());

        // Connect

        for( int i=0; !_mqttClient.connect(server().c_str(), port()); i++)
        {
            if (i > 4)
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

    // for the Writer interface
    void write(int data)
    {
        // noop
    }
    void write(float data)
    {
        //noop
    }
    //----------------------------------------------------------------------------
    // spWriter interface method
    void write(const char * value, bool newline)
    {
        // if we are not connected, ignore
        if (!connected() || !value)
            return;

        // do we have a topic?
        if ( topic().length() == 0 )
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
    spPropertyRWUint16<spMQTTESP32Base, &spMQTTESP32Base::get_bufferSize, &spMQTTESP32Base::set_bufferSize> bufferSize = {0};


    // username and password properties - some brokers requires this 
    spPropertyString<spMQTTESP32Base> username;
    spPropertySecureString<spMQTTESP32Base> password;

  protected:
    CLIENT  _wifiClient;

  private:
    bool _isEnabled;

    spNetwork *_theNetwork;

    MqttClient  _mqttClient;

    uint16_t    _txBufferSize;
    uint16_t    _dynamicBufferSize;
};


class spMQTTESP32: public spMQTTESP32Base<spMQTTESP32, WiFiClient>
{};

class spMQTTESP32Secure: public spMQTTESP32Base<spMQTTESP32Secure, WiFiClientSecure>
{

public:

    spMQTTESP32Secure()
    {
        spRegister(caCertificate, "CA Certificate", 
                   "The Certificate Authority certificate. If set, the connection is secure");
        spRegister(clientCertificate, "Client Certificate", "The certificate for the client connection");
        spRegister(clientKey, "Client Key", "The secure key used for client verification");

        this->setName("MQTT Secure Client", "A MQTT Secure Client for ESP32 Systems");
    }

    bool connect(void);

    // Security certs/keys
    spPropertySecureString<spMQTTESP32Secure> caCertificate;
    spPropertySecureString<spMQTTESP32Secure> clientCertificate;
    spPropertySecureString<spMQTTESP32Secure> clientKey;
};

#endif