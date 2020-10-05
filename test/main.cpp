#include <exception>
#include "UnitTest.h"
#include "TestMqttTeleInfo.h"
#include "TestSerialTeleInfo.h"

using namespace std;

int main()
{
    UnitTest unitTest;
    int ret = 0;


    try
    {
        unitTest.addTestClass(new TestMqttTeleInfo());
        unitTest.addTestClass(new TestSerialTeleInfo());
    }
    catch(const exception &e)
    {
        unitTest.displayError(e.what());
        ret = -1;
    }

    if(ret!=-1)
    {
        if(!unitTest.run()) ret = 1;
    }

	return ret;
}
