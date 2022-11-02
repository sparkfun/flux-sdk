

#pragma once


// Object wrapper around the SD Card object. This allows this object to 
// be part of the framework.


#include "spCore.h"

#include "FS.h"

class _spFSSDCard;


class spFSFile 
{

public:

	spFSFile( ){};
	
	size_t write(const uint8_t *buf, size_t size);

  void close(void);

  operator bool() const;

  void flush(void);

private:
	friend _spFSSDCard;

	void setFile(File & theFile)
	{
		_file = theFile;
	}

	File _file; 

};



// Interface that defines the interaction with a filesystem.

class spIFileSystem
{
public:
  // open file types
  typedef enum {
    kFileWrite = 1,
    kFileRead,
    kFileAppend
  }spFileOpenMode_t;

  // open
  virtual spFSFile open(const char * name, spFileOpenMode_t mode, bool create=false) = 0;

  virtual bool exists(const char *name) = 0;

  virtual bool remove(const char *name) = 0;

  virtual bool rename(const char *nameFrom, const char *nameTo) = 0;

  virtual bool mkdir(const char *path) = 0;

  virtual bool rmdir(const char *path) = 0;
};


class _spFSSDCard : public spIFileSystem
{

public:
    // _spFSSDCard is a singleton
    static _spFSSDCard &get(void)
    {

        static _spFSSDCard instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    _spFSSDCard(_spFSSDCard const &) = delete;
    void operator=(_spFSSDCard const &) = delete;


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
   	spFSFile open(const char * name, spFileOpenMode_t mode, bool create=false);

   	bool exists(const char *name);

   	bool remove(const char *name);

   	bool rename(const char *nameFrom, const char *nameTo);

   	bool mkdir(const char *path);

  	bool rmdir(const char *path);

private:
	// private constructor
	_spFSSDCard() : _isInitalized{false}, _pinCS{0}, _pinPower{0}, _powerOn{false}{}
	bool _isInitalized;

	uint8_t  _pinCS;
	uint8_t  _pinPower;

	bool    _powerOn;

};
extern _spFSSDCard &_theFSSDCard;

// wrapper around the SD file system singleton - 

class spFSSDCard : public spIFileSystem, public spSystemType<spFSSDCard>
{

public:
    
	bool initialize()
	{
		return _theFSSDCard.initialize();
	}
	bool initialize(uint8_t pinCS)
	{
		return _theFSSDCard.initialize(pinCS);
	}
	bool initialize(uint8_t pinCS, uint8_t pinPower)
	{
		return _theFSSDCard.initialize(pinCS, pinPower);
	}	

	// Power interface
	void setPower(bool powerOn)
	{
		_theFSSDCard.setPower(powerOn);
	}
	bool power(void)
	{
		return _theFSSDCard.power();
	};

	void setPowerPin(uint8_t pin)
	{
		_theFSSDCard.setPowerPin(pin);
	};

	void setCSPin(uint8_t pin)
	{
		_theFSSDCard.setCSPin(pin);
	};

	// FS interface methods
	// open
   	spFSFile open(const char * name, spFileOpenMode_t mode, bool create=false)
   	{
   		return _theFSSDCard.open(name, mode, create);
   	}

   	bool exists(const char *name)
   	{
   		return _theFSSDCard.exists(name);
   	}

   	bool remove(const char *name)
   	{
   		return _theFSSDCard.remove(name);
   	}

   	bool rename(const char *nameFrom, const char *nameTo)
   	{
   		return _theFSSDCard.rename(nameFrom, nameTo);
   	}

   	bool mkdir(const char *path)
   	{
   		return _theFSSDCard.mkdir(path);
   	}

  	bool rmdir(const char *path)
  	{
  		return _theFSSDCard.rmdir(path);
  	}

};