/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include "flxCore.h"
#include "flxCoreDevice.h"
#include "flxSerial.h"
#include <memory>

class flxApplication;

// // happy functions for happy users.
// bool spark_start(bool bAutoLoad = true);
// bool spark_loop();

// Define a default app class name
#define kDefaultAppClassName "SFE-FLUX-APPLICATION"

// Define the main framework class  - note it's a singleton
class flxFlux : public flxObjectContainer
{

  public:
    // spark is a singleton
    static flxFlux &get(void)
    {

        static flxFlux instance;
        return instance;
    }

    bool start();

    // Loop/operation method
    bool loop(void);

    // Add items to framework - use overloading to determine destination
    void add(flxAction &theAction)
    {
        add(&theAction);
    }
    void add(flxAction *theAction)
    {
        Actions.push_back(theAction);
    }

    void add(flxDevice &theDevice)
    {
        add(&theDevice);
    }
    void add(flxDevice *theDevice);

    // This is a singleton class - so delete copy & assignment constructors
    flxFlux(flxFlux const &) = delete;
    void operator=(flxFlux const &) = delete;

    // leaving containers public - not sure if this is helpful
    flxDeviceContainer Devices;
    flxActionContainer Actions;

    //---------------------------------------------------------------------------------
    bool save(flxStorage *pStorage);

    //---------------------------------------------------------------------------------
    bool restore(flxStorage *pStorage);

    //--------------------------------------------------------
    // get/find a device by type template.
    //
    // Returns a list/vector off all things found based on
    // the specified type
    //
    // This is returning a smart pointer to a vector of the type passed in.
    // Once the smart pointer goes out of scope, it frees up the underlying
    // vector memory.
    //
    // example - getting all the buttons :
    // -------------------------------
    //
    //  auto buttons = flux.getAll<flxDevButton>();
    //
    //  Serial.printf("Number of buttons: %d \n\r", buttons->size());
    //  for( auto b : *buttons)
    //  {
    //      Serial.printf("Button Name: %s", b->name());
    //
    //      logger.listen(button->on_clicked);  // add to logger
    //  }
    //

    template <class T> std::shared_ptr<flxContainer<T *>> get()
    {
        flxContainer<T *> results;

        flxTypeID type = T::type();

        // name the result container
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "Get Type: 0x%X", type);
        results.setName(szBuffer);

