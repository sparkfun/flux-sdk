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

#include "flxCoreTypes.h"
#include <FS.h>
#include <Stream.h>
#include <memory>
// FS Interfaces

//-----------------------------------------------------------------------
// A file interface

class flxFSFile;

class flxIFile
{
  public:
    virtual size_t write(const uint8_t *buf, size_t size) = 0;

    virtual void close(void) = 0;

    virtual bool isValid(void) = 0;

    virtual void flush(void) = 0;

    virtual size_t size(void) = 0;

    virtual size_t read(uint8_t *buf, size_t size) = 0;

    virtual const char *name(void) = 0;

    virtual bool isDirectory(void) = 0;

    virtual std::string getNextFilename(void) = 0;

    virtual int available(void) = 0;

    virtual Stream *stream(void) = 0;

    virtual flxFSFile openNextFile(void) = 0;

    virtual time_t getLastWrite(void) = 0;

    virtual File filePointer(void) = 0;
};

//-----------------------------------------------------------------------
// Framework file class that is used to provide a standard output
// from our open() call in IFileSystem. This uses the above flxIFile
// interface as a "driver"
class flxFSFile
{

  public:
    flxFSFile() : _file{nullptr} {};

    // Copy assignment
    flxFSFile &operator=(const flxFSFile &rhs)
    {
        _file = rhs._file;
        return *this;
    }

    size_t write(const uint8_t *buf, size_t size)
    {
        if (_file && _file->isValid())
            return _file->write(buf, size);
        return 0;
    }

    bool isValid(void)
    {
        return (_file && _file->isValid());
    }

    void close(void)
    {
        if (_file && _file->isValid())
            _file->close();
    }

    void flush(void)
    {
        if (_file && _file->isValid())
            _file->flush();
    }

    // NOTE: Using smart pointers to managing the file interface.
    void setIFile(std::shared_ptr<flxIFile> theFile)
    {
        _file = theFile;
    }

    operator bool() const
    {
        // if  the file object exist, we exist
        return (_file && _file->isValid());
    }

    size_t read(uint8_t *buf, size_t size)
    {
        if (_file && _file->isValid())
            return _file->read(buf, size);

        return 0;
    }

    size_t size()
    {
        if (_file && _file->isValid())
            return _file->size();

        return 0;
    }

    const char *name(void)
    {
        if (_file && _file->isValid())
            return _file->name();

        return nullptr;
    }

    const bool isDirectory(void)
    {
        if (_file && _file->isValid())
            return (bool)_file->isDirectory();

        return false;
    }

    std::string getNextFilename(void)
    {
        if (_file && _file->isValid())
            return _file->getNextFilename();

        std::string tmp = "";
        return tmp;
    }

    int available(void)
    {
        if (_file && _file->isValid())
            return (bool)_file->available();

        return 0;
    }

    Stream *stream(void)
    {
        if (_file && _file->isValid())
            return _file->stream();

        return nullptr;
    }

    flxFSFile openNextFile(void)
    {
        if (_file && _file->isValid())
            return _file->openNextFile();

        flxFSFile cow;
        return cow;
    }

    time_t getLastWrite(void)
    {
        if (!_file || !_file->isValid())
            return 0;

        return _file->getLastWrite();
    }

    File filePointer(void)
    {
        if (!_file || !_file->isValid())
        {
            File emptyFile;
            return emptyFile;
        }

        return _file->filePointer();
    }

  private:
    // note use of smart pointer for the file
    std::shared_ptr<flxIFile> _file;
};

//-----------------------------------------------------------------------
// Interface that defines the interaction with a filesystem.

class flxIFileSystem : public flxDescriptor
{
  public:
    // open file types
    typedef enum
    {
        kFileWrite = 1,
        kFileRead,
        kFileAppend
    } flxFileOpenMode_t;

    // open
    virtual flxFSFile open(const char *name, flxFileOpenMode_t mode, bool create = false) = 0;

    virtual bool exists(const char *name) = 0;

    virtual bool remove(const char *name) = 0;

    virtual bool rename(const char *nameFrom, const char *nameTo) = 0;

    virtual bool mkdir(const char *path) = 0;

    virtual bool rmdir(const char *path) = 0;

    virtual uint64_t size(void) = 0;

    virtual const char *type(void) = 0;

    virtual bool enabled(void) = 0;

    virtual FS fileSystem(void) = 0;
};
