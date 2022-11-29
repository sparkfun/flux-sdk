

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

    // changing state ...
    // TODO - Should this control connection state?

    _isEnabled = bEnabled;

    // if ( _isEnabled)
    //     (void)start();
    // else
    //     stop();
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

    // if (bConnected)
    //     start();
    // else
    //     stop();
}

void spMQTTESP32::disconnect(void)
{
    if (_mqttClient && _mqttClient->connected() != 0)
        _mqttClient->stop();

    if (_wifiClient && _wifiClient->connected() != 0)
        _wifiClient->stop();
}

bool spMQTTESP32::connect(void)
{

    if (!_isEnabled)
        return false;

    if (_wifiClient && _wifiClient->connected() != 0 && _mqttClient && _mqttClient->connected() != 0)
        return true;

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
        spLog_E(F("%s: MQTT connection failed. Error: %d"), _mqttClient->connectError());
        return false;
    }

    // we're connected
    return true;
}

#endif