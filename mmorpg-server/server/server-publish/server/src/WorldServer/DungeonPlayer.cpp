#include "DungeonPlayer.h"
#include <CLItemDataEntry.h>

DungeonPlayer::DungeonPlayer()
{
    m_isLeave = false;
    m_hasRaceEnd = false;
    m_curAreaId = 0;
    m_monsterExp = 0;
    m_usedTime = 0;
    m_remainHp = 0;
    m_remainMp = 0;
	m_hasGotRaceEndDrop = false;
	m_dailyCount = 0;
	m_reviveCount = 0;
	m_accId = 0;
	m_zoneId = 0;
	m_goldDropNum = 0;
	m_totalCount = 0;
	m_Score = LevelScore::LS_C;
	m_IsAssist = false;
	m_IsRacing = false;
	m_IsGenGuaranteeDrop = false;
	m_bindGoldDropNum = 0; 
	m_cheat = false;
	m_speedCheat = false;
	m_addedDropNum = 0;
	m_isVeteranReturn = false;
	m_isRecordDailyDropNum = false;
	m_BoMonthCard = false;
	m_BoGiftRightCard = false;
}

DungeonPlayer::~DungeonPlayer()
{

}

PlayerDropItem* DungeonPlayer::FindBossDropItem(UInt32 id)
{
	for (auto& dropItem : m_bossDrop)
	{
		if (dropItem.id == id)
		{
			return &dropItem;
		}
	}

	return NULL;
}

UInt32 DungeonPlayer::GetExtra()
{
	std::bitset<32> tempBitSet;
	if (GetBoMonthCard())
	{
		tempBitSet.set(DIE_MONTH_CARD);
	}
	
	if (GetBoGiftRightCard())
	{
		tempBitSet.set(DIE_GIFT_RIGHT_CARD);
	}
	return tempBitSet.to_ulong();
}

void DungeonPlayer::IncBufferItemUsedNum(UInt32 itemid)
{
	if (m_BufferItemUsedNum.find(itemid) == m_BufferItemUsedNum.end())
	{
		m_BufferItemUsedNum[itemid] = 1;
		return;
	}

	m_BufferItemUsedNum[itemid] = m_BufferItemUsedNum[itemid] + 1;
}

UInt32 DungeonPlayer::GetBufferItemUsedNum(UInt32 itemid)
{
	if (m_BufferItemUsedNum.find(itemid) == m_BufferItemUsedNum.end())
	{
		return 0;
	}

	return m_BufferItemUsedNum[itemid];
}

void DungeonPlayer::OnEnterArea(UInt32 areaId)
{
    if (m_curAreaId != 0)
    {
        OnLeaveArea(m_curAreaId);
    }

	if (!HasEnteredArea(areaId))
	{
		m_enteredAreaIds.push_back(areaId);
	}
    
    m_curAreaId = areaId;
	SetEnterAreaTime((UInt32)CURRENT_TIME.Sec());
}

void DungeonPlayer::OnLeaveArea(UInt32 areaId)
{
	m_clearedAreaIds.insert(m_curAreaId);
    m_curAreaId = 0;
}

void DungeonPlayer::SetGotAreaReward(UInt32 areaId)
{
	if (!HasGotAreaReward(areaId))
	{
		m_gotRewardAreaIds.push_back(areaId);
	}
}

bool DungeonPlayer::HasGotAreaReward(UInt32 areaId)
{
    return std::find(m_gotRewardAreaIds.begin(), m_gotRewardAreaIds.end(), areaId) != m_gotRewardAreaIds.end();
}

bool DungeonPlayer::HasEnteredArea(UInt32 areaId)
{
    return std::find(m_enteredAreaIds.begin(), m_enteredAreaIds.end(), areaId) != m_enteredAreaIds.end();
}

bool DungeonPlayer::HasClearedArea(UInt32 areaId)
{
	return m_clearedAreaIds.find(areaId) != m_clearedAreaIds.end();
}

void DungeonPlayer::OnRaceEnd(LevelScore score)
{
    OnLeaveArea(GetCurAreaID());
    m_hasRaceEnd = true;
	m_Score = score;
}

void DungeonPlayer::OnKillMonster(UInt32 monsterId, UInt32 exp)
{
    if (HasKilledMonster(monsterId))
    {
        return;
    }

    m_killedMonster.insert(monsterId);
    m_monsterExp += exp;
}

bool DungeonPlayer::HasKilledMonster(UInt32 monsterId)
{
    return m_killedMonster.find(monsterId) != m_killedMonster.end();
}

UInt32 DungeonPlayer::GetDungenAdditionByType(DungeonAdditionRateType type)
{
	if (type >= DART_NUM)
	{
		ErrorLogStream << "DungeonAdditionRateType was over flow!" << LogStream::eos;
		return 0;
	}

	return m_addition.additionRate[type];
}

bool DungeonPlayer::SetDungenAdditionByType(DungeonAdditionRateType type, UInt32 additionRate)
{
	if (type >= DART_NUM)
	{
		ErrorLogStream << "DungeonAdditionRateType was over flow!" << LogStream::eos;
		return false;
	}
	m_addition.additionRate[type] = additionRate;
	return true;
}

void DungeonPlayer::AddActivityDoubleDropInfo(UInt32 activityMonsterId, std::set<UInt32>& dropItems, UInt32 doubleRate)
{
	ActivityMonsterDropAddition info;
	info.dropItems = dropItems;
	info.doubleRate = doubleRate;
	m_activityDoubleDropInfo[activityMonsterId] = info;
}

UInt32 DungeonPlayer::GetActivityDoubleDropRate(UInt32 activityMonsterId, UInt32 dropItem)
{
	auto itr = m_activityDoubleDropInfo.find(activityMonsterId);
	if (itr == m_activityDoubleDropInfo.end())
	{
		return 0;
	}

	auto& info = itr->second;
	if (info.dropItems.find(dropItem) == info.dropItems.end())
	{
		return 0;
	}
	
	return info.doubleRate;
}

bool DungeonPlayer::HasGoldTitle() const
{
	for (const auto& equip : m_raceInfo.equips)
	{
		auto data = ItemDataEntryMgr::Instance()->FindEntry(equip.id);
		if (data && data->thirdType == IT_ITEM_GOLD_TITLE)
		{
			return true;
		}
	}

	return false;
}