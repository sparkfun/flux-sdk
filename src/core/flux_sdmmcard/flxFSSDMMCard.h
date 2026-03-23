/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include "FS.h"
#include "flxCore.h"
#include "flxFS.h"

class _flxFSSDMMCard;

class flxFSSDMMCFile : public flxIFile
{
  public:
    flxFSSDMMCFile() {};

    size_t write(const uint8_t *buf, size_t size);

    size_t read(uint8_t *buf, size_t size);

    void close(void);

    bool isValid(void);

    void flush(void);

    size_t size(void);

    const char *name(void);

    bool isDirectory(void);

    std::string getNextFilename(void);

    int available(void);

    Stream *stream(void);

    flxFSFile openNextFile(void);

    time_t getLastWrite(void);

    File filePointer(void)
    {
        return _file;
    }

  private:
    friend _flxFSSDMMCard;

    void setFile(File &theFile)
    {
        _file = theFile;
    }

    File _file;
};

class _flxFSSDMMCard : public flxIFileSystem
{

  public:
    // _flxFSSDMMCard is a singleton
    static _flxFSSDMMCard &get(void)
    {

        static _flxFSSDMMCard instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _flxFSSDMMCard(_flxFSSDMMCard const &) = delete;
    void operator=(_flxFSSDMMCard const &) = delete;

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
    flxFSFile open(const char *name, flxFileOpenMode_t mode, bool create = false);

    bool exists(const char *name);

    bool remove(const char *name);

    bool rename(const char *nameFrom, const char *nameTo);

    bool mkdir(const char *path);

    bool rmdir(const char *path);

    uint64_t size(void);

    uint64_t total(void);

    const char *type(void);

    uint64_t used(void);

    bool enabled(void)
    {
        return _isInitalized;
    }
    FS fileSystem(void);

  private:
    // private constructor
    _flxFSSDMMCard() : _isInitalized{false}, _pinCS{0}, _pinPower{0}, _powerOn{false}
    {
    }
    bool _isInitalized;

    uint8_t _pinCS;
    uint8_t _pinPower;

    bool _powerOn;
};
extern _flxFSSDMMCard &_theSDMMCard;

// wrapper around the SD file system singleton -

class flxFSSDMMCard : public flxIFileSystem, public flxSystemType<flxFSSDMMCard>
{

  public:
    flxFSSDMMCard()
    {
        flxIFileSystem::setName("SD Card", "A SD Card file system using the SDMMC interface");
    }
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
    flxFSFile open(const char *name, flxFileOpenMode_t mode, bool create = false)
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

    uint64_t size(void)
    {
        return _theSDMMCard.size();
    }

    uint64_t total(void)
    {
        return _theSDMMCard.total();
    }

    uint64_t used(void)
    {
        return _theSDMMCard.used();
    }

    const char *type(void)
    {
        return _theSDMMCard.type();
    }

    bool enabled(void)
    {
        return _theSDMMCard.enabled();
    };

    FS fileSystem(void)
    {
        return _theSDMMCard.fileSystem();
    }
};