

#pragma once

#include "flxCore.h"
#include "spCoreDevice.h"
#include <memory>




class spApplication;

// happy functions for happy users.
bool spark_start(bool bAutoLoad = true);
bool spark_loop();

// Define the main framework class  - note it's a singleton
class spSpark : public spObjectContainer
{

  public:
    // spark is a singleton
    static spSpark &get(void)
    {

        static spSpark instance;
        return instance;
    }

    bool start(bool bAutoLoad = true);

    // Loop/operation method
    bool loop(void);

    // Add items to framework - use overloading to determine destination
    void add(spAction &theAction)
    {
        add(&theAction);
    }
    void add(spAction *theAction)
    {
        Actions.push_back(theAction);
    }

    void add(spDevice &theDevice)
    {
        add(&theDevice);
    }
    void add(spDevice *theDevice);

    // This is a singleton class - so delete copy & assignment constructors
    spSpark(spSpark const &) = delete;
    void operator=(spSpark const &) = delete;

    // leaving containers public - not sure if this is helpful
    spDeviceContainer Devices;
    spActionContainer Actions;


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
    //  auto buttons = spark.getAll<spDevButton>();
    //
    //  Serial.printf("Number of buttons: %d \n\r", buttons->size());
    //  for( auto b : *buttons)
    //  {
    //      Serial.printf("Button Name: %s", b->name());
    //
    //      logger.listen(button->on_clicked);  // add to logger
    //  }
    //

    template <class T> std::shared_ptr<spContainer<T *>> get()
    {
        spContainer<T *> results;

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
        return std::make_shared<spContainer<T *>>(std::move(results));
    }

    //--------------------------------------------------------
    // Get all that are of the provided type ID
    std::shared_ptr<spOperationContainer> get(flxTypeID type)
    {
        spOperationContainer results;

        spOperation *theItem;
        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
            {
                theItem = (spOperation *)Devices.at(i);
                results.push_back(theItem);
            }
        }
        // make a smart pointer
        return std::make_shared<spOperationContainer>(std::move(results));
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

    spDeviceContainer &connectedDevices(void)
    {
        return Devices;
    }

    spBusSPI & spiDriver()
    {
        // has the driver been initialized?
        if (!_spiDriver.initialized())
            _spiDriver.begin(true);

        return _spiDriver;
    }
    spBusI2C & i2cDriver()
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
        spLog_N("");
        spLog_N(F("%s  %s"), name(), versionString());
        spLog_N(F("%s\n\r"), description());
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

    void setApplication(spApplication &theApp)
    {
        setApplication(&theApp);
    }

    void setApplication(spApplication *theApp)
    {
        if (theApp)
            _theApplication = theApp;
    }

    spApplication * application(void)
    {
        return _theApplication;
    }
  private:

    spBusI2C     _i2cDriver;
    spBusSPI     _spiDriver;

    std::string _strVersion;
    uint32_t    _uiVersion;

    spApplication * _theApplication;

    // Note private constructor...
    spSpark() : _strVersion{"0"}, _uiVersion{0}, _theApplication{nullptr}
    {

        // setup some default heirarchy things ...
        this->setName("spark", "The SparkFun Spark Framework");
        Actions.setName("Settings", "System settings and operations");
        Devices.setName("Devices Settings", "Settings for connected devices");


        // Our container has two children, the device and the actions container
        // Cast the devices and actions to objects to add. And had to use
        // a temp var to get the references to take.
        spObject * pTmp = &Actions;
        this->push_back(pTmp);
        pTmp = &Devices;
        this->push_back(pTmp);
        
    }

    spOperation *_getByType(flxTypeID type)
    {

        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
                return (spOperation *)Devices.at(i);
        }

        for (int i = 0; i < Actions.size(); i++)
        {
            if (type == Actions.at(i)->getType())
                return (spOperation *)Actions.at(i);
        }
        return nullptr;
    }
};

// have a "global" variable that allows access to the spark environment from anywhere...

extern spSpark &spark;

// Define our application class interface.
class spApplication : public spActionType<spApplication>
{

public:
    spApplication()
    {
        spark.setApplication(this);
    }
    
    // Method is called before device auto-load, settings restoratoin and action initalization
    virtual bool setup(void)
    {
        return true;
    }

    // Called after everthing is loaded, restored and initialize
    virtual bool start(void)
    {
        return true;
    }

    void setVersion(const char *strVersion, uint32_t uiVersion)
    {
        spark.setVersion(strVersion, uiVersion);
    }
};