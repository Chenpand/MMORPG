#include "WorldVerifyHandler.h"
#include <CLSysProtocol.h>
#include <CLErrorCode.h>

#include "VerifyServerMgr.h"
#include "WSNetwork.h"
#include "DungeonRaceMgr.h"
#include "WSPlayerMgr.h"
#include "SysRecordMgr.h"
#include "WSApplication.h"
#include "PunishmentMgr.h"
#include "GuildDungeonVerifyMgr.h"

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSInfo& protocol)
{
	for (auto tmpNode : protocol.nodes)
	{
		auto node = VerifyServerMgr::Instance()->FindNode(tmpNode.GetID());
		if (node)
		{
			continue;
		}

		node = new CLVerifyNode();
		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->Connect(tmpNode.GetAddr());
		if (conn != NULL)
		{
			CLProtocol::SysLoginReq req;
			req.nodeid = NODE_ID;

			CL_SEND_PROTOCOL(conn, req);
		}
		else
		{
			CL_SAFE_DELETE(node);
			continue;
		}

		node->Init(tmpNode.GetID(), tmpNode.GetAddr(), conn);
		node->SetAbality(tmpNode.GetAbality());
		VerifyServerMgr::Instance()->AddNode(node);
	}
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOnline& protocol)
{
	auto node = VerifyServerMgr::Instance()->FindNode(protocol.node.GetID());
	if (node)
	{
		return;
	}

	node = new CLVerifyNode();
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->Connect(protocol.node.GetAddr());
	if (conn != NULL)
	{
		CLProtocol::SysLoginReq req;
		req.nodeid = NODE_ID;

		CL_SEND_PROTOCOL(conn, req);
	}
	else
	{
		return;
	}

	node->Init(protocol.node.GetID(), protocol.node.GetAddr(), conn);
	node->SetAbality(protocol.node.GetAbality());
	VerifyServerMgr::Instance()->AddNode(node);
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyVSOffline& protocol)
{
	VerifyServerMgr::Instance()->DelNode(protocol.id);
}

void HandleProtocol(CLProtocol::VerifyCenterNotifyAbility& protocol)
{
	VerifyServerMgr::Instance()->UpdateAbality(protocol.nodeId, protocol.ability);
}

void HandleProtocol(CLProtocol::VerifyCreateTaskRes& res)
{
	auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(res.raceId);
	if (!race)
	{
		ErrorLogStream << "can't find race(" << res.raceId << ")." << LogStream::eos;

		// 通知放弃比赛
		if (WSApplication::Instance()->NotifyGiveupVerifyWhenRaceUnexist())
		{
			InfoLogStream << "notify verify server give up unexist race(" << res.raceId << ")" << LogStream::eos;
			CLProtocol::VerifyReportRaceDismiss report;
			report.raceId = res.raceId;
			res.SendBack(report);
		}
		
		return;
	}

	if (res.result != 0)
	{
		// 验证任务创建失败
		ErrorLogStream << "race(" << res.raceId << ") create verify task failed, reason(" << res.result << ")." << LogStream::eos;
		return;
	}

	DebugLogStream << "race(" << res.raceId << ") create verify task success, node(" << res.GetConnID() << ")." << LogStream::eos;
	race->OnCreateVerifyReturn(res.GetConnID());
}

