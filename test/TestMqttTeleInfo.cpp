#include <iostream>
#include "TestMqttTeleInfo.h"

using namespace std;

TestMqttTeleInfo::TestMqttTeleInfo() : TestClass("MqttTeleInfo", this), mqttTeleInfo(&mockSerial)
{
    mockSerial.SetNextTrame("BASE=0000000;HCHC=000001;HCHP=000002;EJPHN=000003;EJPHPM=000004;BBRHCJB=000005;BBRHPJB=000006;BBRHCJW=000007;BBRHPJW=000008;BBRHCJR=000009;BBRHPJR=000010;PTEC=TH..;PEJP=A;DEMAIN=Z;IINST=750;PAPP=520");

	addTest("Start", &TestMqttTeleInfo::Start);
	addTest("Refresh", &TestMqttTeleInfo::Refresh);
	addTest("Stop", &TestMqttTeleInfo::Stop);

	mqttClient.SetMessageCallback(this);
	mqttClient.Connect();
	mqttClient.Subscribe("teleinfo/#");
}

TestMqttTeleInfo::~TestMqttTeleInfo()
{
	mqttClient.Disconnect();
}

void TestMqttTeleInfo::ThreadStart(MqttTeleInfo* pMqttDev)
{
	char exeName[] = "test";
	char consArg[] = "-console";
	char confArg[] = "--configfile";
	char confName[] = "MqttTeleInfo.conf";
	char levelArg[] = "--loglevel";
	char levelName[] = "2";
	char destArg[] = "--logdestination";
	char destName[] = "cout";
	char* argv[8];

	argv[0] = exeName;
	argv[1] = consArg;
	argv[2] = confArg;
	argv[3] = confName;
	argv[4] = levelArg;
	argv[5] = levelName;
	argv[6] = destArg;
	argv[7] = destName;

	Service* pService = Service::Create("MqttTeleInfo", "Send teleinfo trame to Mqtt messages", pMqttDev);
	pService->Start(8, argv);
}

void TestMqttTeleInfo::on_message(const string& topic, const string& message)
{
	m_Messages[topic] = message;
}

void TestMqttTeleInfo::waitMsg(size_t maxMsg, int maxTime)
{
	int time = 0;
	bool stop = false;

	while (!stop)
	{
		if (m_Messages.size() >= maxMsg) stop = true;
		if (time > maxTime) stop = true;
		time += 50;
		Plateforms::delay(50);
	}
}

bool TestMqttTeleInfo::Start()
{
	thread integrationTest(ThreadStart, &mqttTeleInfo);
	integrationTest.detach();
	waitMsg(16, 2000);

	assert(m_Messages.size() == 16);

	map<string, string>::iterator it;
	it = m_Messages.find("teleinfo/BASE");
	assert(m_Messages.end() != it);
	assert(it->second == "0000000");
	it = m_Messages.find("teleinfo/HCHC");
	assert(m_Messages.end() != it);
	assert(it->second == "000001");
	it = m_Messages.find("teleinfo/PTEC");
	assert(m_Messages.end() != it);
	assert(it->second == "TH..");
	it = m_Messages.find("teleinfo/IINST-TH");
	assert(m_Messages.end() != it);
	assert(it->second == "750");
	it = m_Messages.find("teleinfo/PAPP-TH");
	assert(m_Messages.end() != it);
	assert(it->second == "520");
	m_Messages.clear();

	return true;
}

bool TestMqttTeleInfo::Refresh()
{
	mqttClient.Publish("teleinfo/command", "REFRESH");
	waitMsg(17, 1500);

	assert(m_Messages.size() == 17);

	map<string, string>::iterator it;
	it = m_Messages.find("teleinfo/EJPHN");
	assert(m_Messages.end() != it);
	assert(it->second == "000003");
	it = m_Messages.find("teleinfo/EJPHPM");
	assert(m_Messages.end() != it);
	assert(it->second == "000004");
	it = m_Messages.find("teleinfo/PEJP");
	assert(m_Messages.end() != it);
	assert(it->second == "A");

	m_Messages.clear();

	return true;
}

bool TestMqttTeleInfo::Stop()
{
	Service::Get()->ChangeStatus(Service::StatusKind::PAUSE);
	Plateforms::delay(350);
	Service::Get()->ChangeStatus(Service::StatusKind::START);
	Plateforms::delay(350);
	Service::Get()->ChangeStatus(Service::StatusKind::STOP);
	Plateforms::delay(205);

	return true;
}
