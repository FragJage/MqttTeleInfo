#include "../dependencies/FragFramework/src/StringTools.h"
#include "MockSerial.h"

using namespace std;

MockSerial::MockSerial()
{
    //ctor
}

MockSerial::~MockSerial()
{
    //dtor
}

void MockSerial::SetSerialPort(const string& serialPort)
{
    m_SerialPort = serialPort;
}

map<string, string> MockSerial::GetTrame()
{
    map<string, string> trame;
    vector<string> valuepairs = StringTools::Split(m_NextTrame, ';');
    string key;
    string value;

    for (const auto &valuepair: valuepairs)
    {
        if(StringTools::Split(valuepair, '=', &key, &value))
        {
            trame[key] = value;
        }
    }

    return trame;
}

void MockSerial::SetNextTrame(const string& trame)
{
    m_NextTrame = trame;
}

string MockSerial::GetSerialPort()
{
    return m_SerialPort;
}
