#include "TCSTeamTwo.h"

#include <CLErrorCode.h>
#include <CLTeamCopyDefine.h>
#include <CLCrossDungeonDefine.h>
#include <CLSceneDefine.h>
#include <CLTeamCopyProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLSceneDataEntry.h>
#include <CLTeamCopyFieldDataEntry.h>
#include <CLTeamCopyFlopDataEntry.h>
#include <CLTeamCopyGridMonsterDataEntry.h>

#include "TCSGridObj.h"
#include "TCSGridCfgMgr.h"
#include "TCSPlayerMgr.h"
#include "TCSTeamMgr.h"
#include "TCSDungeonMgr.h"
#include "SysUtility.h"
#include "TCSApplication.h"

TCSTeamTwo::TCSTeamTwo()
{
	UInt32 squadNum = GetTeamCopyValue(TC_VALUE_2_SQUAD_NUM);
	m_SquadVec.resize(squadNum);
	for (UInt32 k = 1; k <= squadNum; ++k)
	{
		TeamCopySquad squad;
		squad.squadId = k;
		squad.squadStatus = TEAM_COPY_SQUAD_STATUS_INIT;
		squad.teamMemberList.clear();

		m_SquadVec[k - 1] = squad;
	}

	m_ObjSeed = 0;
	m_AddReviveCnt = 0;
	m_PassType = TC_2_PASS_TYPE_COMMON;
	m_BossPhase = 0;
	m_bossLinkFieldId = 0;
	m_MainCityGridId = 0;
	m_SecTimer.SetInterval(1000);
}

TCSTeamTwo::~TCSTeamTwo()
{
	for (auto& iter : m_GridObjMap)
	{
		if (iter.second != NULL)
		{
			CL_SAFE_DELETE(iter.second);
		}
	}

	for (auto& iter : m_DeadObjMap)
	{
		if (iter.second != NULL)
		{
			CL_SAFE_DELETE(iter.second);
		}
	}

	m_GridObjMap.clear();
	m_DeadObjMap.clear();
	m_SquadObjMap.clear();
	m_MonsterObjMap.clear();
}

bool TCSTeamTwo::OnTick(const Avalon::Time& now)
{
	if (!TCSTeam::OnTick(now))
	{
		return false;
	}

	// 断线超过 X秒的玩家,换个队长
	if (m_SecTimer.IsTimeout(now))
	{
		_LeaveReconnect();
	}

	std::list<TCGridObj*> destoryObjList;
	for (auto& iter : m_GridObjMap)
	{
		if (NULL == iter.second)
			continue;

		if (iter.second->GetObjStatus() == TC_GRID_OBJ_STATUS_DESTORY)
		{
			destoryObjList.push_back(iter.second);
			continue;
		}

		iter.second->OnTick(now);
	}

	for (auto& it : destoryObjList)
	{
		m_GridObjMap.erase(it->GetObjId());
		CL_SAFE_DELETE(it);
	}

	destoryObjList.clear();
	return true;
}

bool TCSTeamTwo::TeamIsFull()
{
	return m_PlayerSquad.size() >= GetTeamCopyValue(TC_VALUE_2_TEAM_CAPACITY);
}

void TCSTeamTwo::GetTicket(std::vector<TCConsumeItem>& itemVec)
{
	TCConsumeItem item1;
	item1.id = GetTeamCopyValue(TC_VALUE_2_TICKET_ID_1);
	item1.num = GetTeamCopyValue(TC_VALUE_2_TICKET_NUM_1);
	if (item1.id != 0 && item1.num != 0)
		itemVec.push_back(item1);

	TCConsumeItem item2;
	item2.id = GetTeamCopyValue(TC_VALUE_2_TICKET_ID_2);
	item2.num = GetTeamCopyValue(TC_VALUE_2_TICKET_NUM_2);
	if (item2.id != 0 && item2.num != 0)
		itemVec.push_back(item2);
}

UInt32 TCSTeamTwo::GetGoldMaxNum()
{
	return GetTeamCopyValue(TC_VALUE_2_GOLD_MAX_NUM);
}

UInt32 TCSTeamTwo::GetGoldNumErrorCode()
{
	return ErrorCode::TEAM_COPY_TWO_GOLD_FULL;
}

void TCSTeamTwo::AttackDataNotify(UInt32 stage)
{
	CLProtocol::TeamCopyTwoTeamData teamData;
	teamData.gameOverTime = GetGameOverTime();
	BroadcastToMember(teamData);

	// 需要下发的据点数据
	CLProtocol::TeamCopyGridFieldNotify fieldNotify;
	for (auto& iter : m_GridObjMap)
	{
		TCGridObj* fieldObj = iter.second;
		if (NULL == fieldObj || fieldObj->GetObjType() == TC_GRID_OBJ_MONSTER || fieldObj->GetObjType() == TC_GRID_OBJ_SQUAD)
		{
			continue;
		}

		TCGridObjData objData;
		iter.second->GetObjData(objData);
		fieldNotify.fieldVec.push_back(objData);
	}
	BroadcastToMember(fieldNotify);

	// 小队数据下发
	for (auto& iter : m_SquadObjMap)
	{
		SendSquadObjData(iter.second);
	}

	// 怪物数据下发
	CLProtocol::TeamCopyGridMonsterNotify monsterNotify;
	for (auto& iter : m_MonsterObjMap)
	{
		TCGridObjData objData;
		iter.second->GetObjData(objData);
		monsterNotify.monsterVec.push_back(objData);

		TCMonsterPath monsterPath;
		monsterPath.monsterObjId = iter.second->GetObjId();
		monsterPath.pathVec.insert(monsterPath.pathVec.begin(), iter.second->GetMovePath().begin(), iter.second->GetMovePath().end());
		monsterNotify.monsterPathVec.push_back(monsterPath);
	}

	BroadcastToMember(monsterNotify);
}

void TCSTeamTwo::AttackDataNotify(TCSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::TeamCopyTwoTeamData teamData;
	teamData.gameOverTime = GetGameOverTime();
	player->SendProtocol(teamData);

	// 需要下发的据点数据
	CLProtocol::TeamCopyGridFieldNotify fieldNotify;
	CLProtocol::TeamCopyMonsterTransfer mTransfer;
	for (auto& iter : m_GridObjMap)
	{
		TCGridObj* fieldObj = iter.second;
		if (NULL == fieldObj || fieldObj->GetObjType() == TC_GRID_OBJ_MONSTER
			|| fieldObj->GetObjType() == TC_GRID_OBJ_SQUAD)
		{
			continue;
		}

		if (fieldObj->GetObjType() == TC_GRID_OBJ_MONSTER_TRANSFER)
		{
			fieldObj->GetObjData(mTransfer.mTransferData);
			continue;
		}

		TCGridObjData objData;
		fieldObj->GetObjData(objData);
		fieldNotify.fieldVec.push_back(objData);
	}

	// 怪物传送门
	if (mTransfer.mTransferData.objId != 0)
	{
		player->SendProtocol(mTransfer);
	}

	// 死亡的据点
	for (auto& iter : m_DeadObjMap)
	{
		TCGridObj* deadObj = iter.second;
		if (NULL == deadObj)
			continue;

		TCGridObjData objData;
		deadObj->GetObjData(objData);
		fieldNotify.fieldVec.push_back(objData);
	}

	player->SendProtocol(fieldNotify);

	// 小队数据下发
	UInt32 squadId = _GetPlayerSquadId(player->GetID());
	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (squadObj != NULL)
	{
		CLProtocol::TeamCopyGridSquadNotify squadNotify;
		squadNotify.squadId = squadObj->GetSquadId();
		squadNotify.squadStatus = squadObj->GetSquadObjStatus();
		squadNotify.targetGridId = squadObj->GetTargetGridId();
		squadNotify.targetObjId = squadObj->GetTargetObjId();
		squadObj->GetObjData(squadNotify.squadData);
		squadNotify.pathVec.insert(squadNotify.pathVec.begin(), squadObj->GetPathList().begin(), squadObj->GetPathList().end());
		player->SendProtocol(squadNotify);
	}

	for (auto& iter : m_SquadObjMap)
	{
		TCSquadObj* tcSquad = iter.second;
		if (NULL == tcSquad)
			continue;

		// 战斗中的小队和据点
		TCGridObj* battleObj = tcSquad->GetBattleObj();
		if (battleObj != NULL)
		{
			CLProtocol::TeamCopyGridSquadBattleNotify battleNotify;
			battleNotify.squadId = tcSquad->GetSquadId();
			battleNotify.squadGridId = tcSquad->GetGridId();
			battleNotify.fieldGridId = battleObj->GetGridId();
			battleNotify.isBattle = 1;
			player->SendProtocol(battleNotify);
		}

		// 其他小队数据
		if (iter.first != squadId)
		{
			CLProtocol::TeamCopyOtherSquadData squadData;
			squadData.squadId = tcSquad->GetSquadId();
			squadData.squadStatus = tcSquad->GetSquadObjStatus();
			squadData.squadGridId = tcSquad->GetGridId();
			squadData.squadTargetId = tcSquad->GetTargetGridId();
			squadData.squadMemberNum = _SquadMemberNum(tcSquad->GetSquadId());
			player->SendProtocol(squadData);
		}
	}

	// 怪物数据下发
	CLProtocol::TeamCopyGridMonsterNotify monsterNotify;
	for (auto& iter : m_MonsterObjMap)
	{
		TCGridObjData objData;
		iter.second->GetObjData(objData);
		monsterNotify.monsterVec.push_back(objData);

		TCMonsterPath monsterPath;
		monsterPath.monsterObjId = iter.second->GetObjId();
		monsterPath.pathVec.insert(monsterPath.pathVec.begin(), iter.second->GetMovePath().begin(), iter.second->GetMovePath().end());
		monsterNotify.monsterPathVec.push_back(monsterPath);
	}

	player->SendProtocol(monsterNotify);
}

