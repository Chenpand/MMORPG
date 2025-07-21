#include "ExpeditionMember.h"

#include <CLRecordClient.h>


ExpeditionMember::ExpeditionMember()
{
	m_AccId = 0;
	m_RoleId = 0;
	m_CreateTime = 0;
}

void ExpeditionMember::DeleteExpeditionMember(ExpeditionMember*& p)
{
	//delete p;
	CL_SAFE_DELETE(p);
}

bool ExpeditionMember::CheckExpeditionMemberExpire()
{
	//UInt32 curTime = (UInt32)CURRENT_TIME.Sec();
	//UInt32 expireTime = GetCreateTime() + GetDuration();
	//if (curTime < expireTime)
	//{
	//	return false;
	//}
	//InfoLogStream << LogStr() << " expired, create time(" << GetCreateTime() << ", duration(" << GetDuration() << "), expire time(" << expireTime << ")" << LogStream::eos;
	return true;
}

const std::string ExpeditionMember::LogStr()
{
	std::ostringstream oss;
	oss << "Expedition member(" << GetID() << "," << GetAccId() << "," << GetRoleId() << ")";
	return oss.str();
}

void ExpeditionMember::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		m_CreateTime = (UInt32)CURRENT_TIME.Sec();
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_expedition_member", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_expedition_member", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}

void ExpeditionMember::DeleteFromDB()
{
	InfoLogStream << LogStr() << " delete..." << LogStream::eos;

	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_expedition_member", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}