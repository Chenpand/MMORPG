#include "PetMgr.h"
#include "Player.h"
#include <CLPetProtocol.h>
#include <CLPetDataEntry.h>
#include <CLErrorCode.h>
#include <CLSystemValueDataEntry.h>
#include <CLSceneDefine.h>

BattlePetMap::BattlePetMap()
{
}

BattlePetMap::~BattlePetMap()
{}

UInt32 BattlePetMap::SetBattlePet(PetType petType, UInt64 petId)
{
	if (petType >= PetType::PET_TYPE_NUM || petType <= PetType::PET_TYPE_INVALID) return ErrorCode::PET_SOLT_NOT_EXIST;
	m_BattlePetMap[petType] = petId;
	SetDirty();
	return ErrorCode::SUCCESS;
}

std::vector<UInt64> BattlePetMap::GetBattlePetList()
{
	std::vector<UInt64> result;
	std::map<PetType, UInt64>::iterator iter = m_BattlePetMap.begin();
	while (iter != m_BattlePetMap.end())
	{
		if (iter->second != 0)
		{
			result.push_back(iter->second);
		}
		++iter;
	}
	return result;
}

UInt64 BattlePetMap::GetBattlePetId(PetType petType)
{
	if (petType >= PetType::PET_TYPE_NUM || petType <= PetType::PET_TYPE_INVALID) return 0;
	std::map<PetType, UInt64>::iterator iter = m_BattlePetMap.find(petType);
	if (iter != m_BattlePetMap.end())
	{
		return iter->second;
	}
	return 0;
}

void BattlePetMap::DecodeString(std::istringstream& is)
{
	char split = 0;
	while (true)
	{
		UInt32 key = 0;
		UInt64 value = 0;
		is >> key;
		if (key == 0) break;
		is >> split >> value >> split;
		m_BattlePetMap[(PetType)key] = value;
	}
}

void BattlePetMap::EncodeString(std::ostringstream& os)
{
	std::map<PetType, UInt64>::iterator iter = m_BattlePetMap.begin();
	while (iter != m_BattlePetMap.end())
	{
		UInt32 key = iter->first;
		UInt64 value = iter->second;
		os << key << "," << value << ";";
		++iter;
	}
}


PetMgr::PetMgr()
{
	m_PetStartTime = 0;
	m_BattlePetRestSec = 0;
	m_BattlePetBattleSec = 0;
	m_BattlePetRestStartTime = 0;
	m_BattlePetBattleStartTime = 0;
	m_BattlePetRestIntervalSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_REST_INTERVAL_SEC);
	m_BattlePetBattleIntervalSec = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_BATTLE_INTERVAL_SEC);
	m_PetIntervalSec = m_BattlePetRestIntervalSec;
	
}

PetMgr::~PetMgr()
{
	Clear();
}

