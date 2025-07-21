#include "PVPLogic.h"
#include <CLMatchProtocol.h>
#include "VerifyMgr.h"
#include "VerifyTask.h"

bool PVPLogic::OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket)
{
	if (!ILogic::OnInit(task, raceInitPacket))
	{
		return false;
	}

	if (!VerifyMgr::Instance()->LC_StartPVP(GetID(), raceInitPacket))
	{
		ErrorLogStream << "task(" << GetID() << ") start pvp failed." << LogStream::eos;
		return false;
	}

	CLProtocol::WorldNotifyRaceStart init;
	try
	{
		if (!init.Decode(raceInitPacket))
		{
			return false;
		}
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << e.what();
	}

	for (auto& fighter : init.players)
	{
		m_Task->AddGamer(fighter.roleId, fighter.pos);
	}

	return true;
}

bool PVPLogic::IsNeedReportRaceEndToServer() const
{
	return true;
}