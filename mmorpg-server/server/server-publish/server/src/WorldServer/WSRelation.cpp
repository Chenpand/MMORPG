#include "WSRelation.h"

#include <CLRelationProtocol.h>
#include <CLMatchDefine.h>

#include "WSPlayerMgr.h"
#include "CLRecordClient.h"
#include "CLSystemValueDataEntry.h"

WSRelation::WSRelation(UInt8 type)
{
	m_Type = type;
	m_Guid = 0;
	m_AccId = 0;
	m_Status = FRIEND_MATCH_STATUS_OFFLINE;
	m_offlineTime.SetDBValue(0);
	m_DailyMasterTaskState = 0;
	m_HeadFrame = 0;
	m_GuildId = 0;
	m_ReturnYearTitle = 0;
}

WSRelation::WSRelation(UInt8 type, WSPlayer* player)
{
	m_Type = type;
	m_Guid = 0;
	SetID(player->GetID());
	m_AccId = player->GetAccId();
	m_Name = player->GetName();
	m_SeasonLv = player->GetSeasonLv();
	m_Level = player->GetLevel();
	m_Occu = player->GetOccu();
	m_giveRestTime = 0;
	m_dayGiveNum = FRIEND_DAY_GIVE_MAX;
	m_vipLv = player->GetVipLvl();
	m_AddTime = UInt32(CURRENT_TIME.Sec());
	m_Status = player->GetMatchStatus();
	m_dayMasterGiveNum.SetDBValue(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_MASTER_GIVE_MAX));
	m_DailyMasterTaskState = 0;
	m_HeadFrame = player->GetHeadFrame();
	m_GuildId = player->GetGuildID();
	m_ReturnYearTitle = player->GetReturnYearTitle();
}

WSRelation::~WSRelation()
{
}

void WSRelation::SetIntimacy(UInt16 intimacy)
{
	if (m_Type != RELATION_FRIEND 
		&& m_Type != RELATION_MASTER
		&& m_Type != RELATION_DISCIPLE)
	{
		return;
	}

	m_Intimacy = intimacy;
}


void WSRelation::SetCloseFriend(UInt8 flag)
{
	if(m_Type != RELATION_FRIEND) return;
	if(m_bCloseFriend == flag) return;

	m_bCloseFriend = flag;
}

void WSRelation::SetOnlineNotify(UInt8 bNotify)
{
	if(!IsSetOnlineNotify(m_Type)) return;
	if(bNotify > 1) return;

	m_OnlineNotify = bNotify;
}

bool WSRelation::IsOnline() const
{
	return WSPlayerMgr::Instance()->FindNormalPlayer(GetID()) != NULL;
}

UInt32 WSRelation::GetOfflineTime()
{
	return m_offlineTime.GetDBValue();
}

void WSRelation::SetOfflineTime(UInt32 offlineTime)
{
	m_offlineTime = offlineTime;
}

void WSRelation::UpdateGivePresentTime()
{
	// 好友赠礼时间到了重置
	if ((GetType() == RELATION_FRIEND || GetType() == RELATION_MASTER || GetType() == RELATION_DISCIPLE) &&
		(UInt32)CURRENT_TIME.Sec() >= GetGiveResetTime())
	{
		SetDayGiftNum(FRIEND_DAY_GIVE_MAX);
	}

}

void WSRelation::UpdateMasterGiveTime()
{
	// 好友赠礼时间到了重置
	if (GetType() == RELATION_DISCIPLE &&
		(UInt32)CURRENT_TIME.Sec() >= GetMasterGiveResetTime())
	{
		SetMasterDayGiftNum(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_MASTER_GIVE_MAX));
	}
}

void WSRelation::SetDailyMasterTaskState(UInt8 state)
{ 
	m_DailyMasterTaskState = state; 
}

void WSRelation::LoadDBData(CLRecordCallback* callback)
{
	if(callback == NULL) return;

	SetID(callback->GetData("id"));
	m_AccId = callback->GetData("accid");
	SetDBData(callback);

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(GetID());
	if(player != NULL)
	{
		if(GetLevel() != player->GetLevel())
		{
			SetLevel(player->GetLevel());
		}
	}
}

void WSRelation::InsertToDB(ObjID_t owner)
{
	CLInsertCommand *insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_relation", m_Guid);
	insertCmd->PutData("id",	GetID());
	insertCmd->PutData("accid",	m_AccId);
	insertCmd->PutData("owner", owner);
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
}

void WSRelation::SyncToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", m_Guid, true);
	GetDBData(updateCmd, true);
	if(updateCmd->GetDataNum() > 0) CLRecordClient::Instance()->SendCommand(updateCmd);
	else CL_SAFE_DELETE(updateCmd);
}

void WSRelation::DelMe()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_relation", m_Guid);
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}

void WSRelation::UpdateDailyMasterTaskStateOnLogicDayChange()
{
	if (this->GetType() == RelationType::RELATION_DISCIPLE)
	{
		if (this->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNRECEIVED)
		{
			this->SetDailyMasterTaskState(MasterDailyTaskState::MDTST_UNPUB_UNRECE);
			this->SyncToDB();
		}
		else
		{
			InfoLogStream << "UpdateDailyMasterTaskStateOnLogicDayChange RELATION_DISCIPLE relationId(" << this->GetGuid() << ") id(" << this->GetID() << ")" << LogStream::eos;
			this->SetDailyMasterTaskState(MasterDailyTaskState::MDTST_UNPUBLISHED);
			this->SyncToDB();
		}
	}
	else if (this->GetType() == RelationType::RELATION_MASTER)
	{
		InfoLogStream << "UpdateDailyMasterTaskStateOnLogicDayChange RELATION_MASTER relationId(" << this->GetGuid() << ") id(" << this->GetID() << ")" << LogStream::eos;
		this->SetDailyMasterTaskState(MasterDailyTaskState::MDTST_UNPUBLISHED);
		this->SyncToDB();
	}
}

void WSRelation::SyncToClient(WSPlayer* player)
{
	if (player == NULL) return;
	
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SYNC_RELATIONDATA);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	stream & GetID();
	if (this->Encode(stream, UInt32(1) << this->GetType(), true))
	{
		packet->SetSize(UInt16(stream.GetPos()));
		player->SendPacket(packet);
		packet->SetSize(0);
	}
	Avalon::Packet::Destroy(packet);
}

void WSRelation::SetMutualFriendMark()
{
	if (m_Mark & RELATION_MARK_MUTUAL_FRIEND)
	{
		return;
	}

	m_Mark = m_Mark | RELATION_MARK_MUTUAL_FRIEND;
	this->SyncToDB();
}

bool WSRelation::IsMutualFriend() 
{ 
	return m_Mark & RELATION_MARK_MUTUAL_FRIEND; 
}

void WSRelation::ClearMutualFriendMark()
{
	m_Mark = m_Mark & (~RELATION_MARK_MUTUAL_FRIEND);
	this->SyncToDB();
}