void PetMgr::Clear()
{
	for (PetMap::iterator iter = m_PetMap.begin(); iter != m_PetMap.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_PetMap.clear();
}

void PetMgr::GetRacePets(std::vector<RacePet>& pets)
{
	std::vector<UInt64> petIds = m_BattlePetMap.GetBattlePetList();
	for (UInt32 i = 0; i < petIds.size(); ++i)
	{
		Pet* pet = FindPet(petIds[i]);
		if (pet == NULL) continue;

		RacePet racePet;
		racePet.m_DataId = pet->GetDataId();
		racePet.m_Hunger = pet->GetHunger();
		racePet.m_Level = pet->GetLevel();
		racePet.m_SkillIndex = pet->GetSkillIndex();
		pets.push_back(racePet);
	}
}

void PetMgr::GetPetBaseInfos(std::vector<PetBaseInfo>& pets)
{
	std::vector<UInt64> petIds = m_BattlePetMap.GetBattlePetList();
	for (UInt32 i = 0; i < petIds.size(); ++i)
	{
		Pet* pet = FindPet(petIds[i]);
		if (pet == NULL) continue;

		PetBaseInfo petBaseInfo;
		petBaseInfo.m_DataId = pet->GetDataId();
		petBaseInfo.m_Level = pet->GetLevel();
		petBaseInfo.m_Hunger = pet->GetHunger();
		petBaseInfo.m_SkillIndex = pet->GetSkillIndex();
		pets.push_back(petBaseInfo);
	}
}

Pet* PetMgr::FindPet(ObjID_t id)
{
	PetMap::iterator iter = m_PetMap.find(id);
	if (iter == m_PetMap.end())
		return NULL;
	return iter->second;
}

UInt32 PetMgr::AddPet(UInt32 itemId)
{
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (itemData == NULL) return ErrorCode::PET_EGG_DATA_NOT_EXIST;
	if (GetOwner()->GetLevel() < itemData->needLevel) return ErrorCode::PET_PLAYER_LEVEL;

	PetDataEntry* petData = PetDataEntryMgr::Instance()->GetPetByEgg(itemId);
	if (petData == NULL) return ErrorCode::PET_DATA_NOT_EXIST;

	UInt32 maxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_MAX_NUM);
	if (GetPetSize() >= maxNum) return ErrorCode::PET_MAX_NUM_ERROR;

	UInt32 nRes = ErrorCode::SUCCESS;
	UInt16 maxHungry = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_HUNGER_MAX_NUM);
	Pet* pet = new Pet(m_pOwner, petData->id, maxHungry);
	nRes = pet->Init();

	if (nRes != ErrorCode::SUCCESS)
	{
		CL_SAFE_DELETE(pet);
		ErrorLogStream << PLAYERINFO(GetOwner()) << " AddPet(" << itemId << ") error: " << nRes << LogStream::eos;
		return nRes;
	}

	pet->InsertPet();

	m_PetMap.insert(std::make_pair(pet->GetID(), pet));

	pet->SyncProperty(false);

	InfoLogStream << PLAYERINFO(GetOwner()) << " AddPet(" << itemId << ", " << pet->GetID() << ") success " << nRes << LogStream::eos;
	
	GetOwner()->OnAddPet(pet->GetType(), pet->GetQuality());
	GetOwner()->SendPetAddUdpLog(pet->GetID(), pet->GetDataId(), pet->GetItemId(), pet->GetType(), pet->GetQuality());

	return nRes;

}

UInt32 PetMgr::SetPetSlot(PetType petType, UInt64 petId)
{
	UInt64 oldPetId = m_BattlePetMap.GetBattlePetId(petType);
	if (oldPetId == petId) return ErrorCode::SUCCESS;

	UInt32 nRes = ErrorCode::SUCCESS;
	UInt32 petDataId = 0;
	if (petId != 0)
	{
		Pet* pet = FindPet(petId);
		if (pet == NULL) return ErrorCode::PET_NOT_EXIST;
		if (pet->GetType() != petType) return ErrorCode::PET_SOLT_TYPE_ERROR;
		petDataId = pet->GetDataId();
	}

	nRes = m_BattlePetMap.SetBattlePet(petType, petId);

	if (nRes == ErrorCode::SUCCESS)
	{
		if (oldPetId == m_FollowPetId)
		{
			//卸下的宠物是跟随的宠物
			SetPetFollow(petId);
		}

		UInt32 oldDataId = 0;
		if (oldPetId != 0)
		{
			Pet* oldPet = FindPet(oldPetId);
			if (oldPet != NULL)
			{
				oldDataId = oldPet->GetDataId();
			}
		}

		GetOwner()->SendPetBattleUdpLog(petId, petDataId, oldPetId, oldDataId);
	}

	return nRes;
}

UInt32 PetMgr::GetPetSize(PetType petType, PetQuality petQuality, UInt16 level)
{
	UInt32 nRes = 0;
	PetMap::iterator iter = m_PetMap.begin();
	while (iter != m_PetMap.end())
	{
		Pet* pet = iter->second;
		++iter;

		if (pet == NULL) continue;

		if (petType != PetType::PET_TYPE_INVALID && petType != pet->GetType())
			continue;

		if (petQuality != PetQuality::PET_QUALITY_INVALID && petQuality != pet->GetQuality())
			continue;

		if (level != 0 && pet->GetLevel() < level)
			continue;

		nRes++;
	}
	return nRes;
}

