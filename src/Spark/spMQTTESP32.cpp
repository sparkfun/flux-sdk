

#ifdef ESP32

#include "spMQTTESP32.h"

//----------------------------------------------------------------------------
bool spMQTTESP32Secure::connect(void)
{
    // Already connected?
    if (connected())
        return true;

    
    if (caCertificate().length() > 0)
        _wifiClient.setCACert(caCertificate().c_str());

    if (clientCertificate().length() > 0)
        _wifiClient.setCertificate(clientCertificate().c_str());

    if (clientKey().length() > 0)
        _wifiClient.setPrivateKey(clientKey().c_str());

    return spMQTTESP32Base::connect();

   
}



#endif