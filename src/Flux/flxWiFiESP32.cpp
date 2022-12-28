

#ifdef ESP32

#include "flxWiFiESP32.h"

// ESP32 library...
#include "WiFi.h"

// WiFi client for ESP32 boards

// Define a connection iteration value - exceed this, skip the connection

#define kMaxConnectionTries  10

//----------------------------------------------------------------
// Enabled Property setter/getters
void flxWiFiESP32::set_isEnabled(bool bEnabled)
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
bool flxWiFiESP32::get_isEnabled(void)
{
    return _isEnabled;
}

//----------------------------------------------------------------
// Connection Management

bool flxWiFiESP32::connect(void)
{

    if ( !_isEnabled )
        return false;

    // If we are already connected, return
    if ( WiFi.isConnected() )
        return true;

    // Do we have credentials?
    if (SSID().length() == 0 || password().length() == 0 )
    {
        flxLog_E(F("WiFi: No credentials provided. Unable to connect"));
        return false;
    }


    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID().c_str(), password().c_str());
    
    flxLog_I_(F("Connecting to WiFi..."));

    int i=0;
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        flxLog_N_(F("."));
        i++;
        if ( i > kMaxConnectionTries)
        {
            flxLog_E(F("Error connecting to WiFi access point - %s"), SSID().c_str());
            WiFi.disconnect(true);
            return false;
        }
    }
    flxLog_N(F("Connected to %s"), SSID().c_str());
    // okay, we're connected.
    _wasConnected = true;
    on_connectionChange.emit(true);

    return true;
}

//----------------------------------------------------------------
void flxWiFiESP32::disconnect(void)
{

    if ( WiFi.isConnected() )
    {
        if ( !WiFi.disconnect(true) )
        {
            flxLog_E(F("WiFi disconnect() - error disconnecting"));
        }
    }
    if ( _wasConnected )
        on_connectionChange.emit(false);

    _wasConnected = false;

}

//----------------------------------------------------------------
bool flxWiFiESP32::isConnected()
{
    return ( _isEnabled && WiFi.isConnected());
}

//----------------------------------------------------------------
bool flxWiFiESP32::initialize(void)
{
    return connect();
}

bool flxWiFiESP32::loop(void)
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