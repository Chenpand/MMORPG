#include "MasterSectRelation.h"

#include "WSPlayerMgr.h"
#include "CLRecordClient.h"

MasterSectRelation::MasterSectRelation(UInt8 type)
{
	m_Type = type;
	m_Guid = 0;
	m_AccId = 0;
	m_Status = FRIEND_MATCH_STATUS_OFFLINE;
}

MasterSectRelation::MasterSectRelation(UInt8 type, WSPlayer* player)
{
	m_Type = type;
	m_Guid = 0;
	SetID(player->GetID());
	m_AccId = player->GetAccId();
	m_Name = player->GetName();
	m_Level = player->GetLevel();
	m_Occu = player->GetOccu();
	m_vipLv = player->GetVipLvl();
	m_AddTime = UInt32(CURRENT_TIME.Sec());
	m_Status = player->GetMatchStatus();
}

void MasterSectRelation::LoadDBData(CLRecordCallback* callback)
{
	if (callback == NULL) return;

	SetID(callback->GetData("id"));
	m_AccId = callback->GetData("accid");
	SetDBData(callback);

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(GetID());
	if (player != NULL)
	{
		if (GetLevel() != player->GetLevel())
		{
			SetLevel(player->GetLevel());
		}
	}
}

void MasterSectRelation::InsertToDB(ObjID_t owner)
{
	CLInsertCommand *insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_mastersect_relation", m_Guid);
	insertCmd->PutData("id", GetID());
	insertCmd->PutData("accid", m_AccId);
	insertCmd->PutData("owner", owner);
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
}

void MasterSectRelation::SyncToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", m_Guid, true);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0) CLRecordClient::Instance()->SendCommand(updateCmd);
	else CL_SAFE_DELETE(updateCmd);
}

void MasterSectRelation::DeleteFromDB()
{
	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_mastersect_relation", m_Guid);
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}

bool MasterSectRelation::IsOnline() const
{
	return WSPlayerMgr::Instance()->FindNormalPlayer(GetID()) != NULL;
}

void MasterSectRelation::SetFinSchMark(bool isFinSch)
{
	m_IsFinshSchool = (UInt8)isFinSch;
}