void TCSTeamTwo::CheckCondFinish()
{
	TCSTeam::CheckCondFinish();

	SetCurStage(TEAM_COPY_STAGE_ONE);

	InfoLogStream << TEAMINFO(this) << " check cond finish!" << LogStream::eos;

	// 初始化据点
	UInt32 squadId = 0;
	for (auto& iter : TCSGridCfgMgr::Instance()->GetFieldMap(GetTeamGrade()))
	{
		TCSGridCfg* gridCfg = iter.second;
		if (NULL == gridCfg)
		{
			ErrorLogStream << TEAMINFO(this) << " init grid config null!" << LogStream::eos;
			continue;
		}

		TCGridObj* objPtr = _CreateGridObj(gridCfg->type);
		if (NULL == objPtr)
		{
			ErrorLogStream << TEAMINFO(this) << " create grid obj failed! type:" << gridCfg->type << LogStream::eos;
			continue;
		}

		objPtr->SetObjId(GenObjId());
		objPtr->SetGridId(gridCfg->gridId);
		objPtr->SetFieldId(gridCfg->fieldId);
		objPtr->SetTeam(this);
		if (!objPtr->InitData(gridCfg->paramVec, gridCfg->paramVec2))
		{
			ErrorLogStream << TEAMINFO(this) << "init grid obj failed! gridId:" << gridCfg->gridId << LogStream::eos;
			CL_SAFE_DELETE(objPtr);
			continue;
		}

		InfoLogStream << TEAMINFO(this) << " make init grid obj: " << objPtr->GetObjStr() << LogStream::eos;

		if (TC_GRID_OBJ_BORN_POINT == gridCfg->type)
		{
			TCSquadObj* squadObj = dynamic_cast<TCSquadObj*>(objPtr);
			if (NULL == squadObj)
			{
				ErrorLogStream << TEAMINFO(this) << "squad obj error!" << LogStream::eos;
				CL_SAFE_DELETE(objPtr);
				continue;
			}

			++squadId;
			squadObj->SetSquadId(squadId);
			m_SquadObjMap[squadId] = squadObj;

			if (_SquadMemberNum(squadId) == 0)
			{
				InfoLogStream << TEAMINFO(this) << " squad member empty! id:" << squadId << LogStream::eos;
				continue;
			}
		}
		else if (TC_GRID_OBJ_MAIN_CITY == gridCfg->type)
		{
			m_MainCityGridId = gridCfg->gridId;
		}

		m_GridObjMap[objPtr->GetObjId()] = objPtr;
	}

	// 刷怪物
	class TeamCopyGridMonsterEntryVisitor : public Avalon::DataEntryVisitor<TeamCopyGridMonsterDataEntry>
	{
	public:

		TeamCopyGridMonsterEntryVisitor(TCSTeamTwo* team) : m_Team(team) {}

		bool operator()(TeamCopyGridMonsterDataEntry* entry)
		{
			if (entry->monsterType != TC_GRID_MONSTER_TYPE_INIT || entry->grade != m_Team->GetTeamGrade())
			{
				return true;
			}

			TCGridObj* monster = new TCMonsterObj();
			monster->SetObjId(m_Team->GenObjId());
			monster->SetGridId(entry->bornPos);
			monster->SetFieldId(GetTeamCopyValue(TC_VALUE_2_MONSTER_FIELD_ID));
			monster->SetTeam(m_Team);
			if (!monster->InitData({ TC_GRID_MONSTER_TYPE_INIT }, std::vector<UInt32>()))
			{
				ErrorLogStream << TEAMINFO(m_Team) << " init monster failed! grid:" << entry->bornPos << LogStream::eos;
				CL_SAFE_DELETE(monster);
				return true;
			}

			InfoLogStream << TEAMINFO(m_Team) << " make init monster: " << monster->GetObjStr() << LogStream::eos;

			m_Team->AddMonster(monster);
			return true;
		}

	private:
		TCSTeamTwo* m_Team;
	};

	TeamCopyGridMonsterEntryVisitor vistor(this);
	TeamCopyGridMonsterDataEntryMgr::Instance()->Visit(vistor);

	InfoLogStream << TEAMINFO(this) << " map init finish!" << LogStream::eos;

	AttackDataNotify(GetCurStage());
}

void TCSTeamTwo::MoveGrid(TCSPlayer* player, UInt32 targetGridId, UInt32 targetObjId)
{
	if (NULL == player) return;

	UInt32 squadId = _GetPlayerSquadId(player->GetID());
	CLProtocol::TeamCopyGridMoveRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.squadId = squadId;

	do
	{
		TeamCopySquad* squad = _FindSquad(squadId);
		if (NULL == squad) return;

		if (squad->GetSquadStatus() == TEAM_COPY_SQUAD_STATUS_BATTLE)
		{
			res.retCode = ErrorCode::TEAM_COPY_IN_BATTLE;
			break;
		}

		TeamCopyMember* member = _FindMember(player->GetID());
		if (NULL == member) return;

		if (!_OperatorPower(member))
		{
			res.retCode = ErrorCode::TEAM_COPY_NO_POWER;
			break;
		}

		TCSquadObj* squadObj = _FindSquadObj(squadId);
		if (NULL == squadObj)
		{
			res.retCode = ErrorCode::TEAM_COPY_SQUAD_INVALID;
			break;
		}

		UInt32 realSearchGrid = 0;
		if (0 == targetGridId && 0 == targetObjId)	// 取消寻路
		{

		}
		else if (targetObjId != 0)	// 寻路到目标
		{
			// 重复寻路
			if (squadObj->GetTargetObjId() == targetObjId)
			{
				res.retCode = ErrorCode::TEAM_COPY_IN_TARGET_GRID;
				break;
			}

			TCGridObj* gridObj = FindGridObjByObjId(targetObjId);
			if (NULL == gridObj)
			{
				res.retCode = ErrorCode::TEAM_COPY_NOT_FIND_TARGET;
				break;
			}

			realSearchGrid = gridObj->GetGridId();
		}
		else if (targetGridId != 0)
		{
			// 寻路到空格子，检查格子是否有阻挡
			TCSGridCfg* gridCfg = TCSGridCfgMgr::Instance()->FindGrid(GetTeamGrade(), targetGridId);
			if (NULL == gridCfg)
			{
				res.retCode = ErrorCode::TEAM_COPY_NOT_FIND_GRID;
				break;
			}

			// 格子是阻挡
			if (gridCfg->block)
			{
				res.retCode = ErrorCode::TEAM_COPY_GRID_BLOCK;
				break;
			}

			realSearchGrid = targetGridId;
		}
		else
		{
			ErrorLogStream << TEAMINFO(this) << " unknown operator! targetObjId:" << targetObjId
				<< " targetGridId:" << targetGridId << LogStream::eos;
			return;
		}

		// 目标已经是该格子了
		if (squadObj->GetGridId() == realSearchGrid || squadObj->GetTargetGridId() == realSearchGrid)
		{
			res.retCode = ErrorCode::TEAM_COPY_IN_TARGET_GRID;
			break;
		}

		// 被原来的目标移除
		if (squadObj->GetTargetObjId() != 0)
		{
			TCMonsterObj* monsterObj = _FindMonsterObj(squadObj->GetTargetObjId());
			if (monsterObj != NULL)
			{
				monsterObj->RemoveSquadTarget(squadId);
			}
		}

		if (realSearchGrid == 0)
		{
			squadObj->StopMove();
		}
		else
		{
			std::list<UInt32> searchPathList = GetSearchPath(squadObj->GetGridId(), realSearchGrid);
			if (searchPathList.empty())
			{
				res.retCode = ErrorCode::TEAM_COPY_GRID_MOVE_FAILED;
				break;
			}

			TCGridObj* targetObjPtr = FindGridObjByGridId(realSearchGrid);
			if (targetObjPtr != NULL)
			{
				squadObj->Move(realSearchGrid, targetObjPtr->GetObjId(), searchPathList);
				if (targetObjPtr->GetObjType() == TC_GRID_OBJ_MONSTER)
				{
					TCMonsterObj* monsterObj = dynamic_cast<TCMonsterObj*>(targetObjPtr);
					if (monsterObj != NULL)
					{
						monsterObj->AddSquadTarget(squadId);
					}
				}
			}
			else
			{
				squadObj->Move(realSearchGrid, 0, searchPathList);
			}
		}

		res.squadStatus = squadObj->GetSquadObjStatus();
		res.targetGridId = targetGridId;
		res.targetObjId = targetObjId;
		res.pathVec.insert(res.pathVec.begin(), squadObj->GetPathList().begin(), squadObj->GetPathList().end());
		SendCurSquadDataToOtherSquad(squadObj);

	} while (0);

	BroadcastToMember(res, squadId);
}

