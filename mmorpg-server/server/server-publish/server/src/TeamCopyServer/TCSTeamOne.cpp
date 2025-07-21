#include "TCSTeamOne.h"

#include <CLErrorCode.h>
#include <CLTeamCopyTargetDataEntry.h>
#include <CLTeamCopyBossDataEntry.h>
#include <CLTeamCopyFlopDataEntry.h>
#include <CLTeamCopyProtocol.h>
#include <CLDungeonProtocol.h>
#include <CLGameSessionProtocol.h>
#include <CLCrossDungeonDefine.h>
#include <CLTeamCopyDefine.h>

#include "TCSDungeonMgr.h"
#include "TCSPlayerMgr.h"
#include "TCSTeamMgr.h"
#include "SysUtility.h"

TCSTeamOne::TCSTeamOne()
{
	UInt32 squadNum = GetTeamCopyValue(TC_VALUE_SQUAD_NUM);
	m_SquadVec.resize(squadNum);
	for (UInt32 k = 1; k <= squadNum; ++k)
	{
		TeamCopySquad squad;
		squad.squadId = k;
		squad.squadStatus = TEAM_COPY_SQUAD_STATUS_INIT;
		squad.teamMemberList.clear();

		m_SquadVec[k - 1] = squad;
	}

	m_TickTimers.SetInterval(1000);
}

TCSTeamOne::~TCSTeamOne()
{

}

bool TCSTeamOne::OnTick(const Avalon::Time& now)
{
	if ( !TCSTeam::OnTick(now))
	{
		return false;
	}

	// 据点重生
	std::vector<UInt32> filedVec;
	for (auto& iter : m_FieldMap)
	{
		TeamCopyField& field = iter.second;
		if (field.rebornTime == 0 || now.Sec() < field.rebornTime)
		{
			continue;
		}

		// 重生
		++field.oddNum;
		field.state = TEAM_COPY_FIELD_STATUS_INIT;
		field.rebornTime = 0;

		if (field.config->rebornTime != 0 && field.oddNum < field.config->defeatCond)
		{
			field.rebornTime = UInt32(CURRENT_TIME.Sec()) + field.config->rebornTime;
			field.state = TEAM_COPY_FIELD_STATUS_REBORN;
		}

		filedVec.push_back(iter.first);
	}

	// 通知据点信息
	if (!filedVec.empty())
	{
		FieldNotify(filedVec);
	}

	// 噩梦难度据点能量据点恢复
	if (GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF && IsInBattle() && m_TickTimers.IsTimeout(now))
	{
		TeamCopyField* field = FindField(GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD));
		if (field != NULL && field->energyReviveTime != 0)
		{
			UInt32 energyPro = (UInt32)CURRENT_TIME.Sec() - (field->energyReviveTime);
			UInt32 oneRate = GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_BUFF_ONE_RATE);
			UInt32 twoRate = GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_BUFF_TWO_RATE);

			// 前后给一秒误差，通知能量恢复关联据点
			if ((energyPro >= (oneRate - 1) && energyPro <= (oneRate + 1))
				|| (energyPro >= (twoRate - 1) && energyPro <= (twoRate + 1)))
			{
				_HandleEnergyReviveField(energyPro, field->config->dungeonId);
			}
		}
	}

	return true;
}

void TCSTeamOne::SquadAttackDataNotify()
{
	CLProtocol::TeamCopyTargetNotify notify;
	notify.teamTarget = m_TeamTarget;

	for (auto& iter : m_SquadVec)
	{
		notify.squadTarget = iter.squadTarget;
		BroadcastToMember(notify, iter.squadId);
	}
}

void TCSTeamOne::GetTicket(std::vector<TCConsumeItem>& itemVec)
{
	TCConsumeItem item1;
	item1.id = GetTeamCopyValue(TC_VALUE_TICKET_ID_1);
	item1.num = GetTeamCopyValue(TC_VALUE_TICKET_NUM_1);
	if (item1.id != 0 && item1.num != 0)
		itemVec.push_back(item1);

	TCConsumeItem item2;
	item2.id = GetTeamCopyValue(TC_VALUE_TICKET_ID_2);
	item2.num = GetTeamCopyValue(TC_VALUE_TICKET_NUM_2);
	if (item2.id != 0 && item2.num != 0)
		itemVec.push_back(item2);
}

