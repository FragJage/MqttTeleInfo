#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "MqttTeleInfo.h"
#include "SimpleFolders.h"

using namespace std;

MqttTeleInfo::MqttTeleInfo(ISerialCommunicator* serialCommunicator) : MqttDaemon("teleinfo", "MqttTeleInfo"), m_RefreshIndexesInterval(120), m_RefreshInstantValuesInterval(30), m_SerialCommunicator(serialCommunicator)
{
    if(m_SerialCommunicator==nullptr) m_SerialCommunicator = &m_SerialTeleInfo;
}

MqttTeleInfo::~MqttTeleInfo()
{
}

void MqttTeleInfo::DaemonConfigure(SimpleIni& iniFile)
{
	int ivalue;
	string svalue;

	svalue = iniFile.GetValue("teleinfo", "SerialPort", "/dev/ttyAMA0");
	m_SerialCommunicator->SetSerialPort(svalue);

	ivalue = iniFile.GetValue("teleinfo", "RefreshIndexesInterval", 120);
	m_RefreshIndexesInterval = ivalue;

	ivalue = iniFile.GetValue("teleinfo", "RefreshInstantValuesInterval", 30);
	m_RefreshInstantValuesInterval = ivalue;
}

string MqttTeleInfo::GetPTEC(const map<string, string>& trame)
{
    map<string, string>::const_iterator it = trame.find("PTEC");
    if(it == trame.end()) return "";

    string ptec = it->second;
    if(ptec.at(2)=='.') ptec = ptec.substr(0,2);

    return ptec;
}

bool MqttTeleInfo::RefreshValues(const string& key, map<string, string> trame, bool withPTEC, bool forceRefresh)
{
    map<string, string>::iterator itnew = trame.find(key);
    map<string, string>::iterator itold = m_OldTrame.find(key);

    if(itnew == trame.end()) return false;
    if((!forceRefresh) && (itold != m_OldTrame.end()) && (itold->second==itnew->second) ) return false;

    if((key=="PTEC") && (itold != m_OldTrame.end()) && (m_OldTrame[key] != trame[key]))
    {
        string ptec = GetPTEC(m_OldTrame);
        m_MqttQueue.emplace("IINST-"+ptec, "0");
        m_MqttQueue.emplace("PAPP-"+ptec, "0");
        LOG_VERBOSE(m_Log) << "Reset value for IINST-" << ptec << " and PAPP-" << ptec;
    }

    m_OldTrame[key] = trame[key];

    if(withPTEC)
    {
        map<string, string>::iterator itPTEC = trame.find("PTEC");
        if(itPTEC == trame.end()) withPTEC = false;
    }

    lock_guard<mutex> lock(m_MqttQueueAccess);
    if(withPTEC)
    {
        string key2 = key+"-"+GetPTEC(trame);
        m_MqttQueue.emplace(key2, trame[key]);
        LOG_VERBOSE(m_Log) << "New value for " << key2 << " : " << trame[key];
    }
    else
    {
        m_MqttQueue.emplace(key, trame[key]);
        LOG_VERBOSE(m_Log) << "New value for " << key << " : " << trame[key];
    }

	return true;
}

void MqttTeleInfo::Refresh(bool forceRefresh)
{
	 time_t timeNow = time((time_t*)0);
	 static time_t lastRefreshIndexes = timeNow;
	 static time_t lastRefreshInstantValues = timeNow;
	 bool refreshIndexes = false;
	 bool refreshInstantValues = false;


	if((forceRefresh)||(timeNow-lastRefreshIndexes>=m_RefreshIndexesInterval))
    {
        lastRefreshIndexes=timeNow;
        if(forceRefresh)
            LOG_VERBOSE(m_Log) << "Refresh indexes forced";
        else
            LOG_VERBOSE(m_Log) << "Refresh indexes needed";
		refreshIndexes = true;
    }

	if((forceRefresh)||(timeNow-lastRefreshInstantValues>=m_RefreshInstantValuesInterval))
    {
        lastRefreshInstantValues=timeNow;
        if(forceRefresh)
            LOG_VERBOSE(m_Log) << "Refresh instantaneous values forced";
        else
            LOG_VERBOSE(m_Log) << "Refresh instantaneous values needed";
        refreshInstantValues = true;
    }

    if((!refreshIndexes)&&(!refreshInstantValues)) return;

    bool newvalue = false;
    map<string, string> trame = m_SerialCommunicator->GetTrame();

    if(refreshIndexes)
    {
        if(RefreshValues("BASE",    trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("HCHC",    trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("HCHP",    trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("EJPHN",   trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("EJPHPM",  trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHCJB", trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHPJB", trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHCJW", trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHPJW", trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHCJR", trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("BBRHPJR", trame, false, forceRefresh)) newvalue = true;
    }

    if(refreshInstantValues)
    {
        if(RefreshValues("PTEC",  trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("PEJP",  trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("DEMAIN",trame, false, forceRefresh)) newvalue = true;
        if(RefreshValues("IINST", trame, true,  forceRefresh)) newvalue = true;
        if(RefreshValues("PAPP",  trame, true,  forceRefresh)) newvalue = true;
    }

    if(newvalue) m_MqttQueueCond.notify_one();
}

void MqttTeleInfo::on_message(const string& topic, const string& message)
{
	LOG_VERBOSE(m_Log) << "Mqtt receive " << topic << " : " << message;

	string mainTopic = GetMainTopic();
	if (topic.substr(0, mainTopic.length()) != mainTopic)
	{
		LOG_WARNING(m_Log) << "Not for me (" << mainTopic << ")";
		return;
	}

	if (topic.substr(mainTopic.length(), 7) != "command")
	{
		LOG_WARNING(m_Log) << "Not a command (waiting " << mainTopic+"command" << ")";
		return;
	}

	if (topic.length() != mainTopic.length() + 7)
    {
		LOG_WARNING(m_Log) << "Invalid topic (waiting just " << mainTopic+"command" << ")";
		return;
    }

	if (message == "REFRESH") Refresh(true);
}

int MqttTeleInfo::DaemonLoop(int argc, char* argv[])
{
	LOG_ENTER;
	Subscribe(GetMainTopic() + "command/#");
	LOG_VERBOSE(m_Log) << "Subscript to : " << GetMainTopic() + "command/#";

	Refresh(true);
	bool bStop = false;
	bool bPause = false;
	while (!bStop)
	{
		int cond = Service::Get()->WaitFor({ m_MqttQueueCond }, 200);
		if (cond == Service::STATUS_CHANGED)
		{
			switch (Service::Get()->GetStatus())
			{
			case Service::StatusKind::PAUSE:
				bPause = true;
				break;
			case Service::StatusKind::START:
				bPause = false;
				cond = 1;
				break;
			case Service::StatusKind::STOP:
				bStop = true;
				break;
			}
		}
		if (!bPause)
		{
			Refresh(false);
			SendMqttMessages();
		}
	}

	LOG_EXIT_OK;
    return 0;
}

void MqttTeleInfo::SendMqttMessages()
{
	lock_guard<mutex> lock(m_MqttQueueAccess);
	while (!m_MqttQueue.empty())
	{
		MqttQueue& mqttQueue = m_MqttQueue.front();
		LOG_VERBOSE(m_Log) << "Send " << mqttQueue.Topic << " : " << mqttQueue.Message;
		Publish(mqttQueue.Topic, mqttQueue.Message);
		m_MqttQueue.pop();
	}
}
