#include "Retinue.h"
#include "Player.h"

#include <CLRecordClient.h>


UInt32 ChangeSkillData::Init(RetinueSkillDataEntry* dataEntry, UInt32 minusFixNum, UInt32 missNum)
{
	if (dataEntry == NULL) return ErrorCode::RETINUE_NOT_SKILL_GROUP_ERROR;
	m_pDataEntry = dataEntry;
	m_Weight.SetWeightBase(m_pDataEntry->weight);
	m_Weight.SetPlusFix(m_pDataEntry->fix1);
	m_Weight.SetMinusFix(m_pDataEntry->fix2);
	m_Weight.SetMinusFixNumBase(m_pDataEntry->fix2Num);
	m_Weight.SetWeightUpper(m_pDataEntry->fixMax);
	m_Weight.SetWeightLower(m_pDataEntry->fixMin);
	m_Weight.SetMinusFixNum(minusFixNum);
	m_Weight.SetMissNum(missNum);
	m_Weight.SetOwner(this);
	return ErrorCode::SUCCESS;
}


bool ChangeSkillData::IsCanUsed(UInt8 starLevel, UInt32 lockCount, UInt32 haveUsed)
{
	//num为0表示无限 , 检查数量是否被消耗完
	if ((m_pDataEntry->dataNum != 0 && m_pDataEntry->dataNum <= haveUsed)
		|| (m_pDataEntry->starLevel > starLevel)
		|| (m_pDataEntry->lockSkillCount > lockCount))
		return false;
	return true;
}

ChangeSkillFakeWeights::ChangeSkillFakeWeights(){}
ChangeSkillFakeWeights::~ChangeSkillFakeWeights(){}

ChangeSkillData* ChangeSkillFakeWeights::GetChangeSkillData(UInt32 id)
{
	std::map<UInt32, ChangeSkillData>::iterator iter = m_ChangeSkillDatas.find(id);
	if (iter != m_ChangeSkillDatas.end())
	{
		return &(iter->second);
	}
	return NULL;
}

ChangeSkillData* ChangeSkillFakeWeights::GetChangeSkillData(RetinueSkillDataEntry* data)
{
	if (data == NULL) return NULL;
	ChangeSkillData* result = GetChangeSkillData(data->id);

	if (result == NULL)
	{
		std::pair<std::map<UInt32, ChangeSkillData>::iterator, bool> tmpIter = m_ChangeSkillDatas.insert(std::make_pair(data->id, ChangeSkillData()));
		if (!tmpIter.second) { return NULL; }

		UInt32 nRes = tmpIter.first->second.Init(data);
		if (nRes != ErrorCode::SUCCESS) 
		{
			m_ChangeSkillDatas.erase(tmpIter.first);
			return NULL;
		}
		return &(tmpIter.first->second);
	}
	return result;
}

void ChangeSkillFakeWeights::ReadChangeSkillData(UInt32 id, UInt32 minusFixNum, UInt32 missNum)
{
	RetinueSkillDataEntry* dataEntry = RetinueSkillDataEntryMgr::Instance()->FindEntry(id);
	if (dataEntry == NULL || GetChangeSkillData(dataEntry) == NULL)
	{
		ErrorLogStream << "RetinueSkillData(" << id << ") init error is not find Data Entry." << LogStream::eos;
		return;
	}
}

void ChangeSkillFakeWeights::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_ChangeSkillDatas;
}

void ChangeSkillFakeWeights::Decode(Avalon::NetInputStream& stream)
{
	stream & m_ChangeSkillDatas;
}

void ChangeSkillFakeWeights::DecodeString(std::istringstream& is)
{
	char split = 0;
	do
	{
		UInt32 id = 0;
		UInt32 minusFixNum = 0;
		UInt32 missNum = 0;
		is >> id;
		if (id == 0) break;
		is >> split >> minusFixNum >> split >> missNum >> split;
		ReadChangeSkillData(id, minusFixNum, missNum);
	} while (true);
}