void TCSTeamTwo::QueryRacePlayerInfoTimeOut(UInt32 squadId)
{
	TCSTeam::QueryRacePlayerInfoTimeOut(squadId);

	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (NULL == squadObj)
		return;

	squadObj->SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	SendSquadObjData(squadObj);

	TCGridObj* gridObj = squadObj->GetBattleObj();
	if (gridObj != NULL)
	{
		if (gridObj->GetObjType() == TC_GRID_OBJ_ALTAR || gridObj->GetObjType() == TC_GRID_OBJ_TERROR_ALTAR)
		{
			gridObj->SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
		}
		else
		{
			gridObj->SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		}

		SendGridObjData(gridObj);
	}

	squadObj->SetBattleObj(NULL);

	ErrorLogStream << TEAMINFO(this) << " query player info time out! squadId:" << squadId << LogStream::eos;
}

UInt32 TCSTeamTwo::StartRace(UInt32 squadId, UInt32 fieldId)
{
	if (IsGameOver())
	{
		return ErrorCode::TEAM_COPY_GAME_OVER;
	}

	TeamCopyFieldDataEntry* fieldData = TeamCopyFieldDataEntryMgr::Instance()->FindEntry(fieldId);
	if (NULL == fieldData)
	{
		return ErrorCode::TEAM_COPY_FIELD_UNLOCK;
	}

	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << LogStream::eos;
		return ErrorCode::TEAM_COPY_SQUAD_INVALID;
	}

	std::set<UInt64> playerIds;
	for (auto& it : squad->teamMemberList)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(it.playerId);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << " not find player id:" << it.playerId << LogStream::eos;
			return ErrorCode::TEAM_COPY_PLAYER_LEAVE;
		}

		if (TCSPlayerMgr::Instance()->PlayerIsExpire(it.playerId))
		{
			DebugLogStream << TEAMINFO(this) << "expire playerId:" << it.playerId << LogStream::eos;
			continue;
		}

		if (player->GetIsLoading())
		{
			return ErrorCode::TEAM_COPY_TEAM_MATE_LOADING;
		}

		if (player->GetSceneType() != SCENE_TYPE_TEAM_COPY)
		{
			// SCENE_TYPE_ACTIVITY（地下城场景）
			if (player->GetSceneType() == SCENE_TYPE_ACTIVITY)
			{
				return ErrorCode::TEAM_COPY_PLAYER_NOT_IN_SCENE;
			}

			continue;
		}

		playerIds.insert(it.playerId);
	}

	// 所有人都掉线了，或者在loading，或者不在团本场景
	if (playerIds.empty())
	{
		return ErrorCode::TEAM_COPY_SQUAD_ALL_DISCONNECT;
	}

	UInt32 teamSquadId = GET_TEAM_SQUAD_ID(GetTeamId(), squad->squadId);

	// 去scene查数据
	CLProtocol::TeamCopyQueryPlayerInfoReq req;
	req.teamSquadId = teamSquadId;
	req.dungeonId = fieldData->dungeonId;

	for (auto& iter : playerIds)
	{
		req.roleId = iter;

		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter);
		if (NULL == player)
		{
			ErrorLogStream << TEAMINFO(this) << "not find player id:" << iter << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		InfoLogStream << TEAMINFO(this) << "start race teamSquadId:" << teamSquadId << " query player:" << iter << LogStream::eos;

		player->SendToScene(req);
	}

	// 更新小队状态
	squad->SetSquadStatus(TEAM_COPY_SQUAD_STATUS_BATTLE);
	TCSTeamMgr::Instance()->CreateTeamPlayerInfo(teamSquadId, fieldData->dungeonId, fieldId, 0, playerIds);

	InfoLogStream << TEAMINFO(this) << "start race teamSquadId:" << teamSquadId
		<< " fieldId:" << fieldId << " dungeonId:" << fieldData->dungeonId << LogStream::eos;

	return ErrorCode::SUCCESS;
}

void TCSTeamTwo::StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res)
{
	UInt32 squadId = GET_SQUAD_ID(res.teamId);
	if (res.result != ErrorCode::SUCCESS)
	{
		ErrorLogStream << TEAMINFO(this) << "start race failed teamSquadId:" << res.teamId << " enter dungeonId:" << res.dungeonId
			<< " result:" << res.result << LogStream::eos;

		CLProtocol::TeamCopyStartChallengeRes challengeRes;
		challengeRes.retCode = res.result;
		BroadcastToMember(challengeRes, squadId);

		// 更新小队状态
		SetSquadStatus(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
		SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);

		TCSquadObj* squadObj = _FindSquadObj(squadId);
		if (squadObj != NULL)
		{
			TCGridObj* fieldObj = squadObj->GetBattleObj();
			if (fieldObj != NULL)
			{
				fieldObj->SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
				SendGridObjData(fieldObj);
			}

			squadObj->BattleEnd(false);
			SendSquadObjData(squadObj);
		}

		return;
	}

	UInt32 fieldId = TCSTeamMgr::Instance()->GetAttackFieldId(res.teamId);

	InfoLogStream << TEAMINFO(this) << "start race success teamSquadId:" << res.teamId << " fieldId:" << fieldId << " enter dungeonId:"
		<< res.dungeonId << " relayId:" << res.relayServerID << " gamesessionId:" << res.gamesessionID << " address:"
		<< res.address.ToString() << LogStream::eos;

	// 通知战斗
	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (squadObj != NULL && squadObj->GetBattleObj() != NULL)
	{
		squadObj->Battle(squadId);
		SendSquadObjData(squadObj);

		TCGridObj* fieldObj = squadObj->GetBattleObj();
		fieldObj->Battle(squadId);
		SendGridObjData(fieldObj);

		// 据点增加比赛
		_AddFieldRace(fieldObj->GetObjId(), res.gamesessionID);

		CLProtocol::TeamCopyGridSquadBattleNotify notify;
		notify.squadId = squadId;
		notify.squadGridId = squadObj->GetGridId();
		notify.fieldGridId = fieldObj->GetGridId();
		notify.isBattle = 1;
		BroadcastToMember(notify);

		InfoLogStream << TEAMINFO(this) << " race start race:" << res.gamesessionID << LogStream::eos;

		// 通知小队状态
		SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_BATTLE);

		// 比赛正式开始，创建比赛
		TCSDungeonMgr::Instance()->AddRace(res.gamesessionID, res.teamId, fieldId);

		// 增加比赛的复活次数
		_SendReviveCntToRace(res.gamesessionID);

		// 联动
		if (fieldObj->GetObjType() == TC_GRID_OBJ_BOSS)
		{
			BossLink();
		}
	}
	else
	{
		ErrorLogStream << TEAMINFO(this) << "squad obj error! race:" << res.gamesessionID << LogStream::eos;

		TCSDungeonMgr::Instance()->DungeonRaceEnd(res.gamesessionID, CDER_TEAMCOPY_FILED_DESTORY);

		// 更新小队状态
		SetSquadStatus(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
		SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);

		if (squadObj != NULL)
		{
			squadObj->BattleEnd(false);
			SendSquadObjData(squadObj);
		}

		return;
	}
}

void TCSTeamTwo::RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime)
{
	UInt32 squadId = GET_SQUAD_ID(end.teamId);
	UInt64 raceId = end.endInfo.gamesessionId;

	// 更新小队状态
	SetSquadStatus(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
	SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);

	// udp据点结算
	UdpFieldSettle(squadId, fieldId, dungeonId, createTime, end.result);

	InfoLogStream << TEAMINFO(this) << "race end teamSquadId:" << end.teamId << " dungeonId:" << dungeonId <<
		" fieldId:" << fieldId << " raceId:" << raceId << " result:" << end.result << LogStream::eos;

	// 找到小队对象，据点对象
	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (NULL == squadObj)
	{
		ErrorLogStream << TEAMINFO(this) << " not find squad obj:" << squadId << LogStream::eos;
		return;
	}

	bool isForceEnd = _IsForceEndRace(raceId);
	bool isVictory = (end.result == CDERT_SUCC && !isForceEnd);
	if (isVictory)
	{
		_RaceSettlement(squadId, raceId);
	}
	
	// 先保存下小队攻打的据点
	TCGridObj* fieldObj = squadObj->GetBattleObj();

	// 然后结算小队
	squadObj->BattleEnd(isVictory);
	SendSquadObjData(squadObj);

	// 据点还在，最后结算怪物
	UInt32 fieldGridId = 0;
	UInt32 fieldObjId = 0;
	if (fieldObj != NULL)
	{
		// 据点比赛结束
		fieldGridId = fieldObj->GetGridId();
		fieldObjId = fieldObj->GetObjId();
		fieldObj->BattleEnd(isVictory);
		SendGridObjData(fieldObj);

		InfoLogStream << TEAMINFO(this) << " fieldObj id:" << fieldObjId << " race end!" << LogStream::eos;
	}

	// 广播本场比赛结束
	CLProtocol::TeamCopyGridSquadBattleNotify notify;
	notify.squadId = squadId;
	notify.squadGridId = squadObj->GetGridId();
	notify.fieldGridId = fieldGridId;
	notify.isBattle = 0;
	BroadcastToMember(notify);

	FieldRaceEnd(fieldObj, raceId, isVictory);

	if (!isVictory)
	{
		// boss据点挑战失败，祭坛里的比赛直接结束
		if (fieldObj != NULL && fieldObj->GetObjType() == TC_GRID_OBJ_BOSS)
		{
			EndAltarRace(false);
		}

		// 非强制比赛失败，小队直接返回出生点
		if (!isForceEnd)
		{
			SquadReturnBorn(squadObj);
		}
		SendSquadObjData(squadObj);
	}
}

