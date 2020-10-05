#ifndef SERIALTELEINFO_H
#define SERIALTELEINFO_H

#include <string>
#include <map>
#include "ISerialCommunicator.h"

class SerialTeleInfo : public ISerialCommunicator
{
    public:
        SerialTeleInfo();
        virtual ~SerialTeleInfo();
        virtual void SetSerialPort(const std::string& serialPort);
        virtual std::map<std::string, std::string> GetTrame();

    private:
        std::map<std::string, std::string> ReadTrame(int fd);
        std::string m_serialPort;
};

#endif // SERIALTELEINFO_H
