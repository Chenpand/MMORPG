#include "TCSGridObj.h"

#include <CLErrorCode.h>
#include <CLTeamCopyProtocol.h>
#include <CLTeamCopyGridMonsterDataEntry.h>
#include "TCSGridCfgMgr.h"
#include "TCSTeamTwo.h"


TCGridObj::TCGridObj()
{
	m_ObjId = 0;
	m_GridId = 0;
	m_FieldId = 0;
	m_ObjType = TC_GRID_OBJ_NULL;
	m_ObjStatus = TC_GRID_OBJ_STATUS_NORMAL;
	m_ObjDeadReason = TC_OBJ_DEAD_REASON_LIVE;
	m_Team = NULL;
}

void TCGridObj::GetObjData(TCGridObjData& data)
{
	data.objId = GetObjId();
	data.gridId = GetGridId();
	data.gridType = GetObjType();
	data.gridStatus = GetObjStatus();
}

bool TCGridObj::IsCanAttack()
{
	switch (GetObjStatus())
	{
	case TC_GRID_OBJ_STATUS_NORMAL:
	case TC_GRID_OBJ_STATUS_BATTLE:
	case TC_GRID_OBJ_STATUS_PRE_BATTLE:
		return true;
	default:break;
	}

	return false;
}

void TCGridObj::PreBattle()
{
	SetObjStatus(TC_GRID_OBJ_STATUS_PRE_BATTLE);
}

void TCGridObj::Battle(UInt32 squadId)
{
	SetObjStatus(TC_GRID_OBJ_STATUS_BATTLE);
}

void TCGridObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_DEAD)
	{
		return;
	}

	if (isDestory)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
		GetTeam()->AddDeadField(this);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

std::string TCGridObj::GetObjStr()
{
	std::stringstream ss;
	ss << " objId:" << GetObjId();
	ss << ", objType:" << GetObjType();
	ss << ", gridId:" << GetGridId();
	ss << ", fieldId:" << GetFieldId();
	return ss.str();
}

bool TCGridObj::IsDead()
{
	return m_ObjStatus == TC_GRID_OBJ_STATUS_DEAD;
}

TCSquadObj::TCSquadObj()
{
	m_ObjType = TC_GRID_OBJ_SQUAD;
	m_SquadId = 0;
	m_Status = TC_GRID_SQUAD_STATUS_NORMAL;
	m_MoveCd = 0;
	m_MoveEndStamp = 0;
	m_BornGrid = 0;
	m_TargetGridId = 0;
	m_TargetObjId = 0;
	m_MoveCnt = 0;
	m_BattleObj = NULL;
}

TCSquadObj::~TCSquadObj()
{

}

bool TCSquadObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	m_SquadId = 0;
	m_Status = TC_GRID_SQUAD_STATUS_NORMAL;
	m_MoveCd = GetTeamCopyValue(TC_VALUE_2_SQUAD_MOVE_CD);
	m_MoveEndStamp = 0;
	m_GridId = GetGridId();
	m_BornGrid = GetGridId();

	return true;
}

void TCSquadObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_DOWN_TIME, m_MoveCd));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_END_STAMP, m_MoveEndStamp));
}

void TCSquadObj::OnTick(const Avalon::Time& now)
{
	// cd结束移动格子
	if (m_MoveEndStamp != 0 && GetObjStatus() == TC_GRID_OBJ_STATUS_NORMAL && UInt32(now.Sec()) >= m_MoveEndStamp)
	{
		if (m_PathList.empty())
		{
			m_MoveEndStamp = 0;
		}
		else
		{
			_MoveNextGrid();
		}
	}
}

bool TCSquadObj::IsCanAttack()
{
	return GetObjStatus() == TC_GRID_OBJ_STATUS_NORMAL;
}

void TCSquadObj::PreBattle()
{
	TCGridObj::PreBattle();
	StopMove();
}

void TCSquadObj::Battle(UInt32 squadId)
{
	TCGridObj::Battle(squadId);
	SetSquadObjStatus(TC_GRID_SQUAD_STATUS_BATTLE);
}