void TCSTeamTwo::PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate)
{
	if (phase <= m_BossPhase)
		return;

	InfoLogStream << TEAMINFO(this) << " boss phase: " << phase << " frame:" << curFrame << LogStream::eos;
	m_BossPhase = phase;

	// 进入第二阶段，结束祭坛中的比赛
	if (m_BossPhase == 2)
	{
		EndAltarRace(true);
	}
}

void TCSTeamTwo::GameOver(UInt32 overReason)
{
	if (IsGameOver())
	{
		return;
	}
	TCSTeam::GameOver(overReason);

	UInt32 costTime = (UInt32)CURRENT_TIME.Sec() - _GetGameStartTime();

	// 比赛结束，删除消耗
	RemoveAllMemberConsume();
	AllRaceEnd();

	InfoLogStream << TEAMINFO(this) << " game over reason: " << overReason << LogStream::eos;

	switch (overReason)
	{
	case TC_GAME_OVER_REASON_VICTORY:
	{
		// 阶段结束,2成功
		UdpStageEnd(costTime, 2);
		_StageEnd(GetCurStage(), m_PassType);

		// 小阶段翻牌
		_FlopReward(GetCurStage());
		
		// 大阶段翻牌
		_FlopReward(TEAM_COPY_STAGE_FINAL);

		SetStatus(TEAM_COPY_TEAM_STATUS_VICTORY);
		_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_VICTORY);

		// 扣除佣金
		_GameOverCommission(false);
		// 佣金分成
		_CommissionBonus(10048, 10093, 10094);
		// 增加模式通关次数
		_AddModelPassNum(GetModel(), costTime);

		// roll奖励
		_RollReward();
	}
	break;
	case TC_GAME_OVER_REASON_MAIN_CITY_DEAD:
	{
		UdpStageEnd(costTime, 1);
		SetStatus(TEAM_COPY_TEAM_STATUS_FAILED);
		_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_FAILED);

		if (TCSApplication::Instance()->IsTC2FailedReturnCommission())
		{
			// 退还佣金
			_GameOverCommission(true);
		}
		// 失败，恢复挑战次数
		FailedReturnChallengeNum();
	}
	break;
	case TC_GAME_OVER_REASON_TIME_OVER:
	{
		UdpStageEnd(costTime, 1);

		SetStatus(TEAM_COPY_TEAM_STATUS_FAILED);
		_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_FAILED);

		if (TCSApplication::Instance()->IsTC2FailedReturnCommission())
		{
			// 退还佣金
			_GameOverCommission(true);
		}
		
		// 失败，恢复挑战次数
		FailedReturnChallengeNum();
	}
	break;
	default:return;
	}

	SetGameOverTime(0);
}

void TCSTeamTwo::GMClearAllField(UInt32 squadId)
{
	InfoLogStream << TEAMINFO(this) << "gm clear boss obj!" << LogStream::eos;

	// 直接打败boss据点
	AllRaceEnd();

	for (auto& iter : m_SquadVec)
	{
		_RaceSettlement(iter.squadId, 500);
	}

	GameOver(TC_GAME_OVER_REASON_VICTORY);
}

void TCSTeamTwo::SetSquadPro(UInt64 playerId, UInt32 gridId, UInt32 cd)
{
	InfoLogStream << TEAMINFO(this) << "gm set grid playerId:" << playerId << " gridId:" << gridId << LogStream::eos;

	UInt32 squadId = GetPlayerSquadId(playerId);

	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (NULL == squadObj)
	{
		return;
	}

	TCSGridCfg* gridCfg = TCSGridCfgMgr::Instance()->FindGrid(GetTeamGrade(), gridId);
	if (gridCfg != NULL && !gridCfg->block)
	{
		if (!FindGridObjByGridId(gridId))
		{
			squadObj->SetGridId(gridId);
		}
	}

	squadObj->ReduceMoveCD(cd);
	SendSquadObjData(squadObj);
}

void TCSTeamTwo::GMClearGridObj(UInt64 playerId, UInt32 gridType)
{
	if ( !IsValidGridObjType(gridType))
	{
		return;
	}

	switch (gridType)
	{
	case TC_GRID_OBJ_TRANSFER:
	case TC_GRID_OBJ_BORN_POINT:
	case TC_GRID_OBJ_SQUAD:
		return;
	default:
		break;
	}

	InfoLogStream << TEAMINFO(this) << "clear grid type:" << gridType << LogStream::eos;

	std::list<TCGridObj*> objList;
	for (auto& iter : m_GridObjMap)
	{
		if (gridType != iter.second->GetObjType())
		{
			continue;
		}

		objList.push_back(iter.second);
	}

	for (auto& it : objList)
	{
		it->BattleEnd(true);
		SendGridObjData(it);
	}
}

UInt32 TCSTeamTwo::IsAllowChangeSeat(UInt32 srcSquadId, UInt32 destSquadId)
{
	TCSquadObj* srcSquadObj = _FindSquadObj(srcSquadId);
	TCSquadObj* destSquadObj = _FindSquadObj(destSquadId);
	if (NULL == srcSquadObj || NULL == destSquadObj)
	{
		return 0;
	}

	// 两小队都不空
	if (_SquadMemberNum(srcSquadId) != 0 && _SquadMemberNum(destSquadId) != 0)
	{
		// 两小队位置是否挨着
		if (!TCSGridCfgMgr::Instance()->GridIsNear(GetTeamGrade(), srcSquadObj->GetGridId(), destSquadObj->GetGridId()))
		{
			return ErrorCode::TEAM_COPY_SQUAD_GRID_NOT_NEAR;
		}
	}

	return 0;
}

void TCSTeamTwo::ChangeSeatFinish(UInt32 srcSquadId, bool srcIsDec, UInt32 destSquadId, bool destIsInc)
{
	// 同一小队不处理
	if (srcSquadId == destSquadId || !IsYetStartBattle())
		return;

	if (!srcIsDec || !destIsInc)
	{
		return;
	}

	// 小队队员变成0，从地图上消失
	if (_SquadMemberNum(srcSquadId) == 0)
	{
		TCSquadObj* srcSquadObj = _FindSquadObj(srcSquadId);
		if (srcSquadObj != NULL)
		{
			m_GridObjMap.erase(srcSquadObj->GetObjId());
		}
	}

	// 小队队员变成1,回到出生点
	if (_SquadMemberNum(destSquadId) == 1)
	{
		TCSquadObj* destSquadObj = _FindSquadObj(destSquadId);
		if (destSquadObj != NULL)
		{
			SquadReturnBorn(destSquadObj);
			m_GridObjMap[destSquadObj->GetObjId()] = destSquadObj;
		}
	}
}

void TCSTeamTwo::SquadAttackDataNotify()
{
	SendSquadObjData();
}

void TCSTeamTwo::MemberExpire(UInt64 playerId, UInt64 expireTime)
{
	TCSTeam::MemberExpire(playerId, expireTime);

	// 重连回来
	if (expireTime == 0)
	{
		_LeaveReconnect();
	}
}

void TCSTeamTwo::SquadMemberNull(UInt32 squadId)
{
	TCSquadObj* squadObj = _FindSquadObj(squadId);
	if (NULL == squadObj)
		return;

	m_GridObjMap.erase(squadObj->GetObjId());
	SendCurSquadDataToOtherSquad(squadObj);
}

void TCSTeamTwo::UdpStageEnd(UInt32 costTime, UInt32 result)
{
	// 主城血量
	std::list<TCGridObj*> mainCityObjList;
	_GetObjListByObjType(TC_GRID_OBJ_MAIN_CITY, mainCityObjList);
	if (mainCityObjList.empty())
	{
		ErrorLogStream << TEAMINFO(this) << " not find main city obj!" << LogStream::eos;
		return;
	}

	TCMainCityObj* mainCityObj = dynamic_cast<TCMainCityObj*>(*mainCityObjList.begin());
	UInt32 mainCityBlood = mainCityObj != NULL ? mainCityObj->GetBlood() : 0;

	for (auto& iter : m_SquadVec)
	{
		for (auto& it : iter.teamMemberList)
		{
			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(it.playerId);
			if (NULL == player)
				continue;

			player->SendUdpLog("team_copy_stage_end", LOG_TEAM_COPY_STAGE_END, GetTeamId(), GetCurStage(), costTime, result,
				GetTeamGrade(), GetTeamType(), it.changeSquadCnt, _GetSquadMoveCnt(iter.squadId), m_PassType, mainCityBlood);
		}
	}
}