UInt32 PetMgr::FeedPet(ObjID_t id, PetFeedType petFeedType, UInt8& isCritical, UInt32& value)
{
	Pet* pet = FindPet(id);
	if (pet == NULL) return ErrorCode::PET_NOT_EXIST;

	FeedItem feedItem;
	if (PetFeedType::PET_FEED_GOLD == petFeedType)
	{
		if (pet->IsFullLevel())
		{
			return ErrorCode::PET_FEED_LEVEL_FULL;
		}
		UInt32 goldFeedMaxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_GOLD_FEED_MAX);
		if (pet->GetGoldFeedCount() >= goldFeedMaxNum)
		{
			return ErrorCode::PET_FEED_GOLD_NUM_ERROR;
		}

		if (!PetFeedDataEntryMgr::Instance()->GetFeedItem(petFeedType, pet->GetGoldFeedCount(), feedItem))
		{
			return ErrorCode::PET_FEED_INDEX_ERROR;
		}
		if (GetOwner()->GetItemNum(feedItem.itemId) < feedItem.itemNum)
		{
			return ErrorCode::PET_FEED_GOLD_ERROR;
		}
		GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_PET_FEED).c_str(), feedItem.itemId, feedItem.itemNum);
		value = feedItem.value;
		pet->IncExp(value);
		pet->IncGoldFeedCount();
		pet->SyncProperty();
		
	}
	else if (PetFeedType::PET_FEED_POINT == petFeedType)
	{
		// 安全锁开启，不允许消耗点券喂养
		if (!GetOwner()->SecurityLockCheck())
		{
			return ErrorCode::SECURITY_LOCK_FORBID_OP;
		}

		if (pet->IsFullLevel())
		{
			return ErrorCode::PET_FEED_LEVEL_FULL;
		}
		UInt32 pointFeedMaxNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_POINT_FEED_MAX);
		if (pet->GetPointFeedCount() >= pointFeedMaxNum)
		{
			return ErrorCode::PET_FEED_POINT_NUM_ERROR;
		}

		if (!PetFeedDataEntryMgr::Instance()->GetFeedItem(petFeedType, pet->GetPointFeedCount(), feedItem))
		{
			return ErrorCode::PET_FEED_INDEX_ERROR;
		}
		if (GetOwner()->GetItemNum(feedItem.itemId) < feedItem.itemNum)
		{
			return ErrorCode::PET_FEED_POINT_ERROR;
		}
		GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_PET_FEED).c_str(), feedItem.itemId, feedItem.itemNum);

		//点卷可能暴击
		UInt32 odds = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_POINT_FEED_CRITICAL_CHANCE);
		UInt32 criticalNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_POINT_FEED_CRITICAL_NUM);
		value = feedItem.value;

		UInt32 rand = Avalon::Random().RandBetween(1, 100);
		if (rand <= odds)
		{
			value = value * criticalNum;
			isCritical = 1;
		}

		pet->IncExp(value);
		pet->IncPointFeedCount();
		pet->SyncProperty();
	}
	else if (PetFeedType::PET_FEED_ITEM == petFeedType)
	{
		if (pet->IsFullHunger())
		{
			return ErrorCode::PET_FEED_HUNGER_FULL;
		}
		if (!PetFeedDataEntryMgr::Instance()->GetFeedItem(petFeedType, 0, feedItem))
		{
			return ErrorCode::PET_FEED_INDEX_ERROR;
		}
		if (GetOwner()->GetItemNum(feedItem.itemId) < feedItem.itemNum)
		{
			return ErrorCode::PET_FEED_ITEM_ERROR;
		}

		GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_PET_FEED).c_str(), feedItem.itemId, feedItem.itemNum);
		value = feedItem.value;
		pet->AddHunger(value);
		pet->SyncProperty();
	}
	else
	{
		return ErrorCode::PET_FEED_TYPE_ERROR;
	}

	GetOwner()->OnFeedPet(petFeedType);

	return ErrorCode::SUCCESS;
}

