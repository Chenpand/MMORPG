#include "AuctionAttentRecord.h"
#include "WSSelectInitDataCallback.h"
#include "CLErrorCode.h"
#include "WSPlayer.h"
#include "NewAuctionMgr.h"
#include "WSServiceSwitchMgr.h"
#include <CLRecordClient.h>

using namespace ErrorCode;

AuctionAttentRecord::AuctionAttentRecord()
{
}

AuctionAttentRecord::~AuctionAttentRecord()
{
}

void AuctionAttentRecord::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_auction_attention", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void AuctionAttentRecord::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_auction_attention", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void AuctionAttentRecord::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_auction_attention", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool AuctionAttentRecord::IsShouldDelete(const Avalon::Time &now)
{
	return m_delete_time > 0 && m_delete_time < (UInt32)now.Sec();
}

void AuctionAttentRecord::OnAttented()
{
	m_attent_time = (UInt32)CURRENT_TIME.Sec();
	m_delete_time = 0;
	Update();
}

void AuctionAttentRecord::OnCanceed()
{
	m_delete_time = (UInt32)CURRENT_TIME.Sec() + retent_time;
	m_attent_time = 0;
	Update();
}

AuctionAttentMgr::AuctionAttentMgr()
{
}

AuctionAttentMgr::~AuctionAttentMgr()
{
}

/**
*@breif 查询拍卖行关注
*/
class SelectAuctuinAttentCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return AuctionAttentMgr::Instance()->OnLoadRecordsRet(this);
	}
};

void AuctionAttentMgr::Init()
{
	this->LoadRecords();
	m_checkTimer.SetInterval(5 * 60 * 1000);
}

void AuctionAttentMgr::LoadRecords()
{
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_auction_attention");

	CLRecordClient::Instance()->SendCommand(selectCmd, new SelectAuctuinAttentCallback());
}

bool AuctionAttentMgr::OnLoadRecordsRet(CLRecordCallback *pCallback)
{
	if (pCallback == NULL) return false;
	while (pCallback->NextRow())
	{
		ObjID_t id = pCallback->GetKey();
		if (GetRecord(id) != NULL) continue;	//重复

		AuctionAttentRecord* record = new AuctionAttentRecord();
		record->SetDBData(pCallback);
		record->SetID(pCallback->GetKey());
		record->ClearDirty();
		this->AddOneRecord(record);
		if (record->IsValid())
		{
			AddOneAttentedRecord(record->m_owner_id, record);
			IncAuctionAttentNum(record->m_auction_id);
		}
	}
	return true;
}

AuctionAttentRecord* AuctionAttentMgr::GetRecord(UInt64 uid)
{
	AuctionAttentMap::iterator it = m_records.find(uid);
	if (it == m_records.end()) return NULL;

	return it->second;
}

AuctionAttentRecord* AuctionAttentMgr::GetRecord(ObjID_t playerId, UInt64 auctionId)
{
	for (auto it : m_records)
	{
		auto record = it.second;
		if (!record) continue;
		if (record->m_owner_id == playerId && record->m_auction_id == auctionId)
		{
			return record;
		}
	}
	return NULL;
}

void AuctionAttentMgr::RemoveAttentRecord(UInt64 auctionId)
{
	std::vector<UInt64> ids;
	for (auto iter : m_records)
	{
		if (iter.second && iter.second->m_auction_id == auctionId)
		{
			ids.push_back(iter.first);
			iter.second->Delete();
			CL_SAFE_DELETE(iter.second);
		}
	}
	for (auto id : ids)
	{
		m_records.erase(id);
	}
}

bool AuctionAttentMgr::AddOneRecord(AuctionAttentRecord* record)
{
	if (!record) return false;
	AuctionAttentMap::iterator it = m_records.find(record->GetID());
	if (it != m_records.end()) return false;
	m_records.insert(std::pair<UInt64, AuctionAttentRecord*>(record->GetID(), record));
	return true;
}