UInt32 TCSTeamOne::GetGoldMaxNum()
{
	return GetTeamCopyValue(TC_VALUE_GOLD_MAX_NUM);
}

UInt32 TCSTeamOne::GetGoldNumErrorCode()
{
	return ErrorCode::TEAM_COPY_ONE_GOLD_FULL;
}

bool TCSTeamOne::SetTeamTarget(UInt32 stage)
{
	const TeamCopyTargetCfg* targetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetFirstTarget(GetTeamType(), GetTeamGrade(), stage, TEAM_COPY_GRADE_TEAM);
	if (NULL == targetCfg)
	{
		ErrorLogStream << TEAMINFO(this) << "not find grade stage config:" << TEAM_COPY_GRADE_TEAM << LogStream::eos;
		return false;
	}

	InfoLogStream << TEAMINFO(this) << "set team targetId:" << targetCfg->id << LogStream::eos;

	m_TeamTarget.targetId = targetCfg->id;
	m_TeamTarget.config = targetCfg;
	m_TeamTarget.targetDetailVec.clear();

	for (auto& tt : targetCfg->feildVec)
	{
		TeamCopyTargetDetail detail;
		detail.fieldId = tt.fieldId;
		detail.curNum = 0;
		detail.totalNum = tt.totalNum;

		m_TeamTarget.targetDetailVec.push_back(detail);
	}

	return true;
}

void TCSTeamOne::AttackDataNotify(UInt32 stage)
{
	CLProtocol::TeamCopyStageNotify notify;
	notify.stageId = stage;
	notify.gameOverTime = GetGameOverTime();
	notify.teamTarget = m_TeamTarget;

	for (auto& it : m_FieldMap)
	{
		if (!TeamCopyTargetDataEntryMgr::Instance()->IsHasFieldByStage(GetTeamGrade(), stage, it.first))
		{
			continue;
		}

		notify.fieldList.push_back(it.second);
	}

	for (auto& iter : m_SquadVec)
	{
		notify.squadTarget = iter.squadTarget;
		BroadcastToMember(notify, iter.squadId);
	}
}

void TCSTeamOne::AttackDataNotify(TCSPlayer* player)
{
	if (NULL == player || GetCurStage() == 0)
		return;

	CLProtocol::TeamCopyStageNotify notify;
	notify.gameOverTime = GetGameOverTime();
	notify.stageId = GetCurStage();
	notify.teamTarget = m_TeamTarget;

	for (auto& it : m_FieldMap)
	{
		if (!TeamCopyTargetDataEntryMgr::Instance()->IsHasFieldByStage(GetTeamGrade(), GetCurStage(), it.first))
		{
			continue;
		}
		notify.fieldList.push_back(it.second);
	}

	TeamCopySquad* squad = _FindSquad(_GetPlayerSquadId(player->GetID()));
	if (squad != NULL)
	{
		notify.squadTarget = squad->squadTarget;
	}

	player->SendProtocol(notify);

	BossPhaseNotify(player);

	if (player->GetID() == GetChiefId())
	{
		_NotifyForceEndFlag();
	}
}

