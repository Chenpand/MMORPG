#include "SkillMgr.h"
#include <AvalonRandom.h>
#include <CLSkillProtocol.h>
#include <CLGuildProtocol.h>
#include <CLBuffDataEntry.h>
#include <CLSkillRecommendDataEntry.h>
#include <CLJobDataEntry.h>

#include "Creature.h"
#include "Scene.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "CLErrorCode.h"
#include "SSRouter.h"
#include "SSFigureStatueMgr.h"

bool Skill::Init()
{
	dataEntry = SkillDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) 
		return false;

	return true;
}

SkillMgr::SkillMgr()
{
	m_pOwner = NULL;
	m_Timer.SetInterval(1000);
	m_SkillConfigType = SkillConfigType::SKILL_CONFIG_PVE;
}

SkillMgr::~SkillMgr()
{
}

SkillMgr & SkillMgr::operator=(const SkillMgr & rsh)
{
	for (auto &i : m_Skills)
	{
		i.second.level =0;
		i.second.bdirty = true;
	}
	for (auto &i : rsh.m_Skills)
	{
		auto iter = m_Skills.find(i.first);
		if (iter != m_Skills.end())
		{
			iter->second = i.second;
		}
		else
		{
			m_Skills.insert(i);
		}
	}
	for (auto &i : m_Skills)
	{
		i.second.bdirty = true;
	}
	return *this;
}

void SkillMgr::SetOwner(Creature* owner)
{
	m_pOwner = owner;
}

Skill* SkillMgr::FindSkill(UInt16 skillid)
{
	SkillMap::iterator iter = m_Skills.find(skillid);
	if(iter != m_Skills.end() && iter->second.IsValid())
	{
		return &iter->second;
	}
	return NULL;
}

bool SkillMgr::CheckNextSkill(UInt16 id, UInt8 level)
{
	bool res = true;
	SkillMap::iterator iter = m_Skills.begin();
	while (iter != m_Skills.end() && iter->second.IsValid())
	{
		if (!iter->second.dataEntry->needSkills.empty())
		{
			NeedSkillVec::iterator skillIter = iter->second.dataEntry->needSkills.begin();
			while (skillIter != iter->second.dataEntry->needSkills.end())
			{
				if (skillIter->id == id && skillIter->level > level)
				{
					res = false;
					break;
				}
			}
		}
		++iter;
	}
	return res;
}

void SkillMgr::VisitSkills(SkillVisitor& visitor,bool bAll)
{
	for(SkillMap::iterator iter = m_Skills.begin()
		;iter != m_Skills.end();)
	{
		Skill* skill = &iter->second;
		++iter;
		if(bAll || skill->IsValid())
		{
			if(!visitor(skill)) return;
		}
	}
} 

UInt32 SkillMgr::CheckChangeSkills(Player* player, std::vector<ChangeSkill> skills, SkillMap& skillMap, Int32& sp, UInt32& removeGlod, bool isConsume)
{
	if (player == NULL) return ErrorCode::SKILL_ERROR;
	UInt32 skillSize = skills.size();
	UInt32 res;
	JobDataEntry* jobDataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
	if (jobDataEntry == NULL)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills occu "
			<< player->GetOccu() << " is exist " << LogStream::eos;
		return ErrorCode::SKILL_ERROR;
	}

	for (UInt32 i = 0; i < skillSize; i++)
	{
		SkillMap::iterator iter = m_Skills.find(skills[i].id);
		Int32 skillLevel = 0;
		UInt32 initLevel = jobDataEntry->GetInitLevel(skills[i].id);
		if (iter != m_Skills.end())
		{
			skillLevel = iter->second.level + skills[i].dif;
		}
		else
		{
			skillLevel = skills[i].dif;
		}
		Skill addSkill;
		addSkill.id = skills[i].id;
		addSkill.level = skillLevel;
		if (!addSkill.Init())
		{
			ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills "
				<< addSkill.id << " Init Error " << LogStream::eos;
			return ErrorCode::SKILL_ERROR;
		}
		if (!skillMap.insert(std::make_pair(skills[i].id, addSkill)).second)
		{
			ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills "
				<< addSkill.id << " insert Error " << LogStream::eos;
			return ErrorCode::SKILL_ERROR;
		}

		//如果是预转职技能,设置初始等级
		if (addSkill.dataEntry->isPreJob && initLevel < 1)
		{
			initLevel = 1;
		}

		res = CheckSkill(player, addSkill.dataEntry, skillLevel, initLevel, skills[i].dif);
		if (res != ErrorCode::SUCCESS) { return res; }

		//大于初始等级的加点和减点都是需要计算SP的,不大于初始等级不需要消耗SP
		if (skillLevel > (Int32)initLevel || skills[i].dif < 0)
		{
			sp = sp - addSkill.dataEntry->needSP * skills[i].dif;
		}

		UInt32 downSkillGlod = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SKILL_DOWNLEVEL_COSTGOLD);
		if (skills[i].dif < 0) { removeGlod += (skills[i].dif * -1) * addSkill.dataEntry->needSP * downSkillGlod; }
		InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills "
			<< skills[i].id << " to level " << int(skillLevel) << " over." << LogStream::eos;
	}

	if (sp < 0)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills "
			<< " sp "<< GetSP() << "|" << sp << " is not enough " << LogStream::eos;
		return ErrorCode::SKILL_SP_NOT_ENOUGH;
	}

	if (isConsume && player->GetGoldBoth() < removeGlod)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills "
			<< " glod " << player->GetGoldBoth() << "|" << removeGlod << " is not glod " << LogStream::eos;
		return ErrorCode::SKILL_NEED_ITEM_ERROR;
	}
	return CheckSkillRelation(skillMap);
}