UInt32 PetMgr::DevourPet(ObjID_t id, std::vector<ObjID_t> petIds, UInt32& exp)
{
	Pet* pet = FindPet(id);
	if (pet == NULL) return ErrorCode::PET_NOT_EXIST;

	UInt32 nRes = ErrorCode::SUCCESS;
	exp = 0;
	bool isSync = false;
	for (UInt32 i = 0; i < petIds.size(); ++i)
	{
		if (petIds[i] == id || IsBattlePet(petIds[i]))
			continue;

		if (pet->IsFullLevel())
		{
			nRes = ErrorCode::PET_IS_FULL_LEVEL;
			break;
		}

		Pet* beDevourPet = FindPet(petIds[i]);
		if (beDevourPet == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " DevourPet(" << petIds[i] << ") is not exist " << nRes << LogStream::eos;
			nRes = ErrorCode::PET_NOT_EXIST;
			
			break;
		}

		UInt32 devourExp = beDevourPet->GetDevourExp();
		UInt32 devourGold = devourExp * SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_DEVOUR_GOLD_RATIO);

		if (GetOwner()->GetGold() < devourGold) return ErrorCode::PET_DEVOUR_GOLD_ERROR;

		GetOwner()->RemoveGold(GetReason(LogSourceType::SOURCE_TYPE_PET_DEVOUR, beDevourPet->GetDataId()).c_str(), devourGold);

		UInt32 oldExp = pet->GetExp();
		UInt32 oldLevel = pet->GetLevel();

		pet->IncExp(devourExp);
		exp += devourExp;

		GetOwner()->SendPetDevourUdpLog(pet->GetID(), pet->GetDataId(), oldLevel, pet->GetLevel(), oldExp, pet->GetExp(), devourExp,
			beDevourPet->GetID(), beDevourPet->GetDataId(), beDevourPet->GetLevel(), beDevourPet->GetExp());

		DeletePet(beDevourPet->GetID());

		isSync = true;

		InfoLogStream << PLAYERINFO(GetOwner()) << " DevourPet(" << petIds[i] << ") IncExp(" << devourExp << ") Gold(" << devourGold << ") " << nRes << LogStream::eos;
	}
	
	if (isSync)
	{
		pet->SyncProperty();
	}

	return nRes;
}

UInt32 PetMgr::SellPet(ObjID_t id)
{
	Pet* pet = FindPet(id);
	if (pet == NULL) return ErrorCode::PET_NOT_EXIST;

	std::vector<ItemReward> sellReward = pet->GetSellReward();

	UInt32 result = ErrorCode::SUCCESS;
	result = DeletePet(id);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}

	RewardGroup* rewards = GetOwner()->GetRewardGroup();
	rewards->AddRewards(sellReward);
	result = GetOwner()->CheckAddRewards(rewards);
	if (result != ErrorCode::SUCCESS)
	{
		return result;
	}
	GetOwner()->AddRewards(GetReason(SOURCE_TYPE_PET_SELL).c_str(), rewards, true);

	return ErrorCode::SUCCESS;
}

UInt32 PetMgr::ChangePetSkill(ObjID_t id, UInt8 skillIndex)
{
	Pet* pet = FindPet(id);
	if (pet == NULL) return ErrorCode::PET_NOT_EXIST;

	UInt8 oldIndexSkill = pet->GetSkillIndex();

	UInt32 nRes = pet->SetSkillIndex(skillIndex);
	if (nRes == ErrorCode::SUCCESS)
	{
		pet->SyncProperty();

		GetOwner()->SendPetChangeSkillUdpLog(pet->GetID(), pet->GetDataId(), oldIndexSkill, pet->GetSkillIndex());
	}

	return nRes;
}

UInt32 PetMgr::SetPetFollow(ObjID_t id)
{
	Pet* pet = NULL;
	if (id != 0)
	{
		pet = FindPet(id);
		if (pet == NULL)
		{
			return ErrorCode::PET_NOT_EXIST;
		}
	}

	m_FollowPetId = id;
	SyncScenePet(pet);

	return ErrorCode::SUCCESS;
}

