#include "HonorMgr.h"

#include <math.h>
#include <CLRecordClient.h>
#include <CLGameDefine.h>
#include <CLHonorLevelDataEntry.h>
#include <CLHonorRewardDataEntry.h>
#include <CLHonorRankExpDataEntry.h>
#include <CLNewTitleProtocol.h>
#include <CLHonorProtocol.h>

#include "GameParamMgr.h"
#include "SSRouter.h"
#include "PlayerMgr.h"

#define HONOR_RANK_RATE		(10000)
#define HONOR_EXP_RATE		(100)

#define MAX_VAL(a, b)		(((a) > (b)) ? (a) : (b))

/**
*@brief 加载角色荣誉
*/
class SelectRoleHonorCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		while (NextRow())
		{
			RoleHonor* roleHonor = new RoleHonor();
			roleHonor->SetID(GetKey());
			roleHonor->SetDBData(this);
			roleHonor->ResetHonorLvl();
			HonorMgr::Instance()->AddRoleHonor(GetKey(), roleHonor);
		}
	}

	void OnFailed(UInt32 errorcode) 
	{
		ErrorLogStream << "load table t_honor_role error!" << LogStream::eos;
	}
};

RoleHonor::RoleHonor()
{
	m_RoleId = 0;
	m_Honor = 0;
	m_HonorTime = 0;
	m_HonorLvl = 0;
	m_HonorExp = 0;
	m_LastWeekRank = 0;
	m_HistoryRank = 0;
	m_HighestHonorLvl = 0;
}

RoleHonor::~RoleHonor()
{

}

void RoleHonor::ResetHonorLvl()
{
	m_HonorLvl = HonorLevelDataEntryMgr::Instance()->GetHonorLvl(m_HonorExp);
	if (m_HonorLvl == 1 && m_HighestHonorLvl == 0)
	{
		m_HonorLvl = 0;
	}

	UpdateToDB(false);
}

std::string RoleHonor::GetHonorStr()
{
	std::stringstream ss;
	ss << " RoleId:" << m_RoleId;
	ss << ", Honor:" << m_Honor;
	ss << ", HonorTime:" << m_HonorTime;
	ss << ", HonorLvl:" << m_HonorLvl;
	ss << ", HonorExp:" << m_HonorExp;
	ss << ", LastWeekRank:" << m_LastWeekRank;
	ss << ", HistoryRank:" << m_HistoryRank;
	ss << ", HighestHonorLvl:" << m_HighestHonorLvl;

	return ss.str();
}

void RoleHonor::UpdateToDB(bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_honor_role", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void RoleHonor::InsertToDB()
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_honor_role", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

HonorMgr::HonorMgr()
{
	m_RankTimeSync = false;
}

HonorMgr::~HonorMgr()
{
	for (auto& iter : m_HonorMap)
	{
		CL_SAFE_DELETE(iter.second);
	}

	m_HonorMap.clear();
}

void HonorMgr::Init()
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_honor_role");
	CLRecordClient::Instance()->SendCommand(cmd, new SelectRoleHonorCallback());
}

void HonorMgr::OnTick(const Avalon::Time& now)
{
	if ( !IsSyncRankTime())
	{
		return;
	}

	if (now.Sec() >= NextRankTime())
	{
		Avalon::Date date(now);
		date.Hour(6);
		date.Min(0);
		date.Sec(0);

		UInt32 honorRankTime = UInt32(date.ToTime().Sec()) - (date.WDay() - 1) * DAY_TO_SEC;
		GameParamMgr::Instance()->SetValue(HONOR_RANK_TIME, honorRankTime);

		ChangeWeekRank();
	}
}

void HonorMgr::PvpEnd(UInt64 roldId, UInt32 getHonor)
{
	InfoLogStream << "playerId:" << roldId << " add honor:" << getHonor << LogStream::eos;

	RoleHonor* roleHonor = FindRoleHonor(roldId);
	if (roleHonor != NULL)
	{
		roleHonor->AddHonor(getHonor);
		roleHonor->SetHonorTime((UInt32)CURRENT_TIME.Sec());
		roleHonor->UpdateToDB(true);
	}
	else
	{
		RoleHonor* honor = new RoleHonor();
		honor->SetID(roldId);
		honor->SetHonor(getHonor);
		honor->SetHonorTime((UInt32)CURRENT_TIME.Sec());
		AddRoleHonor(roldId, honor);
		honor->InsertToDB();

		Player* player = PlayerMgr::Instance()->FindPlayer(roldId);
		if (player != NULL)
		{
			player->SetRoleHonor();
		}
	}
}

