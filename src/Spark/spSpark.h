

#pragma once

#include "spCore.h"
#include "spCoreDevice.h"
#include <memory>

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

        spTypeID type = T::type();

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
    std::shared_ptr<spOperationContainer> get(spTypeID type)
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
  private:


    spStorage *  _settingsStorage;

    spBusI2C     _i2cDriver;
    spBusSPI     _spiDriver;

    // Note private constructor...
    spSpark() : _settingsStorage{nullptr}
    {

        // setup some default heirarchy things ...
        this->setName("spark");
        Devices.setName("Devices", "The devices connected to this system.");
        Actions.setName("Actions", "The operations/actions registered with this system.");

        // Our container has two children, the device and the actions container
        // Cast the devices and actions to objects to add. And had to use
        // a temp var to get the references to take.
        spObject *pTmp = &Devices;
        this->push_back(pTmp);
        pTmp = &Actions;
        this->push_back(pTmp);
    }

    spOperation *_getByType(spTypeID type)
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