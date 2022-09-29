

#pragma once

#include "spCore.h"
#include "spDevI2C.h"
#include "spDevice.h"
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

    // get/find a device by type template.
    template <class T> T *get(void)
    {
        return (T *)_getByType(T::type());
    }

    template <class T> T *get(spTypeID type)
    {
        return (T *)_getByType(type);
    }

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

    // Experimental - a get method to get every instance of a type
    //
    // This is returning a smart pointer to a vector. Once the smart pointer
    // goes out of scope, it should free up the underlying vectory memory.
    //
    // example - getting all the buttons :
    // -------------------------------
    //
    // 	auto buttons = spark.getAll<spDevButton>();
    //
    // 	Serial.printf("Number of buttons: %d \n\r", buttons->size());
    // 	for( auto b : *buttons)
    //     	Serial.printf("Button Name: %s", b->name());

    template <class T> std::shared_ptr<spDeviceContainer> getAll()
    {
        spDeviceContainer results;

        spTypeID type = T::type();

        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
                results.push_back(Devices.at(i));
        }
        // make a smart pointer
        return std::make_shared<spDeviceContainer>(std::move(results));
    }

  private:
    spDevI2C _i2cDriver;

    // Note private constructor...
    spSpark()
    {

        // setup some default heirarchy things ...
        this->setName("spark");
        Devices.setName("devices");
        Actions.setName("actions");

        // Our container has two children, the device and the actions container
        // TODO
        // this->add(&Devices);
        // this->add(&Actions);
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