TCGridObj* TCSTeamTwo::FindGridObjByGridId(UInt32 gridId)
{
	for (auto& iter : m_GridObjMap)
	{
		if (iter.second->GetGridId() == gridId)
		{
			return iter.second;
		}
	}

	return NULL;
}

TCGridObj* TCSTeamTwo::FindGridObjByObjId(UInt32 objId)
{
	auto iter = m_GridObjMap.find(objId);
	return iter != m_GridObjMap.end() ? iter->second : NULL;
}

void TCSTeamTwo::SquadReSearchWay(UInt32 squadId, UInt32 gridId, UInt32 objId)
{
	auto iter = m_SquadObjMap.find(squadId);
	if (iter == m_SquadObjMap.end())
	{
		ErrorLogStream << TEAMINFO(this) << " not find squad grid obj:" << squadId << LogStream::eos;
		return;
	}

	TCSquadObj* squadObj = iter->second;
	if (NULL == squadObj)
	{
		ErrorLogStream << TEAMINFO(this) << " transfer squad obj error! " << squadId << LogStream::eos;
		return;
	}

	std::list<UInt32> pathList = GetSearchPath(squadObj->GetGridId(), gridId);
	squadObj->Move(gridId, objId, pathList);

	// 通知客户端刷新寻路信息
	CLProtocol::TeamCopyGridMoveRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.squadId = squadId;
	res.squadStatus = squadObj->GetSquadObjStatus();
	res.targetGridId = squadObj->GetTargetGridId();
	res.targetObjId = squadObj->GetTargetObjId();
	res.pathVec.insert(res.pathVec.begin(), pathList.begin(), pathList.end());
	BroadcastToMember(res, squadId);

	// 通知其他小队，本小队的目标变了
	SendCurSquadDataToOtherSquad(squadObj);
}

void TCSTeamTwo::SendCurSquadDataToOtherSquad(TCSquadObj* squadObj)
{
	if (NULL == squadObj)
		return;

	CLProtocol::TeamCopyOtherSquadData squadData;
	squadData.squadId = squadObj->GetSquadId();
	squadData.squadStatus = squadObj->GetSquadObjStatus();
	squadData.squadGridId = squadObj->GetGridId();
	squadData.squadTargetId = squadObj->GetTargetGridId();
	squadData.squadMemberNum = _SquadMemberNum(squadObj->GetSquadId());
#
	for (auto& iter : m_SquadVec)
	{
		if (squadObj->GetSquadId() == iter.squadId)
		{
			continue;
		}

		BroadcastToMember(squadData, iter.squadId);
	}
}

void TCSTeamTwo::TransferFieldActive()
{
	InfoLogStream << TEAMINFO(this) << " transfer obj active!" << LogStream::eos;

	std::list<TCGridObj*> transferObjList;
	_GetObjListByObjType(TC_GRID_OBJ_TRANSFER, transferObjList);
	for (auto& iter : transferObjList)
	{
		if(NULL == iter)
			continue;

		iter->SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		SendGridObjData(iter);
	}
}

UInt32 TCSTeamTwo::SquadBattle(TCGridObj* battleObj, TCSquadObj* squadObj)
{
	if (NULL == battleObj || NULL == squadObj)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if ( !squadObj->IsCanAttack() || !battleObj->IsCanAttack())
	{
		DebugLogStream << TEAMINFO(this) << " battle obj can,t attack! type:" << battleObj->GetObjType() << LogStream::eos;
		return ErrorCode::TEAM_COPY_IN_BATTLE;
	}

	// boss据点有其他比赛
	if (battleObj->GetObjType() == TC_GRID_OBJ_BOSS && _IsHasFieldRace(battleObj->GetObjId()))
	{
		BroadcastToMember(squadObj->GetSquadId(), ErrorCode::TEAM_COPY_BOSS_FIELD);
		return ErrorCode::TEAM_COPY_BOSS_FIELD;
	}

	InfoLogStream << TEAMINFO(this) << " squad:" << squadObj->GetSquadId() << " attack obj id:" << battleObj->GetObjId() << LogStream::eos;

	UInt32 ret = StartRace(squadObj->GetSquadId(), battleObj->GetFieldId());
	if (ErrorCode::SUCCESS != ret)
	{
		DebugLogStream << TEAMINFO(this) << " squad:" << squadObj->GetSquadId() << " start race failed!:"
			<< battleObj->GetObjId() << " ret:" << ret << LogStream::eos;

		if (ret != ErrorCode::TEAM_COPY_SQUAD_ALL_DISCONNECT)
			BroadcastToMember(squadObj->GetSquadId(), ret);

		return ret;
	}

	battleObj->PreBattle();
	SendGridObjData(battleObj);

	squadObj->PreBattle();
	squadObj->SetBattleObj(battleObj);
	SendSquadObjData(squadObj);

	return ErrorCode::SUCCESS;
}

void TCSTeamTwo::SendSquadObjData()
{
	for (auto& iter : m_SquadObjMap)
	{
		SendSquadObjData(iter.second);
	}
}

void TCSTeamTwo::SendSquadObjData(TCSquadObj* squadObj, bool isSendToOtherSquad)
{
	if (NULL == squadObj)
		return;

	CLProtocol::TeamCopyGridSquadNotify squadNotify;
	squadNotify.squadId = squadObj->GetSquadId();
	squadNotify.squadStatus = squadObj->GetSquadObjStatus();
	squadNotify.targetGridId = squadObj->GetTargetGridId();
	squadNotify.targetObjId = squadObj->GetTargetObjId();
	squadObj->GetObjData(squadNotify.squadData);
	squadNotify.pathVec.insert(squadNotify.pathVec.begin(), squadObj->GetPathList().begin(), squadObj->GetPathList().end());
	BroadcastToMember(squadNotify, squadObj->GetSquadId());

	if (isSendToOtherSquad)
	{
		SendCurSquadDataToOtherSquad(squadObj);
	}
}

void TCSTeamTwo::SendGridObjData(TCGridObj* fieldObj)
{
	if (NULL == fieldObj || fieldObj->GetObjStatus() == TC_GRID_OBJ_STATUS_DESTORY
		|| fieldObj->GetObjStatus() == TC_GRID_OBJ_STATUS_PRE_BATTLE)
		return;

	if (fieldObj->GetObjType() == TC_GRID_OBJ_MONSTER)
	{
		SendMonsterObjData(dynamic_cast<TCMonsterObj*>(fieldObj));
	}
	else if (fieldObj->GetObjType() == TC_GRID_OBJ_MONSTER_TRANSFER)
	{
		SendMonsterTransferObjData(dynamic_cast<TCMonsterTransferObj*>(fieldObj));
	}
	else
	{
		CLProtocol::TeamCopyGridFieldNotify fieldNotify;
		TCGridObjData objData;
		fieldObj->GetObjData(objData);
		fieldNotify.fieldVec.push_back(objData);
		BroadcastToMember(fieldNotify);
	}
}

void TCSTeamTwo::SendMonsterObjData(TCMonsterObj* monsterObj)
{
	if (NULL == monsterObj)
		return;

	CLProtocol::TeamCopyGridMonsterNotify monsterNotify;
	{
		TCGridObjData objData;
		monsterObj->GetObjData(objData);
		monsterNotify.monsterVec.push_back(objData);
	}
	{
		TCMonsterPath monsterPath;
		monsterPath.monsterObjId = monsterObj->GetObjId();
		monsterPath.pathVec.insert(monsterPath.pathVec.begin(), monsterObj->GetMovePath().begin(), monsterObj->GetMovePath().end());
		monsterNotify.monsterPathVec.push_back(monsterPath);
	}
	BroadcastToMember(monsterNotify);
}

void TCSTeamTwo::SendMonsterTransferObjData(TCMonsterTransferObj* mTransferObj)
{
	if (NULL == mTransferObj)
		return;

	CLProtocol::TeamCopyMonsterTransfer notify;
	mTransferObj->GetObjData(notify.mTransferData);
	BroadcastToMember(notify);
}

void TCSTeamTwo::SquadReturnBorn(TCSquadObj* squadObj)
{
	if (NULL == squadObj)
		return;

	// 传送回的出生点
	TCGridObj* bornObj = FindGridObjByGridId(squadObj->GetBornGridId());
	if (NULL == bornObj)
	{
		if (squadObj->GetGridId() == squadObj->GetBornGridId())
			return;

		// 出生点没有物体，直接返回出生点
		squadObj->SetPos(squadObj->GetBornGridId());
		SendSquadObjData(squadObj);
	}
	else
	{
		if (squadObj->GetObjId() == bornObj->GetObjId())
			return;

		_SquadTransferToGrid(squadObj, squadObj->GetBornGridId());
	}
}