void TCSquadObj::BattleEnd(bool isDestory)
{
	SetSquadObjStatus(TC_GRID_SQUAD_STATUS_NORMAL);
	SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	SetBattleObj(NULL);
	SetTargetObjId(0);
	SetTargetGridId(0);
}

bool TCSquadObj::Move(UInt32 targetGridId, UInt32 targetObjId, std::list<UInt32>& pathList)
{
	if (targetGridId == 0 || pathList.empty())
	{
		return false;
	}

	m_Status = TC_GRID_SQUAD_STATUS_MOVE;
	m_TargetGridId = targetGridId;
	m_TargetObjId = targetObjId;
	m_PathList = pathList;

	if (m_MoveEndStamp == 0)
	{
		_MoveNextGrid();
	}

	return true;
}

void TCSquadObj::StopMove()
{
	SetSquadObjStatus(TC_GRID_SQUAD_STATUS_NORMAL);
	SetTargetObjId(0);
	SetTargetGridId(0);
	m_PathList.clear();
}

void TCSquadObj::SetPos(UInt32 gridId)
{
	SetGridId(gridId);
	StopMove();
}

void TCSquadObj::TargetDead(bool isBattleObj)
{
	StopMove();
	if (isBattleObj)
	{
		m_BattleObj = NULL;
	}
}

void TCSquadObj::ReduceMoveCD(UInt32 cd)
{
	// cd最少1秒
	if (cd >= m_MoveCd)
	{
		m_MoveCd = 1;
	}
	else
	{
		m_MoveCd -= cd;
	}
}

void TCSquadObj::_MoveNextGrid()
{
	// 下一个格子有怪物、据点 直接触发战斗
	UInt32 nextGridId = m_PathList.front();
	if (0 == nextGridId) return;

	TCGridObj* objPtr = GetTeam()->FindGridObjByGridId(nextGridId);
	if (NULL == objPtr)
	{
		m_PathList.pop_front();
		SetGridId(nextGridId);
		m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
		SetSquadObjStatus(TC_GRID_SQUAD_STATUS_MOVE);
		if (m_PathList.empty())
		{
			SetSquadObjStatus(TC_GRID_SQUAD_STATUS_NORMAL);
		}

		InfoLogStream << TEAMINFO(GetTeam()) << " squad:" << GetSquadId() << " move to grid:" << nextGridId << LogStream::eos;
		++m_MoveCnt;

		GetTeam()->SendSquadObjData(this);
		return;
	}

	bool isBlock = m_PathList.size() > 1;
	StopMove();
	GetTeam()->SendSquadObjData(this);

	if (objPtr->GetObjType() == TC_GRID_OBJ_SQUAD 
		|| objPtr->GetObjType() == TC_GRID_OBJ_MAIN_CITY
		|| objPtr->GetObjType() == TC_GRID_OBJ_MONSTER_TRANSFER
		|| objPtr->GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS
		|| objPtr->GetObjStatus() == TC_GRID_OBJ_STATUS_SEAL)
	{
		UInt32 notifyId = isBlock ? 3215 : 3214;
		GetTeam()->BroadcastToMember(GetSquadId(), notifyId);

		// 碰上小队，主城，废墟、封印，停止移动
		InfoLogStream << TEAMINFO(GetTeam()) << " squad:" << GetSquadId() << " stop move cur grid:" << GetGridId() << LogStream::eos;
	}
	else if (objPtr->GetObjType() == TC_GRID_OBJ_TRANSFER && objPtr->GetObjStatus() == TC_GRID_OBJ_STATUS_NORMAL)
	{
		// 碰上传送点，传送小队
		InfoLogStream << TEAMINFO(GetTeam()) << " transfer point squad:" << GetSquadId() << LogStream::eos;

		TCTransferObj* transferObj = dynamic_cast<TCTransferObj*>(objPtr);
		if (transferObj != NULL)
		{
			transferObj->TransferSquad(this);
		}
	}
	else if (objPtr->GetObjType() == TC_GRID_OBJ_ENERGY_POINT && objPtr->GetObjStatus() == TC_GRID_OBJ_STATUS_DEAD)
	{
		// 能量据点被摧毁，传送小队
		InfoLogStream << TEAMINFO(GetTeam()) << " energy point squad:" << GetSquadId() << LogStream::eos;

		TCEnergyPointObj* energyObj = dynamic_cast<TCEnergyPointObj*>(objPtr);
		if (energyObj != NULL)
		{
			energyObj->TransferSquad(this);
		}
	}
	else
	{
		if (GetTeam()->SquadBattle(objPtr, this) != ErrorCode::SUCCESS)
		{
			InfoLogStream << TEAMINFO(GetTeam()) << " squad:" << GetSquadId() << " start battle failed! obj id:" 
				<< objPtr->GetObjId() << LogStream::eos;
		}
	}
}

