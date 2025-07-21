#include "CLSystemValueDataEntry.h"

SystemValueDataEntry::SystemValueDataEntry()
{
	id = 0;
	type = SVT_INVALID;
	type2 = SVT_INVALID;
	type3 = SVT_INVALID;
	value = 0;
}

SystemValueDataEntry::~SystemValueDataEntry()
{
}

bool SystemValueDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	type = (SystemValueType)row.ReadUInt32();
	type2 = (SystemValueType)row.ReadUInt32();
	type3 = (SystemValueType)row.ReadUInt32();
	value = row.ReadUInt32();
	return true;
}

SystemValueDataEntryMgr::SystemValueDataEntryMgr()
{
}

SystemValueDataEntryMgr::~SystemValueDataEntryMgr()
{
}

bool SystemValueDataEntryMgr::AddEntry(SystemValueDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<SystemValueDataEntry>::AddEntry(entry)) return false;

	OnAddEntry(entry);

	return true;

}

void SystemValueDataEntryMgr::OnReload()
{
	class SystemValueDataEntryVisitor :public Avalon::DataEntryVisitor<SystemValueDataEntry>
	{
	public:
		SystemValueDataEntryVisitor(){}

		bool operator()(SystemValueDataEntry* entry)
		{
			SystemValueDataEntryMgr::Instance()->OnAddEntry(entry);
			return true;
		}
	};
	
	SystemValueDataEntryVisitor visitor;
	SystemValueDataEntryMgr::Instance()->Visit(visitor);
}

void SystemValueDataEntryMgr::OnAddEntry(SystemValueDataEntry* entry)
{
	if (entry->type != 0)
	{
		m_SystemValues[entry->type] = entry;
	}
	else if (entry->type2 != 0)
	{
		m_SystemValues[entry->type2] = entry;
	}
	else if (entry->type3 != 0)
	{
		m_SystemValues[entry->type3] = entry;
	}
}

UInt32 SystemValueDataEntryMgr::GetSystemValue(SystemValueType type)
{
	SystemValueMap::iterator iter = m_SystemValues.find(type);
	if (iter != m_SystemValues.end())
		return iter->second->value;
	return 0;
}

SystemSwitchData::SystemSwitchData()
{

}

SystemSwitchData::~SystemSwitchData()
{

}

bool SystemSwitchData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	value = row.ReadUInt8();
	return true;
}

SystemSwitchDataMgr::SystemSwitchDataMgr()
{

}

SystemSwitchDataMgr::~SystemSwitchDataMgr()
{

}

class SysSwitchVistor : public Avalon::DataEntryVisitor<SystemSwitchData>
{
public: 
	SysSwitchVistor()	
	{
		m_datas.clear();
	}
	~SysSwitchVistor()	{}

	bool operator()(SystemSwitchData* dataEntry)
	{
		if (dataEntry->value == 1)
		{
			return true;
		}

		m_datas.push_back(dataEntry);
		return true;
	}

	void GetData(std::vector<SystemSwitchData*>& datas)
	{
		datas = m_datas;
	}

private:

	std::vector<SystemSwitchData*> m_datas;
};

void SystemSwitchDataMgr::GetCloseSysInfo(std::vector<SystemSwitchData*>& infos)
{
	SysSwitchVistor visitor;
	Visit(visitor);
	visitor.GetData(infos);
}
