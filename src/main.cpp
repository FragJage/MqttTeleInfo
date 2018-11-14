#include <iostream>
#include <exception>
#include "MqttTeleInfo.h"

using namespace std;

int main(int argc, char* argv[])
{
    int res = 0;

    try
    {
        MqttTeleInfo mqttTeleInfo;

        Service* pService = Service::Create("MqttTeleInfo", "Send teleinfo trame to Mqtt messages", &mqttTeleInfo);
        res = pService->Start(argc, argv);
        Service::Destroy();
    }
    catch(const exception &e)
    {
        std::cout << e.what();
    }

	#if defined(_MSC_VER) && defined(DEBUG)
	cout << "Press any key..." << endl;
	getchar();
	#endif

    return res;
}