void TCSTeamOne::ClearField(UInt32 squadId, UInt32 fieldId)
{
	// 已经结束了
	if (IsGameOver())
	{
		return;
	}

	// 更新据点信息,解锁新据点
	UInt32 fieldState = _UpdateField(fieldId);
	if (fieldState == TEAM_COPY_FIELD_STATUS_INVALID)
	{
		return;	// 据点未解锁
	}

	// 团队目标
	for (auto& iter : m_TeamTarget.targetDetailVec)
	{
		if (iter.fieldId != fieldId)
			continue;

		iter.curNum++;
	}

	// 团队目标完成,进入下一阶段
	if (m_TeamTarget.isFinish())
	{
		// 自动完成所有小队目标
		for (auto& squadIt : m_SquadVec)
		{
			for (auto& iter : squadIt.squadTarget.targetDetailVec)
			{
				if (iter.curNum < iter.totalNum)
					iter.curNum = iter.totalNum;
			}
		}

		// 阶段完成udp，2成功
		UInt32 costTime = (UInt32)CURRENT_TIME.Sec() - _GetGameStartTime();
		UdpStageEnd(costTime, 2);

		InfoLogStream << TEAMINFO(this) << "team target finish stage:" << GetCurStage() << LogStream::eos;

		// 通知所有在地下城的小队退出
		NotifyAllSquadLeaveDungeon();

		// 上一阶段结束
		_StageEnd(GetCurStage());

		// 翻牌奖励
		_FlopReward(GetCurStage());

		// 完成最终目标
		if (GetCurStage() == TEAM_COPY_STAGE_FINAL - 1)
		{
			GameOver(TC_GAME_OVER_REASON_VICTORY);
			return;
		}

		// 设置下一阶段，修改阶段
		SetCurStage(GetCurStage() + 1);

		// 设置团队目标
		SetTeamTarget(GetCurStage());

		// 分配小队目标
		if (GetPlanModel() == TEAM_COPY_PLAN_MODEL_GUIDE)
		{
			for (auto& tt : m_SquadVec)
			{
				const TeamCopyTargetCfg* targetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetFirstTarget(GetTeamType(), GetTeamGrade(), GetCurStage(), m_SquadGrade[tt.squadId]);
				if (targetCfg != NULL)
				{
					SetSquadTarget(tt.squadId, targetCfg);
				}
				else
				{
					ErrorLogStream << TEAMINFO(this) << "not find grade stage config:" << m_SquadGrade[tt.squadId] << " stage:" << GetCurStage() << LogStream::eos;
				}
			}
		}

		AttackDataNotify(GetCurStage());

		return;
	}

	if (GetPlanModel() == TEAM_COPY_PLAN_MODEL_GUIDE)
	{
		//据点被歼灭，所有拥有该目标的小队自动完成目标
		if (fieldState == TEAM_COPY_FIELD_STATUS_DEFEAT)
		{
			InfoLogStream << TEAMINFO(this) << "team defeat field id:" << fieldId << LogStream::eos;

			for (auto& squadIt : m_SquadVec)
			{
				const TeamCopyTargetCfg* nextTargetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetTarget(squadIt.squadTarget.config->nextTarget);
				if (nextTargetCfg != NULL)
				{
					SetSquadTarget(squadIt.squadId, nextTargetCfg);
				}
				else
				{
					for (auto& iter : squadIt.squadTarget.targetDetailVec)
					{
						if (iter.fieldId != fieldId)
							continue;

						iter.curNum++;
					}
				}
			}
		}
		else
		{
			TeamCopySquad* squad = _FindSquad(squadId);
			if (NULL == squad)
			{
				ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << LogStream::eos;
				return;
			}

			// 更新玩家小队目标
			for (auto& squadIter : squad->squadTarget.targetDetailVec)
			{
				if (squadIter.fieldId != fieldId)
					continue;

				squadIter.curNum++;
			}

			InfoLogStream << TEAMINFO(this) << " squadId:" << squadId << " fieldId:" << fieldId << LogStream::eos;

			// 当前目标完成，接收下一目标
			if (squad->squadTarget.isFinish())
			{
				const TeamCopyTargetCfg* nextTargetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetTarget(squad->squadTarget.config->nextTarget);
				if (nextTargetCfg != NULL)
				{
					TeamCopyField* tmpField = FindField(nextTargetCfg->id);
					if (NULL != tmpField)
					{
						SetSquadTarget(squadId, nextTargetCfg);
					}
				}
			}
		}
	}

	// 小队攻坚数据通知
	SquadAttackDataNotify();
}

void TCSTeamOne::SetSquadGrade(UInt32 squadId, UInt32 grade)
{
	m_SquadGrade[squadId] = grade;
}

