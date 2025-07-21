#include "CounterMgr.h"

#include <AvalonDate.h>
#include <CLRecordClient.h>
#include "Player.h"
#include "SSApplication.h"


/**
*@brief 插入Counter回调
*/
class InsertCounterCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(InsertCounterCallback, Avalon::Mutex)
public:
	InsertCounterCallback(ObjID_t playerId, std::string counterName, UInt32 value)
		:CLRecordCallback(), m_PlayerId(playerId), m_CounterName(counterName), m_Value(value) {}
	~InsertCounterCallback(){}

public:
	void OnFinished()override
	{
		InfoLogStream << "Player(" << m_PlayerId << ")" << "Insert Counter name : " << m_CounterName.c_str() << " value(" << m_Value << ") " << LogStream::eos;
	}

	void OnFailed(UInt32 errorcode)override
	{
		ErrorLogStream << "Player(" << m_PlayerId << ")" << "Insert Counter name : " << m_CounterName.c_str() << " value(" << m_Value << ") insert fail! errorcode=" << errorcode << LogStream::eos;
	}

private:
	//玩家id
	ObjID_t m_PlayerId;
	std::string m_CounterName;
	UInt32 m_Value;
};


/**
*@brief 更新Counter回调
*/
class UpdateCounterCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(UpdateCounterCallback, Avalon::Mutex)
public:
	UpdateCounterCallback(ObjID_t playerId, ObjID_t counterId, UInt32 value)
		:CLRecordCallback(), m_PlayerId(playerId), m_CounterId(counterId), m_Value(value) {}
	~UpdateCounterCallback(){}

public:
	void OnFinished()override
	{
		InfoLogStream << "Player(" << m_PlayerId << ")" << "Update Counter Id : " << (UInt64)m_CounterId << " value(" << m_Value << ") " << LogStream::eos;
	}

	void OnFailed(UInt32 errorcode)override
	{
		ErrorLogStream << "Player(" << m_PlayerId << ")" << "Update Counter Id : " << (UInt64)m_CounterId << " value(" << m_Value << ") update fail!errorcode=" << errorcode << LogStream::eos;
	}

private:
	//玩家id
	ObjID_t m_PlayerId;
	ObjID_t m_CounterId;
	UInt32 m_Value;
};


CounterMgr::CounterMgr()
{
	m_pOwner = NULL;
}

CounterMgr::~CounterMgr()
{
}

UInt32 CounterMgr::IncCounter(const char* name, UInt32 value)
{
	if(name == NULL || value == 0) return 0;

	std::string strname(name);
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(strname);
	if (counterConfig == NULL || counterConfig->type == COUNTER_CYCLE_INVALID) return 0;

	SetDirty();
	CounterMap::iterator counterIter = m_Counters.find(strname);
	if(counterIter == m_Counters.end())
	{
		Counter counter;
		counter.id = CLRecordClient::Instance()->GenGuid();
		counter.updateTime = UInt32(CURRENT_TIME.Sec());
		counter.value = value;
		counter.dirty = true;
		InsertToDB(strname, counter);
		m_Counters.insert(std::make_pair(strname, counter));

		if (SSApplication::Instance()->IsCounterLog())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Insert IncCounter name : " << strname.c_str() << " value(" << value << ") " << LogStream::eos;
		}

		return value;
	}
	else
	{
		UInt32 oldValue = counterIter->second.value;
		if (counterIter->second.value == 0 || CounterCfg::Instance()->CheckReset(counterIter->second.updateTime, counterConfig))
		{
			counterIter->second.updateTime = UInt32(CURRENT_TIME.Sec());
			counterIter->second.value = value;
		}
		else
		{
			counterIter->second.updateTime = UInt32(CURRENT_TIME.Sec());
			counterIter->second.value += value;
		}

		SaveToDB(counterIter->second);
		counterIter->second.dirty = true;

		if (SSApplication::Instance()->IsCounterLog() && value != oldValue)
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Update IncCounter name : " << strname.c_str() << " value(" << value << " | " << oldValue << ") " << LogStream::eos;
		}

		return counterIter->second.value;
	}
}

UInt32 CounterMgr::IncCounterDynamic(const char* preName, UInt32 value, std::vector<UInt32>& suffix)
{
	if (!preName)
	{
		return 0;
	}

	std::ostringstream os;
	os << preName;

	for (size_t i = 0; i < suffix.size(); ++i)
	{
		os << suffix[i];
	}

	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	return IncCounter(os.str().c_str(), value);
}

UInt32 CounterMgr::GetCounter(const char* name)
{
	if(name == NULL) return 0;
	
	std::string strname(name);
	auto config = CounterCfg::Instance()->GetCycleConfig(strname);
	if (config == NULL) return 0;

	CounterMap::const_iterator counterIter = m_Counters.find(strname);
	if(counterIter == m_Counters.end()) return 0;

	if (counterIter->second.value == 0 || !CounterCfg::Instance()->CheckReset(counterIter->second.updateTime, config))
	{
		return counterIter->second.value;
	}
	SetCounter(name, 0);
	return counterIter->second.value;
}

UInt32 CounterMgr::GetCounterDynamic(const char* preName, std::vector<UInt32>& suffix)
{
	if (!preName)
	{
		return 0;
	}

	std::ostringstream os;
	os << preName;

	for (size_t i = 0; i < suffix.size(); ++i)
	{
		os << suffix[i];
	}
	
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	return GetOwner()->GetCount(os.str().c_str());
}