void PetMgr::OnOnline(bool bDayChanged)
{
	if (bDayChanged)
	{
		ResetFeedCount();
	}

	Pet* pet = FindPet(m_FollowPetId);
	if (pet != NULL)
	{
		SyncScenePet(pet);
	}

	//检查出战宠物
	InitBattlePet();

	//下发宠物数据
	SyncPetList();

	m_PetStartTime = (UInt32)(CURRENT_TIME.Sec());

	if (bDayChanged)
	{
		SendPetLoginUdpLog();
	}
}

void PetMgr::OnOffline()
{
	m_PetStartTime = 0;
	m_BattlePetRestStartTime = 0;
	m_BattlePetBattleStartTime = 0;
	m_BattlePetRestSec = 0;
	m_BattlePetBattleSec = 0;
}

void PetMgr::OnLogicDayChanged()
{
	ResetFeedCount();
}

void PetMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	if (callback == NULL) return;

	while (callback->NextRow())
	{
		ObjID_t id = callback->GetKey();
		UInt32 dataId = callback->GetData("dataid");
		UInt16 level = callback->GetData("level");
		UInt32 exp = callback->GetData("exp");
		UInt16 hunger = callback->GetData("hunger");
		UInt8 skillIndex = callback->GetData("skill_index");
		UInt8 goldFeedCount = callback->GetData("gold_feed_count");
		UInt8 pointFeedCount = callback->GetData("point_feed_count");
		UInt8 selectSkillCount = callback->GetData("select_skill_count");
		Pet* pet = new Pet(GetOwner(), dataId, id, level, exp, hunger, skillIndex, goldFeedCount, pointFeedCount, selectSkillCount);
		UInt32 nRes = pet->Init();
		if (nRes != ErrorCode::SUCCESS)
		{
			CL_SAFE_DELETE(pet);
			continue;
		}

		nRes = LoadPet(pet);
		if (nRes != ErrorCode::SUCCESS)
		{
			CL_SAFE_DELETE(pet);
			continue;
		}
	}
}

void PetMgr::OnRemoveFatigue(UInt16 fatigue)
{
	std::vector<UInt64> battlePetList = m_BattlePetMap.GetBattlePetList();
	for (UInt32 i = 0; i < battlePetList.size(); ++i)
	{
		Pet* pet = FindPet(battlePetList[i]);
		if (pet == NULL) continue;
		pet->OnRemoveFatigue(fatigue);
	}
}

void PetMgr::OnEnterScene(UInt32 type)
{
	UInt32 nowSec = (UInt32)(CURRENT_TIME.Sec());
	if (IsPetRestScene(type))
	{
		if (m_BattlePetRestStartTime == 0)
		{
			m_BattlePetRestStartTime = nowSec;
			m_BattlePetBattleStartTime = 0;
		}
	}
	else
	{
		if (m_BattlePetBattleStartTime == 0)
		{
			m_BattlePetRestStartTime = 0;
			m_BattlePetBattleStartTime = nowSec;
		}
	}
}

void PetMgr::OnEnterDungeonRace()
{
	UInt32 nowSec = (UInt32)(CURRENT_TIME.Sec());
	m_BattlePetRestStartTime = 0;
	m_BattlePetBattleStartTime = nowSec;
}

void PetMgr::OnTick(Avalon::Time now)
{
	UInt32 intervalTime = 0;
	UInt32 nowSec = (UInt32)now.Sec();
	if (m_BattlePetRestStartTime != 0)
	{
		intervalTime = nowSec - m_BattlePetRestStartTime;
		m_BattlePetRestSec += intervalTime;
		m_BattlePetRestStartTime = nowSec;
		OnBattlePetRest();
	}

	if (m_BattlePetBattleStartTime != 0)
	{
		intervalTime = nowSec - m_BattlePetBattleStartTime;
		m_BattlePetBattleSec += intervalTime;
		m_BattlePetBattleStartTime = nowSec;
		OnBattlePetBattle();
	}

	intervalTime = nowSec - m_PetStartTime;
	if (intervalTime >= m_PetIntervalSec)
	{
		OnPetRest();
		m_PetStartTime = nowSec;
	}
}

