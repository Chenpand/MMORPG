#include "SkillMgr.h"
#include <AvalonRandom.h>
#include <CLSkillProtocol.h>
#include <CLGuildProtocol.h>
#include <CLBuffDataEntry.h>
#include <CLSkillRecommendDataEntry.h>
#include <CLChiJiSkillDataEntry.h>
#include <CLBattleProtocol.h>

#include "Creature.h"
#include "Scene.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "CLErrorCode.h"
#include "SSRouter.h"
#include "SSApplication.h"
#include "SceneSysNotify.h"

void SpMgr::Encode(Avalon::NetOutputStream & stream, UInt32 type, bool bDirty)
{
	stream & static_cast<UInt16>(1) & GetOwner()->GetPvpSP();
}

void SpMgr::Decode(Avalon::NetInputStream & stream)
{
	UInt16 uselessU16;
	UInt32 uselessU32;
	stream & uselessU16 & uselessU32;
}

bool Skill::Init()
{
	dataEntry = SkillDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return false;
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
}

void SkillMgr::OnChangeOccu()
{
	if (SERVER_TYPE == ST_BSCENE) return;

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
		GetSkillBar().PushGrid(1, skillIds);
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
	if (SERVER_TYPE == ST_BSCENE) return;
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

void SkillMgr::OnCheckSkillSP()
{
	Player* player = (Player*)GetOwner();

	if (player->IsCheckSP()) return;

	UInt32 oldSp = GetSkillSP() + GetSP();
	UInt32 newSp = player->GetAllSP();

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

SkillBar& SkillMgr::GetSkillBar()
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->GetPvpSkillBar();
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
		return player->GetPvpSP();
	}
	else
	{
		return player->GetPveSP();
	}
}

bool SkillMgr::RemoveSP(const char* reason, UInt32 num)
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->RemovePvpSP(reason, num);
	}
	else
	{
		return player->RemovePveSP(reason, num);
	}
}

void SkillMgr::AddSP(const char* reason, UInt32 num)
{
	Player* player = (Player*)GetOwner();
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return player->AddPvpSP(reason, num);
	}
	else
	{
		return player->AddPveSP(reason, num);
	}
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
		GetSkillBar().ClearNotValidEntry();
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
		GetSkillBar().ClearAllEntry();
		GetSkillBar().SetBarIndex(0);
		GetSkillBar().PushFixGrid(1, skill_gird_vec);
	}

	return ret;
}


bool SkillMgr::IsSetting(SkillDataEntry* dataEntry)
{
	if (dataEntry == NULL) return false;
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

		if (NULL == skillRecommendVec)
			return ErrorCode::ITEM_DATA_INVALID;
	}

	// 15级之前，先比对玩家技能位是否和推荐一致
	if (player->GetLevel() < 15 && isStudy)
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

		if (!GetSkillBar().CheckRecommendGrid(1, skill_gird_vec))
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

		 SkillDataEntry* skillEntry = NULL;
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
		 if (GetOwner()->GetLevel() < skillEntry->playerLevel)
			 continue;

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
			if (skillEntry->upLevelInterval != 0 && skillEntry->playerLevel <= GetOwner()->GetLevel())
				study_max_level = (GetOwner()->GetLevel() - skillEntry->playerLevel) / skillEntry->upLevelInterval + 1;

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
	GetSkillBar().PushFixGrid(1, skill_gird_vec);
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
	GetSkillBar().ClearNotValidEntry();
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

	UInt32 tempPageCnt, tempCurrentPage;
	stream & tempPageCnt & tempCurrentPage;
	std::vector<std::vector<Skill>> skills;
	stream & skills;

	for (auto &skillPage : skills)
	{
		for (auto &skill : skillPage)
		{
			if (skill.Init())
			{
				m_Skills.insert(std::make_pair(skill.id, skill));
			}
			
		}
	}

	//Skill skill;
	//stream & skill;

	//while(skill.id != 0)
	//{
	//	if(skill.Init()) m_Skills.insert(std::make_pair(skill.id, skill));

	//	stream & skill;
	//}
}

void SkillMgr::Output(Avalon::NetOutputStream& stream)
{
	stream & static_cast<UInt32>(1) & static_cast<UInt32>(0);
	stream & static_cast<UInt16>(1);

	class EncodeVisitor : public SkillVisitor
	{
	public:
		EncodeVisitor(std::vector<Skill>& skills)
			:m_Skills(skills) {}

		bool operator()(Skill* skill)
		{
			if (!IsSkillNeedSyncNet(skill->dataEntry->type)) return true;

			m_Skills.push_back(*skill);

			return true;
		}
	private:
		std::vector<Skill>& m_Skills;
	};

	std::vector<Skill> skills;
	EncodeVisitor visitor(skills);
	VisitSkills(visitor, true);
	stream & skills;
}

void SkillMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & static_cast<UInt32>(1) & static_cast<UInt32>(0);
	stream & static_cast<UInt16>(1);

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
	UInt32 tempPageCnt, tempCurrentPage;
	stream & tempPageCnt & tempCurrentPage;
	std::vector<std::vector<Skill>> skills;
	stream & skills;

	for (auto &skillPage : skills)
	{
		for (auto &skill : skillPage)
		{
			if (skill.Init())
			{
				m_Skills.insert(std::make_pair(skill.id, skill));
			}
		}
	}

	//Skill skill;
	//stream & skill.id;

	//while(skill.id != 0)
	//{
	//	stream & skill.level;

	//	if(skill.Init())
	//	{
	//		m_Skills.insert(std::make_pair(skill.id, skill));
	//	}

	//	stream & skill.id;
	//}
}

void SkillMgr::DecodeString(std::istringstream& is)
{
	Skill skill;
	char split = 0;
	int level = 0;

	while((is >> skill.id) && skill.id != 0)
	{
		is >> split >> level >> split;

		skill.level = level;
		if(skill.Init())
		{
			//skill.usetime = m_pOwner->GetSavebackTime() + cdtime;
			m_Skills.insert(std::make_pair(skill.id,skill));
		}
	}
}

void SkillMgr::EncodeString(std::ostringstream& os)
{
	class EncodeStringVisitor : public SkillVisitor
	{
	public:
		EncodeStringVisitor(std::ostringstream& _os):ostr(_os){}

		bool operator()(Skill* skill)
		{
			if(!IsSkillNeedSyncDB(skill->dataEntry->type)) return true;	//普通物理攻击不同步

			//ostr << skill->id << ',' << int(skill->level) << ',' << skill->GetLeftCD() << ';';
			ostr << skill->id << ',' << int(skill->level) << ';';
			return true;
		}
	private:
		std::ostringstream& ostr;
	};
	EncodeStringVisitor visitor(os);
	VisitSkills(visitor);
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

void SkillMgr::StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl)
{
	auto iter = m_Skills.find(skillId);
	if (iter != m_Skills.end())
	{
		iter->second.level = skillLvl;
		iter->second.bdirty = true;

		SetDirty();
		GetOwner()->SyncProperty();
	}
	else
	{
		Skill addSkill;
		addSkill.id = skillId;
		addSkill.level = skillLvl;
		addSkill.bdirty = true;

		if (addSkill.Init())
		{
			m_Skills.insert(std::make_pair(addSkill.id, addSkill));
			DebugLogStream << "player(" << PLAYERINFO(GetOwner()) << ") add skill(" << 
				addSkill.id << ", " << addSkill.level << ")." << LogStream::eos;
		}
		else
		{
			ErrorLogStream << "Not Find Skill Id:" << skillId << LogStream::eos;
			return;
		}

		// 是否需要设置槽
		if (IsSetting(addSkill.dataEntry))
		{
			// 找个空槽，放上去
			std::vector<UInt16> skillIds;
			skillIds.push_back(skillId);
			GetSkillBar().PushGrid(1, skillIds);

			DebugLogStream << "player(" << PLAYERINFO(GetOwner()) << ") skill(" << 
				addSkill.id << ", " << addSkill.level << ") push to grid." << LogStream::eos;
		}

		SetDirty();
		GetOwner()->SyncProperty();
	}
}

void SkillMgr::ClearChiJiSkill()
{
	m_Skills.clear();
	GetSkillBar().ClearAllEntry();
	SetDirty();
	GetOwner()->SyncProperty();
}

