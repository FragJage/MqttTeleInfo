#ifndef MQTTTELEINFO_H
#define MQTTTELEINFO_H

#include <queue>
#include <map>
#include "MqttDaemon.h"
#include "TeleInfo.h"

struct MqttQueue
{
	MqttQueue(std::string topic, std::string msg) : Topic(topic), Message(msg) {};
	std::string Topic;
	std::string Message;
};

class MqttTeleInfo : public MqttDaemon
{
    public:
        MqttTeleInfo();
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
		TeleInfo m_TeleInfo;
		int m_RefreshIndexesInterval;
		int m_RefreshInstantValuesInterval;
		std::map<std::string, std::string> m_OldTrame;
};

#endif // MQTTTELEINFO_H
