#include "WorldDungeonHandler.h"
#include "DropItemMgr.h"
#include <AvalonPacket.h>
#include <AvalonThread.h>
#include <AsyncMsgCallbackMgr.h>
#include "DungeonRaceMgr.h"
#include "WSPlayerMgr.h"
#include "SysRecordMgr.h"
#include "WSTeamMgr.h"
#include "AccountMgr.h"
#include "WSApplication.h"

void HandleProtocol(ObjID_t id, CLProtocol::SceneRareItemReq& req)
{
    // 稀有控制(现在先无视稀有控制)
    // ToDo...
    CLProtocol::SceneRareItemRes  res;
	res.roleId = req.roleId;

	if (req.items.size() == 0)
		return;

	ItemReward item;
	item.id = req.items[0].id;
	item.num = SysRecordMgr::Instance()->RareItemRemainNum(req.roleId, req.items[0].id, req.items[0].num);
	res.items.push_back(item);

    AsyncMsgCallbackMgr::Instance()->SendBackReturnMsg(id, req, res);
}

void HandleProtocol(CLProtocol::WorldDungeonHardUpdate& req)
{
	auto account = AccountMgr::Instance()->FindAccount(req.accid);
	if (!account)
	{
		ErrorLogStream << "account(" << req.accid << ") update dungeon(" << req.info.id << ") hard(" << req.info.unlockedHardType << ")." << LogStream::eos;
		return;
	}

	account->UpdateDungeonHardInfo(req.info.id, (DungeonHardType)req.info.unlockedHardType);
}

void HandleProtocol(CLProtocol::SceneGotRaceItem& req)
{
	SysRecordMgr::Instance()->RecordGotRareItem(req.roleId, req.itemId, req.num);
}

void HandleProtocol(CLProtocol::SceneDungeonChestReq& req)
{
    CLProtocol::SceneDungeonChestRes res;
    res.dungeonRaceId = req.dungeonRaceId;
    res.playerId = req.playerId;

    for (int i = 0; i < DCT_NUM; i++)
    {
        UInt32 dropSetId = req.dropSetIds[i];

        if (!dropSetId)
        {
            continue;
        }

		GenDropItemResult genChestRet = DropItemMgr::Instance()->GenRaceEndChest(0, req.playerId, dropSetId, req.occu);
        if (genChestRet.dropItems.size() != 1 && genChestRet.dropItems.size() != 0)
        {
            ErrorLogStream << "dungeon race(" << req.dungeonRaceId << ") player(" << req.playerId
                << ") gen chest(" << dropSetId << ") failed." << LogStream::eos;
            continue;
        }

        DungeonChest chest;
        chest.type = (DungeonChestType)i;
        if (genChestRet.dropItems.size() == 1)
        {
            chest = genChestRet.dropItems[0];
            chest.isRareControl = genChestRet.dropItems[0].isRareControl;
        }

		if (chest.isRareControl)
		{
			// 记录稀有控制
			SysRecordMgr::Instance()->RecordGotRareItem(req.playerId, chest.typeId, chest.num);
		}

        res.chests.push_back(chest);
    }

    req.SendBack(res);
}