void SkillMgr::SendChiJiChoiceSkill()
{
	m_ChijiSkillChoiceMap.clear();

	//最多取3个技能
	UInt32 skillNum = 3;
	UInt32 choiceNum = SSApplication::Instance()->GetChiJiChoiceSkillNum();
	UInt32 randSkillNum = choiceNum != m_ChijiBoxSkillSet.size() ? skillNum : 0;
	std::vector<UInt32> skillVec;
	ChiJiSkillDataEntryMgr::Instance()->GetSkillVec(GetOwner()->GetOccu(), randSkillNum, m_ChijiBoxSkillSet, skillVec);

	for (auto& it : m_ChijiBoxSkillSet)
	{
		auto tt = m_Skills.find(it);
		if (tt == m_Skills.end())
		{
			continue;
		}

		ChiJiSkillDataEntry* chiJiSkillData = ChiJiSkillDataEntryMgr::Instance()->FindEntry(it);
		if (chiJiSkillData == NULL || tt->second.level >= chiJiSkillData->maxLvl)
		{
			continue;
		}

		skillVec.push_back(it);
	}

	if (skillVec.empty())
	{
		((Player*)GetOwner())->SendNotify(4200014);
		DebugLogStream << PLAYERINFO(GetOwner()) << "skillVec empty!" << LogStream::eos;
		return;
	}

	std::random_shuffle(skillVec.begin(), skillVec.end());

	CLProtocol::BattleSkillChoiceListNotify notify;

	for (auto& iter : skillVec)
	{
		if (skillNum == 0) break;
		
		UInt32 level = 1;
		auto skill = m_Skills.find(iter);
		if (skill != m_Skills.end())
		{
			level = skill->second.level + 1;
		}

		m_ChijiSkillChoiceMap[iter] = level;

		CLProtocol::ChiJiSkill chijiSkill(iter, level);
		notify.chiJiSkillVec.push_back(chijiSkill);

		--skillNum;
	}

	((Player*)GetOwner())->SendProtocol(notify);
}

void SkillMgr::ChoiceChiJiSkill(UInt32 skillId)
{
	// pk不能捡技能
	if (GetOwner()->GetStatus() == CREATURE_STATUS_PK)
	{
		((Player*)GetOwner())->SendNotify(10070);
		m_ChijiSkillChoiceMap.clear();
		return;
	}

	CLProtocol::BattleChoiceSkillRes res;

	do 
	{
		auto it = m_ChijiSkillChoiceMap.find(skillId);
		if (it == m_ChijiSkillChoiceMap.end())
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		auto iter = m_Skills.find(skillId);
		if (iter != m_Skills.end())
		{
			iter->second.level = it->second;
			iter->second.bdirty = true;

			SetDirty();
			GetOwner()->SyncProperty();
		}
		else
		{
			// 直接学习新的技能
			Skill addSkill;
			addSkill.id = it->first;
			addSkill.level = it->second;
			addSkill.bdirty = true;

			if (addSkill.Init())
			{
				m_Skills.insert(std::make_pair(addSkill.id, addSkill));
				DebugLogStream << "player(" << PLAYERINFO(GetOwner()) << ") add skill(" << addSkill.id << ", " << addSkill.level << ")." << LogStream::eos;
			}
			else
			{
				ErrorLogStream << "Not Find Skill Id:" << it->first << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			// 是否需要设置槽
			if (IsSetting(addSkill.dataEntry))
			{
				// 找个空槽，放上去
				std::vector<UInt16> skillIds;
				skillIds.push_back(it->first);
				GetSkillBar().PushGrid(1, skillIds);

				DebugLogStream << "player(" << PLAYERINFO(GetOwner()) << ") skill(" << addSkill.id << ", " << addSkill.level << ") push to grid." << LogStream::eos;
			}

			SetDirty();
			GetOwner()->SyncProperty();
		}

		m_ChijiBoxSkillSet.insert(skillId);

		res.skillId = skillId;
		res.skillLvl = it->second;
		res.retCode = ErrorCode::SUCCESS;

		Sys::SendUdpLog("chiji_skill", LOG_CHIJI_SKILL, ((Player*)GetOwner())->GetBattleID(), GetOwner()->GetID(), skillId, it->second);

	} while (0);

	m_ChijiSkillChoiceMap.clear();
	((Player*)GetOwner())->SendProtocol(res);
}

bool SkillMgr::IsCanPickSkill()
{
	UInt32 choiceNum = SSApplication::Instance()->GetChiJiChoiceSkillNum();
	if (choiceNum != m_ChijiBoxSkillSet.size())
	{
		return true;
	}

	for (auto& iter : m_ChijiBoxSkillSet)
	{
		auto tt = m_Skills.find(iter);
		if (tt == m_Skills.end())
			continue;

		ChiJiSkillDataEntry* chiJiSkillData = ChiJiSkillDataEntryMgr::Instance()->FindEntry(iter);
		if (chiJiSkillData == NULL)
			continue;

		if (tt->second.level < chiJiSkillData->maxLvl)
			return true;
	}

	return false;
}