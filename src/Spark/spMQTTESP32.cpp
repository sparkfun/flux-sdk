



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
    if (_mqttClient.connected())
        _mqttClient.disconnect();

    if (_wifiClient.connected() != 0)
        _wifiClient.stop();

    _isConnected = false;

}

bool spMQTTESP32::connect(void)
{

    if (!_isEnabled)
        return false;

    if (_isConnected)
        return true;

    // Is this a secure connection?
    if (caCertificate().length() == 0)
        _wifiClient.setInsecure();
    else
    {
        _wifiClient.setCACert(caCertificate().c_str());
        if (clientCertificate().length() > 0)
            _wifiClient.setCertificate(clientCertificate().c_str());

        if (clientKey().length() > 0)
            _wifiClient.setPrivateKey(clientKey().c_str());
    }

    // mqtt time
    _mqttClient.begin(server().c_str(), port(), _wifiClient);

    bool connected = false;
    for (int i =0; i <  kMQTTConnectionTries; i ++)
    {
        connected = _mqttClient.connect(clientName().c_str());
        if (connected)
            break;

        delay(100);
    }

    if (!connected)
    {
        spLog_E("Error connected to MQTT broker: %s, %u", server().c_str(), port());

        disconnect();
        return false;
    }
    // we're connected 
    return true;
}

#endif