UInt32 SkillMgr::CheckSkill(Player* player, SkillDataEntry* dataEntry, Int32 skillLevel, UInt32 initLevel, Int8 difLevel)
{
	if (player == NULL) return ErrorCode::SKILL_ERROR;
	if (dataEntry == NULL) return ErrorCode::SKILL_ERROR;
	UInt16 skillId = dataEntry->id;
	//检查技能最小.
	if (skillLevel < (Int32)initLevel)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skill "
			<< skillId << " to level " << skillLevel << " error, min skill level " << initLevel << LogStream::eos;
		return ErrorCode::SKILL_MIN_SKILL_LEVEL;
	}

	//检查最大等级
	if (skillLevel > dataEntry->maxLevel)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skill "
			<< skillId << " to level " << skillLevel << " error, Max skill level " << dataEntry->maxLevel << LogStream::eos;
		return ErrorCode::SKILL_MAX_SKILL_LEVEL;
	}

	//增加技能等级时需要检查职业
	if (difLevel > 0)
	{
		//检查职业是否合法,如果是通用技能,不需要检查职业
		if (!IsGeneral(dataEntry->type) && !dataEntry->CheckOccu(player->GetOccu()))
		{
			ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skill "
				<< skillId << " to level " << skillLevel << " error, player occu " << player->GetOccu() << LogStream::eos;
			return ErrorCode::SKILL_OCCU_ERROR;
		}

		//检查觉醒技能相关
		if (IsAwaken(dataEntry->type) && ((AwakenStatus)player->GetAwaken()) <= AwakenStatus::AWAKEN_NOTHING)
		{
			ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skill "
				<< skillId << " to level " << skillLevel << " error, player awaken " << ((UInt32)player->GetAwaken()) << LogStream::eos;
			return ErrorCode::SKILL_AWAKEN_ERROR;
		}

		if (m_SkillConfigType != SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
		{
			//检查玩家等级.
			UInt16 needPlayerLevel = dataEntry->playerLevel + dataEntry->upLevelInterval * (skillLevel - 1);
			if (player->GetLevel() < needPlayerLevel && skillLevel > (Int32)initLevel)
			{
				ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skill "
					<< skillId << " to level " << skillLevel << " error, player level " << player->GetLevel() << ", ask for level "
					<< needPlayerLevel << LogStream::eos;
				return ErrorCode::SKILL_PLAYER_LEVEL;
			}
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 SkillMgr::CheckSkillRelation(SkillMap skillMap)
{
	UInt32 res;
	//检查后置技能
	SkillMap::iterator skillIter = skillMap.begin();
	while (skillIter != skillMap.end())
	{
		res = CheckPostSkill(skillMap, skillIter->second);
		if (res != ErrorCode::SUCCESS) { return res; }
		res = CheckPreSkill(skillMap, skillIter->second);
		if (res != ErrorCode::SUCCESS) { return res; }
		++skillIter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 SkillMgr::CheckPreSkill(SkillMap skillMap, Skill skill)
{
	NeedSkillVec preSkills = skill.dataEntry->needSkills;
	NeedSkillVec::iterator preSkillIter = preSkills.begin();
	while (preSkillIter != preSkills.end())
	{
		SkillMap::iterator preSkill = skillMap.find(preSkillIter->id);
		if (preSkill == skillMap.end())
		{
			preSkill = m_Skills.find(preSkillIter->id);
			if (preSkill == m_Skills.end())
			{
				return ErrorCode::SKILL_NEED_SKILL_ERROR;
			}
		}

		if (skill.IsValid() && preSkill->second.level < preSkillIter->level)
		{
			return ErrorCode::SKILL_NEED_SKILL_ERROR;
		}
		++preSkillIter;
	}
	return ErrorCode::SUCCESS;
}

UInt32 SkillMgr::CheckPostSkill(SkillMap skillMap, Skill skill)
{
	//获取后置技能
	std::vector<UInt16> postSkills = SkillDataEntryMgr::Instance()->GetPostSkills(skill.id);
	//遍历后置技能
	for (UInt32 i = 0; i < postSkills.size(); i++)
	{
		//检查每个技能的前置技能是否符合.
		SkillMap::iterator postSkill = skillMap.find(postSkills[i]);
		//检查后置技能是否存在或有效.
		if (postSkill == skillMap.end())
		{
			postSkill = m_Skills.find(postSkills[i]);
			if (postSkill == m_Skills.end() || !postSkill->second.IsValid())
			{
				continue;
			}
		}
		else if (!postSkill->second.IsValid())
		{
			continue;
		}

		NeedSkillVec preSkills = postSkill->second.dataEntry->needSkills;
		NeedSkillVec::iterator preSkillIter = preSkills.begin();
		while (preSkillIter != preSkills.end())
		{
			//找到与需要改变的技能ID相同的技能
			if (preSkillIter->id == skill.id)
			{
				if (!skill.IsValid() || preSkillIter->level > skill.level)
				{
					return ErrorCode::SKILL_NEXT_SKILL_ERROR;
				}
			}
			++preSkillIter;
		}
	}
	return ErrorCode::SUCCESS;
}

void SkillMgr::OnOnline()
{
	if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
	{
		if (m_Page == 1)
		{
			return;
		}
		Player* player = (Player*)GetOwner();
		auto *dataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
		if (dataEntry)
		{
			if (!player->GetCounter(EQUAL_PVP_SKILL_HAVE_SP))
			{
				player->SetCounter(EQUAL_PVP_SKILL_HAVE_SP, 1);
				player->SetEqualPvpSP(dataEntry->EqualPvPSP);
				OnBirth();
			}
			else
			{
				UInt32 equalSp = 0;
				if (dataEntry->EqualPvPSP > GetSkillSP())
				{
					equalSp = dataEntry->EqualPvPSP - GetSkillSP();
				}
				
				player->SetEqualPvpSP(equalSp);
			}
		}
		else
		{
			ErrorLogStream << PLAYERINFO(player) << "SkillMgr::OnOnline() can't FindEntry " << player->GetOccu() << ")" << LogStream::eos;
		}
	}

	//检查不满足并已经学习的技能
	std::vector<ChangeSkill> skills;
	SkillMap::iterator iter = m_Skills.begin();
	while (iter != m_Skills.end())
	{
		if (iter->second.dataEntry != NULL)
		{
			if (iter->second.dataEntry->type == SKILL_TYPE_GUILD)
			{
				iter++;
				continue;
			}

			if (!iter->second.dataEntry->CheckOccu(GetOwner()->GetOccu()))
			{
				ChangeSkill entry;
				entry.id = iter->second.id;
				entry.dif = Int8(iter->second.level) * -1;
				skills.push_back(entry);
			}
		}
		iter++;
	}

	if (ChangeSkills(skills) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") OnOnline Check not find skill" << LogStream::eos;
	}

	OnCheckSkillSP();

	CLProtocol::WorldGuildSyncSkill sync;
	sync.id = GetOwner()->GetID();

	for (auto itr : m_Skills)
	{
		auto& skill = itr.second;
		if (skill.dataEntry->type == SKILL_TYPE_GUILD)
		{
			SkillBase info;
			info.id = skill.id;
			info.level = skill.level;
			sync.skills.push_back(info);
		}
	}

	Router::SendToWorld(sync);

	std::vector<FigureStatueType> figureStatueList;
	
	SSFigureStatueMgr::Instance()->GetFigureStatuesById(figureStatueList, GetOwner()->GetID());
	bool isAdd = false;
	for (UInt32 i = 0; i < figureStatueList.size(); ++i)
	{
		if (figureStatueList[i] == FigureStatueType::FST_GUILD)
		{
			AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
			isAdd = true;
			break;
		}
	}
	if (!isAdd)
	{
		DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_STATUE_SKILL_ID);
	}

	isAdd = false;
	for (UInt32 i = 0; i < figureStatueList.size(); ++i)
	{
		if (figureStatueList[i] == FigureStatueType::FST_GUILD_ASSISTANT || 
			figureStatueList[i] == FigureStatueType::FST_GUILD_ASSISTANT_TWO)
		{
			AddStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
			isAdd = true;
			break;
		}
	}
	if (!isAdd)
	{
		DelStatueSkill(SystemValueType::SVT_GUILD_BATTLE_ASSISTANT_STATUE_SKILL_ID);
	}
}

void SkillMgr::OnChangeOccu()
{
	//检查不满足并已经学习的技能
	Player* owner = (Player*)GetOwner();
	JobDataEntry* baseJobDataEntry = JobDataEntryMgr::Instance()->FindEntry(owner->GetBaseOccu());
	if (baseJobDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") OnChangeOccu base Job ERROR" << LogStream::eos;
		return;
	}

	UInt32 removeSp = 0;

	std::vector<ChangeSkill> skills;
	SkillMap::iterator iter = m_Skills.begin();
	while (iter != m_Skills.end())
	{
		if (iter->second.dataEntry->type == SKILL_TYPE_GUILD)
		{
			iter++;
			continue;
		}

		if (iter->second.dataEntry != NULL)
		{
			if (!iter->second.dataEntry->CheckOccu(GetOwner()->GetOccu()) && iter->second.level > 0)
			{
				UInt32 initLevel = baseJobDataEntry->GetInitLevel(iter->second.id);
				if (initLevel > 0)
				{
					removeSp = removeSp + iter->second.dataEntry->needSP * initLevel;
				}

				ChangeSkill entry;
				entry.id = iter->second.id;
				entry.dif = Int8(iter->second.level) * -1;
				skills.push_back(entry);
			}
		}
		iter++;
	}

	if (ChangeSkills(skills) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") OnChangeOccu Check not find skill" << LogStream::eos;
	}

	//补扣SP点
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_SKILL_UP_LEVEL);
	RemoveSP(reason.c_str(), removeSp);

	//ForgetAllSkills();
	OnBirth();
}

void SkillMgr::OnBirth()
{
	if (GetOwner()->GetType() != SOT_PLAYER) { return; }
	Player* player = (Player*)GetOwner();
	JobDataEntry* jobDataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
	if (jobDataEntry == NULL)
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills skills occu "
			<< player->GetOccu() << " is exist " << LogStream::eos;
	}

	//加上预转职技能
	JobDataEntry* preJobDataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetPreOccu());
	if (preJobDataEntry == NULL)
	{
		preJobDataEntry = jobDataEntry;
	}

	std::vector<ChangeSkill> changeSkill;
	ChangeSkill skill;
	std::vector<UInt16> skillIds;
	std::vector<UInt16> initSkill = jobDataEntry->GetInitSkill();
	initSkill.insert(initSkill.end(), preJobDataEntry->preJobSkills.begin(), preJobDataEntry->preJobSkills.end());
	std::vector<UInt16>::iterator skillIter = initSkill.begin();
	while (skillIter != initSkill.end())
	{
		Skill* tmpSkill = FindSkill(*skillIter);
		if (tmpSkill == NULL)
		{
			skill.id = *skillIter;
			skill.dif = 1;
			SkillDataEntry* skillDataEntry = SkillDataEntryMgr::Instance()->FindEntry(skill.id);
			if (skillDataEntry != NULL)
			{
				changeSkill.push_back(skill);
				if (IsSetting(skillDataEntry))
				{
					skillIds.push_back(skill.id);
				}
			}
		}
		skillIter++;
	}

	UInt32 res = ChangeSkills(changeSkill);

	if (res == ErrorCode::SUCCESS)
	{
		GetSkillBar().PushGrid(m_Page+1, skillIds);
	}
}

void SkillMgr::OnAwaken()
{
	if (GetOwner()->GetType() != SOT_PLAYER) return;
	Player* player = (Player*)GetOwner();
	if (player == NULL) return;

	switch ((AwakenStatus)player->GetAwaken())
	{
	case AWAKEN_NOTHING:
		break;
	case AWAKEN_ONE:
		OnAwakenOne();
		return;
	case AWAKEN_TWO:
		return;
	default:
		break;
	}

	return;
}

void SkillMgr::OnLevelUp()
{
	if (GetOwner()->GetType() != SOT_PLAYER) return;
	Player* player = (Player*)GetOwner();
	if (player == NULL) return;

	/*
	bool isDirty = false;
	SkillDataEntryMgr::SkillVec skills = SkillDataEntryMgr::Instance()->GetAwakenSkills(player->GetOccu());
	for (UInt32 i = 0; i < skills.size(); ++i)
	{
		if (skills[i] == NULL) continue;
		//主动技能自动升级,被动技能需要玩家自己花SP点.
		if (!IsActiveSkill(skills[i]->useType)) continue;

		Skill* skill = FindSkill(skills[i]->id);
		if (skill == NULL) continue;

		if ((player->GetLevel() >= skills[i]->playerLevel + skill->level * skills[i]->upLevelInterval)
			&& (skill->level + 1 <= skills[i]->maxLevel))
		{
			skill->level++;
			skill->bdirty = true;
			isDirty = true;
		}
	}
	if (isDirty) { SetDirty(); }
	*/

	//ChangeSkill
	SkillDataEntryMgr::SkillVec autoSkills = SkillDataEntryMgr::Instance()->GetAutoStudySkills((UInt8)player->GetLevel(), player->GetOccu());
	std::vector<ChangeSkill> changeSkills;
	for (UInt32 i = 0; i < autoSkills.size(); ++i)
	{
		Skill* skill = FindSkill(autoSkills[i]->id);
		if (skill != NULL) continue;
		ChangeSkill changeSkill(autoSkills[i]->id, 1);
		changeSkills.push_back(changeSkill);
	}

	ChangeSkills(changeSkills);

	//小于15级,自动学习升级技能
	if (player->GetLevel() < 15)
	{
		RecommendSkills(true);
	}
}

void SkillMgr::OnCheckSkillSP(bool skipCheck)
{
	Player* player = (Player*)GetOwner();

	if (m_SkillConfigType != SkillConfigType::SKILL_CONFIG_EQUAL_PVP && !skipCheck)
	{
		if (player->IsCheckSP()) return;
	}

	UInt32 oldSp = GetSkillSP() + GetSP();
	UInt32 newSp = player->GetAllSP();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		auto * dataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
		if (dataEntry)
		{
			newSp = dataEntry->EqualPvPSP;
		}
		else
		{
			ErrorLogStream << PLAYERINFO(player) << "OnCheckSkillSP can't FindEntry " << player->GetOccu() << ")" << LogStream::eos;
		}
	}

	if (oldSp > newSp)
	{
		UInt32 removeSp = oldSp - newSp;
		InfoLogStream << PLAYERINFO(player) << " recovery sp!! oldSp = " << oldSp << " newSp = " << newSp << " removeSp = " << removeSp << LogStream::eos;

		if (GetSP() < removeSp)
		{
			UInt32 needSp = removeSp - GetSP();
			RecoverySP(needSp);
		}

		if (GetSP() >= removeSp)
		{
			RemoveSP(GetReason(LogSourceType::SOURCE_TYPE_PLAYER_ONLINE).c_str(), removeSp);
		}
	}
	else if (oldSp < newSp)
	{
		ErrorLogStream << PLAYERINFO(player) << "is Error oldSp = " << oldSp << " newSp = " << newSp << LogStream::eos;
		UInt32 addSp = newSp - oldSp;
		AddSP(GetReason(LogSourceType::SOURCE_TYPE_PLAYER_ONLINE).c_str(), addSp);
	}
	else if (oldSp == newSp)
	{
		InfoLogStream << PLAYERINFO(player) << " Nothing Ever Happened!! oldSp = " << oldSp << " newSp = " << newSp << LogStream::eos;
	}
}

