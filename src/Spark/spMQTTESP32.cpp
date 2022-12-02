

#ifdef ESP32

#include "spMQTTESP32.h"

#define kMaxConnectionTries 3
#define kConnectionDelayMS 700

//----------------------------------------------------------------
// Enabled Property setter/getters
void spMQTTESP32::set_isEnabled(bool bEnabled)
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

    // Anything change?
    if (bConnected == connected())
        return; 

    if (bConnected)
    {
        spLog_I_(F("Connecting to MQTT endpoint %s:%u ..."), server().c_str(), port());
        if (connect())
            spLog_N(F("Connected"));
        // the connect method will print out sufficient error messages 
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
//    return (_isEnabled && _wifiClient.connected() != 0 && _mqttClient && _mqttClient->connected() != 0);
    return (_isEnabled && _wifiClient.connected() != 0 && _mqttClient.connected() != 0);    
}

//----------------------------------------------------------------------------
void spMQTTESP32::disconnect(void)
{
    if (_mqttClient.connected() != 0)
        _mqttClient.stop();

    if (_wifiClient.connected() != 0)
        _wifiClient.stop();
}

//----------------------------------------------------------------------------
bool spMQTTESP32::connect(void)
{
    // Already connected?
    if (connected())
        return true;

    // NOTE: For future impl of secure clients connections
    //       The Secure WiFi connection, when in InSecure() mode crashes
    //       MQTT - so for now using a simple connection. Will debug 
    //
    // Is this a secure connection?
    // if (caCertificate().length() == 0)
    // {
    //     Serial.println("DEBUG : MQTT - Insecure connection");
    //     _wifiClient.setInsecure();
    // }
    // else
    // {
    //     _wifiClient.setCACert(caCertificate().c_str());

    //     if (clientCertificate().length() > 0)
    //         _wifiClient.setCertificate(clientCertificate().c_str());

    //     if (clientKey().length() > 0)
    //         _wifiClient.setPrivateKey(clientKey().c_str());
    // }

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

    _mqttClient.setId(clientName().c_str());
    _mqttClient.setKeepAliveInterval(60 * 1000);
    _mqttClient.setConnectionTimeout(5 * 1000);

   //  // Username/password provided?

    if (username().length() > 0 && password().length() > 0)
        _mqttClient.setUsernamePassword(username().c_str(), password().c_str());

   //  // Connect
   //  //Serial.printf("MQTT Connect: %s:%d\n\t", server().c_str(), port());

    for( int i=0; !_mqttClient.connect(server().c_str(), port()); i++)
    {
        if (i > kMaxConnectionTries)
        {
            spLog_E(F("%s: MQTT connection failed. Error Code: %d"), name(), _mqttClient.connectError());
            return false;
        }
        spLog_N_(".");
        delay(kConnectionDelayMS);
    }

    // we're connected
    return true;
}

//----------------------------------------------------------------------------

void spMQTTESP32::set_bufferSize(uint16_t buffSize)
{
    if (buffSize > 0)
    {
        _mqttClient.setTxPayloadSize(buffSize);
        _dynamicBufferSize = buffSize;
    }
    _txBufferSize = buffSize;
}
//----------------------------------------------------------------------------
uint16_t spMQTTESP32::get_bufferSize(void)
{
    return _txBufferSize;
}

//----------------------------------------------------------------------------
// spWriter interface method
void spMQTTESP32::write(const char * value, bool newline)
{
    // if we are not connected, ignore
    if (!connected() || !value)
        return;

    // do we have a topic?
    if ( topic().length() == 0 )
    {
        spLog_E(F("%s : No MQTT topic provided."), name());
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
#endif