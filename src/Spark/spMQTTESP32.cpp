

#ifdef ESP32

#include "spMQTTESP32.h"


//----------------------------------------------------------------------------
bool spMQTTESP32Secure::connect(void)
{
    // Already connected?
    if (connected())
        return true;

    if ( _pCACert != nullptr )
        _wifiClient.setCACert(_pCACert);

    if ( _pClientCert != nullptr)
        _wifiClient.setCertificate(_pClientCert);

    if (_pClientKey != nullptr)
        _wifiClient.setPrivateKey(_pClientKey);

    return spMQTTESP32Base::connect();

   
}



#endif