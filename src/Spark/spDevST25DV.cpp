/*
 *
 *  spDevST25DV.cpp
 *
 *  Device object for the Qwiic ST25DV device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevST25DV.h"

/*
  DATA = 0x53,          // E2 = 0, E1 = 1
  SYSTEM = 0x57,        // E2 = 1, E1 = 1
  RF_SWITCH_OFF = 0x51, // E2 = 0, E1 = 0
  RF_SWITCH_ON = 0x55,  // E2 = 1, E1 = 0
*/
uint8_t spDevST25DV::defaultDeviceAddress[] = {0x57, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevST25DV);
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevST25DV::spDevST25DV()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ST25DV RFID Tag");

    // Register Properties
    spRegister(ssid, "WiFi Record SSID", "WiFi Record SSID");
    spRegister(password, "WiFi Key / Password", "WiFi Key / Password");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevST25DV::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the IC_REF. Check it is 0x24 or 0x26
    uint8_t icRefReg[2] = { REG_IC_REF >> 8, REG_IC_REF & 0xFF };
    if (!i2cDriver.write(address, icRefReg, 2))
        return false;
    uint8_t icRef = 0;
    if (!i2cDriver.receiveResponse(address, &icRef, 1))
        return false;
    return ((icRef == 0x24) || (icRef == 0x26));
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevST25DV::onInitialize(TwoWire &wirePort)
{

    bool rc = SFE_ST25DV64KC_NDEF::begin(wirePort);

    if (!rc)
        spLog_E("ST25DV - begin failed");

    return rc;
}

// methods for the read-write properties
std::string spDevST25DV::get_ssid()
{
    bool success = false;
    if (!_ssid)
    {
        success = SFE_ST25DV64KC_NDEF::readNDEFWiFi(_readSsid, MAX_SSID_PASWORD_LEN - 1, _readPassword, MAX_SSID_PASWORD_LEN - 1); // Read the first WiFi Record
    }
    if (success)
    {
        _password = true;
    }
    _ssid = false;

    std::string theString = _readSsid;

    return theString;
}
void spDevST25DV::set_ssid(std::string newSsid)
{
    bool success = true;
    strncpy(_readSsid, newSsid, MAX_SSID_PASWORD_LEN); // Record the SSID
    _ssidW = true;
    if (_ssidW && _passwordW)
    {
        success = SFE_ST25DV64KC_NDEF::writeNDEFEmpty();
        success &= SFE_ST25DV64KC_NDEF::writeNDEFWiFi((const char *)_readSsid, (const char *)_readPassword);
        _ssidW = false;
        _passwordW = false;
    }
    if (!success)
        spLog_E("ST25DV set_ssid writeNDEFWiFi failed!");
}
std::string spDevST25DV::get_password()
{
    bool success = false;
    if (!_password)
    {
        success = SFE_ST25DV64KC_NDEF::readNDEFWiFi(_readSsid, MAX_SSID_PASWORD_LEN - 1, _readPassword, MAX_SSID_PASWORD_LEN - 1); // Read the first WiFi Record
    }
    if (success)
    {
        _ssid = true;
    }
    _password = false;

    std::string theString = _readPassword;

    return theString;
}
void spDevST25DV::set_password(std::string newPassword)
{
    bool success = true;
    strncpy(_readPassword, newPassword, MAX_SSID_PASWORD_LEN); // Record the Password
    _passwordW = true;
    if (_ssidW && _passwordW)
    {
        success = SFE_ST25DV64KC_NDEF::writeNDEFEmpty();
        success &= SFE_ST25DV64KC_NDEF::writeNDEFWiFi((const char *)_readSsid, (const char *)_readPassword);
        _ssidW = false;
        _passwordW = false;
    }
    if (!success)
        spLog_E("ST25DV set_password writeNDEFWiFi failed!");
}

//----------------------------------------------------------------------------------------------------------
// Loop

bool spDevST25DV::loop(void)
{
    bool result = false;

    static unsigned long lastMillis = millis();

    if (millis() > (lastMillis + 5000)) // Check tag every 5 seconds to avoid possible I2C/RF collisions
    {
        new_WiFi_record.emit();
    }

    return result;
}