void TCSTeamOne::SetSquadTarget(UInt32 squadId, const TeamCopyTargetCfg* targetCfg)
{
	if (NULL == targetCfg)
	{
		ErrorLogStream << TEAMINFO(this) << "target cfg null !" << LogStream::eos;
		return;
	}

	TeamCopySquad* squad = _FindSquad(squadId);
	if (NULL == squad)
	{
		ErrorLogStream << TEAMINFO(this) << "not find squad id:" << squadId << LogStream::eos;
		return;
	}

	InfoLogStream << TEAMINFO(this) << "set squad id:" << squadId << " targetId:" << targetCfg->id << LogStream::eos;

	squad->squadTarget.targetId = targetCfg->id;
	squad->squadTarget.config = targetCfg;
	squad->squadTarget.targetDetailVec.clear();

	for (auto& tt : targetCfg->feildVec)
	{
		TeamCopyTargetDetail detail;
		detail.fieldId = tt.fieldId;
		detail.curNum = 0;
		detail.totalNum = tt.totalNum;

		squad->squadTarget.targetDetailVec.push_back(detail);
	}
}


bool TCSTeamOne::AddField(const TeamCopyFieldDataEntry* fieldEntry)
{
	if (NULL == fieldEntry) return false;

	TeamCopyField field;
	field.fieldId = fieldEntry->id;
	field.oddNum = fieldEntry->defeatCond;
	field.state = fieldEntry->appearStatus;
	field.config = fieldEntry;

	if (!m_FieldMap.insert(std::make_pair(fieldEntry->id, field)).second)
	{
		ErrorLogStream << TEAMINFO(this) << "repeated add field id:" << fieldEntry->id << LogStream::eos;
		return false;
	}

	for (auto& squadIt : m_SquadVec)
	{
		if (squadIt.squadTarget.targetId == 0)
			continue;

		if (!squadIt.squadTarget.isFinish())
			continue;

		const TeamCopyTargetCfg* nextTargetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetTarget(squadIt.squadTarget.config->nextTarget);
		if (NULL == nextTargetCfg)
		{
			continue;
		}

		for (auto& tt : nextTargetCfg->feildVec)
		{
			if (tt.fieldId != fieldEntry->id)
			{
				continue;
			}

			SetSquadTarget(squadIt.squadId, nextTargetCfg);
		}
	}

	return true;
}

void TCSTeamOne::FieldRaceEnd(UInt32 fieldId)
{
	TeamCopyField* field = FindField(fieldId);
	if (NULL == field)
	{
		return;
	}

	for (auto& tt : field->fieldRaceList)
	{
		DebugLogStream << TEAMINFO(this) << "race end fieldId:" << fieldId << "raceId:" << tt << LogStream::eos;

		TCSDungeonMgr::Instance()->DungeonRaceEnd(tt, CDER_TEAMCOPY_FILED_DESTORY);
		_AddForceEndRace(tt);
	}

	field->fieldRaceList.clear();
}

void TCSTeamOne::BossPhaseNotify(TCSPlayer* player)
{
	if (m_BossPhase.phase != 0 && m_BossPhase.bossBloodRate != 0)
	{
		CLProtocol::TeamCopyFieldUnlockRate notify;
		notify.bossPhase = m_BossPhase.phase;
		notify.bossBloodRate = m_BossPhase.bossBloodRate;
		notify.fieldId = m_BossPhase.fieldId;
		notify.unlockRate = 100 - m_BossPhase.bossBloodRate;
		player->SendProtocol(notify);
	}
}

void TCSTeamOne::FieldNotify(const std::vector<UInt32>& filedVec)
{
	CLProtocol::TeamCopyFieldNotify notify;
	for (auto id : filedVec)
	{
		if (!TeamCopyTargetDataEntryMgr::Instance()->IsHasFieldByStage(GetTeamGrade(), GetCurStage(), id))
		{
			continue;
		}

		TeamCopyField* copyField = FindField(id);
		if (NULL == copyField)
		{
			ErrorLogStream << TEAMINFO(this) << "data error field id:" << id << LogStream::eos;
			continue;
		}

		notify.fieldList.push_back(*copyField);
	}

	if (!notify.fieldList.empty())
	{
		BroadcastToMember(notify);
	}
}

void TCSTeamOne::FieldNotify(UInt32 fieldId)
{
	if (fieldId == 0) return;

	if (!TeamCopyTargetDataEntryMgr::Instance()->IsHasFieldByStage(GetTeamGrade(), GetCurStage(), fieldId))
	{
		return;
	}

	TeamCopyField* copyField = FindField(fieldId);
	if (NULL == copyField)
	{
		return;
	}

	CLProtocol::TeamCopyFieldNotify notify;
	notify.fieldList.push_back(*copyField);
	BroadcastToMember(notify);
}