void SkillMgr::OnTask(UInt32 id)
{
	Player* player = dynamic_cast<Player*>(GetOwner());
	if (NULL == player)
	{
		return;
	}

	UInt32 taskId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_12_SKILL_SLOT_TASK_ID);
	if (id != taskId)
	{
		return;
	}

	CLProtocol::SceneSkillSlotUnlockNotify notify;
	notify.slot = GetSkillBar().GetMaxBarSize();
	player->SendProtocol(notify);
}

SkillBar& SkillMgr::GetSkillBar()
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->GetPvpSkillBar();
	}
	else if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		return player->GetEqualPvpSkillBar();
	}
	else
	{
		return player->GetSkillBar();
	}
}

UInt32 SkillMgr::GetSP()
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->GetPvpSP(m_Page);
	}
	else if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		return player->GetEqualPvpSP();
	}
	else
	{
		return player->GetPveSP(m_Page);
	}
}

bool SkillMgr::RemoveSP(const char* reason, UInt32 num)
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->RemovePvpSP(reason, num, m_Page);
	}
	else if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		return player->RemoveEqualPvpSP(reason, num);
	}
	else
	{
		return player->RemovePveSP(reason, num, m_Page);
	}
}

void SkillMgr::AddSP(const char* reason, UInt32 num)
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->AddPvpSP(reason, num, m_Page);
	}
	else if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		return player->AddEqualPvpSP(reason, num);
	}
	else
	{
		return player->AddPveSP(reason, num, m_Page);
	}
}

UInt32 SkillMgr::GetNeedSyncSkillCnt()
{
	UInt32 cnt = 0;
	for (SkillMap::iterator iter = m_Skills.begin(); iter != m_Skills.end();)
	{
		Skill* skill = &iter->second;
		++iter;
		if (IsSkillNeedSyncNet(skill->dataEntry->type))
		{
			++cnt;
		}
	}
	return cnt;
}