void HandleProtocol(CLProtocol::VerifyReportTaskResult& protocol)
{
	DebugLogStream << "recv VerifyReportTaskResult raceId : " << protocol.raceId << ", roleId : " << protocol.roleId << ", result : " << protocol.result << LogStream::eos;
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!player)
	{
		DebugLogStream << "not find player raceId : " << protocol.raceId  << LogStream::eos;
		return;
	}

	if (protocol.result != PLAYER_CHEAT_REASON_NONE)
	{
		ErrorLogStream << "race(" << protocol.raceId << ") player(" << protocol.roleId << ", " << player->GetAccId() << ") dungeon(" << protocol.dungeonId << ") cheated." << LogStream::eos;

		PunishmentMgr::Instance()->OnReportPlayerCheat(player, (PlayerCheatReason)protocol.result);
		player->SendCheatLog((PlayerCheatReason)protocol.result, protocol.raceId, protocol.dungeonId);
		SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0, 1);
		auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(protocol.dungeonId);

		if (dungeonData && dungeonData->subType == DST_DEVILDDOM)
		{
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT_DEVIL, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0, 1);
			UInt32 cheatNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT_DEVIL, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0);

			if (cheatNum >= WSApplication::Instance()->DevilddomDailyCheatNum())
			{
				InfoLogStream << "player(" << protocol.roleId << ", " << player->GetAccId() << ") devilddom cheat, curNum(" << cheatNum << ") max(" << WSApplication::Instance()->DevilddomDailyCheatNum() << ")." << LogStream::eos;
				player->KickOff(8932, "cheat");
				return;
			}
		}

		if (dungeonData && dungeonData->subType == DST_GUILD_DUNGEON)
		{
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_DUNGEON_GUILD_INDEX, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0, 1);
			UInt32 cheatNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_GUILD_INDEX, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0);

			if (cheatNum >= WSApplication::Instance()->GuildDungeonDailyCheatNum())
			{
				InfoLogStream << "player(" << protocol.roleId << ", " << player->GetAccId() << ") guild dungeon cheat, curNum(" << cheatNum << ") max(" << WSApplication::Instance()->GuildDungeonDailyCheatNum() << ")." << LogStream::eos;
				player->KickOff(8932, "cheat");
				return;
			}
		}

		if (dungeonData && dungeonData->type == LT_TOWER && WSApplication::Instance()->TowerKickWhenCheat())
		{
			InfoLogStream << "player(" << protocol.roleId << ") tower cheat." << LogStream::eos;
			player->KickOff(8932, "cheat");
			return;
		}

		if (dungeonData && dungeonData->subType == DST_RAID_DUNGEON)
		{
			SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_TEAM_COPY_INDEX, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0, 1);
			UInt32 cheatNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_TEAM_COPY_INDEX, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0);

			UInt32 forbidTime = WSApplication::Instance()->GetTeamCopyCheatForbidTime(cheatNum);
			if (forbidTime != 0)
			{
				PunishmentMgr::Instance()->Punish(player->GetAccId(), PUNISH_FORBIDLOGIN, forbidTime * 60, FORBID_LOGIN_CHEAT_TEAM_COPY);
				InfoLogStream << PLAYERINFO(player) << "team copy cheat forbid! num:" << cheatNum << " time" << forbidTime << LogStream::eos;

				player->SendUdpLog("team_copy_cheat", LOG_TEAM_COPY_CHEAT, cheatNum, forbidTime);
			}

			if (cheatNum >= WSApplication::Instance()->TeamCopyDailyCheatNum())
			{
				InfoLogStream << "player(" << protocol.roleId << ", " << player->GetAccId() << ") team copy cheat, curNum(" << cheatNum << ") max(" << WSApplication::Instance()->TeamCopyDailyCheatNum() << ")." << LogStream::eos;
				player->KickOff(8932, "cheat");
				return;
			}
		}

		UInt32 cheatNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_DUNGEON_CHEAT, SYS_BEHAVIOR_OWNER_ROLE, protocol.roleId, 0);
		if (cheatNum >= WSApplication::Instance()->GetKickCheatNum())
		{
			InfoLogStream << "player(" << protocol.roleId << ", " << player->GetAccId() << ") daily cheat(" << cheatNum << ")." << LogStream::eos;
			player->KickOff(8932, "cheat");
			return;
		}
	}
	else
	{
		// 验证成功
		auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(protocol.raceId);
		if (race)
		{
			race->OnVerifySuccess(protocol.roleId);
		}
	}
	
}

void HandleProtocol(CLProtocol::VerifyReportTaskRaceEnd& protocol)
{
	auto raceid = protocol.raceId;
	DebugLogStream << "recv race(" << raceid << ")'s verify race end ." << LogStream::eos;

	if (protocol.raceType != RT_DUNGEON)
	{
		return;
	}

	// 解析结算信息
	CLProtocol::RelaySvrDungeonRaceEndReq raceEnd;
	try
	{
		auto packet = protocol.packet;
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		
		if (!raceEnd.Decode(packet))
		{
			ErrorLogStream << "[VerifyRaceEnd] decode race(" << raceid << ")'s dungeon race end failed." << LogStream::eos;
			return;
		}

		DebugLogStream << "[VerifyRaceEnd] race(" << raceid << ")'s race end success, " << raceEnd.raceEndInfo.ToString() << LogStream::eos;
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "[VerifyRaceEnd] serialize race(" << raceid << ")'s dungeon race end failed." << e.what() << LogStream::eos;
		return;
	}

	// 不是公会副本的，不走下面的流程
	DungeonDataEntry* dungeonEntry = DungeonDataEntryMgr::Instance()->FindEntry(raceEnd.raceEndInfo.dungeonId);
	if (NULL == dungeonEntry || dungeonEntry->subType != DST_GUILD_DUNGEON)
	{
		return;
	}

	// 先找到比赛
	auto race = DungeonRaceMgr::Instance()->FindDungeonRaceById(raceid);
	if (!race)
	{
		race = DungeonRaceMgr::Instance()->FindDungeonRaceBySession(raceid);
	}

	if (!race)
	{
		ErrorLogStream << "Not Find Race:" << raceid << " DungeonId:" << raceEnd.raceEndInfo.dungeonId << LogStream::eos;
		return;
	}

	raceid = race->GetID();

	for (auto& iter : raceEnd.raceEndInfo.infoes)
	{
		GuildDungeonVerifyMgr::Instance()->VerifyServerRes(raceid, iter.roleId, raceEnd.raceEndInfo.dungeonId, iter.bossDamage, raceEnd.raceEndInfo.usedTime);
	}
}