void TCSTeamTwo::GridObjDestory(TCGridObj* destoryObj, bool isKill)
{
	if (NULL == destoryObj)
	{
		ErrorLogStream << TEAMINFO(this) << " destroy obj null!" << LogStream::eos;
		return;
	}

	// 此时如果有小队把怪物当作目标
	for (auto& iter : m_SquadObjMap)
	{
		TCSquadObj* squadObj = iter.second;
		if (NULL == squadObj)
			continue;

		TCGridObj* gridObj = squadObj->GetBattleObj();
		if ((gridObj != NULL && gridObj->GetObjId() == destoryObj->GetObjId()))
		{
			squadObj->TargetDead(true);
			SendSquadObjData(squadObj);
		}

		if (squadObj->GetTargetObjId() == destoryObj->GetObjId())
		{
			BroadcastToMember(squadObj->GetSquadId(), 3216);
			squadObj->TargetDead(false);
			SendSquadObjData(squadObj);
		}
	}

	InfoLogStream << TEAMINFO(this) << " destroy obj id:" << destoryObj->GetObjId()
		<< " type:" << destoryObj->GetObjType() << LogStream::eos;

	// 怪物
	if (destoryObj->GetObjType() == TC_GRID_OBJ_MONSTER)
	{
		CLProtocol::TeamCopyGridMonsterDead monsterDead;
		monsterDead.monsterObjId = destoryObj->GetObjId();
		monsterDead.isKill = isKill ? 1 : 0;
		BroadcastToMember(monsterDead);

		// 怪物变成待销毁状态
		destoryObj->SetObjStatus(TC_GRID_OBJ_STATUS_DESTORY);
		m_MonsterObjMap.erase(destoryObj->GetObjId());
	}
	else if (destoryObj->GetObjType() == TC_GRID_OBJ_MONSTER_TRANSFER)
	{
		destoryObj->SetObjStatus(TC_GRID_OBJ_STATUS_DESTORY);
	}
}

void TCSTeamTwo::LaboratoryLink(TCLaboratoryObj* laboratory)
{
	if (NULL == laboratory) return;

	bool otherLaborIsInRevive = false;
	std::list<TCGridObj*> laboratoryList;
	_GetObjListByObjType(TC_GRID_OBJ_LABORATORY, laboratoryList);
	for (auto& iter : laboratoryList)
	{
		if (NULL == iter || iter->GetObjId() == laboratory->GetObjId())
		{
			continue;
		}

		TCLaboratoryObj* otherLabor = dynamic_cast<TCLaboratoryObj*>(iter);
		if (NULL == otherLabor)
		{
			continue;
		}

		otherLaborIsInRevive = otherLabor->IsInRevive();
		if (otherLaborIsInRevive)
		{
			otherLabor->AddReviveCd();
		}
		break;
	}

	if (otherLaborIsInRevive)
	{
		laboratory->AddReviveCd();
	}
}

void TCSTeamTwo::MakeMonster(UInt32 monsterType, const std::vector<UInt32>& gridVec)
{
	UInt32 selectGrid = SelectNullGrid(gridVec);
	if (selectGrid == 0)
	{
		ErrorLogStream << TEAMINFO(this) << " select monster born error!" << LogStream::eos;
		return;
	}

	TCGridObj* newObj = new TCMonsterObj();
	if (NULL == newObj) return;

	newObj->SetObjId(GenObjId());
	newObj->SetGridId(selectGrid);
	newObj->SetFieldId(GetTeamCopyValue(TC_VALUE_2_MONSTER_FIELD_ID));
	newObj->SetTeam(this);
	if (!newObj->InitData({ monsterType }, std::vector<UInt32>()))
	{
		ErrorLogStream << TEAMINFO(this) << " init monster failed! grid:" << selectGrid << LogStream::eos;
		CL_SAFE_DELETE(newObj);
		return;
	}

	InfoLogStream << TEAMINFO(this) << " make new monster: " << newObj->GetObjStr() << LogStream::eos;

	AddMonster(newObj);
	SendGridObjData(newObj);
}

void TCSTeamTwo::MakeMonsterTransfer(UInt32 liveTime, const std::vector<UInt32>& gridVec)
{
	UInt32 selectGrid = SelectNullGrid(gridVec);
	if (selectGrid == 0)
	{
		ErrorLogStream << TEAMINFO(this) << " select monster transfer born error!" << LogStream::eos;
		return;
	}

	TCGridObj* newObj = new TCMonsterTransferObj();
	if (NULL == newObj) return;

	newObj->SetObjId(GenObjId());
	newObj->SetGridId(selectGrid);
	newObj->SetFieldId(0);
	newObj->SetTeam(this);
	if (!newObj->InitData({ liveTime }, std::vector<UInt32>()))
	{
		ErrorLogStream << TEAMINFO(this) << " init monster transfer failed! grid:" << selectGrid << LogStream::eos;
		CL_SAFE_DELETE(newObj);
		return;
	}

	InfoLogStream << TEAMINFO(this) << " make new monster transfer: " << newObj->GetObjStr() << LogStream::eos;

	m_GridObjMap[newObj->GetObjId()] = newObj;
	SendGridObjData(newObj);
}

UInt32 TCSTeamTwo::SelectNullGrid(const std::vector<UInt32>& gridVec)
{
	for (auto& iter : gridVec)
	{
		TCSGridCfg* gridCfg = TCSGridCfgMgr::Instance()->FindGrid(GetTeamGrade(), iter);
		if (NULL == gridCfg)
		{
			ErrorLogStream << TEAMINFO(this) << "not find monster born gridId:" << iter << LogStream::eos;
			return 0;
		}

		// 该格子没有对象
		if (NULL == FindGridObjByGridId(iter))
		{
			return iter;
		}
	}

	return 0;
}

void TCSTeamTwo::AddMonster(TCGridObj* newObj)
{
	if (NULL == newObj)
	{
		return;
	}

	UInt32 objId = newObj->GetObjId();
	m_GridObjMap[objId] = newObj;

	TCMonsterObj* monsterObj = dynamic_cast<TCMonsterObj*>(newObj);
	if (monsterObj != NULL)
	{
		m_MonsterObjMap[objId] = monsterObj;
	}
}

void TCSTeamTwo::AddReviveCnt(UInt32 reviveCnt)
{
	m_AddReviveCnt += reviveCnt;

	InfoLogStream << TEAMINFO(this) << " add revive cnt: " << reviveCnt << " total cnt:" << m_AddReviveCnt << LogStream::eos;

	// 增加复活次数后发送到正在进行的比赛
	for (auto& iter : m_FieldRaceMap)
	{
		for (auto it : iter.second)
		{
			_SendReviveCntToRace(it);
		}
	}
}

void TCSTeamTwo::UpdateGridObjCd(UInt32 squadRecudeCd, UInt32 monsterAddCd, UInt32 makeMonsterAddCd)
{
	InfoLogStream << TEAMINFO(this) << "update squadRecudeCd:" << squadRecudeCd << " monsterAddCd:" << monsterAddCd 
		<< " makeMonsterAddCd:" << makeMonsterAddCd <<  LogStream::eos;

	// 小队缩减cd
	if (squadRecudeCd != 0)
	{
		for (auto& iter : m_SquadObjMap)
		{
			TCSquadObj* squadObj = iter.second;
			if (NULL == squadObj)
				continue;

			squadObj->ReduceMoveCD(squadRecudeCd);
			// 此时不通知cd修改，下次移动的时候通知
			//SendSquadObjData(squadObj, false);
		}
	}

	// 怪物增加cd
	if (monsterAddCd != 0)
	{
		for (auto& iter : m_MonsterObjMap)
		{
			TCMonsterObj* monsterObj = iter.second;
			if (NULL == monsterObj)
				continue;

			monsterObj->AddMoveCD(monsterAddCd);
		}
	}

	// 兵营增加制造怪物的cd
	if (makeMonsterAddCd != 0)
	{
		std::list<TCGridObj*> laboratoryList;
		_GetObjListByObjType(TC_GRID_OBJ_LABORATORY, laboratoryList);
		for (auto& iter : laboratoryList)
		{
			if (NULL == iter)
				continue;

			TCLaboratoryObj* laboratoryObj = dynamic_cast<TCLaboratoryObj*>(iter);
			if (laboratoryObj != NULL)
			{
				laboratoryObj->AddMakeMonsterCd(makeMonsterAddCd);
			}
		}
	}
}