void PetMgr::OnBattlePetRest()
{
	if (m_BattlePetRestSec >= m_BattlePetRestIntervalSec)
	{
		std::vector<UInt64> petList = m_BattlePetMap.GetBattlePetList();
		for (UInt32 i = 0; i < petList.size(); ++i)
		{
			Pet* pet = FindPet(petList[i]);
			if (pet == NULL) continue;
			if (!pet->IsFullHunger())
			{
				pet->AddHunger(1);
				pet->SyncProperty();
			}
		}
		m_BattlePetRestSec = m_BattlePetRestSec - m_BattlePetRestIntervalSec;
	}
}

void PetMgr::OnBattlePetBattle()
{
	if (m_BattlePetBattleSec >= m_BattlePetBattleIntervalSec)
	{
		/*
		UInt64 petId = m_BattlePetMap.GetBattlePetId(PetType::PET_TYPE_ATTACK);
		Pet* pet = FindPet(petId);
		if (pet != NULL && pet->GetHunger() > 0)
		{
			pet->ConsumeHunger(1);
			pet->SyncProperty();
		}
		*/

		std::vector<UInt64> petList = m_BattlePetMap.GetBattlePetList();
		for (UInt32 i = 0; i < petList.size(); ++i)
		{
			Pet* pet = FindPet(petList[i]);
			if (pet == NULL) continue;

			if (pet->GetHunger() > 0)
			{
				pet->ConsumeHunger(1);
				pet->SyncProperty();
			}
		}
		
		m_BattlePetBattleSec = m_BattlePetBattleSec - m_BattlePetBattleIntervalSec;
	}
}

void PetMgr::OnPetRest()
{
	PetMap::iterator iter = m_PetMap.begin();
	while (iter != m_PetMap.end())
	{
		PetMap::iterator tmpIter = iter;
		++iter;
		if (tmpIter->second == NULL) continue;
			
		Pet* pet = tmpIter->second;
		UInt64 petId = m_BattlePetMap.GetBattlePetId(pet->GetType());

		if (petId == pet->GetID()) continue;

		if (!pet->IsFullHunger())
		{
			pet->AddHunger(1);
			pet->SyncProperty();
		}
	}
}

void PetMgr::RemovePetHunger(UInt16 hunger)
{
	std::vector<UInt64> petIds = m_BattlePetMap.GetBattlePetList();
	for (UInt32 i = 0; i < petIds.size(); ++i)
	{
		Pet* pet = FindPet(petIds[i]);
		if (pet == NULL) continue;

		pet->ConsumeHunger(hunger);
		pet->SyncProperty();
	}
}


void PetMgr::AddPetHunger(UInt16 hunger)
{
	std::vector<UInt64> petIds = m_BattlePetMap.GetBattlePetList();
	for (UInt32 i = 0; i < petIds.size(); ++i)
	{
		Pet* pet = FindPet(petIds[i]);
		if (pet == NULL) continue;

		pet->AddHunger(hunger);
		pet->SyncProperty();
	}
}

UInt32 PetMgr::LoadPet(Pet* pet)
{
	if (pet == NULL)
	{
		return ErrorCode::PET_NOT_EXIST;
	}

	m_PetMap.insert(std::make_pair(pet->GetID(), pet));
	return ErrorCode::SUCCESS;
}

UInt32 PetMgr::DeletePet(ObjID_t id)
{
	ErrorLogStream << PLAYERINFO(GetOwner()) << "petId = " << id << LogStream::eos;

	PetMap::iterator iter = m_PetMap.find(id);
	if (iter == m_PetMap.end() || iter->second == NULL)
	{
		return ErrorCode::PET_NOT_EXIST;
	}

	Pet* pet = iter->second;
	SyncDeletePet(pet);
	m_PetMap.erase(iter);
	CL_SAFE_DELETE(pet);

	return ErrorCode::SUCCESS;
}

std::vector<PetInfo> PetMgr::GetPetList()
{
	std::vector<PetInfo> result;
	PetMap::iterator iter = m_PetMap.begin();
	while (iter != m_PetMap.end())
	{
		PetInfo info;
		iter->second->GetPetInfo(info);
		result.push_back(info);
		++iter;
	}
	return result;
}