void HonorMgr::AddRoleHonor(UInt64 roldId, RoleHonor* honor)
{
	m_HonorMap[roldId] = honor;
}

RoleHonor* HonorMgr::FindRoleHonor(UInt64 roldId, bool isCreate)
{
	auto iter = m_HonorMap.find(roldId);
	if (iter != m_HonorMap.end())
	{
		return iter->second;
	}

	if (isCreate)
	{
		RoleHonor* roleHonor = new RoleHonor();
		roleHonor->SetID(roldId);
		AddRoleHonor(roldId, roleHonor);
		roleHonor->InsertToDB();
		return roleHonor;
	}

	return NULL;
}

struct HonorGreaterSort
{
	bool operator()(const RoleHonor* a, const RoleHonor* b) const
	{
		if (a->GetHonor() != b->GetHonor())
		{
			return a->GetHonor() > b->GetHonor();
		}

		return a->GetHonorTime() < b->GetHonorTime();
	}
};

void HonorMgr::ChangeWeekRank()
{
	if (m_HonorMap.empty())
	{
		InfoLogStream << "honor map empty!" << LogStream::eos;
		return;
	}

	UInt32 getHonorRoleNum = 0;
	std::vector<RoleHonor*> rankVec;
	for (auto& iter : m_HonorMap)
	{
		if (iter.second->GetHonor() != 0)
		{
			++getHonorRoleNum;
		}
		rankVec.push_back(iter.second);
	}

	std::sort(rankVec.begin(), rankVec.end(), HonorGreaterSort());

	InfoLogStream << "start honor rank size:" << UInt32(m_HonorMap.size()) << " getHonorRoleNum:" << getHonorRoleNum << LogStream::eos;

	UInt32 rate_1 = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HONOR_EXP_RATE_1);
	UInt32 rate_2 = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_HONOR_EXP_RATE_2);

	UInt32 rank = 0;
	for (auto& it : rankVec)
	{
		InfoLogStream << "Honor Rank Start Log" << it->GetHonorStr() << LogStream::eos;

		// 经验
		UInt32 newExp = 0;
		if (it->GetHonor() == 0)
		{
			it->SetHistoryRank(it->GetLastWeekRank());
			it->SetLastWeekRank(0);

			if (it->IsUseGuardCard())
				newExp = it->GetHonorExp();
			else
				newExp = it->GetHonorExp() * rate_2 / HONOR_EXP_RATE;

			InfoLogStream << "Honor Exp:" << newExp << " Rank:0" << " Guard:" << it->IsUseGuardCard() << LogStream::eos;
		}
		else
		{
			it->SetHistoryRank(it->GetLastWeekRank());

			++rank;
			it->SetLastWeekRank(rank);

			UInt32 rewardExp = 0;
			const HonorRankExpDataEntry* rewardData = HonorRankExpDataEntryMgr::Instance()->
				GetRankReward(rank * HONOR_RANK_RATE / getHonorRoleNum);
			if (rewardData != NULL)
			{
				const HonorLevelDataEntry* lvlData = HonorLevelDataEntryMgr::Instance()->FindEntry(it->GetHonorLvl());
				if (NULL == lvlData)
				{
					ErrorLogStream << "playerId:" << it->GetID() << "not find honor lvl config:" << it->GetHonorLvl() << LogStream::eos;
					continue;
				}

				UInt32 power = 0;
				if (rewardData->rankLvl > lvlData->honorLvlGrade)
				{
					power = rewardData->rankLvl - lvlData->honorLvlGrade;
				}

				rewardExp = rewardData->rewardExp * pow(0.95, power);
				if (rewardExp > lvlData->weekAddLimit)
				{
					rewardExp = lvlData->weekAddLimit;
				}
			}
			else
			{
				ErrorLogStream << "playerId:" << it->GetID() << "not find honor reward config:" << rank << LogStream::eos;
			}

			UInt32 exp_1 = it->GetHonorExp() * rate_1 / HONOR_EXP_RATE + rewardExp;
			UInt32 exp_2 = it->GetHonorExp() * rate_2 / HONOR_EXP_RATE;
			newExp = MAX_VAL(exp_1, exp_2);

			if (it->IsUseGuardCard())
			{
				newExp = MAX_VAL(newExp, it->GetHonorExp());
			}

			InfoLogStream << "Honor Exp:" << newExp << " Rank:" << rank << " reward exp:" << rewardExp 
				<< " Guard:" << it->IsUseGuardCard() << LogStream::eos;
		}

		// 等级
		UInt32 newLvl = HonorLevelDataEntryMgr::Instance()->GetHonorLvl(newExp);

		do 
		{
			const HonorLevelDataEntry* newLvlData = HonorLevelDataEntryMgr::Instance()->FindEntry(newLvl);
			if (newLvlData != NULL)
			{
				// 头衔奖励
				CLProtocol::SceneNewTitleAddWorld protocol;
				protocol.roleId = it->GetID();
				protocol.titleId = newLvlData->title;
				protocol.dueTime = UInt32(CURRENT_TIME.Sec()) + newLvlData->titleDueTime * MIN_TO_SEC;
				Router::SendToWorld(protocol);

				if (it->GetHonor() != 0 && newLvlData->rankLeastNum != 0 && getHonorRoleNum >= newLvlData->rankLeastNum)
				{
					if (newLvl > it->GetHonorLvl())
					{
						UInt32 rankRate = UInt32(rank * HONOR_RANK_RATE / getHonorRoleNum);
						if (rankRate <= newLvlData->lvlNeedRankRate)
						{
							it->SetHonorLvl(newLvl);
							UpdateRoleHonorInfo(it->GetID(), newLvl);
						}

						// 等级不变
						break;
					}
				}
			}

			if (it->GetHonorLvl() != newLvl)
			{
				UpdateRoleHonorInfo(it->GetID(), newLvl);
				it->SetHonorLvl(newLvl);
			}

		} while (0);

		it->ResetGuildCard();
		it->SetHonorExp(newExp);
		it->SetHonor(0);
		it->SetHighestHonorLvl(MAX_VAL(it->GetHonorLvl(), it->GetHighestHonorLvl()));
		it->UpdateToDB(true);

		InfoLogStream << "Honor Rank End Log" << it->GetHonorStr() << LogStream::eos;
	}

	InfoLogStream << "end honor rank!" << LogStream::eos;
}