void SkillMgr::OnAwakenOne()
{
	/*
	if (GetOwner()->GetType() != SOT_PLAYER) return;
	Player* player = (Player*)GetOwner();
	if (player == NULL) return;

	JobDataEntry* dataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
	for (UInt32 i = 0; i < dataEntry->awakenSkills.size(); ++i)
	{
		AddFreeSkill(dataEntry->awakenSkills[i]);
	}
	*/
	//OnLevelUp();
}

void SkillMgr::RecoverySP(UInt32 sp)
{
	UInt32 tmpSp = sp;
	Player* player = (Player*)GetOwner();
	
	if (tmpSp <= 0) player->FinishCheckSP();

	while (tmpSp > 0)
	{
		std::vector<ChangeSkill> changeSkills;
		SkillMap::iterator maxIter = m_Skills.end();
		SkillMap::iterator iter = m_Skills.begin();
		while (iter != m_Skills.end())
		{
			//必须有SP点,扣到1级就不扣了,需要等级最大的先扣
			if (iter->second.dataEntry->needSP > 0 && iter->second.level > 1)
			{
				if (maxIter == m_Skills.end() || maxIter->second.dataEntry->playerLevel < iter->second.dataEntry->playerLevel)
				{
					maxIter = iter;
				}
			}
			++iter;
		}

		if (maxIter == m_Skills.end())
		{
			break;
		}

		ChangeSkill changeSkill;
		changeSkill.id = maxIter->second.id;
		//changeSkill.dif 

		 UInt32 needLevel = (tmpSp / maxIter->second.dataEntry->needSP);
		if (tmpSp % maxIter->second.dataEntry->needSP != 0)
		{
			needLevel += 1;
		}

		if (maxIter->second.level > needLevel)
		{
			changeSkill.dif = ((Int8)needLevel) * -1;
			tmpSp = 0;
		}
		else
		{
			changeSkill.dif = ((Int8)maxIter->second.level - 1) * -1;
			tmpSp = tmpSp - ((maxIter->second.level - 1) * maxIter->second.dataEntry->needSP);
		}
		changeSkills.push_back(changeSkill);

		UInt32 nRes = ChangeSkills(changeSkills);
		if (nRes != ErrorCode::SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(player) << "RecoverySP nRes = " << nRes << " newSp = " << LogStream::eos;
		}
	}
}

void SkillMgr::CheckTaskUnlockSlot()
{
	Player* player = dynamic_cast<Player*>(GetOwner());
	if (player != NULL && IsTaskUnlockSlot())
	{
		CLProtocol::SceneSkillSlotUnlockNotify notify;
		notify.slot = GetSkillBar().GetMaxBarSize();
		player->SendProtocol(notify);
	}
}

bool SkillMgr::IsTaskUnlockSlot()
{
	Player* player = dynamic_cast<Player*>(GetOwner());
	if (NULL == player)
	{
		return false;
	}

	//公平竞技场默认解锁所有格位
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
	{
		return true;
	}

	UInt32 taskId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_12_SKILL_SLOT_TASK_ID);
	return player->CheckTaskFinished(taskId);
}

void SkillMgr::AddFreeSkill(UInt16 id, UInt8 level)
{
	if (level <= 0) return;

	SkillMap::iterator iter = m_Skills.find(id);
	if (iter != m_Skills.end())
	{
		iter->second.level = level;
		iter->second.bdirty = true;
	}
	else
	{
		Skill addSkill;
		addSkill.id = id;
		addSkill.level = level;
		if (addSkill.Init())
		{
			addSkill.bdirty = true;
			m_Skills.insert(std::make_pair(addSkill.id, addSkill));
		}
	}
	SetDirty();
}

void SkillMgr::DelFreeSkill(UInt16 id)
{
	Skill* skill = FindSkill(id);
	if (skill != NULL)
	{
		skill->level = 0;
		skill->bdirty = true;
		SetDirty();
	}
}

void SkillMgr::GetGuildSkillDungeonAddition(DungeonAddition& addition)
{
	for (auto itr : m_Skills)
	{
		const auto& skill = itr.second;
		if (!skill.dataEntry || skill.dataEntry->type != SKILL_TYPE_GUILD)
		{
			continue;
		}

		for (auto buffInfoId : skill.dataEntry->buffInfoList)
		{
			auto buffInfoData = BuffInfoEntryMgr::Instance()->FindEntry(buffInfoId);
			if (!buffInfoData)
			{
				continue;
			}

			auto buffData = BuffDataEntryMgr::Instance()->FindEntry(buffInfoData->bufferId);
			if (!buffData)
			{
				continue;
			}

			//addition.guildSkillExpRate += buffData->expAddRate.GetValue(skill.level);
			//addition.goldRate += buffData->goldAddRate.GetValue(skill.level);
			addition.additionRate[DART_GUID_SKILL_EXP_RATE] += buffData->expAddRate.GetValue(skill.level);
			addition.additionRate[DART_GOLD_RATE] += buffData->goldAddRate.GetValue(skill.level);
		}
	}

	//addition.expRate += addition.guildSkillExpRate;
	addition.additionRate[DART_EXP_RATE] += addition.additionRate[DART_GUID_SKILL_EXP_RATE];
}