void TCSTeamOne::PhaseBossInfo(UInt64 roleId, UInt32 phase, UInt32 curFrame, UInt32 bossBloodRate)
{
	DebugLogStream << TEAMINFO(this) << "roleId:" << roleId << " start boss phase:" << phase
		<< " curFrame:" << curFrame << " bossBloodRate:" << bossBloodRate << LogStream::eos;

	if (bossBloodRate > 100)
	{
		ErrorLogStream << TEAMINFO(this) << "bossPhase:" << phase << "error bossBloodRate:" << bossBloodRate << LogStream::eos;
		return;
	}

	const TeamCopyBossDataEntry* bossPhaseEntry = TeamCopyBossDataEntryMgr::Instance()->GetBossData(GetTeamType(), GetTeamGrade(), phase);
	if (NULL == bossPhaseEntry)
	{
		ErrorLogStream << TEAMINFO(this) << "bossPhase:" << phase << "not find config!" << LogStream::eos;
		return;
	}

	// 阶段切换
	if (m_BossPhase.phase < phase)
	{
		// 处理上一个阶段的据点
		const TeamCopyBossDataEntry* lastPhaseEntry = TeamCopyBossDataEntryMgr::Instance()->GetBossData(GetTeamType(), GetTeamGrade(), m_BossPhase.phase);
		if (lastPhaseEntry != NULL)
		{
			TeamCopyField* lastField = FindField(lastPhaseEntry->appearField);
			if (lastField != NULL)
			{
				lastField->state = TEAM_COPY_FIELD_STATUS_INIT;
				lastField->oddNum = lastField->config->defeatCond;
				FieldNotify(lastField->fieldId);

				InfoLogStream << TEAMINFO(this) << "roleId:" << roleId << " start boss phase:" << phase << " curFrame:"
					<< curFrame << " bossBloodRate:" << bossBloodRate << " field open:" << lastField->fieldId << LogStream::eos;
			}
		}

		if (bossPhaseEntry->closeField != 0)
		{
			TeamCopyField* closeField = FindField(bossPhaseEntry->closeField);
			if (closeField != NULL)
			{
				FieldRaceEnd(closeField->fieldId);
				closeField->state = TEAM_COPY_FIELD_STATUS_DEFEAT;
				FieldNotify(closeField->fieldId);

				CLProtocol::TeamCopyFieldUnlockRate protocol;
				protocol.bossPhase = phase;
				protocol.bossBloodRate = bossBloodRate;
				protocol.fieldId = closeField->fieldId;
				protocol.unlockRate = 0;
				BroadcastToMember(protocol);

				InfoLogStream << TEAMINFO(this) << "roleId:" << roleId << " start boss phase:" << phase << " curFrame:"
					<< curFrame << " bossBloodRate:" << bossBloodRate << " field close:" << closeField->fieldId << LogStream::eos;
			}
		}

		// 更新阶段
		m_BossPhase.phase = phase;
		m_BossPhase.roleId = roleId;
		m_BossPhase.fieldId = bossPhaseEntry->appearField;
		m_BossPhase.unlockFrame = curFrame;
		m_BossPhase.bossBloodRate = bossBloodRate;
	}
	else if (m_BossPhase.phase == phase)
	{
		if (m_BossPhase.unlockFrame >= curFrame || m_BossPhase.bossBloodRate == bossBloodRate)
		{
			return;
		}

		m_BossPhase.roleId = roleId;
		m_BossPhase.unlockFrame = curFrame;
		m_BossPhase.bossBloodRate = bossBloodRate;
	}

	CLProtocol::TeamCopyFieldUnlockRate protocol;
	protocol.bossPhase = phase;
	protocol.bossBloodRate = bossBloodRate;
	protocol.fieldId = bossPhaseEntry->appearField;
	protocol.unlockRate = 100 - bossBloodRate;
	BroadcastToMember(protocol);
}

void TCSTeamOne::PhaseBossEnd()
{
	m_BossPhase.Clear();

	// 通知回到第一阶段
	CLProtocol::TeamCopyFieldUnlockRate protocol;
	protocol.bossPhase = 1;
	protocol.bossBloodRate = 100;
	protocol.fieldId = 0;
	protocol.unlockRate = 0;
	BroadcastToMember(protocol);
}

