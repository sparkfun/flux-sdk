

#pragma once

#include "spCore.h"
#include "spDevI2C.h"
#include "spDevice.h"

// happy functions for happy users.
bool spark_start(bool bAutoLoad = true);
bool spark_loop();

// Define the main framework class  - note it's a singleton
class Spark_ : public spContainer<spBase>
{

  public:
    // spark is a singleton
    static Spark_ &getInstance(void)
    {

        static Spark_ instance;
        return instance;
    }

    bool start(bool bAutoLoad = true);

    // Loop/operation method
    bool loop(void);

    // Add items to framework - use overloading to determine destination
    void add(spAction &theAction)
    {
        Actions.add(&theAction);
    }
    void add(spAction *theAction)
    {
        Actions.add(theAction);
    }

    void add(_spDevice &theDevice)
    {
        Devices.add(&theDevice);
    }
    void add(_spDevice *theDevice)
    {
        Devices.add(theDevice);
    }

    // This is a singleton class - so delete copy & assignment constructors
    Spark_(Spark_ const &) = delete;
    void operator=(Spark_ const &) = delete;

    // leaving containers public - not sure if this is helpful
    spDeviceContainer Devices;
    spActionContainer Actions;

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

    spDeviceList &connectedDevices(void)
    {
        return Devices;
    }
    // -- Alternative route - won't require user t cast the object.

  private:
    spDevI2C _i2cDriver;

    Spark_()
    {

        // setup some default heirarchy things ...
        this->name = "spark";
        Devices.name = "devices";
        Actions.name = "actions";

        // Our container has two children, the device and the actions container
        this->spContainer::add(&Devices);
        this->spContainer::add(&Actions);

    } // private Constructor
    spBase *_getByType(spType *type)
    {
        for (int i = 0; i < Devices.size(); i++)
        {
            if (type == Devices.at(i)->getType())
                return Devices.at(i);
        }
        return nullptr;
    }

    spBase *_getByType(spType &type)
    {
        return _getByType(&type);
    }
};

// create a typedef that hinds the reference
typedef Spark_ &Spark;

#define Spark() Spark_::getInstance()

extern Spark spark;