void SkillMgr::AddStatueSkill(SystemValueType systemValueType)
{
	UInt16 skillId = (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(systemValueType);

	if (FindSkill(skillId) == NULL)
	{
		AddFreeSkill(skillId);
	}
}

void SkillMgr::DelStatueSkill(SystemValueType systemValueType)
{
	UInt16 skillId = (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(systemValueType);
	if (FindSkill(skillId) != NULL)
	{
		DelFreeSkill(skillId);
	}
}

UInt32 SkillMgr::ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify, bool isConsume)
{
	UInt32 skillSize = skills.size();
	if (skillSize <= 0) return ErrorCode::SUCCESS;
	if (GetOwner()->GetType() != SOT_PLAYER)
	{
		return ErrorCode::SKILL_ERROR;
	}
	Player* player = (Player*)GetOwner();

	UInt32 removeGlod = 0;
	SkillMap skillMap;
	Int32 sp = (Int32)GetSP();
	UInt32 res = CheckChangeSkills(player, skills, skillMap, sp, removeGlod, isConsume);
	if (res == ErrorCode::SUCCESS)
	{
		SkillMap::iterator tmpIter = skillMap.begin();
		while (tmpIter != skillMap.end())
		{
			SkillMap::iterator skillIter = m_Skills.find(tmpIter->second.id);
			if (skillIter != m_Skills.end())
			{
				UInt8 oldLevel = skillIter->second.level;
				skillIter->second.level = tmpIter->second.level;
				skillIter->second.bdirty = true;
				player->OnChangeSkill(tmpIter->second.id, tmpIter->second.dataEntry->name, tmpIter->second.level - oldLevel, oldLevel, tmpIter->second.level, IsSetting(tmpIter->second.dataEntry));
			}
			else
			{
				Skill addSkill;
				addSkill.id = tmpIter->second.id;
				addSkill.level = tmpIter->second.level;
				addSkill.Init();
				addSkill.bdirty = true;
				m_Skills.insert(std::make_pair(addSkill.id, addSkill));
				player->OnChangeSkill(addSkill.id, tmpIter->second.dataEntry->name, tmpIter->second.level, 0, tmpIter->second.level, IsSetting(tmpIter->second.dataEntry));

			}
			++tmpIter;
		}
		std::string reason;
		if (sp > 0 && sp <= (Int32)GetSP())
		{
			reason = GetReason(LogSourceType::SOURCE_TYPE_SKILL_UP_LEVEL);
			RemoveSP(reason.c_str(), GetSP() - sp);
			if (isNotify) player->SendNotify(1081);
		}
		else
		{
			reason = GetReason(LogSourceType::SOURCE_TYPE_SKILL_DOWN_LEVEL);
			AddSP(reason.c_str(), sp - GetSP());
			if(isNotify) player->SendNotify(1082);
		}

		//消耗物品
		if (removeGlod > 0)
		{
			player->RemoveGoldBoth(reason.c_str(), removeGlod);
			InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") remove glod " << removeGlod << LogStream::eos;
		}

		//刷新技能快捷栏
		GetSkillBar().ClearNotValidEntry(m_Page);
		SetDirty();
		InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") change skills succeed" << LogStream::eos;
	}
	else
	{
		ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") CheckChangeSkills error" << LogStream::eos;
	}
	return res;
}


UInt32 SkillMgr::ForgetAllSkills(bool isConsume)
{
	std::vector<ChangeSkill> skills;

	if (GetOwner()->GetType() != SOT_PLAYER)
	{
		return ErrorCode::SKILL_ERROR;
	}

	Player* player = (Player*)GetOwner();
	JobDataEntry* jobDataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
	if (jobDataEntry == NULL) return ErrorCode::SKILL_OCCU_ERROR;

	SkillGridVec skill_gird_vec;
	SkillMap::iterator iter = m_Skills.begin();
	for (; iter != m_Skills.end(); ++iter)
	{
		Skill& skill = iter->second;
		if (skill.IsValid() && (skill.dataEntry->type == SKILL_TYPE_OCCUPATION || skill.dataEntry->type == SKILL_TYPE_GENERAL || skill.dataEntry->type == SKILL_TYPE_AWAKEN))
		{
			UInt32 initLv = jobDataEntry->GetInitLevel(iter->second.id);
			
			// 预转职职业在未转职之前不卸下
			if (skill.dataEntry->isPreJob && player->IsInitOccu())
			{
				SkillBarGrid skill_gird;
				skill_gird.id = skill.id;
				skill_gird.slot = 4;	// 预转职默认槽位4
				skill_gird_vec.push_back(skill_gird);
			}

			//如果是预转职技能或自动的学习技能,设置初始等级
			if (skill.dataEntry->isPreJob || skill.dataEntry->isStudy)
				initLv = 1;

			ChangeSkill change_skill;
			change_skill.id = skill.id;
			change_skill.dif = skill.level * -1 + initLv;
			if (change_skill.dif == 0)
				continue;

			skills.push_back(change_skill);
		}
	}

	UInt32 ret = ChangeSkills(skills, false, isConsume);
	if (ret == ErrorCode::SUCCESS)
	{
		//设置初始技能.
		GetSkillBar().ClearAllEntry(m_Page);
		//GetSkillBar().SetBarIndex(m_Page);
		GetSkillBar().PushFixGrid(m_Page + 1, skill_gird_vec);
	}

	return ret;
}


bool SkillMgr::IsSetting(SkillDataEntry* dataEntry)
{
	if (dataEntry == NULL) return false;
	if ((m_SkillConfigType == SKILL_CONFIG_PVP||m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP) && dataEntry->canUseInPVP != 1)
	{
		return false;
	}
	if (IsActiveSkill(dataEntry->useType) && IsNeedSetting(dataEntry->type) && !dataEntry->isBuff && !dataEntry->isQTE)
	{
		return true;
	}
	return false;
}

UInt32 SkillMgr::InitAllSkills()
{
	UInt32 ret = ForgetAllSkills();
	if (ErrorCode::SUCCESS == ret)
		GetOwner()->SyncProperty();

	return ret;
}

UInt32 SkillMgr::RecommendSkills(bool isStudy)
{
	Player* player = (Player*)GetOwner();
	if (NULL == player) return ErrorCode::SYS_ERROR;

	const SkillRecommendVec* skillRecommendVec = SkillRecommendDataEntryMgr::Instance()->
		GetRecommendSkillVec(player->GetOccu(), m_SkillConfigType, player->GetLevel());
	if (skillRecommendVec == NULL)
	{
		if (m_SkillConfigType == SKILL_CONFIG_PVP)
		{
			skillRecommendVec = SkillRecommendDataEntryMgr::Instance()->
				GetRecommendSkillVec(player->GetOccu(), SKILL_CONFIG_PVE, player->GetLevel());
		}
		else if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
		{
			skillRecommendVec = SkillRecommendDataEntryMgr::Instance()->
				GetRecommendSkillVec(player->GetOccu(), SKILL_CONFIG_PVP, PlayerDataEntryMgr::Instance()->GetMaxLevel());
		}

		if (NULL == skillRecommendVec)
			return ErrorCode::ITEM_DATA_INVALID;
	}

	// 15级之前，先比对玩家技能位是否和推荐一致
	if ((player->GetLevel() < 15) && isStudy && (m_SkillConfigType != SKILL_CONFIG_EQUAL_PVP))
	{
		SkillGridVec skill_gird_vec;
		for (auto& iter : *skillRecommendVec)
		{
			if (NULL == iter)
				continue;

			if (iter->skillPos == 0)
				continue;

			SkillBarGrid barGrid;
			barGrid.id = iter->skillID;
			barGrid.slot = iter->skillPos;
			skill_gird_vec.push_back(barGrid);
		}

		if (!GetSkillBar().CheckRecommendGrid(m_Page + 1, skill_gird_vec))
			return ErrorCode::SUCCESS;
	}

	// 推荐技能先初始化技能
	InitAllSkills();

	UInt32 cur_sp = GetSP();
	//if (cur_sp == 0)
	//	return ErrorCode::SKILL_SP_NOT_ENOUGH;

	// 创建技能列表,先把所有推荐的技能升到1级
	std::map<UInt32, ChangeSkill> changeSkillMap;
	for (UInt32 k = 0;k < skillRecommendVec->size();++k)
	{
		 const SkillDetails* skill_details = skillRecommendVec->at(k);
		 if (NULL == skill_details)
			 continue;

		 // 槽位12需要任务解锁
		 if (skill_details->skillPos == 12 && !IsTaskUnlockSlot())
		 {
			 continue;
		 }

		 SkillDataEntry* skillEntry = NULL;

		 skillEntry = SkillDataEntryMgr::Instance()->FindEntry(skill_details->skillID);

		 if (NULL == skillEntry || (skillEntry->type == SkillType::SKILL_TYPE_AWAKEN && player->GetAwaken() == AwakenStatus::AWAKEN_NOTHING))
		 {
			 continue;
		 }

		 auto iter = m_Skills.find(skill_details->skillID);
		 if (iter == m_Skills.end())
		 {
			 skillEntry = SkillDataEntryMgr::Instance()->FindEntry(skill_details->skillID);
			 if (NULL == skillEntry)
			 {
				 ErrorLogStream << "RecommendSkills SkillDataEntry SkillID:"<<skill_details->skillID <<" Config Not Find." << LogStream::eos;
				 continue;
			 }
		 }
		 else
		 {
			 Skill& skill = iter->second;
			 if (skill.level == 1)
			 {
				 ChangeSkill change_skill;
				 change_skill.id = skill_details->skillID;
				 change_skill.dif = 0;
				 changeSkillMap[change_skill.id] = (change_skill);
				 continue;
			 }

			 skillEntry = skill.dataEntry;
		 }

		 // 玩家没达到学这个技能的等级
		 if (m_SkillConfigType != SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
		 {
			 if (GetOwner()->GetLevel() < skillEntry->playerLevel)
				 continue;
		 }

		 if (skillEntry->needSP > cur_sp)
			 continue;

		 cur_sp -= skillEntry->needSP;

		 ChangeSkill change_skill;
		 change_skill.id = skill_details->skillID;
		 change_skill.dif = 1;
		 changeSkillMap[change_skill.id] = (change_skill);
	}

	// 再按优先级把技能升到最高
	SkillGridVec skill_gird_vec;
	std::vector<ChangeSkill> changeSkillVec;
	for (UInt32 k = 0; k < skillRecommendVec->size(); ++k)
	{
		const SkillDetails* skill_details = skillRecommendVec->at(k);
		if (NULL == skill_details)
			continue;

		SkillDataEntry* skillEntry = SkillDataEntryMgr::Instance()->FindEntry(skill_details->skillID);
		if (NULL == skillEntry)
		{
			ErrorLogStream << "RecommendSkills SkillDataEntry SkillID:" << skill_details->skillID << " Config Not Find." << LogStream::eos;
			continue;
		}

		auto iter = changeSkillMap.find(skill_details->skillID);
		if (iter == changeSkillMap.end())
		{
			continue;
		}
		else
		{
			if (skill_details->skillPos != 0 && IsSetting(skillEntry))
			{
				SkillBarGrid skill_gird;
				skill_gird.id = UInt16(skill_details->skillID);
				skill_gird.slot = UInt8(skill_details->skillPos);
				skill_gird_vec.push_back(skill_gird);
			}
		}

		//检查剩余的技能点够升多少级
		Int8 num = 0;
		UInt32 num_1 = 0;
		if (skillEntry->needSP != 0)
			num_1 = cur_sp / skillEntry->needSP;

		if (num_1 > 127)
			num = 127;
		else
			num = (Int8)num_1;
	
		if (num > 0)
		{
			// 玩家当前等级下，学习该技能能达到的最大等级
			Int8 study_max_level = 1;

			if (m_SkillConfigType != SkillConfigType::SKILL_CONFIG_EQUAL_PVP)
			{
				if (skillEntry->upLevelInterval != 0 && skillEntry->playerLevel <= GetOwner()->GetLevel())
					study_max_level = (GetOwner()->GetLevel() - skillEntry->playerLevel) / skillEntry->upLevelInterval + 1;
			}
			else
			{
				if (skillEntry->upLevelInterval != 0 && skillEntry->playerLevel <= PlayerDataEntryMgr::Instance()->GetMaxLevel())
					study_max_level = (PlayerDataEntryMgr::Instance()->GetMaxLevel() - skillEntry->playerLevel) / skillEntry->upLevelInterval + 1;
			}


			if (study_max_level > skillEntry->maxLevel)
				study_max_level = skillEntry->maxLevel;

			// 前面已经升了1级，现在统一减回来
			if (study_max_level > 0)
				study_max_level--;

			if (num > study_max_level)
				num = study_max_level;

			cur_sp -= num * skillEntry->needSP;
			iter->second.dif += num;
		}

		changeSkillVec.push_back(iter->second);
	}

	UInt32 ret = ChangeSkills(changeSkillVec);

	if (ret != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," 
			<< GetOwner()->GetName() << ") recommend skill error:"<<ret<< LogStream::eos;
	}

	// 放到固定的技能位上
	GetSkillBar().PushFixGrid(m_Page + 1, skill_gird_vec);
	GetOwner()->SyncProperty();

	return ret;
}

void SkillMgr::ClearSkills()
{
	if (GetOwner()->GetLevel() > RESET_SKILL_LEVEL) return;
	ForgetAllSkills();
}

void SkillMgr::ClearPreOccuSkills()
{
	Player* player = (Player*)GetOwner();

	if (player->GetPreOccu() == 0) return;

	JobDataEntry* preJobDataEntry = JobDataEntryMgr::Instance()->FindEntry(player->GetPreOccu());
	if (preJobDataEntry == NULL) return;

	for (UInt32 i = 0; i < preJobDataEntry->preJobSkills.size(); ++i)
	{
		UInt16 skillId = preJobDataEntry->preJobSkills[i];
		SkillMap::iterator iter = m_Skills.find(skillId);
		if (iter != m_Skills.end())
		{
			iter->second.level = 0;
			iter->second.bdirty = true;
		}
	}

	//刷新技能快捷栏
	GetSkillBar().ClearNotValidEntry(m_Page);
	SetDirty();
}

UInt32 SkillMgr::GetSkillSP()
{
	UInt32 result = 0;
	JobDataEntry* jobDataEntry = JobDataEntryMgr::Instance()->FindEntry(GetOwner()->GetOccu());
	SkillMap::iterator iter = m_Skills.begin();
	while (iter != m_Skills.end())
	{
		Int32 initLevel = jobDataEntry->GetInitLevel(iter->second.id);
		if (iter->second.dataEntry->isPreJob && initLevel < 1)
		{
			initLevel = 1;
		}

		if (iter->second.level > initLevel)
		{
			result = result + ((iter->second.level - initLevel) * iter->second.dataEntry->needSP);
		}
		++iter;
	}
	return result;
}


void SkillMgr::OnTick(const Avalon::Time& now)
{
}

void SkillMgr::Input(Avalon::NetInputStream& stream)
{
	std::vector<Skill> skills;
	stream & skills;
	for (auto &skill : skills)
	{
		if (skill.id != 0)
		{
			if (skill.Init())
			{
				m_Skills.insert(std::make_pair(skill.id, skill));
			}
		}
	}
}

void SkillMgr::Output(Avalon::NetOutputStream& stream)
{
	class OutputVisitor : public SkillVisitor
	{
	public:
		OutputVisitor(std::vector<Skill>& skills):m_Skills(skills){}

		bool operator()(Skill* skill)
		{
			if(!IsSkillNeedSyncNet(skill->dataEntry->type)) return true;

			m_Skills.push_back(*skill);
			return true;
		}

	private:
		std::vector<Skill>& m_Skills;
	};
	std::vector<Skill> skills;
	OutputVisitor visitor(skills);
	VisitSkills(visitor);
	stream & skills;

	ClearInvalidData();
}

void SkillMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	class EncodeVisitor : public SkillVisitor
	{
	public:
		EncodeVisitor(std::vector<Skill>& skills, bool _bdirty)
			:m_Skills(skills),m_bDirty(_bdirty){}

		bool operator()(Skill* skill)
		{
			if(!IsSkillNeedSyncNet(skill->dataEntry->type)) return true;

			if((m_bDirty && skill->bdirty) || (!m_bDirty && skill->IsValid()))
			{
				m_Skills.push_back(*skill);
			}
			return true;
		}
	private:
		std::vector<Skill>& m_Skills;
		bool	m_bDirty;
	};
	std::vector<Skill> skills;
	EncodeVisitor visitor(skills, bDirty);
	VisitSkills(visitor,true);
	stream & skills;

	ClearInvalidData(); 
}

void SkillMgr::Decode(Avalon::NetInputStream& stream)
{
	std::vector<Skill> skills;
	stream & skills;
	for (auto &skill : skills)
	{
		if (skill.id != 0)
		{
			if (skill.Init())
			{
				m_Skills.insert(std::make_pair(skill.id, skill));
			}
		}
	}
}

void SkillMgr::DecodeString(std::istringstream& is)
{
	Skill skill;
	char split = 0;
	int level = 0;

	auto player = GetOwner()->ToPlayer();
	if (player != nullptr)
	{
		if (m_SkillConfigType == SKILL_CONFIG_PVE)
		{
			if (player->GetCounter(COUNTER_NEW_PVE_SKILL_PAGE) == 0)
			{
				while ((is >> skill.id) && skill.id != 0)
				{
					is >> split >> level >> split;

					skill.level = level;
					if (skill.Init())
					{
						m_Skills.insert(std::make_pair(skill.id, skill));
					}
				}
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_PVP_SKILL_PAGE) == 0)
			{
				while ((is >> skill.id) && skill.id != 0)
				{
					is >> split >> level >> split;

					skill.level = level;
					if (skill.Init())
					{
						m_Skills.insert(std::make_pair(skill.id, skill));
					}
				}
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_EQUAL_PVP_SKILL_PAGE) == 0)
			{
				while ((is >> skill.id) && skill.id != 0)
				{
					is >> split >> level >> split;

					skill.level = level;
					if (skill.Init())
					{
						m_Skills.insert(std::make_pair(skill.id, skill));
					}
				}
				return;
			}
		}
	}

	UInt32 num = 0;
	is >> num >> split;
	if (num > 0)
	{
		while (num--)
		{
			UInt32 tempId = 0;
			UInt32 tempLevel = 0;
			is >> tempId >> split >> tempLevel >> split;
			skill.id = tempId;
			skill.level = tempLevel;
			if (skill.Init())
			{
				//skill.usetime = m_pOwner->GetSavebackTime() + cdtime;
				m_Skills.insert(std::make_pair(skill.id, skill));
			}
		}
	}
}