void PetMgr::SyncPetList()
{
	CLProtocol::SceneSyncPetList protocol;
	protocol.followPetId = m_FollowPetId;
	protocol.battlePets = m_BattlePetMap.GetBattlePetList();
	protocol.petList = GetPetList();
	GetOwner()->SendProtocol(protocol);
}

void PetMgr::SyncDeletePet(Pet* pet)
{
	if (pet == NULL) return;

	CLProtocol::SceneDeletePet protocol;
	protocol.id = pet->GetID();
	GetOwner()->SendProtocol(protocol);

	pet->DeletePet();
}

void PetMgr::SyncScenePet(Pet* pet)
{
	/*
	if (pet != NULL)
	{
		m_ScenePet.SetPet(pet->GetID(), pet->GetDataId(), pet->GetLevel());
	}
	else
	{
		m_ScenePet.SetPet(0, 0, 0);
	}
	*/
	if (pet != NULL)
	{
		if (m_FollowPetDataId != pet->GetDataId())
		{
			m_FollowPetDataId = pet->GetDataId();
		}
	}
	else
	{
		m_FollowPetDataId = 0;
	}
}

bool PetMgr::IsPetRestScene(UInt32 type)
{
	switch (type)
	{
	case SCENE_TYPE_NORMAL:
	case SCENE_TYPE_GHOST:
	case SCENE_TYPE_SINGLE:
	case SCENE_TYPE_DUNGEON_ENTRY:
	case SCENE_TYPE_PK_PREPARE:
	case SCENE_TYPE_PK:
	case SCENE_TYPE_ACTIVITY:
	case SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC:
		return true;
	}
	return false;
}

void PetMgr::ResetFeedCount()
{
	//没有宠物就返回
	//if (GetPetSize() == 0) return;

	//修改数据库
	//std::string strCond = "owner=" + Avalon::StringUtil::ConvertToString(GetOwner()->GetID());
	//CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_pet", strCond.c_str());
	//updateCmd->PutData("gold_feed_count", (UInt8)0);
	//updateCmd->PutData("point_feed_count", (UInt8)0);
	//CLRecordClient::Instance()->SendCommand(updateCmd);

	//修改数据
	PetMap::iterator iter = m_PetMap.begin();
	while (iter != m_PetMap.end())
	{
		Pet* pet = iter->second;
		if (pet != NULL)
		{
			pet->ResetFeedCount();
			pet->SyncProperty();
		}
		++iter;
	}
}

void PetMgr::InitBattlePet()
{
	for (UInt32 i = PetType::PET_TYPE_ATTACK; i < PetType::PET_TYPE_NUM; ++i)
	{
		UInt64 petId = m_BattlePetMap.GetBattlePetId((PetType)i);
		Pet* pet = FindPet(petId);
		if (pet == NULL) continue;

		if (pet->GetType() != i)
		{
			m_BattlePetMap.SetBattlePet((PetType)i, 0);
		}
	}
}

bool PetMgr::IsBattlePet(UInt64 petId)
{
	for (UInt32 i = PetType::PET_TYPE_ATTACK; i < PetType::PET_TYPE_NUM; ++i)
	{
		UInt64 battlePetId = m_BattlePetMap.GetBattlePetId((PetType)i);
		if (petId == battlePetId)
		{
			return true;
		}
	}
	return false;
}

void PetMgr::SendPetLoginUdpLog()
{
	PetMap::iterator iter = m_PetMap.begin();
	while (iter != m_PetMap.end())
	{
		Pet* pet = iter->second;
		++iter;
		if (pet == NULL)
		{
			continue;
		}

		UInt32 slot = PetType::PET_TYPE_INVALID;
		if (IsBattlePet(pet->GetID()))
		{
			slot = pet->GetType();
		}
		GetOwner()->SendPetLoginUdpLog(pet->GetID(), pet->GetDataId(), pet->GetLevel(), pet->GetExp(), pet->GetType(), slot);
	}
}