void ChangeSkillFakeWeights::EncodeString(std::ostringstream& os)
{
	std::map<UInt32, ChangeSkillData>::iterator iter = m_ChangeSkillDatas.begin();
	while (iter != m_ChangeSkillDatas.end())
	{
		if ((iter->second.GetWeight()->GetPlusFix() != 0 || iter->second.GetWeight()->GetMinusFix() != 0) &&
			(iter->second.GetWeight()->GetMinusFixNum() != 0 || iter->second.GetWeight()->GetMissNum() != 0))
		{
			os << iter->second.GetEntryId() << "," << iter->second.GetWeight()->GetMinusFixNum() << "," << iter->second.GetWeight()->GetMissNum() << ";";
		}
		++iter;
	}
}

RetinueSkillList::RetinueSkillList() {}
RetinueSkillList::~RetinueSkillList() {}

void RetinueSkillList::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_Skills;
}

void RetinueSkillList::Decode(Avalon::NetInputStream& stream)
{
	stream & m_Skills;
}

UInt32 RetinueSkillList::GetSkillCount(UInt32 id, UInt8 level) const
{
	UInt32 result = 0;
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		if (m_Skills[i].id == id || id == 0)
		{
			++result;
		}
	}
	return result;
}

void RetinueSkillList::AddSkill(RetinueSkill skill)
{
	m_Skills.push_back(skill);
	SetDirty();
}

RetinueSkill RetinueSkillList::GetRetinueSkill(UInt32 skillId)
{
	RetinueSkill result;
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		if (m_Skills[i].id == skillId)
		{
			result = m_Skills[i];
			break;
		}
	}
	return result;
}

void RetinueSkillList::ChangeSkill(std::vector<UInt32> lockSkillIds, std::vector<RetinueSkill>& newSkills, std::vector<RetinueSkill>& oldSkills, std::vector<RetinueSkill>& lockSkills)
{
	if (newSkills.size() <= 0) return;
	UInt32 maxIndex = lockSkillIds.size();
	UInt32 newSkillIndex = 0;
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		UInt32 index = 0;
		bool isLock = false;
		while (index < maxIndex)
		{
			if (maxIndex > 0 && m_Skills[i].id == lockSkillIds[index])
			{
				lockSkillIds[index] = lockSkillIds[maxIndex - 1];
				maxIndex--;
				isLock = true;
				lockSkills.push_back(m_Skills[i]);
				break;
			}
			index++;
		}

		if (!isLock && newSkillIndex < newSkills.size())
		{
			oldSkills.push_back(m_Skills[i]);
			UInt32 oldId = m_Skills[i].id;
			UInt32 oldBuffId = m_Skills[i].buffId;
			m_Skills[i].buffId = newSkills[newSkillIndex].buffId;
			m_Skills[i].id = newSkills[newSkillIndex].id;

			DebugLogStream << "RetinueSkillList::ChangeSkill oldId=" << oldId << " oldBuffId=" << oldBuffId << " id=" << m_Skills[i].id << " buffId=" << m_Skills[i].buffId << LogStream::eos;

			newSkillIndex++;
		}
	}

	if (newSkillIndex > 0)
	{
		SetDirty();
	}
}

void RetinueSkillList::GetSkillIds(std::vector<UInt32>& skillIds)
{
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		skillIds.push_back(m_Skills[i].id);
	}
}

void RetinueSkillList::GetBuffIds(std::vector<UInt32>& buffIds)
{
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		buffIds.push_back(m_Skills[i].buffId);
	}
}


void RetinueSkillList::DecodeString(std::istringstream& is)
{
	char split = 0;
	do
	{
		UInt32 id = 0;
		UInt32 skillId = 0;
		is >> id;
		if (id == 0) break;
		is >> split >> skillId >> split;
		RetinueSkill skill(id, skillId);
		m_Skills.push_back(skill);
	} while (true);
}