        T *theItem;
        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
            {
                theItem = (T *)Devices.at(i);
                results.push_back(theItem);
            }
        }
        // make a smart pointer
        return std::make_shared<flxContainer<T *>>(std::move(results));
    }

    //--------------------------------------------------------
    // Get all that are of the provided type ID
    std::shared_ptr<flxOperationContainer> get(flxTypeID type)
    {
        flxOperationContainer results;

        // name the result container
        char szBuffer[64];
        snprintf(szBuffer, sizeof(szBuffer), "Get Type: 0x%X", type);
        results.setName(szBuffer);

        flxOperation *theItem;
        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
            {
                theItem = (flxOperation *)Devices.at(i);
                results.push_back(theItem);
            }
        }
        // make a smart pointer
        return std::make_shared<flxOperationContainer>(std::move(results));
    }

    //--------------------------------------------------------

    template <class T> bool isConnected()
    {

        for (int i = 0; i < Devices.size(); i++)
        {
            if (T::Type == Devices.at(i)->getType())
                return true;
        }
        return false;
    }

    flxDeviceContainer &connectedDevices(void)
    {
        return Devices;
    }

    flxBusSPI &spiDriver()
    {
        // has the driver been initialized?
        if (!_spiDriver.initialized())
            _spiDriver.begin(true);

        return _spiDriver;
    }
    flxBusI2C &i2cDriver()
    {
        // has the driver been initialized?
        if (!_i2cDriver.initialized())
            _i2cDriver.begin();

        return _i2cDriver;
    }

    // Version Things
    void setVersion(uint32_t major, uint32_t minor, uint32_t point, const char *desc, uint32_t build)
    {
        _v_major = major;
        _v_minor = minor;
        _v_point = point;
        _v_build = build;
        _v_desc = desc;
    }

    void versionString(char *buffer, size_t nbuffer, bool bFull = false)
    {
        if (bFull)
            snprintf(buffer, nbuffer, "%02u.%02u.%02u %s - build %06x", _v_major, _v_minor, _v_point, _v_desc.c_str(),
                     _v_build);
        else
            snprintf(buffer, nbuffer, "%02u.%02u.%02u", _v_major, _v_minor, _v_point);
    }

    uint32_t version()
    {
        return _v_major * 10000 + _v_minor * 100 + _v_point;
        ;
    }
    void version(uint32_t &major, uint32_t &minor, uint32_t &point)
    {
        major = _v_major;
        minor = _v_minor;
        point = _v_point;
    }

    uint32_t build(void)
    {
        return _v_build;
    }

    void setAppClassID(const char *ID, char prefix[5])
    {
        _appClassID = ID;
        memcpy(_v_idprefix, prefix, 5);
    }
    const char *appClassID(void)
    {
        return _appClassID;
    }

    void writeBanner(void)
    {
        char szBuffer[128];
        versionString(szBuffer, sizeof(szBuffer), true);
        flxSerial.textToWhite();
        flxLog_N("");
        flxLog_N(F("%s   %s"), name(), description());
        flxLog_N(F("Version: %s\n\r"), szBuffer);
        flxSerial.textToNormal();
    }

    const char *deviceId(void);

    // user set app specific name
    void setLocalName(std::string name)
    {
        _localName = name;
    }

    std::string localName(void)
    {
        return _localName;
    }

    void setApplication(flxApplication &theApp)
    {
        setApplication(&theApp);
    }

    void setApplication(flxApplication *theApp)
    {
        if (theApp)
        {
            _theApplication = theApp;
            // set the app as the first entry of our actions list
            Actions.insert(Actions.begin(), (flxAction *)theApp);
        }
    }

    flxApplication *application(void)
    {
        return _theApplication;
    }

    void setAppToken(const uint8_t *data, size_t len);

    bool getAppToken(uint8_t token[32]);

    // should we add a unique value to device names?
    void setVerboseDevNames(bool bVerbose);
    bool verboseDevNames(void);

    void setAutoload(bool bAuto)
    {
        _deviceAutoload = bAuto;
    }
    void setLoadSettings(bool bLoad)
    {
        _loadSettings = bLoad;
    }

    // more of a debug setting ...
    void dumpDeviceAutoLoadTable(void)
    {
        flxDeviceFactory::get().dumpDeviceTable();
    }

  private:
    flxBusI2C _i2cDriver;
    flxBusSPI _spiDriver;

    uint32_t _v_major;
    uint32_t _v_minor;
    uint32_t _v_point;
    uint32_t _v_build;
    std::string _v_desc;
    char _v_idprefix[5];

    const char *_appClassID;

    flxApplication *_theApplication;

    uint8_t _token[32];
    bool _hasToken;
    bool _verboseDevNames;

    std::string _localName;

    bool _deviceAutoload;
    bool _loadSettings;
    // Note private constructor...
    flxFlux()
        : _v_major{0}, _v_minor{0}, _v_point{0}, _v_build{0}, _v_desc{""}, _v_idprefix{"0000"},
          _appClassID{kDefaultAppClassName}, _theApplication{nullptr}, _token{0}, _hasToken{false},
          _verboseDevNames{false}, _deviceAutoload{true}, _loadSettings{true}
    {

        // setup some default hierarchy things ...
        this->setName("Flux", "The SparkFun Flux Framework");
        Actions.setName("Settings", "System settings and operations");
        Devices.setName("Devices Settings", "Settings for connected devices");

        // Our container has two children, the device and the actions container
        // Cast the devices and actions to objects to add. And had to use
        // a temp var to get the references to take.
        flxObject *pTmp = &Actions;
        this->push_back(pTmp);
        pTmp = &Devices;
        this->push_back(pTmp);
    }

    flxOperation *_getByType(flxTypeID type)
    {

        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
                return (flxOperation *)Devices.at(i);
        }

        for (int i = 0; i < Actions.size(); i++)
        {
            if (type == Actions.at(i)->getType())
                return (flxOperation *)Actions.at(i);
        }
        return nullptr;
    }
};

// have a "global" variable that allows access to the spark environment from anywhere...

extern flxFlux &flux;

// Define our application class interface.
class flxApplication : public flxActionType<flxApplication>
{

  public:
    flxApplication()
    {
        flux.setApplication(this);

        // Set a name/descriptor that shows up in the menu system.
        ((flxObject *)this)->setName("Application Settings", "Main Application Settings");
    }

    // Name things - the overall base object of the system has a descriptor - we want this
    // method signature to name the app, but also allow this app object (which is an action)
    // to be used in the menuing system ... so
    //          - override the flxDescriptor methods on the app object
    //              -- use a descriptor instance variable to store the data
    //          - Methods called on the application will get these name/desc values
    //          - Methods called on sub-class objects get the base object name/desc
    // So we can use the same methods is both cases, but leverage the fact that
    // the base descriptor class methods are not virtual.

    void setName(const char *name)
    {
        appDesc.setName(name);
    }
    void setDescription(const char *desc)
    {
        appDesc.setDescription(desc);
    }
    void setName(const char *name, const char *desc)
    {
        this->setName(name);
        this->setDescription(desc);
    }
    const char *name(void)
    {
        return appDesc.name();
    }

    const char *description(void)
    {
        return appDesc.description();
    }

    // Method is called before anything - allows pre system setup - for example I/O init.
    virtual void onInit(void)
    {
        return;
    }
    // Method is called before device auto-load, settings restoration and action initialization
    virtual bool onSetup(void)
    {
        return true;
    }

    // Called after everything is loaded, restored and initialize
    virtual bool onStart(void)
    {
        return true;
    }

    // called after device autoload occurs, but before system state restore is called during
    // startup
    virtual void onDeviceLoad(void)
    {
        return;
    }

    // called just prior to system restore on startup
    virtual void onRestore(void)
    {
        return;
    }

    void setVersion(uint major, uint minor, uint point, const char *desc, uint32_t build)
    {
        flux.setVersion(major, minor, point, desc, build);
    }

    void setAppClassID(const char *ID, char prefix[5])
    {
        flux.setAppClassID(ID, prefix);
    }

  private:
    flxDescriptor appDesc;
};