bool AuctionAttentMgr::CheckAttentIsFull(ObjID_t playerId)
{
	PlayerAuctionAttents::iterator iter = m_player_auct_attents.find(playerId);
	if (iter == m_player_auct_attents.end())
	{
		return false;
	}
	UInt32 maxnum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUCTION_ATTENT_MAXNUM);
	maxnum = maxnum > 0 ? maxnum : 12;
	if (iter->second.size() >= maxnum)
	{
		return true;
	}
	return false;
}

void AuctionAttentMgr::IncAuctionAttentNum(UInt64 auctionId)
{
	m_auct_attent_num[auctionId]++;
}

void AuctionAttentMgr::DecAuctionAttentNum(UInt64 auctionId)
{
	if (m_auct_attent_num[auctionId] > 0)
	{
		m_auct_attent_num[auctionId]--;
	}
}

UInt32 AuctionAttentMgr::GetAuctionAttentNum(UInt64 auctionId)
{
	return m_auct_attent_num[auctionId];
}

void AuctionAttentMgr::RemoveAuctionAttentNum(UInt64 auctionId)
{
	m_auct_attent_num.erase(auctionId);
}

AuctionAttentRecord* AuctionAttentMgr::GetAttentedRecord(ObjID_t playerId, UInt64 auction_guid)
{
	PlayerAuctionAttents::iterator iter = m_player_auct_attents.find(playerId);
	if (iter == m_player_auct_attents.end())
	{
		return NULL;
	}

	std::list<AuctionAttentRecord*>& records = iter->second;

	for (auto record : records)
	{
		if (record && record->m_auction_id == auction_guid)
		{
			return record;
		}
	}
	return NULL;
}

void AuctionAttentMgr::AddOneAttentedRecord(ObjID_t playerId, AuctionAttentRecord* record)
{
	if (!record) return;
	m_player_auct_attents[playerId].push_back(record);
}

void AuctionAttentMgr::RemoveAttentedRecord(ObjID_t playerId, AuctionAttentRecord* record)
{
	if (!record) return;
	PlayerAuctionAttents::iterator iter = m_player_auct_attents.find(playerId);
	if (iter == m_player_auct_attents.end())
	{
		return;
	}
	std::list<AuctionAttentRecord*>& records = iter->second;
	std::list<AuctionAttentRecord*>::iterator  it = records.begin();
	for (; it != records.end(); ++it)
	{
		if (*it == record)
		{
			records.erase(it);
			return;
		}
	}
}

void AuctionAttentMgr::GetAttentedAuctions(ObjID_t playerId, std::vector<UInt64>& auctionIds)
{
	PlayerAuctionAttents::iterator iter = m_player_auct_attents.find(playerId);
	if (iter == m_player_auct_attents.end() || iter->second.empty())
	{
		return;
	}
	std::list<AuctionAttentRecord*>& records = iter->second;
	std::list<AuctionAttentRecord*>::iterator  it = records.begin();
	for (; it != records.end(); ++it)
	{
		auctionIds.push_back((*it)->m_auction_id);
	}
}

void AuctionAttentMgr::RemoveAttentedRecordsByOnAuctionOffSale(UInt64 auctionId)
{
	PlayerAuctionAttents::iterator iter = m_player_auct_attents.begin();
	for (; iter != m_player_auct_attents.end(); ++iter)
	{
		std::list<AuctionAttentRecord*>& records = iter->second;
		std::list<AuctionAttentRecord*>::iterator it = records.begin();
		for ( ; it != records.end(); )
		{
			if ((*it)->m_auction_id == auctionId)
			{
				records.erase(it++);
			}
			else{
				++it;
			}
		}
	}
}

void AuctionAttentMgr::OnTick(const Avalon::Time &now)
{
	std::vector<AuctionAttentRecord*> records;
	for (auto it : m_records)
	{
		if (it.second && it.second->IsShouldDelete(now))
		{
			records.push_back(it.second);
		}
	}

	for (auto record : records)
	{
		if (record)
		{
			m_records.erase(record->GetID());
			record->Delete();
			CL_SAFE_DELETE(record);
		}
	}
}