TCMonsterObj::TCMonsterObj()
{
	m_ObjType = TC_GRID_OBJ_MONSTER;
	m_MoveCd = 0;
	m_MoveEndStamp = 0;
	m_ProId = 0;
	m_IsInitPath = true;
}

TCMonsterObj::~TCMonsterObj()
{

}

bool TCMonsterObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() < 1)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " monster param error!" << LogStream::eos;
		return false;
	}

	TeamCopyGridMonsterDataEntry* cfg = TeamCopyGridMonsterDataEntryMgr::Instance()->
		GetGridMonsterData(GetTeam()->GetTeamGrade(), paramVec[0], GetGridId());
	if (NULL == cfg)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " not find grid monster cfg:" << GetGridId() << LogStream::eos;
		return false;
	}

	m_MoveCd = cfg->moveCd;
	m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
	m_ProId = cfg->id;
	m_MovePathList = cfg->attackPathList;

	return true;
}

void TCMonsterObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_DOWN_TIME, m_MoveCd));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_END_STAMP, m_MoveEndStamp));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_MONSTER_PRO_ID, m_ProId));
}

void TCMonsterObj::OnTick(const Avalon::Time& now)
{
	if (UInt32(now.Sec()) < m_MoveEndStamp)
		return;

	if (GetObjStatus() != TC_GRID_OBJ_STATUS_NORMAL)
	{
		m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
		GetTeam()->SendGridObjData(this);
		return;
	}

	_MoveNextGrid();
}

void TCMonsterObj::Battle(UInt32 squadId)
{
	TCGridObj::Battle(squadId);

	RemoveSquadTarget(squadId);
}

void TCMonsterObj::BattleEnd(bool isDestory)
{
	// 需要小队先结算，然后怪物后结算
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_DEAD || GetObjStatus() == TC_GRID_OBJ_STATUS_DESTORY)
	{
		return;
	}

	if (isDestory)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		// 看下是否还有其他人在打该怪物，如果没有怪物前进一格
		if (!GetTeam()->IsSquadAttackMonster(GetObjId()))
		{
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec();
			SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		}
	}
}

void TCMonsterObj::AddSquadTarget(UInt32 squadId)
{
	m_SquadTargetSet.insert(squadId);
}

void TCMonsterObj::RemoveSquadTarget(UInt32 squadId)
{
	m_SquadTargetSet.erase(squadId);
}