void TCSTeamTwo::OpenAltar()
{
	InfoLogStream << TEAMINFO(this) << "open altar! " << LogStream::eos;

	std::list<TCGridObj*> objList;
	_GetObjListByObjType(TC_GRID_OBJ_ALTAR, objList);
	_GetObjListByObjType(TC_GRID_OBJ_TERROR_ALTAR, objList);
	for (auto& it : objList)
	{
		// 如果是废墟就不打开了
		if (it->GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
		{
			continue;
		}

		it->SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		SendGridObjData(it);
	}
}

void TCSTeamTwo::EndAltarRace(bool isPhaseEnd)
{
	InfoLogStream << TEAMINFO(this) << " end altar obj race!" << LogStream::eos;

	std::list<TCGridObj*> objList;
	_GetObjListByObjType(TC_GRID_OBJ_ALTAR, objList);
	_GetObjListByObjType(TC_GRID_OBJ_TERROR_ALTAR, objList);
	for (auto& it : objList)
	{
		FieldRaceEnd(it, 0, true);

		// 普通模式下，如果祭坛变成废墟则不处理
		if (GetTeamGrade() == TEAM_COPY_TEAM_GRADE_COMMON
			&& it->GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
		{
			continue;
		}

		// 噩梦模式下，重置通关类型
		if (GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF )
		{
			if (isPhaseEnd && it->GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
			{
				continue;
			}
			else
			{
				SetPassType(TC_2_PASS_TYPE_COMMON);
			}
		}

		it->SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
		SendGridObjData(it);
	}

	for (auto& iter : m_SquadObjMap)
	{
		TCSquadObj* squadObj = iter.second;
		if (NULL == squadObj || NULL == squadObj->GetBattleObj())
			continue;

		if (squadObj->GetBattleObj()->GetObjType() == TC_GRID_OBJ_ALTAR
			|| squadObj->GetBattleObj()->GetObjType() == TC_GRID_OBJ_TERROR_ALTAR)
		{
			if (isPhaseEnd)
			{
				BroadcastToMember(squadObj->GetSquadId(), 3219);
			}
			SquadReturnBorn(squadObj);
		}
	}
}

void TCSTeamTwo::AltarDestory(TCGridObj* altarObj)
{
	if (NULL == altarObj || altarObj->GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
		return;

	InfoLogStream << TEAMINFO(this) << " destroy altar obj type:" << altarObj->GetObjType()
		<< " boss stage:" << m_BossPhase << LogStream::eos;

	// 小于第二阶段击败祭坛，boss难度发生变化
	if (m_BossPhase < 2)
	{
		m_bossLinkFieldId = altarObj->GetFieldId();
		BossLink();

		if (altarObj->GetObjType() == TC_GRID_OBJ_TERROR_ALTAR)
			m_PassType = TC_2_PASS_TYPE_ENHANCE;
		else
			m_PassType = TC_2_PASS_TYPE_WEAKEN;

		InfoLogStream << TEAMINFO(this) << "refresh pass type:" << m_PassType << LogStream::eos;
	}

	// 另一个祭坛也死掉
	std::list<TCGridObj*> altarList;
	if (altarObj->GetObjType() == TC_GRID_OBJ_TERROR_ALTAR)
	{
		_GetObjListByObjType(TC_GRID_OBJ_ALTAR, altarList);
	}
	else
	{
		_GetObjListByObjType(TC_GRID_OBJ_TERROR_ALTAR, altarList);
	}

	for (auto& tt : altarList)
	{
		tt->SetObjStatus(TC_GRID_OBJ_STATUS_RUINS);
		tt->SetObjDeadReason(TC_OBJ_DEAD_REASON_GATE_LINK);
		FieldRaceEnd(tt, 0, true);
		SendGridObjData(tt);
	}
}

void TCSTeamTwo::FieldReborn(UInt32 objType, UInt32 objStatus)
{
	std::list<TCGridObj*> objList;
	_GetDeadObjListByObjType(objType, objList);

	for (auto& iter : objList)
	{
		iter->SetObjStatus(TCGridObjStatus(objStatus));
		SendGridObjData(iter);
		m_GridObjMap[iter->GetObjId()] = iter;
		m_DeadObjMap.erase(iter->GetObjId());
	}
}

void TCSTeamTwo::BossLink()
{
	if (m_bossLinkFieldId == 0)
		return;

	std::set<UInt64> bossRaceSet;
	std::list<TCGridObj*> objList;
	_GetObjListByObjType(TC_GRID_OBJ_BOSS, objList);
	for (auto& iter : objList)
	{
		_GetFieldRace(iter->GetObjId(), bossRaceSet);
	}

	TeamCopyFieldDataEntry* fieldData = TeamCopyFieldDataEntryMgr::Instance()->FindEntry(m_bossLinkFieldId);
	if (NULL == fieldData)
	{
		ErrorLogStream << TEAMINFO(this) << " not find field entry " << m_bossLinkFieldId << LogStream::eos;
		return;
	}

	for (auto& tt : bossRaceSet)
	{
		TCSDungeonMgr::Instance()->DungeonRaceDestroy(tt, fieldData->dungeonId);
	}
}

void TCSTeamTwo::FieldRaceEnd(TCGridObj* gridObj, UInt64 raceId, bool isVictory)
{
	if (NULL == gridObj)
	{
		ErrorLogStream << TEAMINFO(this) << " gridObjId null! " << LogStream::eos;
		return;
	}

	auto iter = m_FieldRaceMap.find(gridObj->GetObjId());
	if (iter == m_FieldRaceMap.end())
	{
		return;
	}

	// 先删除本场比赛
	if (raceId != 0)
	{
		iter->second.erase(raceId);
	}

	// 如果本场比赛胜利，并且据点已经死亡。需要删除其他的比赛
	if (isVictory && (gridObj->IsDead() || raceId == 0))
	{
		for (auto& it : iter->second)
		{
			TCSDungeonMgr::Instance()->DungeonRaceEnd(it, CDER_TEAMCOPY_FILED_DESTORY);
			_AddForceEndRace(it);

			InfoLogStream << TEAMINFO(this) << "obj id:" << gridObj->GetObjId() << " force end race:" << it << LogStream::eos;
		}

		m_FieldRaceMap.erase(iter);
	}
}

void TCSTeamTwo::AllRaceEnd()
{
	for (auto& tt : m_FieldRaceMap)
	{
		for (auto it : tt.second)
		{
			TCSDungeonMgr::Instance()->DungeonRaceEnd(it, CDER_TEAMCOPY_SETTLE);
			_AddForceEndRace(it);
		}
	}

	m_FieldRaceMap.clear();
}

void TCSTeamTwo::AddDeadField(TCGridObj* deadObj)
{
	if (NULL == deadObj)
	{
		return;
	}

	m_GridObjMap.erase(deadObj->GetObjId());
	m_DeadObjMap[deadObj->GetObjId()] = deadObj;
}

UInt32 TCSTeamTwo::GenObjId()
{
	return ++m_ObjSeed;
}

void TCSTeamTwo::PhaseBossEnd()
{
	m_BossPhase = 0;
}

bool TCSTeamTwo::BossObjIsBattle()
{
	std::list<TCGridObj*> bossList;
	_GetObjListByObjType(TC_GRID_OBJ_BOSS, bossList);

	for (auto& iter : bossList)
	{
		if (NULL == iter) continue;

		if(_IsHasFieldRace(iter->GetObjId()))
		{
			return true;
		}
	}

	return false;
}

std::list<UInt32> TCSTeamTwo::GetSearchPath(UInt32 srcGridId, UInt32 destGridId)
{
	std::set<UInt32> gridSet;
	_GetObjGridVec(gridSet);
	return TCSGridCfgMgr::Instance()->SearchWay(GetTeamGrade(), srcGridId, destGridId, gridSet);
}

bool TCSTeamTwo::IsSquadAttackMonster(UInt32 monsterObjId)
{
	for (auto& iter : m_SquadObjMap)
	{
		TCSquadObj* squadObj = iter.second;
		if (NULL == squadObj)
			continue;

		if (squadObj->GetBattleObj() != NULL && squadObj->GetBattleObj()->GetObjId() == monsterObjId)
		{
			return true;
		}
	}

	return false;
}

void TCSTeamTwo::SetPassType(UInt32 type)
{
	m_PassType = TC2PassType(type);
}

TCGridObj* TCSTeamTwo::_CreateGridObj(TCGridObjType type)
{
	switch (type)
	{
	case TC_GRID_OBJ_MAIN_CITY: return new TCMainCityObj();
	case TC_GRID_OBJ_BORN_POINT: return new TCSquadObj();
	case TC_GRID_OBJ_REVIVE_CNT: return new TCReviveCntObj();
	case TC_GRID_OBJ_CD_DOWN: return new TCCdDownObj();
	case TC_GRID_OBJ_LABORATORY: return new TCLaboratoryObj();
	case TC_GRID_OBJ_FIX_POINT: return new TCFixPointObj();
	case TC_GRID_OBJ_ENERGY_POINT: return new TCEnergyPointObj();
	case TC_GRID_OBJ_TRANSFER: return new TCTransferObj();
	case TC_GRID_OBJ_ALTAR: return new TCAltarObj();
	case TC_GRID_OBJ_TERROR_ALTAR: return new TCTerrorAltarObj();
	case TC_GRID_OBJ_BOSS: return new TCBossObj();
	default:break;
	}

	return NULL;
}

TCSquadObj* TCSTeamTwo::_FindSquadObj(UInt32 squadId)
{
	auto iter = m_SquadObjMap.find(squadId);
	return iter != m_SquadObjMap.end() ? iter->second : NULL;
}

TCMonsterObj* TCSTeamTwo::_FindMonsterObj(UInt32 monsterObjId)
{
	auto iter = m_MonsterObjMap.find(monsterObjId);
	return iter != m_MonsterObjMap.end() ? iter->second : NULL;
}

TCGridObj* TCSTeamTwo::_FindDeadObjByGridId(UInt32 gridId)
{
	for (auto& itr : m_DeadObjMap)
	{
		TCGridObj* gridObj = itr.second;
		if (NULL == gridObj)
			continue;

		if (gridObj->GetGridId() == gridId)
		{
			return gridObj;
		}
	}

	return NULL;
}

TCGridObj* TCSTeamTwo::_FindDeadObjByObjId(UInt32 deadObjId)
{
	auto iter = m_DeadObjMap.find(deadObjId);
	return iter != m_DeadObjMap.end() ? iter->second : NULL;
}

void TCSTeamTwo::_GetDeadObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList)
{
	for (auto& iter : m_DeadObjMap)
	{
		if (NULL == iter.second)
			continue;

		if (iter.second->GetObjType() == objType)
		{
			objList.push_back(iter.second);
		}
	}
}

void TCSTeamTwo::_GetObjGridVec(std::set<UInt32>& gridSet)
{
	for (auto& iter : m_GridObjMap)
	{
		gridSet.insert(iter.second->GetGridId());
	}
}

bool TCSTeamTwo::_OperatorPower(TeamCopyMember* member)
{
	if (member->post & TEAM_COPY_POST_CAPTAIN || member->post & TEAM_COPY_POST_CHIEF)
	{
		return true;
	}

	return false;
}

void TCSTeamTwo::_SquadTransferToGrid(TCSquadObj* squadObj, UInt32 gridId)
{
	// 格子有物体，在周围找一个
	for (UInt32 n = 1; n <= SEARCH_GRID_MAX_ROUND; ++n)
	{
		std::vector<UInt32> roundGridVec = TCSGridCfgMgr::Instance()->GetRoundGrid(GetTeamGrade(), gridId, n);
		if (roundGridVec.empty())
		{
			ErrorLogStream << TEAMINFO(this) << " gridId:" << gridId << " round empty:" << n << LogStream::eos;
			continue;
		}

		for (auto& it : roundGridVec)
		{
			TCGridObj* aroundObj = FindGridObjByGridId(it);
			if (NULL == aroundObj)
			{
				squadObj->SetPos(it);
				SendSquadObjData(squadObj);
				return;
			}
		}
	}
}

void TCSTeamTwo::_GetObjListByObjType(UInt32 objType, std::list<TCGridObj*>& objList)
{
	for (auto& iter : m_GridObjMap)
	{
		if (NULL == iter.second)
			continue;

		if (iter.second->GetObjType() == objType)
		{
			objList.push_back(iter.second);
		}
	}
}

void TCSTeamTwo::_FlopReward(UInt32 stage)
{
	const TeamCopyFlopDataEntry* flopCfg = TeamCopyFlopDataEntryMgr::Instance()->GetFlopReward(GetTeamType(), GetTeamGrade(), stage, m_PassType);
	if (NULL == flopCfg)
	{
		ErrorLogStream << TEAMINFO(this) << "not find flop config type:" << stage << LogStream::eos;
		return;
	}

	std::string sender = SysNotifyMgr::Instance()->MakeString(10090);
	std::string title = SysNotifyMgr::Instance()->MakeString(10091);
	std::string content = SysNotifyMgr::Instance()->MakeString(10092, stage == TEAM_COPY_STAGE_FINAL ? stage - 1 : stage);
	std::string reason = GetReason(SOURCE_TYPE_TEAM_COPY_FLOP, GetTeamType(), GetTeamGrade());

	UInt32 m = 0;
	for (auto& iter : m_SquadVec)
	{
		UInt32 k = 0;
		for (auto it : iter.teamMemberList)
		{
			ItemRewardVec rewards;
			TeamCopyFlop flop;
			flop.playerId = it.playerId;
			flop.playerName = it.playerName;
			if (stage == TEAM_COPY_STAGE_FINAL)
				flop.number = ++m;
			else
				flop.number = ++k;

			// 翻牌奖励限制
			TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(it.playerId);
			if (NULL == player)
			{
				ErrorLogStream << TEAMINFO(this) << "not find player:" << it.playerId << LogStream::eos;
				continue;
			}
			else
			{
				UInt32 settleStage = (stage == TEAM_COPY_STAGE_FINAL ? TEAM_COPY_STAGE_ONE : stage);
				if (!_StageIsSettle(settleStage, it.playerId))	// 该阶段是否成功的通关一次,最终阶段依赖第一阶段
				{
					flop.isLimit = TEAM_COPY_FLOP_LIMIT_PASS_GATE;
				}
				else
				{
					flop.isLimit = player->IsCanFlop(stage);
					if (flop.isLimit == TEAM_COPY_FLOP_LIMIT_NULL)
					{
						auto dropResult = DropItemMgr::Instance()->GenDropItems(0, it.playerId, flopCfg->dropId,
							player->GetVipLevel(), it.playerOccu);
						for (auto dropItem : dropResult.dropItems)
						{
							rewards.push_back(ItemReward(dropItem.typeId, dropItem.num, 0, 0));

							flop.rewardId = dropItem.typeId;
							flop.rewardNum = dropItem.num;
							flop.goldFlop = dropItem.CheckMark(DIMT_TEAM_COPY_GOLD);

							player->AddFlopNum(GetTeamType(), GetTeamGrade(), stage, _GameIsCrossDate((UInt32)CURRENT_TIME.Sec()));
							break; // 只有一个奖励
						}

						if (dropResult.dropItems.empty())
						{
							ErrorLogStream << TEAMINFO(this) << "flop reward empty!" << LogStream::eos;
						}
					}
				}

				// 团本翻拍udp
				player->SendUdpLog("team_copy_flop", LOG_TEAM_COPY_FLOP, GetTeamId(), stage, flop.rewardId,
					flop.rewardNum, flop.isLimit, GetTeamGrade(), GetTeamType(), UInt32(m_PassType));
			}

			_AddFlop(stage, flop);

			InfoLogStream << TEAMINFO(this) << "flop reward stage:" << stage << flop.SerializeStr() << LogStream::eos;

			if (!rewards.empty())
			{
				if (!SysUtility::SendMail(it.connId, it.playerId, sender, title, content, reason, rewards) != ErrorCode::SUCCESS)
				{
					ErrorLogStream << TEAMINFO(this) << "flop mail error! stage:" << stage << flop.SerializeStr() << LogStream::eos;
				}
			}
		}
	}
}

void TCSTeamTwo::_AddFieldRace(UInt32 fieldObjId, UInt64 raceId)
{
	auto iter = m_FieldRaceMap.find(fieldObjId);
	if (iter != m_FieldRaceMap.end())
	{
		iter->second.insert(raceId);
	}
	else
	{
		std::set<UInt64> raceIds;
		raceIds.insert(raceId);
		m_FieldRaceMap[fieldObjId] = raceIds;
	}
}

void TCSTeamTwo::_GetFieldRace(UInt32 fieldObjId, std::set<UInt64>& raceSet)
{
	auto iter = m_FieldRaceMap.find(fieldObjId);
	if (iter == m_FieldRaceMap.end())
	{
		return;
	}

	raceSet.insert(iter->second.begin(), iter->second.end());
}

bool TCSTeamTwo::_IsHasFieldRace(UInt32 fieldObjId)
{
	auto iter = m_FieldRaceMap.find(fieldObjId);
	if (iter == m_FieldRaceMap.end())
	{
		return false;
	}

	return !iter->second.empty();
}

void TCSTeamTwo::_SendReviveCntToRace(UInt64 raceId)
{
	if (m_AddReviveCnt == 0)
		return;

	TCSDungeonMgr::Instance()->AddRaceReviveCnt(raceId, m_AddReviveCnt);
}

void TCSTeamTwo::_LeaveReconnect()
{
	if ( !IsYetStartBattle())
		return;

	for (auto& iter : m_SquadVec)
	{
		if (iter.teamMemberList.empty())
			continue;

		std::list<TeamCopyMember*> leaveMemberList;
		TeamCopyMember* onlineMember = NULL;
		for (auto& it : iter.teamMemberList)
		{
			UInt64 leaveTime = TCSPlayerMgr::Instance()->PlayerLeaveTime(it.playerId);
			if (leaveTime != 0 && CURRENT_TIME.Sec() > leaveTime + GetTeamCopyValue(TC_VALUE_2_CHANGE_CAPTAIN_TIME))
			{
				if (it.post & TEAM_COPY_POST_CAPTAIN)
					leaveMemberList.push_back(&it);
				}
			else
			{
				if (NULL == onlineMember)
					onlineMember = &it;
			}
		}

		if (onlineMember != NULL && !leaveMemberList.empty() && leaveMemberList.size() != iter.teamMemberList.size())
		{
			for (auto& tt : leaveMemberList)
			{
				tt->post &= ~(TEAM_COPY_POST_CAPTAIN);
				onlineMember->post |= TEAM_COPY_POST_CAPTAIN;

				DebugLogStream << TEAMINFO(this) << " captain leave:" << tt->playerId << " player to captain:"
					<< onlineMember->playerId << LogStream::eos;
			}

			TeamDataBroadcast();
		}
	}
}

UInt32 TCSTeamTwo::_GetSquadMoveCnt(UInt32 squadId)
{
	TCSquadObj* squadObj = _FindSquadObj(squadId);
	return squadObj != NULL ? squadObj->GetMoveCnt() : 0;
}