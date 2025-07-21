#include "PlayerJoins.h"
#include <CLRecordClient.h>

#include "WSSelectInitDataCallback.h"
#include "CLRedPacketDefine.h"
#include "CLPlayerJoinEventDefine.h"
#include "GameParamMgr.h"
/**
*@breif 加载玩家参加事件记录
*/
class SelectPlayerJoinEventCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return PlayerJoinMgr::Instance()->OnLoadRecordsRet(this);
	}
};

PlayerList::PlayerList()
{}

PlayerList::~PlayerList()
{}

void PlayerList::AddOnePlayer(ObjID_t playerId)
{
	std::pair<std::set<ObjID_t>::iterator, bool> ret;
	ret = m_playerIds.insert(playerId);
	if (ret.second == true)
	{
		SetDirty();
	}
}

bool PlayerList::FindPlayer(ObjID_t playerId)
{
	auto iter = m_playerIds.find(playerId);
	return iter != m_playerIds.end();
}

UInt32 PlayerList::GetNum()
{
	return m_playerIds.size();
}

void PlayerList::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{}

void PlayerList::Decode(Avalon::NetInputStream& stream)
{}

void PlayerList::EncodeString(std::ostringstream& os)
{
	for (auto id : m_playerIds)
	{
		os << id << ",";
	}
}

void PlayerList::DecodeString(std::istringstream& is)
{
	UInt64 id = 0;
	char split;
	while ((is >> id) && id != 0)
	{
		m_playerIds.insert(id);
		is >> split;
	}
}

PlayerJoinRecord::PlayerJoinRecord(UInt8 type, UInt64 param1, UInt64 param2)
{
	m_type = type;
	m_param1 = param1;
	m_param2 = param2;
	m_create_time = (UInt32)CURRENT_TIME.Sec();
	m_saveTimer.SetLastTime(CURRENT_TIME.MSec());
	m_saveTimer.SetInterval(1000);
}

PlayerJoinRecord::~PlayerJoinRecord()
{
}

void PlayerJoinRecord::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_joins", GetID(), false);
		this->GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
		ClearDirty();
	}
}

void PlayerJoinRecord::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_player_joins", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void PlayerJoinRecord::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_player_joins", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void PlayerJoinRecord::OnTick(const Avalon::Time &now)
{
	if (m_saveTimer.IsTimeout(now))
	{
		Update();
	}
}

void PlayerJoinRecord::AddOnePlayer(ObjID_t playerId)
{
	m_join_players.AddOnePlayer(playerId);
}

bool PlayerJoinRecord::IsPlayerJoin(ObjID_t playerId)
{
	return m_join_players.FindPlayer(playerId);
}

bool PlayerJoinRecord::IsShouldDeleted(const Avalon::Time& now)
{
	return (m_create_time + PLAYER_JOINS_EVENT_SAVE_TIME) < (UInt32)now.Sec();
}

UInt32 PlayerJoinRecord::GetJoinNum()
{
	return m_join_players.GetNum();
}

PlayerJoinMgr::PlayerJoinMgr()
{}

PlayerJoinMgr::~PlayerJoinMgr()
{}

void PlayerJoinMgr::Init()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_joins");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectPlayerJoinEventCallback());
}

bool PlayerJoinMgr::OnTick(const Avalon::Time& now)
{
	std::vector<PlayerJoinRecordKey> deleteKeys;
	for (auto itr : m_records)
	{
		auto record = itr.second;
		if (!record)
		{
			continue;
		}

		record->OnTick(now);

		if (record->IsShouldDeleted(now))
		{
			deleteKeys.push_back(itr.first);
		}
	}

	for (auto key : deleteKeys)
	{
		RemoveRecord(key);
	}
	return true;
}

bool PlayerJoinMgr::OnLoadRecordsRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		UInt8  type = pCallback->GetData("type");
		UInt64 param1 = pCallback->GetData("param1");
		UInt64 param2 = pCallback->GetData("param2");
		PlayerJoinRecordKey key(type, param1, param2);
		if (_FindJoinRecord(key) != NULL)	continue;
		PlayerJoinRecord* record = CreateOneRecord(type, param1, param2);
		if (!record)
		{
			continue;
		}
		record->SetDBData(pCallback);
		record->SetID(pCallback->GetKey());
		record->ClearDirty();
	}
	return true;
}

void PlayerJoinMgr::AddOnePlayer(UInt8	type, UInt64 param1, UInt64 param2, ObjID_t playerId)
{
	ErrorLogStream << "type : " << type << ", param1 : " << param1 << ", param2 : " << param2 << ", playerId : " << playerId << LogStream::eos;
	PlayerJoinRecordKey key(type, param1, param2);
	PlayerJoinRecord* record = _FindJoinRecord(key);
	
	if (!record)
	{
		record = CreateOneRecord(type, param1, param2);
		if (!record)
		{
			ErrorLogStream << "create record fail!" << LogStream::eos;
			return;
		}
	}
	if (!record)
	{
		return;
	}

	record->AddOnePlayer(playerId);
}

PlayerJoinRecord* PlayerJoinMgr::CreateOneRecord(UInt8 type, UInt64 param1, UInt64 param2)
{
	PlayerJoinRecord* record = new PlayerJoinRecord(type, param1, param2);
	std::pair<std::map<PlayerJoinRecordKey, PlayerJoinRecord*>::iterator, bool> ret;
	PlayerJoinRecordKey key(type, param1, param2);
	ret = m_records.insert(std::pair<PlayerJoinRecordKey, PlayerJoinRecord*>(key, record));
	if (ret.second == false)
	{
		ErrorLogStream << "insert fail type : " << type << ", param1 : " << param1 << ", param2 : " << param2 << LogStream::eos;
		delete record;
		return NULL;
	}
	return record;
}

bool PlayerJoinMgr::IsPlayerJoinEvent(ObjID_t playerId, UInt8 type, UInt64 param1, UInt64 param2)
{
	PlayerJoinRecordKey key(type, param1, param2);
	PlayerJoinRecord* record = _FindJoinRecord(key);
	if (!record)
	{
		return false;
	}
	return record->IsPlayerJoin(playerId);
}

void PlayerJoinMgr::RemoveRecord(PlayerJoinRecordKey& key)
{
	PlayerJoinRecord* record = _FindJoinRecord(key);
	if (!record)
	{
		return;
	}
	record->Delete();
	CL_SAFE_DELETE(record);
	m_records.erase(key);
}

UInt32 PlayerJoinMgr::GetPlayerJoinNum(UInt8 type, UInt64 param1, UInt64 param2)
{
	PlayerJoinRecordKey key(type, param1, param2);
	PlayerJoinRecord* record = _FindJoinRecord(key);
	if (!record)
	{
		return 0;
	}
	return record->GetJoinNum();
}

PlayerJoinRecord* PlayerJoinMgr::_FindJoinRecord(PlayerJoinRecordKey& key)
{
	auto iter = m_records.find(key);
	if (iter == m_records.end())
	{
		return NULL;
	}
	return iter->second;
}