void HandleProtocol(CLProtocol::WorldDungeonStartRaceReq& req)
{
	CLProtocol::WorldDungeonStartRaceRes res;
	
	res.isHell = req.isHell;
	res.isWeekHell = req.isWeekHell;
	res.isContinue = false;

	UInt32 dungeonId = req.dungeonId;
	UInt32 hellDungeonEntryId = dungeonId;
	if (res.isHell)
	{
		dungeonId = DungeonRaceMgr::Instance()->GenHellDungeonID(dungeonId);
	}
	else if (res.isWeekHell)
	{
		dungeonId = DungeonRaceMgr::Instance()->GenDungeonIDByEntry(dungeonId);
	}
	res.dungeonId = dungeonId;


//#ifndef _DEBUG
	if (WSApplication::Instance()->IsCheckRepeatStartDungeon())
	{
		for (auto& playerInfo : req.playerInfoes)
		{
			auto roleid = playerInfo.raceInfo.roleId;
			if (DungeonRaceMgr::Instance()->IsPlayerInRace(roleid))
			{
				ErrorLogStream << "player(" << roleid << ") is in race." << LogStream::eos;
				for (auto& playerInfo : req.playerInfoes)
				{
					res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
					res.roleId = playerInfo.raceInfo.roleId;
					req.SendBack(res);
				}

				return;
			}
		}
	}
	
//#endif

	//大富翁关卡只有随机到的时候才能进
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (dungeonData != nullptr)
	{
		if (dungeonData->subType == DST_PLAYGAME)
		{
			bool canJoin = true;
			for (auto& playerInfo : req.playerInfoes)
			{
				
				auto player = WSPlayerMgr::Instance()->FindNormalPlayer(playerInfo.raceInfo.roleId);
				if (player == nullptr)
				{
					InfoLogStream << "player: " << playerInfo.raceInfo.roleId << "can't start dungeon: " << dungeonId << LogStream::eos;
					res.result = ErrorCode::DUNGEON_START_TEAM_MEMBER_OFFLINE;
					res.roleId = playerInfo.raceInfo.roleId;
					req.SendBack(res);
					canJoin = false;
					break;
				}
				if (!player->GetMonopolyInfo().CanJoinMission(dungeonId))
				{
					InfoLogStream << "player: " << playerInfo.raceInfo.roleId << "can't start dungeon: " << dungeonId << LogStream::eos;
					res.result = ErrorCode::DUNGEON_START_CONDITION;
					res.roleId = playerInfo.raceInfo.roleId;
					req.SendBack(res);
					canJoin = false;
					break;
				}
			}
			if (!canJoin)
			{
				InfoLogStream << "can't join dungeon" << LogStream::eos;
				return;
			}
		}
	}

	auto dungeonRace = DungeonRaceMgr::Instance()->CreateDungeonRace(dungeonId);
	if (!dungeonRace)
	{
		for (auto& playerInfo : req.playerInfoes)
		{
			res.result = ErrorCode::DUNGEON_START_CREATE_RACE_FAILED;
			res.roleId = playerInfo.raceInfo.roleId;
			req.SendBack(res);
		}
		return;
	}
	dungeonRace->SetHellDungeonEntryID(hellDungeonEntryId);
	dungeonRace->SetTeamID(req.teamId);
	dungeonRace->SetCityMonster(req.cityMonster);

	for (auto& playerInfo : req.playerInfoes)
	{
		auto roleid = playerInfo.raceInfo.roleId;
		DungeonRaceMgr::Instance()->LeaveDungeonRace(roleid, false);
		DungeonRaceMgr::Instance()->JoinDungeonRace(dungeonRace, playerInfo);
	}

	dungeonRace->SetPassedAreaIndex(req.passedAreaIndex);
	if (req.isHell || req.isHellGuide)
	{
		dungeonRace->SetHell();
	}

	dungeonRace->SetZjslDungeonInfo(req.zjslDungeonInfo);
	UInt32 ret = dungeonRace->StartRace(req.session, req.relayAddr, req.relayId);
	if (ret != ErrorCode::SUCCESS)
	{
		// 不回消息，公会地下城CD
		if (ret != ErrorCode::GUILD_DUNGEON_CD)
		{
			for (auto& playerInfo : req.playerInfoes)
			{
				res.result = ret;
				res.roleId = playerInfo.raceInfo.roleId;
				req.SendBack(res);
			}
		}

		DungeonRaceMgr::Instance()->DestoryDungeonRace(dungeonRace);
		return;
	}

	if (req.session > 0)
	{
		DungeonRaceMgr::Instance()->AddDungeonRaceToSessionMgr(req.session, dungeonRace);
	}

	/*for (auto& playerInfo : req.playerInfoes)
	{
		res.result = 0;
		res.roleId = playerInfo.raceInfo.roleId;
		res.isAssist = playerInfo.isAssist;
		req.SendBack(res);
	}*/

	if (req.teamId > 0)
	{
		auto team = WSTeamMgr::Instance()->FindTeam(req.teamId);
		if (team)
		{
			team->SetStatus(TEAM_STATUS_RACING);
			team->ClearInviteList();
			for (auto member : team->GetAllMembers())
			{
				team->OnChangePlayerRacingStatus(member->GetID(), true);
			}
		}

		CLProtocol::WorldTeamDungeonRaceStart notify;
		notify.raceId = dungeonRace->GetID();
		notify.teamId = req.teamId;
		notify.cityMonster = dungeonRace->GetCityMonster();
		req.SendBack(notify);
	}
}

