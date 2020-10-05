#include <iostream>
#include "TestSerialTeleInfo.h"

using namespace std;

TestSerialTeleInfo::TestSerialTeleInfo() : TestClass("SerialTeleInfo", this)
{
    serialTeleInfo.SetSerialPort("./test/data/trame.data");
	addTest("GetTrame", &TestSerialTeleInfo::GetTrame);
}

TestSerialTeleInfo::~TestSerialTeleInfo()
{
}

bool TestSerialTeleInfo::GetTrame()
{
    map<string, string> trame = serialTeleInfo.GetTrame();
	map<string, string>::iterator it;
	it = trame.find("BASE");
	assert(trame.end() != it);
	assert(it->second == "0000000");
	it = trame.find("HCHC");
	assert(trame.end() != it);
	assert(it->second == "000001");
	it = trame.find("PTEC");
	assert(trame.end() != it);
	assert(it->second == "TH..");
	it = trame.find("IINST");
	assert(trame.end() != it);
	assert(it->second == "750");
	it = trame.find("PAPP");
	assert(trame.end() != it);
	assert(it->second == "520");

	return true;
}
