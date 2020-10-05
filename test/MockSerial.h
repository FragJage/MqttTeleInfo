#ifndef MOCKSERIAL_H
#define MOCKSERIAL_H

#include <string>
#include "ISerialCommunicator.h"


class MockSerial : public ISerialCommunicator
{
    public:
        MockSerial();
        virtual ~MockSerial();
        virtual void SetSerialPort(const std::string& serialPort);
        virtual std::map<std::string, std::string> GetTrame();
        void SetNextTrame(const std::string& trame);
        std::string GetSerialPort();

    protected:

    private:
        std::string m_SerialPort;
        std::string m_NextTrame;
};

#endif // MOCKSERIAL_H