void HandleProtocol(CLProtocol::WorldDungeonLeaveRace& protocol)
{
	DungeonRaceMgr::Instance()->LeaveDungeonRace(protocol.roleId, protocol.isDisconnect != 0);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonEnterNextAreaReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to enter next area, but can't find race." << LogStream::eos;
		return;
	}

	CLProtocol::SceneDungeonEnterNextAreaRes res;
	res.areaId = req.areaId;
	res.result = race->OnPlayerEnterNextArea(player, req.areaId, req.lastFrame, req.staticVal);

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonRewardReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to get dungeon reward, but can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerPickUpDropItems(player, req.lastFrame, req.dropItemIds, req.md5);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonRaceEndReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request dungeon race end, but can't find race." << LogStream::eos;
		return;
	}

	if (race->GetSession() > 0)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request dungeon race end, race end info should send from relay server." << LogStream::eos;
		return;
	}

	race->OnPlayerRaceEnd(player, (LevelScore)req.score, req.beHitCount, req.usedTime, req.md5, req.lastFrame, req.maxDamage, 
							req.skillId, req.param, req.totalDamage, req.bossDamage, req.playerRemainHp, req.playerRemainMp, 
							req.boss1ID, req.boss2ID, req.boss1RemainHp, req.boss2RemainHp, req.lastChecksum);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonOpenChestReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to open chest, but can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerOpenChest(player, req.pos);
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonEndDropReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to race end drop, but can't find race." << LogStream::eos;
		return;
	}

	if (!race->OnPlayerRequestRaceEndDrop(player, req.multi))
	{
		CLProtocol::SceneDungeonEndDropRes res;
		res.multi = 0;
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonReviveReq& req)
{
	if (!player)
	{
		return;
	}

	auto race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to revive player, but can't find race." << LogStream::eos;
		return;
	}

	CLProtocol::SceneDungeonReviveRes res;
	res.result = race->OnPlayerReviveRequest(player, req.targetId, req.reviveId, req.reviveCoinNum);
	if (res.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonKillMonsterReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") request to kill monster, but can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerKilledMonster(player, req.monsterIds);
	}