UInt32 AuctionAttentMgr::HandleAttentReq(WSPlayer* player, UInt64 auction_guid)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS))
	{
		return ITEM_DATA_INVALID;
	}
	zjy_auction::AuctionInfo* auctionInfo = zjy_auction::AuctionMgr::Instance()->_FindAuction(auction_guid);
	if (!auctionInfo)
	{
		return AUCTION_OFFSALE;
	}
	if (!auctionInfo->isTreas)
	{
		return ITEM_DATA_INVALID;
	}

	if (auctionInfo->duetime <= (UInt32)CURRENT_TIME.Sec())
	{
		return AUCTION_OFFSALE;
	}

	auto record = GetAttentedRecord(player->GetID(), auction_guid);
	if (record) //已关注
	{
		return HandleCancelAttentAuction(player, *auctionInfo);
		
	}
	else //未关注
	{
		return HandleAttentAuction(player, *auctionInfo);
	}
}

UInt32 AuctionAttentMgr::HandleAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo)
{
	if (!player) return ITEM_DATA_INVALID;
	ObjID_t playerId = player->GetID();
	ObjID_t auction_guid = auctionInfo.guid;

	if (CheckAttentIsFull(playerId))
	{
		return AUCTION_ATTENT_FULL;
	}

	if (GetAttentedRecord(playerId, auction_guid))
	{
		return ITEM_DATA_INVALID;
	}

	auto record = GetRecord(playerId, auction_guid);
	if (record == NULL)
	{
		record = AddNewRecord(playerId, auction_guid);
	}
	if (!record)
	{
		ErrorLogStream << "record is null playerId=" << playerId << ", auctionId="<< auction_guid << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	record->OnAttented();
	AddOneAttentedRecord(playerId, record);
	IncAuctionAttentNum(auction_guid);
	//zjy_auction::AuctionMgr::Instance()->SyncAuctions(player, auctionInfo);
	return SUCCESS;
}

UInt32 AuctionAttentMgr::HandleCancelAttentAuction(WSPlayer* player, zjy_auction::AuctionInfo& auctionInfo)
{
	if (!player) return ITEM_DATA_INVALID;
	ObjID_t playerId = player->GetID();
	ObjID_t auction_guid = auctionInfo.guid;
	
	auto record = GetAttentedRecord(playerId, auction_guid);
	if (!record)
	{
		return ITEM_DATA_INVALID;
	}
	record->OnCanceed();
	RemoveAttentedRecord(playerId, record);
	DecAuctionAttentNum(auction_guid);
	//zjy_auction::AuctionMgr::Instance()->SyncAuctions(player, auctionInfo);
	return SUCCESS;
}

void AuctionAttentMgr::OnAuctionOffSale(const zjy_auction::AuctionInfo& auctionInfo)
{
	if (!auctionInfo.isTreas) return;

	RemoveAttentedRecordsByOnAuctionOffSale(auctionInfo.guid);
	RemoveAuctionAttentNum(auctionInfo.guid);
	RemoveAttentRecord(auctionInfo.guid);
}

void AuctionAttentMgr::GetAuctionAttentInfo(zjy_auction::AuctionInfo& auctionInfo, ObjID_t playerId)
{
	auctionInfo.attentNum = GetAuctionAttentNum(auctionInfo.guid);
	auto attent = GetAttentedRecord(playerId, auctionInfo.guid);
	auctionInfo.attent = attent ? 1 : 0;
	auctionInfo.attentTime = attent ? attent->m_attent_time : 0;
}

AuctionAttentRecord* AuctionAttentMgr::AddNewRecord(ObjID_t playerId, UInt64 auctionId)
{
	AuctionAttentRecord* record = new AuctionAttentRecord();
	record->m_owner_id = playerId;
	record->m_auction_id = auctionId;
	record->m_delete_time = 0;

	record->Update();
	m_records.insert(std::pair<UInt64, AuctionAttentRecord*>(record->GetID(), record));
	return record;
}