UInt32 CounterMgr::GetCounterWithoutCycle(const char* name)
{
	if (name == NULL) return 0;

	std::string strname(name);
	auto config = CounterCfg::Instance()->GetCycleConfig(strname);
	if (config == NULL) return 0;

	CounterMap::const_iterator counterIter = m_Counters.find(strname);
	if (counterIter == m_Counters.end()) return 0;

	UInt32 ret = counterIter->second.value;

	if (CounterCfg::Instance()->CheckReset(counterIter->second.updateTime, config))
	{
		SetCounter(name, 0);
	}
	
	return ret;
}

void CounterMgr::SetCounter(const char* name, UInt32 value)
{
	if (name == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " name is null!" << LogStream::eos;
		return;
	}

	std::string strname(name);
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(strname);
	if (counterConfig == NULL || counterConfig->type == COUNTER_CYCLE_INVALID)
	{
		if (counterConfig == NULL)
		{
			return;
		}

		if (counterConfig->type == COUNTER_CYCLE_INVALID)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " counterConfig is invalid!" << LogStream::eos;
		}
		
		return;
	}
		

	CounterMap::iterator counterIter = m_Counters.find(strname);
	if(counterIter == m_Counters.end())
	{
		if (value == 0)
		{
			return;
		}

		Counter counter;
		counter.id = CLRecordClient::Instance()->GenGuid();
		counter.updateTime = UInt32(CURRENT_TIME.Sec());
		counter.value = value;
		counter.dirty = true;
		InsertToDB(strname, counter);
		m_Counters.insert(std::make_pair(strname, counter));

		if (SSApplication::Instance()->IsCounterLog())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Insert SetCounter name : " << strname.c_str() << " value(" << value << ") " << LogStream::eos;
		}
	}
	else
	{
		UInt32 oldValue = counterIter->second.value;
		counterIter->second.updateTime = UInt32(CURRENT_TIME.Sec());
		counterIter->second.value = value;
		counterIter->second.dirty = true;
		SaveToDB(counterIter->second);
		
		if (SSApplication::Instance()->IsCounterLog())
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "Update SetCounter name : " << strname.c_str() << " value(" << value << " | " << oldValue << ") " << LogStream::eos;
		}
	}

    SetDirty();
}

void CounterMgr::SetCounterDynamic(const char* preName, UInt32 value, std::vector<UInt32>& suffix)
{
	if (!preName)
	{
		return;
	}

	std::ostringstream os;
	os << preName;

	for (size_t i = 0; i < suffix.size(); ++i)
	{
		os << suffix[i];
	}

	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	SetCounter(os.str().c_str(), value);
}

UInt32  CounterMgr::GetCounterUpdateTime(const char* name)
{
	if (name == NULL)
	{
		return 0;
	}

	std::string strname(name);
	CounterMap::iterator counterIter = m_Counters.find(strname);
	if (counterIter == m_Counters.end())
	{
		return 0;
	}

	return counterIter->second.updateTime;
}

void CounterMgr::RefreshCounterUpdateTime(const char* name)
{
	if (name == NULL)
	{
		return;
	}

	std::string strname(name);
	CounterMap::iterator counterIter = m_Counters.find(strname);
	if (counterIter == m_Counters.end())
	{
		return;
	}

	counterIter->second.updateTime = UInt32(CURRENT_TIME.Sec());

	SaveToDB(counterIter->second);
	counterIter->second.dirty = true;
}

void CounterMgr::OnOnline(bool isDayChanged)
{
	if (isDayChanged)
	{
		RefreshCounter();
	}
}

void CounterMgr::OnTick(const Avalon::Time& now)
{
// 	for (auto itr : m_Counters)
// 	{
// 		GetCounter(itr.first.c_str());
// 	}
}

void CounterMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	if(callback == NULL) return;

	while(callback->NextRow())
	{
		Counter counter;
		counter.id = callback->GetKey();
		counter.updateTime = callback->GetData("updatetime");
		counter.value = callback->GetData("value");
		m_Counters.insert(std::make_pair(callback->GetData("name").GetString(), counter));
	}
}

void CounterMgr::InsertToDB(const std::string& name, const Counter& counter)
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_counter", counter.id);
	insertCmd->PutData("owner", GetOwner()->GetID());
	insertCmd->PutData("name", name);
	insertCmd->PutData("updatetime", counter.updateTime);
	insertCmd->PutData("value",	counter.value);
	if (SSApplication::Instance()->IsCounterLog())
	{
		CLRecordClient::Instance()->SendCommand(insertCmd, new InsertCounterCallback(GetOwner()->GetID(), name, counter.value));
	}
	else
	{
		CLRecordClient::Instance()->SendCommand(insertCmd);
	}
	
}

void CounterMgr::SaveToDB(const Counter& counter)
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_counter", counter.id, false);
	updateCmd->PutData("updatetime", counter.updateTime);
	updateCmd->PutData("value",	counter.value);

	if (SSApplication::Instance()->IsCounterLog())
	{
		CLRecordClient::Instance()->SendCommand(updateCmd, new UpdateCounterCallback(GetOwner()->GetID(), counter.id, counter.value));
	}
	else
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
}

void CounterMgr::RefreshCounter()
{
	for (auto itr : m_Counters)
	{
		GetCounter(itr.first.c_str());
	}
}

void CounterMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	CounterMap::iterator iter = m_Counters.begin();
	while (iter != m_Counters.end())
	{
		if ((*iter).second.dirty || !bDirty)
		{
			stream & (UInt8)1 & (*iter).first & (*iter).second.value;
			(*iter).second.dirty = false;
		}
		++iter;
	}
	stream & (UInt8)0;
}

void CounterMgr::OnDayChanged()
{
	RefreshCounter();
}

void CounterMgr::OnLogicDayChanged()
{
	RefreshCounter();
}