void RetinueSkillList::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_Skills.size(); ++i)
	{
		os << m_Skills[i].id << "," << m_Skills[i].buffId << ";";
	}
}

Retinue::Retinue(Player* player, UInt32 _dataId, ObjID_t _id, UInt8 _level, UInt8 _starLevel, std::string _skillStr) : m_pOwner(player)
{
	SetID(_id);
	m_DataId.SetDBValue(_dataId);
	m_Level.SetDBValue(_level);
	m_StarLevel.SetDBValue(_starLevel);
	m_Skills.SetDBValue(_skillStr);
	m_pDataEntry = NULL;
	m_pLevelDataEntry = NULL;
}

Retinue::Retinue(Player* player, UInt32 _dataId) : Retinue(player, _dataId, 0, 1, 1, std::string())
{
}

Retinue::~Retinue()
{
}

UInt32 Retinue::Init()
{
	if (m_pOwner == NULL) return ErrorCode::RETINUE_NOT_PLAYER;
	m_pDataEntry = RetinueDataEntryMgr::Instance()->FindEntry(m_DataId);
	if (m_pDataEntry == NULL) return ErrorCode::RETINUE_DATA_NOT_EXIST;
	if (m_pOwner->GetLevel() < m_pDataEntry->level) return ErrorCode::RETINUE_PLAYER_LEVEL;

	m_pLevelDataEntry = RetinueLevelDataEntryMgr::Instance()->FindEntry(CL_MAKE_RETINUE_LEVEL_KEY(m_Level, m_pDataEntry->quality));
	if (m_pLevelDataEntry == NULL) return ErrorCode::RETINUE_LEVEL_DATA_NOT_EXIST;

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::InitSkill()
{
	std::vector<UInt32> skillIds;
	m_Skills.GetSkillIds(skillIds);
	return RandSkills(skillIds);
}

UInt32 Retinue::ChangeSkill(std::vector<UInt32> skillIds)
{
	if (m_pDataEntry->changeSkillUnlock > GetOwner()->GetLevel()) return ErrorCode::RETINUE_PLAYER_LEVEL;
	if (m_Skills.GetSkills().size() < skillIds.size()) return ErrorCode::RETINUE_NOT_CHANGE_SKILL_ERROR;

	std::vector<UnlockItem> unlockItems;
	m_pDataEntry->GetChangeSkillConsume(unlockItems, skillIds.size());

	for (UInt32 i = 0; i < unlockItems.size(); ++i)
	{
		if (GetOwner()->GetItemNum(unlockItems[i].itemId) < unlockItems[i].num)
			return ErrorCode::RETINUE_NOT_ITEM;
	}

	UInt32 nRes = RandSkills(skillIds);

	if (nRes == ErrorCode::SUCCESS)
	{
		std::string reason = GetReason(SOURCE_TYPE_RETINUE_CHANGE_SKILL, GetID(), GetDataId());
		for (UInt32 i = 0; i < unlockItems.size(); ++i)
		{
			GetOwner()->RemoveItem(reason.c_str(), unlockItems[i].itemId, unlockItems[i].num);
		}
	}
	return nRes;
}

UInt32 Retinue::TestChangeSkill(std::vector<UInt32> skillIds)
{
	std::vector<RetinueSkill> retinueSkills;
	std::vector<RetinueSkill> oldSkills;
	std::vector<RetinueSkill> lockSkills;

	//检查是否能够洗练出来


	for (UInt32 i = 0; i < skillIds.size(); ++i)
	{
		RetinueSkillDataEntry* dataEntry = RetinueSkillDataEntryMgr::Instance()->FindEntry(skillIds[i]);
		if (dataEntry == NULL) continue;
		RetinueSkill skill;
		skill.buffId = dataEntry->dataId;
		skill.id = dataEntry->id;
		retinueSkills.push_back(skill);
	}

	m_Skills.ChangeSkill(std::vector<UInt32>(), retinueSkills, oldSkills, lockSkills);

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::UnlockConsumeItem()
{
	UnlockItem unlockItem;
	if (!m_pDataEntry->GetUnlockItem(unlockItem)) return ErrorCode::RETINUE_STAR_LEVEL_NOT_EXIST;
	UInt32 num = m_pOwner->GetItemNum(unlockItem.itemId);
	if (unlockItem.num > num) return ErrorCode::RETINUE_UNLOCK_NOT_ITEM;

	std::string reason = GetReason(SOURCE_TYPE_RETINUE_UNLOCK, GetID(), GetDataId());
	m_pOwner->RemoveItem(reason.c_str(), unlockItem.itemId, unlockItem.num);

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::IsFollow()
{
	if (m_pOwner->GetLevel() < m_pDataEntry->level) return ErrorCode::RETINUE_PLAYER_LEVEL;
	return ErrorCode::SUCCESS;
}

void Retinue::GetRetinueInfo(RetinueInfo& info)
{
	info.id = GetID();
	info.dataId = GetDataId();
	info.skills = GetSkills();
	info.level = GetLevel();
	info.starLevel = GetStarLevel();
}

void Retinue::GetBuffIds(std::vector<UInt32>& buffIds)
{
	m_Skills.GetBuffIds(buffIds);
}

UInt32 Retinue::UpLevel()
{
	//检查有没有满级
	RetinueLevelDataEntry* levelDataEntry = RetinueLevelDataEntryMgr::Instance()->FindEntry(CL_MAKE_RETINUE_LEVEL_KEY(m_Level + 1, m_pDataEntry->quality));
	if (levelDataEntry == NULL) return ErrorCode::RETINUE_LEVEL_DATA_NOT_EXIST;

	//检查玩家等级.
	if (m_pLevelDataEntry->playerLevel > GetOwner()->GetLevel()) return ErrorCode::RETINUE_PLAYER_LEVEL;

	//检查升级消耗够不够
	UInt32 tmpNum = GetOwner()->GetItemNum(m_pDataEntry->itemId);
	//检查碎片
	if (m_pLevelDataEntry->itemNum > tmpNum) return ErrorCode::RETINUE_LEVEL_NOT_ITEM;
	for (UInt32 i = 0; i < m_pLevelDataEntry->unlockItems.size(); ++i)
	{
		if (GetOwner()->CheckRmMoney(ST_WARRIOR_SOUL, m_pLevelDataEntry->unlockItems[i].num))
			return ErrorCode::RETINUE_WARRIOR_SOUL_ERROR;
	}

	//消耗
	std::string reason = GetReason(SOURCE_TYPE_RETINUE_UPLEVEL, GetID(), GetDataId());
	//消耗碎片
	GetOwner()->RemoveItem(reason.c_str(), m_pDataEntry->itemId, m_pLevelDataEntry->itemNum);
	for (UInt32 i = 0; i < m_pLevelDataEntry->unlockItems.size(); ++i)
	{
		GetOwner()->RemoveWarriorSoul(reason.c_str(), m_pLevelDataEntry->unlockItems[i].num);
	}

	//升级
	m_Level = m_Level + 1;
	m_pLevelDataEntry = levelDataEntry;

	DebugLogStream << PLAYERINFO(GetOwner()) << "UpStar retinueid=" << GetID() << " retinueDataId=" << GetDataId() << " level=" << m_Level << LogStream::eos;

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::UpStar()
{
	if (m_StarLevel >= m_pDataEntry->maxStar) return ErrorCode::RETINUE_MAX_STAR_ERROR;
	UnlockItem unlockItem;
	if (!m_pDataEntry->GetUnlockItem(unlockItem, m_StarLevel)) return ErrorCode::RETINUE_STAR_LEVEL_NOT_EXIST;

	UInt32 itemNum = GetOwner()->GetItemNum(unlockItem.itemId);
	if (itemNum < unlockItem.num) return ErrorCode::RETINUE_UP_STAR_NOT_ITEM;

	//消耗
	std::string reason = GetReason(SOURCE_TYPE_RETINUE_UP_STAR, GetID(), GetDataId());
	GetOwner()->RemoveItem(reason.c_str(), unlockItem.itemId, unlockItem.num);

	m_StarLevel = m_StarLevel + 1;

	//初始化新支援技
	UInt32 nRes = InitSkill();
	if (nRes == ErrorCode::SUCCESS)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "UpStar retinueid=" << GetID() << " retinueDataId=" << GetDataId() << " star=" << m_StarLevel << LogStream::eos;
	}

	return nRes;
}

void Retinue::InsertRetinue()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_retinue", GetID());
	insertCommand->PutData("owner", GetOwner()->GetID());
	insertCommand->PutData("createtime", UInt32(CURRENT_TIME.Sec()));
	this->GetDBData(insertCommand, false);
	CLRecordClient::Instance()->SendCommand(insertCommand);
}

void Retinue::PackWatchData(Avalon::NetOutputStream& stream)
{
	stream & GetID() & GetDataId() & GetLevel() & GetStarLevel() & m_Skills;
}

void Retinue::SyncToDB(bool bFlush)
{
	//同步到数据库
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_retinue", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

UInt32 Retinue::RandSkills(std::vector<UInt32> skillIds)
{
	std::vector<ChangeSkillData*> skillDatas;
	UInt32 nRes = RandSkillDatas(skillDatas, m_pDataEntry->groupId, skillIds);
	if (nRes != ErrorCode::SUCCESS) return nRes;

	std::vector<RetinueSkill> newSkills;
	std::vector<RetinueSkill> oldSkills;
	std::vector<RetinueSkill> lockSkills;

	for (UInt32 i = 0; i < skillDatas.size(); ++i)
	{
		if (skillDatas[i]->GetGroupType() == RGT_BUFF)
		{
			RetinueSkill value;
			value.id = skillDatas[i]->GetEntryId();
			value.buffId = skillDatas[i]->GetDataId();
			newSkills.push_back(value);
		}
	}
	m_Skills.ChangeSkill(skillIds, newSkills, oldSkills, lockSkills);

	GetOwner()->OnRetinueChangeSkill(this, lockSkills, oldSkills, newSkills);

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::RandSkillDatas(std::vector<ChangeSkillData*>& result, UInt32 groupId, std::vector<UInt32>& skillIds, UInt32 randCount)
{
	//获取能够被随机出来的技能
	std::vector<FakeWeightBase*> fakeWeightVec;
	UInt32 nRes = GetFakeWeight(fakeWeightVec, groupId, skillIds, result);
	if (nRes != ErrorCode::SUCCESS) return nRes;

	if (randCount == 0)
	{
		//计算需要随机几个技能
		Int32 needSkillCount = GetStarLevel() - m_Skills.GetSkills().size();
		for (Int32 i = 0; i < needSkillCount; ++i)
		{
			m_Skills.AddSkill(RetinueSkill());
		}
		randCount = m_Skills.GetSkills().size() - skillIds.size();
	}

	//可能库里面随不出需要的个数
	for (UInt32 i = 0; i < randCount; ++i)
	{
		FakeWeightBase* fakeWeight = WeightRander::RandWeight(fakeWeightVec, false);
		if (fakeWeight == NULL) return ErrorCode::RETINUE_NOT_CHANGE_SKILL_ERROR;
		ChangeSkillData* randData = (ChangeSkillData*)fakeWeight->GetOwner();
		if (randData == NULL) return ErrorCode::RETINUE_NOT_CHANGE_SKILL_ERROR;

		if (randData->GetGroupType() == RGT_SOLUTION)
		{
			DebugLogStream << PLAYERINFO(GetOwner()) << "retinueId=" << GetID() << " retinueDataId=" << GetDataId()
				<< " id=" << randData->GetEntryId() << " dataId=" << randData->GetDataId() << " type=" << (UInt32)randData->GetGroupType()
				<< " vec=" << (UInt32)fakeWeightVec.size() << LogStream::eos;

			nRes = RandSkillDatas(result, randData->GetDataId(), skillIds, 1);
			if (nRes != ErrorCode::SUCCESS) return nRes;
		}
		else
		{
			DebugLogStream << PLAYERINFO(GetOwner()) << "retinueId=" << GetID() << " retinueDataId=" << GetDataId()
				<< " id=" << randData->GetEntryId() << " skill Id=" << randData->GetDataId() << " type=" << (UInt32)randData->GetGroupType()
				<< " vec=" << (UInt32)fakeWeightVec.size() << LogStream::eos;

			result.push_back(randData);
			std::vector<FakeWeightBase*>::iterator iter = fakeWeightVec.begin();
			while (iter != fakeWeightVec.end())
			{
				if (*iter == fakeWeight)
				{
					iter = fakeWeightVec.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 Retinue::GetFakeWeight(std::vector<FakeWeightBase*>& fakeWeights, UInt32 groupId, std::vector<UInt32>& skillIds, std::vector<ChangeSkillData*>& result)
{
	std::vector<RetinueSkillDataEntry*> retinueSkillVec = RetinueSkillDataEntryMgr::Instance()->GetRetinueSkills(groupId, GetOwner()->GetOccupation());

	UInt32 lockCount = skillIds.size();

	for (UInt32 i = 0; i < retinueSkillVec.size(); ++i)
	{
		//不重复随机
		bool isContinue = false;
		for (UInt32 j = 0; j < result.size(); ++j)
		{
			if (result[j]->GetEntryId() == retinueSkillVec[i]->id) isContinue = true;
		}
		if (isContinue) continue;

		ChangeSkillData* tmpData = m_ChangeSkillFakeWeights.GetChangeSkillData(retinueSkillVec[i]);
		if (tmpData == NULL) continue;

		UInt32 haveUsed = 0;
		//datanum为0表示不限定数量
		if (retinueSkillVec[i]->dataNum != 0)
		{
			haveUsed = std::count(skillIds.begin(), skillIds.end(), retinueSkillVec[i]->id);
		}

		if (tmpData->IsCanUsed(GetStarLevel(), lockCount, haveUsed))
		{
			fakeWeights.push_back(tmpData->GetWeight());
		}
	}
	return ErrorCode::SUCCESS;
}

UInt32 Retinue::CheckTestChangeSkill(std::vector<UInt32>& skillIds, UInt32 groupId)
{
	std::vector<RetinueSkillDataEntry*> retinueSkillVec = RetinueSkillDataEntryMgr::Instance()->GetRetinueSkills(groupId, GetOwner()->GetOccupation());

	for (UInt32 i = 0; i < retinueSkillVec.size(); ++i)
	{
		std::vector<UInt32>::iterator iter = skillIds.begin();
		UInt32 num = 0;
		while (iter != skillIds.end())
		{
			std::vector<UInt32>::iterator tmpIter = iter;
			++iter;

			if (*tmpIter == retinueSkillVec[i]->id)
			{
				skillIds.erase(tmpIter);
				num++;
				if (num > retinueSkillVec[i]->dataNum)
				{
					return ErrorCode::RETINUE_NOT_CHANGE_SKILL_ERROR;
				}
			}
			else if (retinueSkillVec[i]->groupType == RGT_SOLUTION)
			{
				UInt32 nRes = CheckTestChangeSkill(skillIds, retinueSkillVec[i]->dataId);
				if (nRes == ErrorCode::SUCCESS)
				{
					iter = skillIds.begin();
				}
			}
		}
	}

	return ErrorCode::SUCCESS;
}

std::string Retinue::GetUdpLogString(std::vector<RetinueSkill> skills)
{
	std::ostringstream os;
	for (UInt32 i = 0; i < skills.size(); ++i)
	{
		os << skills[i].id << "," << skills[i].buffId << ";";
	}
	return os.str();
}

