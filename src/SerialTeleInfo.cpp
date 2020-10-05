#include <stdio.h>   /* Standard input/output definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include "SerialTeleInfo.h"

using namespace std;

SerialTeleInfo::SerialTeleInfo()
{
    m_serialPort = "/dev/ttyAMA0";
}

SerialTeleInfo::~SerialTeleInfo()
{
}

void SerialTeleInfo::SetSerialPort(const string& serialPort)
{
    m_serialPort = serialPort;
}

map<string, string> SerialTeleInfo::GetTrame()
{
    map<string, string> trame;

    int fd = open(m_serialPort.c_str(), O_RDWR | O_NOCTTY);
    if (fd == -1) return trame;
    fcntl(fd, F_SETFL, 0);

    trame = ReadTrame(fd);

    close(fd);
    return trame;
}

map<string, string> SerialTeleInfo::ReadTrame(int fd)
{
    map<string, string> trame;
    char readedChar;
    size_t readedSize;
    int automate = 0;
    string key;
    string value;

    do
    {
        readedSize = read(fd, &readedChar, 1);
        if(readedSize==0) return trame;
    } while(readedChar!=0x02);


    do
    {
        readedSize = read(fd, &readedChar, 1);
        if(readedSize==0) return trame;
        if(readedChar==' ')
        {
            automate++;
            continue;
        }
        if(readedChar==0x0A)
        {
            if(key!="") trame[key] = value;
            key = "";
            value = "";
            automate = 0;
            continue;
        }
        if(readedChar<' ')
            continue;

        switch(automate)
        {
            case 0 :
                key = key+readedChar;
                break;
            case 1 :
                value = value+readedChar;
                break;
        }
    } while(readedChar!=0x02);

    return trame;
}