void HandleProtocol(WSPlayer* player, CLProtocol::SceneDungeonClearAreaMonsters& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") clear area monsters, but can't find race." << LogStream::eos;
		return;
	}

	race->OnPlayerClearAreaMonsters(player, req.usedTime, req.remainHp, req.remainMp, req.md5, req.lastFrame);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonEnterRaceReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		//ErrorLogStream << "player(" << player->GetID() << ") enter race, but can't find race." << LogStream::eos;
		return;
	}

	//race->OnPlayerEnterRace(player);

	CLProtocol::WorldDungeonEnterRaceRes res;
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonReportFrameReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") enter race, but can't find race." << LogStream::eos;
		return;
	}

	CLProtocol::WorldDungeonReportFrameRes res;
	res.lastFrame = race->OnPlayerReportFrame(player, req.frames, req.checksums);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::WorldDungeonCalDropReq& req)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (!player)
	{
		return;
	}

	struct DropStatistic
	{
		DropStatistic()
		{
			memset(this, 0, sizeof(*this));
		}

		UInt32 itemId;
		// 掉落数量
		UInt32 num;
		// 掉落次数
		UInt32 count;
	};

	class DropCounter : public Avalon::Thread
	{
	public:
		DropCounter(UInt32 dungeonId, UInt32 times, DungeonRacePlayerInfo playerInfo)
			: m_DungeonId(dungeonId), m_Times(times), m_Finish(false), m_DungeonRacePlayerInfo(playerInfo)
		{
		}

		virtual void Run()
		{
			UInt32 dungeonId = m_DungeonId;
			UInt32 hellDungeonEntryId = dungeonId;

			auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
			if (!dungeonData)
			{
				return;
			}

			bool isHell = dungeonData->IsHellEntry();

			if (isHell)
			{
				dungeonId = DungeonRaceMgr::Instance()->GenHellDungeonID(dungeonId);
			}

			DungeonDataEntry* dataEntry = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
			if (dataEntry == NULL)
			{
				return;
			}

			UInt64 startTime = Avalon::Time::CurrentTime().MSec();

			for (UInt32 idx = 0; idx < m_Times; ++idx)
			{
				DungeonRace* race = new DungeonRace();

				race->SetID(0);
				race->SetDungeonId(dungeonId);
				race->SetDungeonData(dataEntry);
				race->SetHellDungeonEntryID(hellDungeonEntryId);
				race->AddPlayer(m_DungeonRacePlayerInfo);
				race->SetPassedAreaIndex(0);
				if (isHell)
				{
					race->SetHell();
				}

				race->GenDropStatistic(1);

				auto& drops = race->GetDropSta();
				for (auto& drop : drops)
				{
					auto& sta = m_DropStatics[drop.first];
					sta.num += drop.second.num;
					sta.count += drop.second.times;
				}

				delete race;
			}

			UInt64 endTime = Avalon::Time::CurrentTime().MSec();
			UInt64 usedTime = endTime - startTime;
			InfoLogStream << "thread params(dungenId: " << m_DungeonId << ", times: " << m_Times << "), used time: " << usedTime << "ms." << LogStream::eos;
			m_Finish = true;
		}

		bool IsFinish() const { return m_Finish; }

		static void RecordDropCountToFile(UInt32 accid, UInt32 dungenId, UInt8 isHell, UInt32 times, std::map<DropKey, DropStatistic>& dropStas)
		{
			std::string rootpath = ROOT_PATH;
			std::string accidStr = Avalon::StringUtil::ConvertToString(accid);
			std::string dungenIdStr = Avalon::StringUtil::ConvertToString(dungenId);
			std::string isHellStr = Avalon::StringUtil::ConvertToString(isHell);
			std::string timesStr = Avalon::StringUtil::ConvertToString(times);

			std::string postfix = accidStr + "_" + dungenIdStr + "_" + isHellStr + "_" + timesStr + ".csv";
			std::string filename = rootpath + "TropItemCount/" + "TropItemCount_" + postfix;

			std::ofstream ofs;
			ofs.open(filename.c_str());
			if (!ofs.good())
			{
				ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
				return;
			}

			std::string header = "itemid,equipType,name,times,num,color\n";
			ofs.write(header.c_str(), header.length());
			for (auto itr : dropStas)
			{
				auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first.itemTypeId);
				if (!data)
				{
					continue;
				}

				std::ostringstream oss;
				oss << itr.first.itemTypeId << ", " << (UInt32)itr.first.equipType << ", " << data->name << ", " << itr.second.count << ", " << itr.second.num << ", " << (UInt32)data->color << "\n";
				std::string line = oss.str();
				ofs.write(line.c_str(), line.size());
			}

			ofs.close();
		}

	public:
		UInt32							m_DungeonId;
		UInt32							m_Times;
		std::map<DropKey, DropStatistic> m_DropStatics;
		bool							m_Finish;
		DungeonRacePlayerInfo			m_DungeonRacePlayerInfo;
	};

	class DropCounterMgrThread : public Avalon::Thread
	{
	public:
		DropCounterMgrThread(WSPlayer* player, UInt32 dungeonId, UInt32 times, UInt32 threadNum, DungeonRacePlayerInfo info)
		{
			m_DungeonId = dungeonId;
			m_Times = times;
			for (UInt32 i = 0; i < threadNum; ++i)
			{
				auto thread = new DropCounter(dungeonId, times / threadNum, info);
				m_Threads.push_back(thread);
			}
		}

		~DropCounterMgrThread()
		{
			for (auto thread : m_Threads)
			{
				thread->Stop();
				thread->Join();
				delete thread;
			}

			m_Threads.clear();
		}

		virtual void Run()
		{
			UInt64 startTime = Avalon::Time::CurrentTime().MSec();

			for (auto thread : m_Threads)
			{
				thread->Activate();
			}

			while (true)
			{
				bool allFinish = true;
				for (auto thread : m_Threads)
				{
					if (!thread->IsFinish())
					{
						allFinish = false;
						break;
					}
				}

				if (allFinish)
				{
					break;
				}

				Sleep(1);
			}

			UInt64 endTime = Avalon::Time::CurrentTime().MSec();
			UInt64 usedTime = endTime - startTime;
			InfoLogStream << m_Accid << ", params(dungenId: " << m_DungeonId << ", times: " << m_Times << "), used time: " << usedTime << "ms." << LogStream::eos;

			std::map<DropKey, DropStatistic> dropSta;
			for (auto thread : m_Threads)
			{
				for (auto drop : thread->m_DropStatics)
				{
					auto& sta = dropSta[drop.first];
					sta.itemId = drop.second.itemId;
					sta.num += drop.second.num;
					sta.count += drop.second.count;
				}

				thread->Stop();
				delete thread;
			}
			DropCounter::RecordDropCountToFile(m_Accid, m_DungeonId, 0, m_Times, dropSta);
			m_Threads.clear();
		}

	public:
		UInt32							m_Accid;
		UInt32							m_DungeonId;
		UInt32							m_Times;
		std::vector<DropCounter*>		m_Threads;
	};

	auto thread = new DropCounterMgrThread(player, req.dungeonId, req.times, 1, req.playerInfo);
	thread->Activate();
}