void TCMonsterObj::_MoveNextGrid()
{
	if (m_MovePathList.empty())
	{
		m_MovePathList = GetTeam()->GetSearchPath(GetGridId(), GetTeam()->GetMainCityGridId());
		if (m_MovePathList.empty())
		{
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
			GetTeam()->SendGridObjData(this);
			return;
		}
	}

	UInt32 nextGridId = m_MovePathList.front();
	TCGridObj* objPtr = GetTeam()->FindGridObjByGridId(nextGridId);
	if (NULL == objPtr)
	{
		SetGridId(nextGridId);
		if (m_IsInitPath)
		{
			m_MovePathList.pop_front();
		}
		else
		{
			m_MovePathList = GetTeam()->GetSearchPath(GetGridId(), GetTeam()->GetMainCityGridId());
		}
		m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
		GetTeam()->SendGridObjData(this);

		InfoLogStream << TEAMINFO(GetTeam()) << " monster:" << GetObjId() << " move to grid:" << GetGridId() << LogStream::eos;

		// 怪物移动，如果有小队把怪物作为目标，此时通知小队重新寻路
		for (auto& it : m_SquadTargetSet)
		{
			GetTeam()->SquadReSearchWay(it, GetGridId(), GetObjId());
		}
		
		return;
	}
	else if (objPtr->GetObjType() == TC_GRID_OBJ_MAIN_CITY)
	{
		// 怪物进入主城
		TCMainCityObj* mainObj = dynamic_cast<TCMainCityObj*>(objPtr);
		if (mainObj != NULL)
		{
			mainObj->CostBlood();
			InfoLogStream << TEAMINFO(GetTeam()) << " monster:" << GetObjId() << " enter main city blood!" << LogStream::eos;

			GetTeam()->SendGridObjData(objPtr);
			GetTeam()->GridObjDestory(this, false);
		}
		return;
	}
	else if (objPtr->GetObjType() == TC_GRID_OBJ_SQUAD && objPtr->GetObjStatus() == TC_GRID_OBJ_STATUS_NORMAL)
	{
		InfoLogStream << TEAMINFO(GetTeam()) << " monster:" << GetObjId() << " see squad start race!" << LogStream::eos;

		// 开战时，全员掉线。小队直接返回出生点
		UInt32 ret = GetTeam()->SquadBattle(this, dynamic_cast<TCSquadObj*>(objPtr));
		if (ret == ErrorCode::TEAM_COPY_SQUAD_ALL_DISCONNECT)
		{
			GetTeam()->SquadReturnBorn(dynamic_cast<TCSquadObj*>(objPtr));

			// 怪物往前走一格
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec();
		}
		else
		{
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
 			GetTeam()->SendGridObjData(this);
		}
	}
	else
	{
		// 此路不通，怪物下一轮tick再走一次
		m_IsInitPath = false;
		m_MovePathList = GetTeam()->GetSearchPath(GetGridId(), GetTeam()->GetMainCityGridId());
		if (m_MovePathList.empty())
		{
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec() + m_MoveCd;
			GetTeam()->SendGridObjData(this);
		}
		else
		{
			m_MoveEndStamp = (UInt32)CURRENT_TIME.Sec();
		}
	}
}

TCTransferObj::TCTransferObj()
{
	m_ObjType = TC_GRID_OBJ_TRANSFER;
	m_ObjStatus = TC_GRID_OBJ_STATUS_NO_ACTIVE;
}

TCTransferObj::~TCTransferObj()
{

}

bool TCTransferObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	m_TransferGridVec = paramVec2;
	return true;
}

void TCTransferObj::TransferSquad(TCSquadObj* squadObj)
{
	if (NULL == squadObj) return;
	
	for (auto& iter : m_TransferGridVec)
	{
		TCGridObj* gridObj = GetTeam()->FindGridObjByGridId(iter);
		if (NULL == gridObj)
		{
			squadObj->SetPos(iter);
			GetTeam()->SendSquadObjData(squadObj);
			return;
		}
	}

	ErrorLogStream << TEAMINFO(GetTeam()) << " transfer point transfer failed! squad:" << squadObj->GetSquadId() << LogStream::eos;
}

TCFixPointObj::TCFixPointObj()
{
	m_ObjType = TC_GRID_OBJ_FIX_POINT;
	m_Blood = 1;
}

TCFixPointObj::~TCFixPointObj()
{

}

bool TCFixPointObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (GetTeam()->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
	{
		if (paramVec.size() != 1)
		{
			ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
			return false;
		}

		m_Blood = paramVec[0];
	}

	return true;
}

void TCFixPointObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	if (GetTeam()->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
	{
		data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_ODD_BLOOD, m_Blood));
	}
}

void TCFixPointObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_DEAD)
	{
		return;
	}

	if (isDestory)
	{
		if (m_Blood > 0)
		{
			m_Blood--;
		}

		if (m_Blood == 0)
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
			SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
			GetTeam()->GridObjDestory(this);
			GetTeam()->AddDeadField(this);
		}
		else
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		}
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

TCEnergyPointObj::TCEnergyPointObj()
{
	m_ObjType = TC_GRID_OBJ_ENERGY_POINT;
	m_MakeMonsterTransferStamp = 0;
	m_MakeMonsterTransferCd = 0;
	m_MonsterTransferLiveTime = 0;
}

TCEnergyPointObj::~TCEnergyPointObj()
{

}