void HonorMgr::UpdateHonorLevel(UInt64 roleId, UInt32 level)
{
	const HonorLevelDataEntry* honorLvlData = HonorLevelDataEntryMgr::Instance()->FindEntry(level);
	if (NULL == honorLvlData)
	{
		return;
	}

	RoleHonor* roleHonor = FindRoleHonor(roleId);
	if (NULL == roleHonor)
	{
		roleHonor = new RoleHonor();
	}

	roleHonor->SetHonorLvl(level);
	roleHonor->SetHonorExp(honorLvlData->needExp);
	roleHonor->UpdateToDB(false);

	UpdateRoleHonorInfo(roleId, level);
}

void HonorMgr::UpdateRoleHonorInfo(UInt64 playerId, UInt32 honorLvl)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(playerId);
	if (player != NULL)
	{
		player->SetHonorLevel(honorLvl);
		CLProtocol::SceneHonorRedPoint point;
		player->SendProtocol(point);

		if (honorLvl >= GUILD_EVENT_HONOR_LEVEL)
		{
			player->SendGuildEventToWorld(SysNotifyMgr::Instance()->MakeString(14010, player->GetName(), honorLvl));
		}
	}
}

UInt32 HonorMgr::NextRankTime()
{
	UInt32 honorRankTime = GameParamMgr::Instance()->GetValue(HONOR_RANK_TIME);
	return honorRankTime + 7 * DAY_TO_SEC;
}