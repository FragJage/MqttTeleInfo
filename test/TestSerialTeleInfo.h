#include <cassert>
#include "UnitTest.h"
#include "../src/SerialTeleInfo.h"


class TestSerialTeleInfo : public TestClass<TestSerialTeleInfo>
{
public:
	TestSerialTeleInfo();
	~TestSerialTeleInfo();

	bool GetTrame();

private:
	SerialTeleInfo serialTeleInfo;
};