bool TCEnergyPointObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	// 只有噩梦难度才会用到第一个参数列表
	if (GetTeam()->GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
	{
		if (paramVec.size() < 4)
		{
			ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
			return false;
		}

		m_MakeMonsterTransferCd = paramVec[1];
		m_MonsterTransferLiveTime = paramVec[2];
		m_MakeMonsterTransferStamp = UInt32(CURRENT_TIME.Sec()) + m_MakeMonsterTransferCd;
		{
			std::vector<UInt32> bornIndexVec;
			for (UInt32 i = 3; i < paramVec.size(); ++i)
			{
				bornIndexVec.push_back(paramVec[i]);
			}
			TCSGridCfgMgr::Instance()->GetMonsterBornGrid(GetTeam()->GetTeamGrade(), bornIndexVec, m_MonsterGridVec);
		}
	}

	m_TransferGridVec = paramVec2;
	return true;
}

void TCEnergyPointObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_DOWN_TIME, m_MakeMonsterTransferCd));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_END_STAMP, m_MakeMonsterTransferStamp));
}

void TCEnergyPointObj::OnTick(const Avalon::Time& now)
{
	UInt32 curTime = UInt32(now.Sec());

	//***********注意：：只有噩梦难度才会刷出怪物传送门****************//

	// 刷新出怪物传送们
	if (m_MakeMonsterTransferStamp != 0 && curTime >= m_MakeMonsterTransferStamp)
	{
		m_MakeMonsterTransferStamp = curTime + m_MakeMonsterTransferCd;
		GetTeam()->SendGridObjData(this);
		GetTeam()->MakeMonsterTransfer(m_MonsterTransferLiveTime, m_MonsterGridVec);
	}
}

void TCEnergyPointObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_DEAD)
	{
		return;
	}

	if (isDestory)
	{
		ClearMonsterTransferData();
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
		GetTeam()->TransferFieldActive();
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

void TCEnergyPointObj::TransferSquad(TCSquadObj* squadObj)
{
	if (NULL == squadObj) return;
	
	for (auto& iter : m_TransferGridVec)
	{
		TCGridObj* gridObj = GetTeam()->FindGridObjByGridId(iter);
		if (NULL == gridObj)
		{
			squadObj->SetPos(iter);
			GetTeam()->SendSquadObjData(squadObj);
			return;
		}
	}

	ErrorLogStream << TEAMINFO(GetTeam()) << " energy point transfer failed! squad:" << squadObj->GetSquadId() << LogStream::eos;
}

void TCEnergyPointObj::ClearMonsterTransferData()
{
	m_MakeMonsterTransferStamp = 0;
}

TCAltarObj::TCAltarObj()
{
	m_ObjType = TC_GRID_OBJ_ALTAR;
	SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
}

TCAltarObj::~TCAltarObj()
{

}

void TCAltarObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_DEAD_REASON, m_ObjDeadReason));
}

void TCAltarObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
	{
		return;
	}

	if (isDestory)
	{
		GetTeam()->AltarDestory(this);
		SetObjStatus(TC_GRID_OBJ_STATUS_RUINS);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		if (GetTeam()->BossObjIsBattle() && GetTeam()->GetBossPhase() != 2)
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		}
		else
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
		}
	}
}

bool TCAltarObj::IsDead()
{
	return m_ObjStatus == TC_GRID_OBJ_STATUS_RUINS;
}

TCTerrorAltarObj::TCTerrorAltarObj()
{
	m_ObjType = TC_GRID_OBJ_TERROR_ALTAR;
	SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
}

TCTerrorAltarObj::~TCTerrorAltarObj()
{

}

void TCTerrorAltarObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_DEAD_REASON, m_ObjDeadReason));
}

void TCTerrorAltarObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
	{
		return;
	}

	if (isDestory)
	{
		GetTeam()->AltarDestory(this);
		SetObjStatus(TC_GRID_OBJ_STATUS_RUINS);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		if (GetTeam()->BossObjIsBattle() && GetTeam()->GetBossPhase() != 2)
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		}
		else
		{
			SetObjStatus(TC_GRID_OBJ_STATUS_SEAL);
		}
	}
}

bool TCTerrorAltarObj::IsDead()
{
	return m_ObjStatus == TC_GRID_OBJ_STATUS_RUINS;
}

