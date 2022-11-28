

#pragma once

#ifdef ESP32

#include "spSpark.h"
#include "spNetwork.h"

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

// A General MQTT client for the framework - for use on the ESP32

class spMQTTESP32 : public spActionType<spMQTTESP32>
{
private:
    void set_isEnabled(bool bEnabled);
    bool get_isEnabled(void);

    // Event callback 
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected);

public:
    spMQTTESP32() : _isEnabled{true}, _theNetwork{nullptr}, _isConnected{false}
    {
        spRegister(enabled, "Enabled", "Enable or Disable the MQTT Client");

        spRegister(port, "Port", "The MQTT broker port to connect to.");
        spRegister(server, "Server", "The MQTT server to connect to.");
        spRegister(topic, "MQTT Topic", "The MQTT topic to publish to.");
        spRegister(clientName, "Client Name", "Name of this device used for MQTT Communications");

        spRegister(caCertificate, "CA Certificate", "The Certificate Authority certificate. If set, the connection is secure");
        spRegister(clientCertificate, "Client Certificate", "The certificate for the client connection");
        spRegister(clientKey, "Client Key", "The secure key used for client verficiation");

        setName("MQTT Client", "A MQTT Client for ESP32 Systems");

        spark.add(this);
    };

    ~spMQTTESP32()
    {
        disconnect();
    }
    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(spSignalBool &theEvent)
    {
        // Regisgter to get notified on connection changes
        theEvent.call(this, &spMQTTESP32::onConnectionChange);
    }

    void setNetwork(spNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    
    bool connect();
    void disconnect();
    // Properties 

    // Enabled/Disabled
    spPropertyRWBool<spMQTTESP32, &spMQTTESP32::get_isEnabled, &spMQTTESP32::set_isEnabled> enabled;

    spPropertyUint<spMQTTESP32>         port = {1883}; // default mqtt port
    spPropertyString<spMQTTESP32>       server;
    spPropertyString<spMQTTESP32>       topic;
    spPropertyString<spMQTTESP32>       clientName;

    // Security certs/keys
    spPropertySecureString<spMQTTESP32> caCertificate;
    spPropertySecureString<spMQTTESP32> clientCertificate;
    spPropertySecureString<spMQTTESP32> clientKey;


private:
	bool  _isEnabled;

    spNetwork *_theNetwork;

    bool _isConnected;

    WiFiClientSecure  _wifiClient;

    MQTTClient _mqttClient;
};

#endif