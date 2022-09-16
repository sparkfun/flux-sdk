//
// Define interfaces/base classes for output
// 

#pragma once

//-----------------------------------------
// Define an interface for output of log information.

class spWriter{

public:
	virtual void write(bool value)
	{
		write((int)value);
	};
	virtual void write(int)=0;
	virtual void write(float)=0;
	virtual void write(const char *)=0;
	virtual void write(std::string& value)
	{
		write(value.c_str());
	};

};

// Define a formatter for log data 

class spOutputFormat{

public:

	spOutputFormat(){};

	// value methods
	virtual void logValue(const char* tag, bool value)=0;
	virtual void logValue(const char* tag, int value)=0;
	virtual void logValue(const char* tag, float value)=0;
	virtual void logValue(const char* tag, const char* value)=0;
	virtual void logValue(const char *tag, std::string value)
	{
		logValue(tag, value.c_str());
	}

	// structure cycle 

	virtual void beginObservation(const char * szTitle=nullptr)=0;
	virtual void beginSection(const char * szName){};
	virtual void endObservation(void)=0;

	virtual void writeObservation(void)=0;
	virtual void clearObservation(void){};

	virtual void reset(void){};

	void add(spWriter &newWriter){
		add(&newWriter);
	}
	void add(spWriter* newWriter){		
		_Writers.push_back(newWriter);
	}

	void outputObservation(const char * szBuffer){

		for ( auto writer: _Writers)
			writer->write(szBuffer);
	}

private:
	std::vector<spWriter*> _Writers;
};