void SkillMgr::EncodeString(std::ostringstream& os)
{
	class EncodeStringVisitor : public SkillVisitor
	{
	public:
		EncodeStringVisitor(std::vector<Skill>& skills):m_Skills(skills){}

		bool operator()(Skill* skill)
		{
			if(!IsSkillNeedSyncDB(skill->dataEntry->type)) return true;	//普通物理攻击不同步

			m_Skills.push_back(*skill);
			return true;
		}
	private:
		std::vector<Skill>& m_Skills;
	};
	std::vector<Skill> skills;
	EncodeStringVisitor visitor(skills);
	VisitSkills(visitor);
	os << static_cast<UInt32>(skills.size()) << ':';
	for (auto& skill : skills)
	{
		os << static_cast<UInt32>(skill.id) << ',' << static_cast<UInt32>(skill.level) << ';';
	}
}

void SkillMgr::ClearInvalidData()
{
	SkillMap::iterator iter = m_Skills.begin();
	while(iter != m_Skills.end())
	{
		Skill* skill = &iter->second;
		skill->bdirty = false;

		if(!skill->IsValid()) m_Skills.erase(iter++);
		else ++iter;
	}
}

void SkillMgr::SetDirty()
{
	if (m_SkillConfigType == SKILL_CONFIG_PVE)
	{
		GetOwner()->GetSkillMgr().SetDirty();
	}
	else if (m_SkillConfigType == SKILL_CONFIG_PVP)
	{
		auto player = GetOwner()->ToPlayer();
		if (player != nullptr)
		{
			player->GetPvpSkillMgr().SetDirty();
		}
	}
	else if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
	{
		auto player = GetOwner()->ToPlayer();
		if (player != nullptr)
		{
			player->GetEqualPvpSkillMgr().SetDirty();
		}
	}
}

