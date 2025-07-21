#include "WSJarMgr.h"
#include "CLRecordClient.h"
#include "WSSelectInitDataCallback.h"
#include "CLSystemValueDataEntry.h"
#include "CLItemDataEntry.h"

JarRewardRecordsMgr::JarRewardRecordsMgr()
{

}

JarRewardRecordsMgr::~JarRewardRecordsMgr()
{

}

void JarRewardRecordsMgr::QueryRecords(UInt32 id, std::vector<OpenJarRecord>& records)
{
	auto itr = m_records.find(id);
	if (itr == m_records.end())
	{
		return;
	}

	records = itr->second;
}

void JarRewardRecordsMgr::QueryRecords(std::vector<UInt32>& jarIds, std::vector<OpenJarRecord>& records)
{
	for (size_t i = 0; i < jarIds.size(); ++i)
	{
		auto itr = m_records.find(jarIds[i]);
		if (itr != m_records.end() && itr->second.size() > 0)
		{
			records.insert(records.end(), itr->second.begin(), itr->second.begin() + itr->second.size());
		}
	}
}

void JarRewardRecordsMgr::QueryEqrecRecords(std::vector<OpenJarRecord>& records)
{
	records = m_orderEqrecJars;
}

void JarRewardRecordsMgr::AddRecord(UInt32 id, std::string name, UInt32 itemId, UInt32 num, UInt32 color)
{
	UInt32 maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_JAR_RECORD_MAXNUM);

	UInt64 key1 = 0;
	std::vector<OpenJarRecord>& records = m_records[id];
	if (records.size() >= maxNum)
	{
		if (records.size() != 0)
		{
			key1 = records.begin()->uid;
			records.erase(records.begin());
			CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_jar_record", key1);
			CLRecordClient::Instance()->SendCommand(cmd);
		}
	}

	OpenJarRecord record;
	record.uid = CLRecordClient::Instance()->GenGuid();
	record.jarId = id;
	record.name = name;
	record.itemId = itemId;
	record.num = num;
	records.push_back(record);

	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(id);
	if (data && data->type == JT_EQRECO && color > ITEM_QUALITY_PURPLE)
	{
		if (m_orderEqrecJars.size() >= maxNum && m_orderEqrecJars.size() != 0)
		{
			m_orderEqrecJars.erase(m_orderEqrecJars.begin());
		}
		m_orderEqrecJars.push_back(record);
	}

	_SyncDB(record);
}

void JarRewardRecordsMgr::Init()
{
	_LoadDB();
}

class SelectJarRecordCb : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		while (NextRow())
		{
			OpenJarRecord record;
			record.uid = GetKey();
			record.jarId = GetData("jar_id");
			record.name = GetData("name").GetString();
			record.itemId = GetData("item_id");
			record.num = GetData("num");
			JarRewardRecordsMgr::Instance()->OnLoadRecord(record);
		}

		return true;
	}
};

void JarRewardRecordsMgr::_LoadDB()
{
	CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_jar_record");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectJarRecordCb());
}

void JarRewardRecordsMgr::OnLoadRecord(OpenJarRecord& record)
{
	UInt32 maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_JAR_RECORD_MAXNUM);
	//PrepareDeleteCommand(const std::string& table, DBKey_t key)
	//UInt64 key1 = 0;
	std::vector<OpenJarRecord>& records = m_records[record.jarId];
	if (records.size() >= maxNum)
	{
		if (records.size() != 0)
		{
			//key1 = records.begin()->uid;
			records.erase(records.begin());
			//CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_jar_record", key1);
			//CLRecordClient::Instance()->SendCommand(cmd);
		}
	}

	records.push_back(record);
	JarBonusDataEntry* data = JarBonusDataMgr::Instance()->FindEntry(record.jarId);
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(record.itemId);
	if (data && data->type == JT_EQRECO && itemData && itemData->color > ITEM_QUALITY_PURPLE)
	{
		if (m_orderEqrecJars.size() >= maxNum && m_orderEqrecJars.size() != 0)
		{
			m_orderEqrecJars.erase(m_orderEqrecJars.begin());
		}
		m_orderEqrecJars.push_back(record);
	}
}

void JarRewardRecordsMgr::_SyncDB(OpenJarRecord& record)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_jar_record", record.uid);
	cmd->PutData("jar_id", record.jarId);
	cmd->PutData("name", record.name);
	cmd->PutData("item_id", record.itemId);
	cmd->PutData("num", record.num);
	CLRecordClient::Instance()->SendCommand(cmd);
}
