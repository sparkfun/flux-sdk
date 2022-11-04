

#ifdef ESP32

#include "spWiFiESP32.h"

// ESP32 library...
#include "WiFi.h"

// WiFi client for ESP32 boards

// Define a connection iteration value - exceed this, skip the connection

#define kMaxConnectionTries  10

//----------------------------------------------------------------
// Enabled Property setter/getters
void spWiFiESP32::set_isEnabled(bool bEnabled)
{
    // Any changes?
    if (_isEnabled == bEnabled)
        return;  

    // changing state ...
    // TODO - Should this control connection state?

    _isEnabled = bEnabled;

    if ( _isEnabled)
        (void)connect();
    else 
        disconnect();
}

//----------------------------------------------------------------
bool spWiFiESP32::get_isEnabled(void)
{
    return _isEnabled;
}

//----------------------------------------------------------------
// Connection Management

bool spWiFiESP32::connect(void)
{

    if ( !_isEnabled )
        return false;

    // If we are already connected, return
    if ( WiFi.isConnected() )
        return true;

    // Do we have creditials?
    if (SSID().length() == 0 || password().length() == 0 )
    {
        spLog_E(F("WiFi: No credentials provided. Unable to connect"));
        return false;
    }


    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID().c_str(), password().c_str());
    
    spLog_I(F("Connecting to WiFi..."));

    int i=0;
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        i++;
        if ( i > kMaxConnectionTries)
        {
            spLog_E(F("Error connected to WiFi access point - %s"), SSID().c_str());
            WiFi.disconnect(true);
            return false;
        }
    }
    // okay, we're connected.
    _wasConnected = true;
    on_connectionChange.emit(true);

    return true;
}

//----------------------------------------------------------------
void spWiFiESP32::disconnect(void)
{

    if ( WiFi.isConnected() )
    {
        if ( !WiFi.disconnect(true) )
        {
            spLog_E(F("WiFi disconnect() - error disconnecting"));
        }
    }
    if ( _wasConnected )
        on_connectionChange.emit(false);

    _wasConnected = false;

}

//----------------------------------------------------------------
bool spWiFiESP32::isConnected()
{
    return ( _isEnabled && WiFi.isConnected());
}

//----------------------------------------------------------------
bool spWiFiESP32::initialize(void)
{
    return connect();
}

bool spWiFiESP32::loop(void)
{
    // Connection change???
    if ( _isEnabled )
    {
        bool wifiConn = WiFi.isConnected();
        if (wifiConn != _wasConnected )
        {
            _wasConnected = wifiConn;
            on_connectionChange.emit(_wasConnected);
            return true;
        }
    }
    return false;
}

#endif