void TCSTeamOne::CheckCondFinish()
{
	TCSTeam::CheckCondFinish();

	SetCurStage(TEAM_COPY_STAGE_ONE);

	if (TEAM_COPY_PLAN_MODEL_GUIDE == GetPlanModel())
	{
		// 设置小队目标
		for (auto& iter : m_SquadGrade)
		{
			const TeamCopyTargetCfg* targetCfg = TeamCopyTargetDataEntryMgr::Instance()->GetFirstTarget(GetTeamType(), GetTeamGrade(), GetCurStage(), iter.second);
			if (NULL == targetCfg)
			{
				ErrorLogStream << TEAMINFO(this) << "not find grade stage config:" << iter.second << " stage:" << GetCurStage() << LogStream::eos;
				break;
			}

			SetSquadTarget(iter.first, targetCfg);
		}
	}

	// 团队目标
	if (!SetTeamTarget(GetCurStage()))
	{
		ErrorLogStream << TEAMINFO(this) << "set team target failed!" << LogStream::eos;
	}

	// 据点信息
	std::vector<TeamCopyFieldDataEntry*> fieldList;
	TeamCopyFieldDataEntryMgr::Instance()->GetFieldList(GetTeamGrade(), TEAM_COPY_APPEAR_START, fieldList);
	if (fieldList.empty())
	{
		ErrorLogStream << TEAMINFO(this) << "start battle field list null!" << LogStream::eos;
	}

	for (auto& fieldIter : fieldList)
	{
		AddField(fieldIter);
	}

	// 通知第一阶段数据
	AttackDataNotify(GetCurStage());

	DebugLogStream << TEAMINFO(this) << "start stage:" << GetCurStage() << LogStream::eos;
}

void TCSTeamOne::DelBossAttachField()
{
	std::vector<TeamCopyFieldDataEntry*> fieldList;
	TeamCopyFieldDataEntryMgr::Instance()->GetFieldList(GetTeamGrade(), TEAM_COPY_APPEAR_BOSS_ATTACH, fieldList);
	if (fieldList.empty())
		return;

	for (auto& iter : fieldList)
	{
		if (NULL == iter)
			continue;

		auto it = m_FieldMap.find(iter->id);
		if (it == m_FieldMap.end())
			continue;

		// 通知该剧点结束比赛
		for (auto& tt : it->second.fieldRaceList)
		{
			DebugLogStream << TEAMINFO(this) << "del boss attach fieldId:" << iter->id << "raceId:" << tt << LogStream::eos;
			TCSDungeonMgr::Instance()->DungeonRaceEnd(tt, CDER_TEAMCOPY_FILED_DESTORY);
			_AddForceEndRace(tt);
		}

		it->second.state = TEAM_COPY_FIELD_STATUS_UNLOCKING;
		FieldNotify(iter->id);

		CLProtocol::TeamCopyFieldUnlockRate mes;
		mes.bossPhase = 0;
		mes.bossBloodRate = 0;
		mes.fieldId = iter->id;
		mes.unlockRate = 0;
		BroadcastToMember(mes);
	}
}

void TCSTeamOne::NotifyAllSquadLeaveDungeon()
{
	for (auto& iter : m_FieldMap)
	{
		for (auto& tt : iter.second.fieldRaceList)
		{
			DebugLogStream << TEAMINFO(this) << "all squad leave fieldId:" << iter.first << "raceId:" << tt << LogStream::eos;

			TCSDungeonMgr::Instance()->DungeonRaceEnd(tt, CDER_TEAMCOPY_SETTLE);
			_AddForceEndRace(tt);
		}

		iter.second.fieldRaceList.clear();
	}
}

void TCSTeamOne::NotifyAllSquadDungeonDestory(UInt32 dungeonId)
{
	for (auto& iter : m_FieldMap)
	{
		for (auto& it : iter.second.fieldRaceList)
		{
			InfoLogStream << TEAMINFO(this) << "boss attach fieldId:" << iter.first << " dungeonId:" << dungeonId << " raceId:" << it << LogStream::eos;
			TCSDungeonMgr::Instance()->DungeonRaceDestroy(it, dungeonId);
		}
	}
}

