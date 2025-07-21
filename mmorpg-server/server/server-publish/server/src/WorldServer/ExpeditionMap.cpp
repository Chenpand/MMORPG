#include "ExpeditionMap.h"

#include <CLRecordClient.h>


ExpeditionMap::ExpeditionMap()
{
	m_AccId = 0;
	m_ExpeditionMapId = 0;
	m_ExpeditionStartTime = 0;
	m_ExpeditionDuration = 0;
	m_ExpeditionStatus = EXPEDITION_STATUS_PREPARE;
}

ExpeditionMap::~ExpeditionMap()
{
}

void ExpeditionMap::DeleteExpeditionMap(ExpeditionMap*& p)
{
	//delete p;
	CL_SAFE_DELETE(p);
}

bool ExpeditionMap::CheckExpeditionMemberExpire(UInt32 checkTime)
{
	if (checkTime > GetExpeditionEndTime())
	{
		FinishExpedition();
		return true;
	}
	return false;
}

void ExpeditionMap::FinishExpedition()
{
	SetExpeditionStatus(EXPEDITION_STATUS_OVER);
	SaveToDB();
}

const std::string ExpeditionMap::LogStr()
{
	std::ostringstream oss;
	oss << "Expedition map(" << GetID() << "," << GetAccId() << "," << (UInt32)GetExpeditionMapId() << ")"
		<< ", status(" << (UInt32)GetExpeditionStatus() << "), time(" << GetExpeditionDuration()
		<< "," << GetExpeditionEndTime() << ")";
	return oss.str();
}

void ExpeditionMap::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_expedition_map", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_expedition_map", GetID(), true);
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

void ExpeditionMap::DeleteFromDB()
{
	InfoLogStream << LogStr() << " delete..." << LogStream::eos;

	CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_expedition_map", GetID());
	CLRecordClient::Instance()->SendCommand(deleteCmd);
}