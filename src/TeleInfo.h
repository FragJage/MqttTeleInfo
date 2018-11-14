#ifndef TELEINFO_H
#define TELEINFO_H

#include <string>
#include <map>

class TeleInfo
{
    public:
        TeleInfo();
        ~TeleInfo();
        void SetSerialPort(const std::string& serialPort);
        std::map<std::string, std::string> GetTrame();

    private:
        std::map<std::string, std::string> ReadTrame(int fd);
        std::string m_serialPort;
};

#endif // TELEINFO_H
