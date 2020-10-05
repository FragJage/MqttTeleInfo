#ifndef ISERIALCOMMUNICATOR_H
#define ISERIALCOMMUNICATOR_H

#include <string>
#include <map>

class ISerialCommunicator
{
	public:
        	virtual void SetSerialPort(const std::string& serialPort) = 0;
        	virtual std::map<std::string, std::string> GetTrame() = 0;
};
#endif