TCBossObj::TCBossObj()
{
	m_ObjType = TC_GRID_OBJ_BOSS;
}

TCBossObj::~TCBossObj()
{

}

void TCBossObj::BattleEnd(bool isDestory)
{
	TCGridObj::BattleEnd(isDestory);
	GetTeam()->PhaseBossEnd();

	if (isDestory)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		GetTeam()->GameOver(TC_GAME_OVER_REASON_VICTORY);
	}
}

void TCBossObj::Battle(UInt32 squadId)
{
	TCGridObj::Battle(squadId);
	GetTeam()->OpenAltar();
}

TCMainCityObj::TCMainCityObj()
{
	m_ObjType = TC_GRID_OBJ_MAIN_CITY;
	m_Blood = 0;
}

TCMainCityObj::~TCMainCityObj()
{

}

bool TCMainCityObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() != 1)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
		return false;
	}

	m_Blood = paramVec[0];

	return true;
}

void TCMainCityObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_ODD_BLOOD, m_Blood));
}

void TCMainCityObj::BattleEnd(bool isDestory)
{
	// 目前GM使用该接口
	m_Blood = 0;
	SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
	//SetObjDeadReason(TC_OBJ_DEAD_REASON_SQUAD_DESTORY);
	//GetTeam()->AddDeadField(this);
	GetTeam()->GameOver(TC_GAME_OVER_REASON_MAIN_CITY_DEAD);
}

void TCMainCityObj::CostBlood()
{
	if (m_Blood != 0)
	{
		--m_Blood;
	}

	if (m_Blood == 0)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		SetObjDeadReason(TC_OBJ_DEAD_REASON_ENTER_MAIN_CITY);
		//GetTeam()->AddDeadField(this);
		GetTeam()->GameOver(TC_GAME_OVER_REASON_MAIN_CITY_DEAD);
	}
}

TCReviveCntObj::TCReviveCntObj()
{
	m_ObjType = TC_GRID_OBJ_REVIVE_CNT;
	m_ReviveCnt = 0;
}

TCReviveCntObj::~TCReviveCntObj()
{

}

bool TCReviveCntObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() != 1)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
		return false;
	}

	m_ReviveCnt = paramVec[0];

	return true;
}

void TCReviveCntObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
	{
		return;
	}

	if (isDestory)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_RUINS);
		GetTeam()->AddReviveCnt(GetReviveCnt());
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

bool TCReviveCntObj::IsDead()
{
	return m_ObjStatus == TC_GRID_OBJ_STATUS_RUINS;
}

TCCdDownObj::TCCdDownObj()
{
	m_ObjType = TC_GRID_OBJ_CD_DOWN;
	m_SquadReduceCd = 0;
	m_MonsterAddCd = 0;
	m_MakeMonsterAddCd = 0;
}

TCCdDownObj::~TCCdDownObj()
{

}

bool TCCdDownObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() != 3)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
		return false;
	}

	m_SquadReduceCd = paramVec[0];
	m_MonsterAddCd = paramVec[1];
	m_MakeMonsterAddCd = paramVec[2];

	return true;
}

void TCCdDownObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_RUINS)
	{
		return;
	}

	if (isDestory)
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_RUINS);
		GetTeam()->UpdateGridObjCd(m_SquadReduceCd, m_MonsterAddCd, m_MakeMonsterAddCd);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

bool TCCdDownObj::IsDead()
{
	return m_ObjStatus == TC_GRID_OBJ_STATUS_RUINS;
}

TCLaboratoryObj::TCLaboratoryObj()
{
	m_ObjType = TC_GRID_OBJ_LABORATORY;
	m_ReviceCD = 0;
	m_MakeMonsterCD = 0;
	m_AddReviveCD = 0;
	m_ReviveEndStamp = 0;
	m_MakeMonsterEndStamp = 0;
}

TCLaboratoryObj::~TCLaboratoryObj()
{

}

