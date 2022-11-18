

#pragma once

#include <memory>
#include "spCoreTypes.h"
// FS Interfaces

//-----------------------------------------------------------------------
// A file interface

class spIFile
{
  public:
    virtual size_t write(const uint8_t *buf, size_t size) = 0;

    virtual void close(void) = 0;

    virtual bool isValid(void) = 0;

    virtual void flush(void) = 0;

    virtual size_t size(void) = 0;

    virtual size_t read(uint8_t *buf, size_t size) = 0;
};


//-----------------------------------------------------------------------
// Framework file class that is used to provide a standard output
// from our open() call in IFileSystem. This uses the above spIFile
// interface as a "driver"
class spFSFile
{

  public:
    spFSFile() : _file{nullptr} {};

    size_t write(const uint8_t *buf, size_t size)
    {
        if (_file && _file->isValid())
            return _file->write(buf, size);
        return 0;
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
    void setIFile(std::shared_ptr<spIFile> theFile)
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
    
  private:
    // note use of smart pointer for the file
    std::shared_ptr<spIFile> _file;
};

//-----------------------------------------------------------------------
// Interface that defines the interaction with a filesystem.

class spIFileSystem : public spDescriptor
{
  public:
    // open file types
    typedef enum
    {
        kFileWrite = 1,
        kFileRead,
        kFileAppend
    } spFileOpenMode_t;

    // open
    virtual spFSFile open(const char *name, spFileOpenMode_t mode, bool create = false) = 0;

    virtual bool exists(const char *name) = 0;

    virtual bool remove(const char *name) = 0;

    virtual bool rename(const char *nameFrom, const char *nameTo) = 0;

    virtual bool mkdir(const char *path) = 0;

    virtual bool rmdir(const char *path) = 0;

    virtual uint size(void) = 0;

    virtual const char *type(void) = 0;

    virtual bool enabled(void) =0;
};