PlayerSkillMgr::PlayerSkillMgr()
{
	for (UInt32 i = 0; i< CL_MAX_SKILL_PAGE; ++i)
	{
		m_SkillMgrVec.emplace_back(i);
	}
}

void PlayerSkillMgr::SetOwner(Creature * owner)
{
	m_pOwner = owner;
	for (auto &i : m_SkillMgrVec)
	{
		i.SetOwner(owner);
	}
}

void PlayerSkillMgr::Input(Avalon::NetInputStream & stream)
{
	stream & m_PageCnt & m_CurrentPage;
	stream & m_SkillMgrVec;
}

void PlayerSkillMgr::Output(Avalon::NetOutputStream & stream)
{
	stream & m_PageCnt & m_CurrentPage;
	stream & m_SkillMgrVec;
}

void PlayerSkillMgr::Encode(Avalon::NetOutputStream & stream, UInt32 type, bool bDirty)
{
	stream & m_PageCnt & m_CurrentPage;
	stream & static_cast<UInt16>(m_SkillMgrVec.size());
	for (auto &skillPage : m_SkillMgrVec)
	{
		skillPage.Encode(stream, type, bDirty);
	}
	//stream & m_SkillMgrVec;
}

void PlayerSkillMgr::Decode(Avalon::NetInputStream & stream)
{
	stream & m_PageCnt & m_CurrentPage;
	stream & m_SkillMgrVec;
}

void PlayerSkillMgr::DecodeString(std::istringstream & is)
{
	auto player = GetOwner()->ToPlayer();
	if (player != nullptr)
	{
		if (m_SkillConfigType == SKILL_CONFIG_PVE)
		{
			if (player->GetCounter(COUNTER_NEW_PVE_SKILL_PAGE) == 0)
			{
				m_SkillMgrVec[0].DecodeString(is);
				SetDirty();
				//player->SetCounter(COUNTER_NEW_PVE_SKILL_PAGE, 1);
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_PVP_SKILL_PAGE) == 0)
			{
				m_SkillMgrVec[0].DecodeString(is);
				SetDirty();
				//player->SetCounter(COUNTER_NEW_PVP_SKILL_PAGE, 1);
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_EQUAL_PVP_SKILL_PAGE) == 0)
			{
				m_SkillMgrVec[0].DecodeString(is);
				SetDirty();
				//player->SetCounter(COUNTER_NEW_EQUAL_PVP_SKILL_PAGE, 1);
				return;
			}
		}
	}

	char tempChar;
	is >> m_PageCnt >> tempChar >> m_CurrentPage >> tempChar;
	for (auto &i : m_SkillMgrVec)
	{
		i.DecodeString(is);
		is >> tempChar;
	}
}

void PlayerSkillMgr::EncodeString(std::ostringstream & os)
{
	os << m_PageCnt << '@' << m_CurrentPage << '@';
	for (auto &i : m_SkillMgrVec)
	{
		i.EncodeString(os);
		os << '|';
	}
	auto player = GetOwner()->ToPlayer();
	if (player != nullptr)
	{
		if (m_SkillConfigType == SKILL_CONFIG_PVE)
		{
			if (player->GetCounter(COUNTER_NEW_PVE_SKILL_PAGE) == 0)
			{
				player->SetCounter(COUNTER_NEW_PVE_SKILL_PAGE, 1);
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_PVP_SKILL_PAGE) == 0)
			{
				player->SetCounter(COUNTER_NEW_PVP_SKILL_PAGE, 1);
				return;
			}
		}
		else if (m_SkillConfigType == SKILL_CONFIG_EQUAL_PVP)
		{
			if (player->GetCounter(COUNTER_NEW_EQUAL_PVP_SKILL_PAGE) == 0)
			{
				player->SetCounter(COUNTER_NEW_EQUAL_PVP_SKILL_PAGE, 1);
				return;
			}
		}
	}
	else
	{
		ErrorLogStream << "player is null id is " << GetOwner()->GetID() << LogStream::eos;
	}
}

void PlayerSkillMgr::ClearSkills()
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return;
	}
	m_SkillMgrVec[m_CurrentPage].ClearSkills();
}

bool PlayerSkillMgr::GetSkillPageUnlockStatus(UInt8 page)
{
	if (page >= m_PageCnt)
	{
		return false;
	}
	return true;
}

void PlayerSkillMgr::UnlockNewPage()
{
	if (m_PageCnt >= CL_MAX_SKILL_PAGE)
	{
		return;
	}
	//新开技能页需要各种初始化
	m_SkillMgrVec[m_PageCnt].OnBirth();
	m_SkillMgrVec[m_PageCnt].OnCheckSkillSP(true);
	m_SkillMgrVec[m_PageCnt].OnOnline();
	SyncGuildSkillToAllPage();
	++m_PageCnt;
	SetDirty();
}

UInt32 PlayerSkillMgr::ForgetAllSkills(bool isConsume)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return ErrorCode::SYS_ERROR;
	}
	return m_SkillMgrVec[m_CurrentPage].ForgetAllSkills(isConsume);
}

void PlayerSkillMgr::ClearPreOccuSkills()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.ClearPreOccuSkills();
	}
}