TeamCopyField* TCSTeamOne::FindField(UInt32 fieldId)
{
	auto iter = m_FieldMap.find(fieldId);
	return iter != m_FieldMap.end() ? &(iter->second) : NULL;
}

void TCSTeamOne::NotifyRaceEnergyRevive(UInt64 raceId)
{
	TeamCopyField* energyField = FindField(GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD));
	if (NULL == energyField)
	{
		return;
	}

	CLProtocol::TeamCopyNotifyBimsEnergyAccuProgress notify;
	notify.raceSessionId = raceId;
	notify.dungeonId = energyField->config->dungeonId;
	notify.progress = (UInt32)CURRENT_TIME.Sec() - energyField->energyReviveTime;

	if (notify.progress > 1000)
	{
		notify.progress = 1000;
	}

	TCSDungeonMgr::Instance()->SendToDungeonSvr(notify);
}

bool TCSTeamOne::TeamIsFull()
{
	return m_PlayerSquad.size() >= GetTeamCopyValue(TC_VALUE_TEAM_CAPACITY);
}

void TCSTeamOne::StartRaceRes(CLProtocol::DungenServerTeamStartRaceRes& res)
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

		return;
	}

	UInt32 fieldId = TCSTeamMgr::Instance()->GetAttackFieldId(res.teamId);
	if (fieldId != 0)
	{
		// 有小队进入据点
		FieldAddAttackSquad(squadId, fieldId, res.gamesessionID);
	}

	InfoLogStream << TEAMINFO(this) << "start race success teamSquadId:" << res.teamId << " fieldId:" << fieldId << " enter dungeonId:"
		<< res.dungeonId << " relayId:" << res.relayServerID << " gamesessionId:" << res.gamesessionID << " address:"
		<< res.address.ToString() << LogStream::eos;

	// 通知小队状态
	SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_BATTLE);

	// 比赛正式开始，创建比赛
	TCSDungeonMgr::Instance()->AddRace(res.gamesessionID, res.teamId, fieldId);

	// 噩梦难度，通知据点能量恢复进度
	if (GetTeamGrade() == TEAM_COPY_TEAM_GRADE_DIFF)
	{
		TeamCopyFieldDataEntry* fieldCfg = TeamCopyFieldDataEntryMgr::Instance()->FindEntry(fieldId);
		if (fieldCfg != NULL &&
			(fieldCfg->presentedType == TEAM_COPY_APPEAR_BOSS || fieldCfg->presentedType == TEAM_COPY_APPEAR_BOSS_ATTACH))
		{
			NotifyRaceEnergyRevive(res.gamesessionID);
		}
	}
}

void TCSTeamOne::RaceEnd(CLProtocol::DungeonSvrDungeonRaceEnd& end, UInt32 fieldId, UInt32 dungeonId, UInt64 createTime)
{
	UInt32 squadId = GET_SQUAD_ID(end.teamId);
	UInt64 raceId = end.endInfo.gamesessionId;

	// 有小队退出据点
	FieldDelAttackSquad(squadId, fieldId, raceId);

	// 更新小队状态
	SetSquadStatus(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);
	SquadNotify(squadId, TEAM_COPY_SQUAD_STATUS_PREPARE);

	// 如果boss据点结算，不论成功失败，都需要删除衍生剧点,并通知衍生据点退出
	if (TeamCopyFieldDataEntryMgr::Instance()->GetBossFieldId(GetTeamGrade()) == fieldId)
	{
		DelBossAttachField();
		PhaseBossEnd();
	}

	// udp据点结算
	UdpFieldSettle(squadId, fieldId, dungeonId, createTime, end.result);

	if (end.result != CDERT_SUCC)
	{
		InfoLogStream << TEAMINFO(this) << " race end failed teamSquadId:" << end.teamId << " enter dungeonId:" << dungeonId
			<< " fieldId:" << fieldId << " result:" << end.result << " raceId:" << raceId << LogStream::eos;
		return;
	}

	InfoLogStream << TEAMINFO(this) << "race end success teamSquadId:" << end.teamId << " dungeonId:" << dungeonId <<
		" fieldId:" << fieldId << " raceId:" << raceId << LogStream::eos;

	// 处理玩家通关
	_RaceSettlement(squadId, raceId);

	// 处理目标
	ClearField(squadId, fieldId);
}

