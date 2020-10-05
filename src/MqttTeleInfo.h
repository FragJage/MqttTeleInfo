#ifndef MQTTTELEINFO_H
#define MQTTTELEINFO_H

#include <queue>
#include <map>
#include "MqttDaemon.h"
#include "ISerialCommunicator.h"
#include "SerialTeleInfo.h"


struct MqttQueue
{
	MqttQueue(std::string topic, std::string msg) : Topic(topic), Message(msg) {};
	std::string Topic;
	std::string Message;
};

class MqttTeleInfo : public MqttDaemon
{
    public:
        MqttTeleInfo(ISerialCommunicator* serialCommunicator=nullptr);
        ~MqttTeleInfo();

		int DaemonLoop(int argc, char* argv[]);
		void on_message(const std::string& topic, const std::string& message);

    private:
        std::string GetPTEC(const std::map<std::string, std::string>& trame);
		void DaemonConfigure(SimpleIni& iniFile);
		void SendMqttMessages();
		void Refresh(bool forceRefresh);
		bool RefreshValues(const std::string& key, std::map<std::string, std::string> trame, bool withPTEC, bool forceRefresh);

		std::mutex m_MqttQueueAccess;
		ServiceConditionVariable m_MqttQueueCond;
		std::queue<MqttQueue> m_MqttQueue;
		int m_RefreshIndexesInterval;
		int m_RefreshInstantValuesInterval;
		SerialTeleInfo m_SerialTeleInfo;
		ISerialCommunicator* m_SerialCommunicator;
		std::map<std::string, std::string> m_OldTrame;
};

#endif // MQTTTELEINFO_H