UInt32 PlayerSkillMgr::ChangeSkills(std::vector<ChangeSkill> skills, bool isNotify, bool isConsume)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return ErrorCode::SYS_ERROR;
	}
	return m_SkillMgrVec[m_CurrentPage].ChangeSkills(skills, isNotify, isConsume);
}

UInt32 PlayerSkillMgr::InitAllSkills()
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return ErrorCode::SYS_ERROR;
	}
	return m_SkillMgrVec[m_CurrentPage].InitAllSkills();
}

UInt32 PlayerSkillMgr::RecommendSkills(bool isStudy)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return ErrorCode::SYS_ERROR;
	}
	return m_SkillMgrVec[m_CurrentPage].RecommendSkills(isStudy);
}

void PlayerSkillMgr::GetGuildSkillDungeonAddition(DungeonAddition & addition)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return;
	}
	m_SkillMgrVec[m_CurrentPage].GetGuildSkillDungeonAddition(addition);
}

void PlayerSkillMgr::AddFreeSkill(UInt16 id, UInt8 level)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.AddFreeSkill(id, level);
	}
}

void PlayerSkillMgr::DelFreeSkill(UInt16 id)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.DelFreeSkill(id);
	}
}

Skill * PlayerSkillMgr::FindSkill(UInt16 id)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return nullptr;
	}
	return m_SkillMgrVec[m_CurrentPage].FindSkill(id);
}

void PlayerSkillMgr::AddStatueSkill(SystemValueType systemValueType)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.AddStatueSkill(systemValueType);
	}
}

void PlayerSkillMgr::DelStatueSkill(SystemValueType systemValueType)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.DelStatueSkill(systemValueType);
	}
}

bool PlayerSkillMgr::IsSetting(SkillDataEntry * dataEntry)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return false;
	}
	return  m_SkillMgrVec[m_CurrentPage].IsSetting(dataEntry);
}

void PlayerSkillMgr::VisitSkills(SkillVisitor & visitor, bool bAll)
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return;
	}
	m_SkillMgrVec[m_CurrentPage].VisitSkills(visitor, bAll);
}

void PlayerSkillMgr::CheckTaskUnlockSlot()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.CheckTaskUnlockSlot();
	}
}

bool PlayerSkillMgr::IsTaskUnlockSlot()
{
	if (static_cast<Int32>(m_CurrentPage) > static_cast<Int32>(m_SkillMgrVec.size()) - 1)
	{
		return false;
	}
	return m_SkillMgrVec[m_CurrentPage].IsTaskUnlockSlot();
}

void PlayerSkillMgr::OnTick(const Avalon::Time & now)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnTick(now);
	}
}

void PlayerSkillMgr::OnOnline()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnOnline();
	}
}

void PlayerSkillMgr::OnChangeOccu()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnChangeOccu();
	}
}

void PlayerSkillMgr::OnBirth()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnBirth();
	}
}

void PlayerSkillMgr::OnAwaken()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnAwaken();
	}
}

void PlayerSkillMgr::OnLevelUp()
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnLevelUp();
	}
}

void PlayerSkillMgr::OnTask(UInt32 id)
{
	for (auto &i : m_SkillMgrVec)
	{
		i.OnTask(id);
	}
}

void PlayerSkillMgr::CopyWithoutConfigType(const PlayerSkillMgr & rsh)
{
	m_SkillMgrVec = rsh.m_SkillMgrVec;
	m_pOwner = rsh.m_pOwner;
	m_PageCnt = rsh.m_PageCnt;
	m_CurrentPage = rsh.m_CurrentPage;
	SetConfigType(m_SkillConfigType);
	SetDirty();
}

void PlayerSkillMgr::SyncGuildSkillToAllPage()
{
	class OutputVisitor : public SkillVisitor
	{
	public:
		OutputVisitor(std::vector<Skill>& skills) :m_Skills(skills) {}

		bool operator()(Skill* skill)
		{
			m_Skills.push_back(*skill);
			return true;
		}

	private:
		std::vector<Skill>& m_Skills;
	};
	std::vector<Skill> skills;
	OutputVisitor visitor(skills);
	m_SkillMgrVec[0].VisitSkills(visitor, true);
	for (auto &i : skills)
	{
		auto data = SkillDataEntryMgr::Instance()->FindEntry(i.id);
		if (data != nullptr)
		{
			if (IsGuildSkill(data->type)) 
			{
				if (m_SkillMgrVec[1].FindSkill(i.id) == nullptr)
				{
					m_SkillMgrVec[1].AddFreeSkill(i.id, i.level);
				}
			}
		}
	}
}

void PlayerSkillMgr::SetConfigType(SkillConfigType type)
{
	m_SkillConfigType = type;
	for (auto &i : m_SkillMgrVec)
	{
		i.SetConfigType(type);
	}
}

void SpMgr::CopyWithoutConfig(const SpMgr & rsh)
{
	m_Sp = rsh.m_Sp;
	m_CurrentPage = rsh.m_CurrentPage;
	m_pOwner = rsh.m_pOwner;
	SetDirty();
}

void SpMgr::Input(Avalon::NetInputStream & stream)
{
	stream & m_Sp;
}

void SpMgr::Output(Avalon::NetOutputStream & stream)
{
	stream & m_Sp;
}

void SpMgr::Encode(Avalon::NetOutputStream & stream, UInt32 type, bool bDirty)
{
	stream & m_Sp;
}

void SpMgr::Decode(Avalon::NetInputStream & stream)
{
	stream & m_Sp;
}

void SpMgr::DecodeString(std::istringstream & is)
{
	if (m_SkillConfigType == SKILL_CONFIG_PVE)
	{
		if (GetOwner()->GetCounter(COUNTER_NEW_PVE_SKILL_SP) == 0)
		{
			is >> m_Sp[0];
			SetDirty();
			return;
		}
	}
	else if (m_SkillConfigType == SKILL_CONFIG_PVP)
	{
		if (GetOwner()->GetCounter(COUNTER_NEW_PVP_SKILL_SP) == 0)
		{
			is >> m_Sp[0];
			SetDirty();
			return;
		}
	}

	char split;
	UInt32 len;
	is >> len >> split;
	if (len <= 0)
	{
		return;
	}
	UInt32 temp = 0;
	while (len > temp)
	{
		is >> m_Sp[temp++] >> split;
	}
}

void SpMgr::EncodeString(std::ostringstream & os)
{
	os << static_cast<UInt32>(m_Sp.size()) << '|';
	for (auto &i : m_Sp)
	{
		os << i << '|';
	}

	if (m_SkillConfigType == SKILL_CONFIG_PVE)
	{
		if (GetOwner()->GetCounter(COUNTER_NEW_PVE_SKILL_SP) == 0)
		{
			GetOwner()->SetCounter(COUNTER_NEW_PVE_SKILL_SP, 1);
		}
	}
	else if (m_SkillConfigType == SKILL_CONFIG_PVP)
	{
		if (GetOwner()->GetCounter(COUNTER_NEW_PVP_SKILL_SP) == 0)
		{
			GetOwner()->SetCounter(COUNTER_NEW_PVP_SKILL_SP, 1);
		}
	}
}

void SpMgr::Reset()
{
	for (auto &i : m_Sp)
	{
		i = 0;
	}
}

void SpMgr::AddAllPageSp(UInt32 sp)
{
	for (auto &i : m_Sp)
	{
		if (i + sp > MAX_SP)
		{
			i = MAX_SP;
		}
		else
		{
			i += sp;
		}
	}
	SetDirty();
}

void SpMgr::RemoveAllPageSp(UInt32 sp)
{
	for (auto &i : m_Sp)
	{
		if (i < sp)
		{
			i = 0;
		}
		else
		{
			i -= sp;
		}
	}
	SetDirty();
}