void TCSTeamOne::GMClearAllField(UInt32 squadId)
{
	for (auto& it : m_FieldMap)
	{
		while (!it.second.isFinish())
		{
			if (IsGameOver())
				return;

			// 能量据点
			if (it.first == GetTeamCopyValue(TC_VALUE_DIFF_ENERGY_REVIVE_FIELD) && it.second.oddNum == 1)
			{
				break;
			}

			for (auto& iter : m_SquadVec)
			{
				_RaceSettlement(iter.squadId, 500);
			}

			ClearField(squadId, it.first);

			InfoLogStream << TEAMINFO(this) << "squadId:" << squadId << " gm clear fieldId:" << it.first << LogStream::eos;
		}
	}
}

void TCSTeamOne::GameOver(UInt32 overReason)
{
	TCSTeam::GameOver(overReason);

	UInt32 costTime = (UInt32)CURRENT_TIME.Sec() - _GetGameStartTime();

	// 比赛结束，删除消耗
	RemoveAllMemberConsume();

	switch (overReason)
	{
	case TC_GAME_OVER_REASON_VICTORY :
	{
		InfoLogStream << TEAMINFO(this) << "final stage finish, game over success!" << LogStream::eos;

		// 最终翻牌
		_FlopReward(TEAM_COPY_STAGE_FINAL);

		SetStatus(TEAM_COPY_TEAM_STATUS_VICTORY);
		_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_VICTORY);

		// 扣除佣金
		_GameOverCommission(false);
		// 佣金分成
		_CommissionBonus(10048, 10051, 10052);
		// 增加模式通关次数
		_AddModelPassNum(GetModel(), costTime);

		// roll奖励
		_RollReward();
	}
	break;
	case TC_GAME_OVER_REASON_TIME_OVER :
	{
		// 阶段完成udp，1失败
		UdpStageEnd(costTime, 1);

		SetStatus(TEAM_COPY_TEAM_STATUS_FAILED);
		_TeamStatusNotify(TEAM_COPY_TEAM_STATUS_FAILED);

		// 通知所有战斗结束
		NotifyAllSquadLeaveDungeon();
		// 退还佣金
		_GameOverCommission(true);
		// 失败，恢复挑战次数
		FailedReturnChallengeNum();
	}
	break;
	default:return;
	}

	SetGameOverTime(0);
}

void TCSTeamOne::UdpStageEnd(UInt32 costTime, UInt32 result)
{
	for (auto& iter : m_PlayerSquad)
	{
		TCSPlayer* player = TCSPlayerMgr::Instance()->FindTCSPlayer(iter.first);
		if (NULL == player)
			continue;

		player->SendUdpLog("team_copy_stage_end", LOG_TEAM_COPY_STAGE_END, GetTeamId(), GetCurStage(), costTime, result,
			GetTeamGrade(), GetTeamType(), 0, 0, 0, 0);
	}
}

void TCSTeamOne::_FlopReward(UInt32 stage)
{
	const TeamCopyFlopDataEntry* flopCfg = TeamCopyFlopDataEntryMgr::Instance()->GetFlopReward(GetTeamType(), GetTeamGrade(), stage);
	if (NULL == flopCfg)
	{
		ErrorLogStream << TEAMINFO(this) << "not find flop config stage:" << stage << LogStream::eos;
		return;
	}

	std::string sender = SysNotifyMgr::Instance()->MakeString(10048);
	std::string title = SysNotifyMgr::Instance()->MakeString(10049);
	std::string content = SysNotifyMgr::Instance()->MakeString(10050, stage);
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
				UInt32 settleStage = (stage == TEAM_COPY_STAGE_FINAL ? TEAM_COPY_STAGE_FINAL - 1 : stage);
				if (!_StageIsSettle(settleStage, it.playerId))	// 该阶段是否成功的通关一次,最终阶段依赖第二阶段
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
					flop.rewardNum, flop.isLimit, GetTeamGrade(), GetTeamType(), 0);
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