bool TCLaboratoryObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() != 6)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " obj cfg error!" << LogStream::eos;
		return false;
	}

	m_ReviceCD = paramVec[0];
	m_MakeMonsterCD = paramVec[1];
	m_AddReviveCD = paramVec[2];
	m_MakeMonsterEndStamp = UInt32(CURRENT_TIME.Sec()) + m_MakeMonsterCD;

	// 怪物出生点处理
	std::vector<UInt32> bornIndexVec = { paramVec[3], paramVec[4], paramVec[5] };
	TCSGridCfgMgr::Instance()->GetMonsterBornGrid(GetTeam()->GetTeamGrade(), bornIndexVec, m_MonsterBornGridVec);

	return true;
}

void TCLaboratoryObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_DOWN_TIME, m_MakeMonsterCD));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_END_STAMP, m_MakeMonsterEndStamp));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_REVIVE_CD_STAMP, m_ReviveEndStamp));
}

void TCLaboratoryObj::OnTick(const Avalon::Time& now)
{
	// 复活
	if (m_ReviveEndStamp != 0 && UInt32(now.Sec()) >= m_ReviveEndStamp)
	{
		m_ReviveEndStamp = 0;
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
		GetTeam()->SendGridObjData(this);
	}

	// 制造怪物
	if (m_ReviveEndStamp == 0 && UInt32(now.Sec()) >= m_MakeMonsterEndStamp)
	{
		GetTeam()->MakeMonster(TC_GRID_MONSTER_TYPE_MAKE, m_MonsterBornGridVec);
		m_MakeMonsterEndStamp = UInt32(CURRENT_TIME.Sec()) + m_MakeMonsterCD;
		GetTeam()->SendGridObjData(this);
	}
}

void TCLaboratoryObj::BattleEnd(bool isDestory)
{
	if (GetObjStatus() == TC_GRID_OBJ_STATUS_REVIVE)
	{
		return;
	}

	if (isDestory)
	{
		m_ReviveEndStamp = UInt32(CURRENT_TIME.Sec()) + m_ReviceCD;
		SetObjStatus(TC_GRID_OBJ_STATUS_REVIVE);
		GetTeam()->LaboratoryLink(this);
		GetTeam()->GridObjDestory(this);
	}
	else
	{
		SetObjStatus(TC_GRID_OBJ_STATUS_NORMAL);
	}
}

void TCLaboratoryObj::AddReviveCd()
{
	m_ReviveEndStamp += m_AddReviveCD;
	GetTeam()->SendGridObjData(this);
}

TCMonsterTransferObj::TCMonsterTransferObj()
{
	m_ObjType = TC_GRID_OBJ_MONSTER_TRANSFER;
}

TCMonsterTransferObj::~TCMonsterTransferObj()
{

}

bool TCMonsterTransferObj::InitData(const std::vector<UInt32>& paramVec, const std::vector<UInt32>& paramVec2)
{
	if (paramVec.size() < 1)
	{
		ErrorLogStream << TEAMINFO(GetTeam()) << " param size error!" << LogStream::eos;
		return false;
	}

	m_MonsterTransferLiveTime = paramVec[0];
	m_MonsterTransferLiveStamp = UInt32(CURRENT_TIME.Sec()) + m_MonsterTransferLiveTime;

	return true;
}

void TCMonsterTransferObj::GetObjData(TCGridObjData& data)
{
	TCGridObj::GetObjData(data);
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_DOWN_TIME, m_MonsterTransferLiveTime));
	data.gridPro.push_back(TCGridProperty(TC_GRID_PRO_CD_END_STAMP, m_MonsterTransferLiveStamp));
}

void TCMonsterTransferObj::OnTick(const Avalon::Time& now)
{
	UInt32 curTime = UInt32(now.Sec());

	// 怪物传送门死亡，变成一个怪物
	if (m_MonsterTransferLiveStamp != 0 && curTime >= m_MonsterTransferLiveStamp)
	{
		UInt32 oldGrid = GetGridId();
		SetObjStatus(TC_GRID_OBJ_STATUS_DEAD);
		GetTeam()->SendGridObjData(this);
		GetTeam()->GridObjDestory(this);
		SetGridId(0);	// 死亡的传送门就不用占格子,腾出格子给出生的怪物
		GetTeam()->MakeMonster(TC_GRID_MONSTER_TYPE_TRANSFER, { oldGrid });
		m_MonsterTransferLiveStamp = 0;
	}
}
