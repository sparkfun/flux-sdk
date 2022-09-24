

#pragma once

#include "spCore.h"
#include "spDevI2C.h"
#include "spDevice.h"

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
    void add(spAction2 &theAction)
    {
        Actions.add(&theAction);
    }
    void add(spAction2 *theAction)
    {
        Actions.add(theAction);
    }

    void add(_spDevice &theDevice)
    {
        Devices.push_back(&theDevice);
    }
    void add(_spDevice *theDevice);

    // This is a singleton class - so delete copy & assignment constructors
    spSpark(spSpark const &) = delete;
    void operator=(spSpark const &) = delete;

    // leaving containers public - not sure if this is helpful
    spDeviceContainer Devices;
    spActionContainer2 Actions;

    // Call to serliaze the settings of the system as a JSON object
    bool serializeJSON(char *, size_t);

    // get/find a device by type template.
    template <class T> T *get(void)
    {
        spType *type = &T::Type;
        return (T *)_getByType(type);
    }

    template <class T> T *get(spType &type)
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
    // -- Alternative route - won't require user t cast the object.

  private:
    spDevI2C _i2cDriver;

    // Note private constructor...
    spSpark()
    {

        // setup some default heirarchy things ...
        this->name = "spark";
        Devices.name = "devices";
        Actions.name = "actions";

        // Our container has two children, the device and the actions container
        this->add(&Devices);
        this->add(&Actions);

    } 

    spOperation *_getByType(spType *type)
    {
        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
                return Devices.at(i);
        }
         for (int i = 0; i < Actions.size(); i++)
        {
            if (type == Actions.at(i)->getType())
                return Actions.at(i);
        }
        return nullptr;
    }

    spOperation *_getByType(spType &type)
    {
        return _getByType(&type);
    }
};

// have a "global" variable that allows access to the spark environment from anywhere...

extern spSpark& spark;