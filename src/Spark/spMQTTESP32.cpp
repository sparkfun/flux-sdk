

#ifdef ESP32

#include "spMQTTESP32.h"

#define kMQTTClientBufferSize 1024
#define kMQTTConnectionTries 10

//----------------------------------------------------------------
// Enabled Property setter/getters
void spMQTTESP32::set_isEnabled(bool bEnabled)
{
    // Any changes?
    if (_isEnabled == bEnabled)
        return;

    _isEnabled = bEnabled;

    if (_isEnabled)
        (void)connect();
    else
        disconnect();
}

//----------------------------------------------------------------
bool spMQTTESP32::get_isEnabled(void)
{
    return _isEnabled;
}

// Event callback
//----------------------------------------------------------------------------
void spMQTTESP32::onConnectionChange(bool bConnected)
{
    // Are we enabled ...
    if (!_isEnabled)
        return;

    if (bConnected)
    {
        spLog_I(F("Connected to MQTT endpoint %s ..."), clientName().c_str());
        if (connect())
            spLog_I(F("\tConnected"));
        else
            spLog_E(F("\tFailed to connect"));
    }
    else
    {
        spLog_I(F("Disconnecting from MQTT endpoint %s"), clientName().c_str());
        disconnect();
    }
}
//----------------------------------------------------------------------------

bool spMQTTESP32::connected()
{
    return (_isEnabled && _wifiClient && _wifiClient->connected() != 0 && _mqttClient && _mqttClient->connected() != 0);
}

//----------------------------------------------------------------------------
void spMQTTESP32::disconnect(void)
{
    if (_mqttClient && _mqttClient->connected() != 0)
        _mqttClient->stop();

    if (_wifiClient && _wifiClient->connected() != 0)
        _wifiClient->stop();
}

//----------------------------------------------------------------------------
bool spMQTTESP32::connect(void)
{

    if (!connected())
        return false;

    if (!_wifiClient)
    {
        _wifiClient = new WiFiClientSecure;
        if (!_wifiClient)
        {
            spLog_E(F("%s: Unable to allocate a WiFi client."), name());
            return false;
        }
    }
    // Is this a secure connection?
    if (caCertificate().length() == 0)
        _wifiClient->setInsecure();
    else
    {
        _wifiClient->setCACert(caCertificate().c_str());
        if (clientCertificate().length() > 0)
            _wifiClient->setCertificate(clientCertificate().c_str());

        if (clientKey().length() > 0)
            _wifiClient->setPrivateKey(clientKey().c_str());
    }

    // do we have all the parameters we need?
    if (clientName().length() == 0)
    {
        spLog_E(F("%s : No Thing/Client name set. Unable to connect"), name());
        return false;
    }
    if (server().length() == 0)
    {
        spLog_E(F("%s : No server/endpoint set. Unable to connect"), name());
        return false;
    }
    if (port() < 1024)
    {
        spLog_E(F("%s : A valid port is not set %d. Unable to connect"), name(), port());
        return false;
    }

    // mqtt time

    if (!_mqttClient)
    {
        _mqttClient = new MqttClient(_wifiClient);

        if (!_mqttClient)
        {
            spLog_E(F("%s: Unable to create a MQTT connection."), name());
            return false;
        }
    }
    // setup mqtt client
    _mqttClient->setId(clientName().c_str());
    _mqttClient->setKeepAliveInterval(60 * 1000);
    _mqttClient->setConnectionTimeout(5 * 1000);

    // Connect
    if (!_mqttClient->connect(server().c_str(), port()))
    {
        spLog_E(F("%s: MQTT connection failed. Error Code: %d"), _mqttClient->connectError());
        return false;
    }

    // we're connected
    return true;
}

#endif