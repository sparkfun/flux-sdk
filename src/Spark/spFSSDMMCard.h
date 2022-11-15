

#pragma once

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include "FS.h"
#include "spCore.h"
#include "spFS.h"

class _spFSSDMMCard;

class spFSSDMMCFile : public spIFile
{
  public:
    spFSSDMMCFile(){};

    size_t write(const uint8_t *buf, size_t size);

    void close(void);

    bool isValid(void);

    void flush(void);

    size_t size(void);

  private:
    friend _spFSSDMMCard;

    void setFile(File &theFile)
    {
        _file = theFile;
    }

    File _file;
};

class _spFSSDMMCard : public spIFileSystem
{

  public:
    // _spFSSDMMCard is a singleton
    static _spFSSDMMCard &get(void)
    {

        static _spFSSDMMCard instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _spFSSDMMCard(_spFSSDMMCard const &) = delete;
    void operator=(_spFSSDMMCard const &) = delete;

    // setup and lifecycle of the file system interface
    // TODO - make this uniform ...
    bool initialize();
    bool initialize(uint8_t pinCS);
    bool initialize(uint8_t pinCS, uint8_t pinPower);

    // Power interface
    void setPower(bool powerOn);
    bool power(void)
    {
        return _powerOn;
    };

    void setPowerPin(uint8_t pin)
    {
        _pinPower = pin;
    };

    void setCSPin(uint8_t pin)
    {
        _pinCS = pin;
    };

    // FS interface methods
    // open
    spFSFile open(const char *name, spFileOpenMode_t mode, bool create = false);

    bool exists(const char *name);

    bool remove(const char *name);

    bool rename(const char *nameFrom, const char *nameTo);

    bool mkdir(const char *path);

    bool rmdir(const char *path);

    uint size(void);

    const char *type(void);

  private:
    // private constructor
    _spFSSDMMCard() : _isInitalized{false}, _pinCS{0}, _pinPower{0}, _powerOn{false}
    {
    }
    bool _isInitalized;

    uint8_t _pinCS;
    uint8_t _pinPower;

    bool _powerOn;
};
extern _spFSSDMMCard &_theSDMMCard;

// wrapper around the SD file system singleton -

class spFSSDMMCard : public spIFileSystem, public spSystemType<spFSSDMMCard>
{

  public:
    bool initialize()
    {
        return _theSDMMCard.initialize();
    }
    bool initialize(uint8_t pinCS)
    {
        return _theSDMMCard.initialize(pinCS);
    }
    bool initialize(uint8_t pinCS, uint8_t pinPower)
    {
        return _theSDMMCard.initialize(pinCS, pinPower);
    }

    // Power interface
    void setPower(bool powerOn)
    {
        _theSDMMCard.setPower(powerOn);
    }
    bool power(void)
    {
        return _theSDMMCard.power();
    };

    void setPowerPin(uint8_t pin)
    {
        _theSDMMCard.setPowerPin(pin);
    };

    void setCSPin(uint8_t pin)
    {
        _theSDMMCard.setCSPin(pin);
    };

    // FS interface methods
    // open
    spFSFile open(const char *name, spFileOpenMode_t mode, bool create = false)
    {
        return _theSDMMCard.open(name, mode, create);
    }

    bool exists(const char *name)
    {
        return _theSDMMCard.exists(name);
    }

    bool remove(const char *name)
    {
        return _theSDMMCard.remove(name);
    }

    bool rename(const char *nameFrom, const char *nameTo)
    {
        return _theSDMMCard.rename(nameFrom, nameTo);
    }

    bool mkdir(const char *path)
    {
        return _theSDMMCard.mkdir(path);
    }

    bool rmdir(const char *path)
    {
        return _theSDMMCard.rmdir(path);
    }

    uint size(void)
    {
        return _theSDMMCard.size();
    }
    const char *type(void)
    {
        return _theSDMMCard.type();
    }
};