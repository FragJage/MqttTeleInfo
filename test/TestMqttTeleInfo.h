#include <thread>
#include <map>
#include <cassert>
#include "Plateforms.h"
#include "UnitTest.h"
#include "MqttClient.h"
#include "MockSerial.h"
#include "../src/MqttTeleInfo.h"


class TestMqttTeleInfo : public TestClass<TestMqttTeleInfo>, public MqttClient::IMqttMessage
{
public:
	TestMqttTeleInfo();
	~TestMqttTeleInfo();
	static void ThreadStart(MqttTeleInfo* pMqttOwfs);
	void on_message(const std::string& topic, const std::string& message);

	bool Start();
	bool Refresh();
	bool Stop();

private:
	void waitMsg(size_t maxMsg, int maxTime);
	MqttTeleInfo mqttTeleInfo;
	MqttClient mqttClient;
	MockSerial mockSerial;
	std::map<std::string, std::string> m_Messages;
};
