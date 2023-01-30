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
#include <memory>




class flxApplication;

// // happy functions for happy users.
// bool spark_start(bool bAutoLoad = true);
// bool spark_loop();

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

    bool start(bool bAutoLoad = true);

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

    flxBusSPI & spiDriver()
    {
        // has the driver been initialized?
        if (!_spiDriver.initialized())
            _spiDriver.begin(true);

        return _spiDriver;
    }
    flxBusI2C & i2cDriver()
    {
        // has the driver been initialized?
        if (!_i2cDriver.initialized())
            _i2cDriver.begin();

        return _i2cDriver;
    }

    void setVersion(const char *strVersion, uint32_t uiVersion)
    {
        _strVersion=strVersion;
        _uiVersion = uiVersion;
    }
    
    const char * versionString(void)
    {
        return _strVersion.c_str();
    }

    uint32_t version()
    {
        return _uiVersion;
    }

    void writeBanner(void)
    {
        flxLog_N("");
        flxLog_N(F("%s  %s"), name(), versionString());
        flxLog_N(F("%s\n\r"), description());
    }

    const char* deviceId(void)
    {
        // ID is 16 in length, we use last byte as a flag. 
        static char szDeviceID[17]={0};
#ifdef ESP32

        if ( szDeviceID[sizeof(szDeviceID)-1] == 0)
        {
            snprintf(szDeviceID, sizeof(szDeviceID)-1, "%012llX", ESP.getEfuseMac());

            szDeviceID[sizeof(szDeviceID)-1] = 1;
        }

#endif
        return (const char*)szDeviceID;
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
            Actions.insert(Actions.begin(), (flxAction*)theApp);
        }
    }

    flxApplication * application(void)
    {
        return _theApplication;
    }
  private:

    flxBusI2C     _i2cDriver;
    flxBusSPI     _spiDriver;

    std::string _strVersion;
    uint32_t    _uiVersion;

    flxApplication * _theApplication;

    // Note private constructor...
    flxFlux() : _strVersion{"0"}, _uiVersion{0}, _theApplication{nullptr}
    {

        // setup some default heirarchy things ...
        this->setName("spark", "The SparkFun Spark Framework");
        Actions.setName("Settings", "System settings and operations");
        Devices.setName("Devices Settings", "Settings for connected devices");


        // Our container has two children, the device and the actions container
        // Cast the devices and actions to objects to add. And had to use
        // a temp var to get the references to take.
        flxObject * pTmp = &Actions;
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
        ((flxObject*)this)->setName("Application Settings", "Main Application Settings");
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

    // Method is called before device auto-load, settings restoration and action initialization
    virtual bool setup(void)
    {
        return true;
    }

    // Called after everything is loaded, restored and initialize
    virtual bool start(void)
    {
        return true;
    }

    void setVersion(const char *strVersion, uint32_t uiVersion)
    {
        flux.setVersion(strVersion, uiVersion);
    }
private:
    flxDescriptor appDesc;

};