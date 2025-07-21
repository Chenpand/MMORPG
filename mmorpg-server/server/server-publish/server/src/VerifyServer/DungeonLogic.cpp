#include "DungeonLogic.h"
#include <CLMatchProtocol.h>
#include <CLDungeonProtocol.h>
#include <sstream>
#include "VerifyMgr.h"
#include "VerifyTask.h"
#include "VSApplication.h"

std::string DungeonLogic::ShowInfo() const
{
	std::stringstream ss;
	ss << "dungeon(" << m_DungeonId << ").";
	return ss.str();
}

bool DungeonLogic::OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket)
{
	if (!ILogic::OnInit(task, raceInitPacket))
	{
		return false;
	}

	CLProtocol::SceneDungeonStartRes init;
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

	m_DungeonId = init.dungeonId;
	for (auto& fighter : init.players)
	{
		m_Task->AddGamer(fighter.roleId, fighter.pos);
		if (fighter.roleId == 0)
		{
			continue;
		}

		InfoLogStream << "race(" << GetID() << ") dungeon(" << init.dungeonId << ") gamer(" << fighter.accid << ", " << fighter.roleId << ", " << fighter.name << ") start..." << LogStream::eos;
	}

	// 临时代码，魔炎之地都不需要验证
	if (VSApplication::Instance()->IsInDungeonWhiteList(m_DungeonId / 10))
	{
		InfoLogStream << "race(" << GetID() << ") dungeon(" << init.dungeonId << ") don't need vierfy..." << LogStream::eos;
		return false;
	}

	if (!VerifyMgr::Instance()->LC_StartPVE(GetID(), raceInitPacket))
	{
		ErrorLogStream << "task(" << GetID() << ") start pve failed." << LogStream::eos;
		return false;
	}

	return true;
}

bool DungeonLogic::IsNeedReportRaceEndToServer() const
{
	// 只有公会副本需要上报
	/*auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(GetDungeonID());
	if (!dungeonData && dungeonData->subType != DST_GUILD_DUNGEON)
	{
		return false;
	}*/

	return true;
}