void HandleProtocol(CLProtocol::WorldDungeonNotifyResetAreaIndex& notify)
{
	DungeonRace::ResetDungeonAreaIndex(notify.roleId, notify.dungeonId);
	SysRecordMgr::Instance()->SetRecord(SYS_BEHAVIOR_DAILY_DUNGEON_HIDDEN_ROOM_FLAG,
		SYS_BEHAVIOR_OWNER_ROLE, notify.roleId, notify.dungeonId, 0);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonGetAreaIndexReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldDungeonGetAreaIndexRes res;
	res.dungeonId = req.dungeonId;
	res.areaIndex = DungeonRace::GetDungeonAreaIndex(player->GetID(), req.dungeonId);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDungeonRollItemReq& req)
{
	if (!player)
	{
		return;
	}

	DungeonRace* race = DungeonRaceMgr::Instance()->FindDungeonRaceByRoleId(player->GetID());
	if (!race)
	{
		ErrorLogStream << "player(" << player->GetID() << ") enter roll item fail, can't find race." << LogStream::eos;
		return;
	}

	CLProtocol::WorldDungeonRollItemRes res;
	res.dropIndex = req.dropIndex;
	res.opType = req.opType;
	res.result = race->HandleOneRollReq(player, req.dropIndex, req.opType, res.point);

	player->SendProtocol(res);
}
