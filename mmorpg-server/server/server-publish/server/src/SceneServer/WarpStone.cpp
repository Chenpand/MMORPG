#include "Player.h"
#include "WarpStone.h"
#include <CLErrorCode.h>

WarpStone::WarpStone() :m_Id(0), m_Energy(0), m_Level(1), m_IsDirty(true), m_pOwner(NULL)
{
	m_pLevelDataEntry = NULL;
	m_pDataEntry = NULL;
}


WarpStone::WarpStone(Player* owner, UInt32 id, UInt32 level, UInt32 energy)
	:m_Id(id), m_Energy(energy), m_Level(level), m_IsDirty(true), m_pOwner(owner)
{
	m_pLevelDataEntry = NULL;
	m_pDataEntry = NULL;
}


WarpStone::~WarpStone()
{
}

bool WarpStone::Init()
{
	m_pLevelDataEntry = WarpStoneLevelDataEntryMgr::Instance()->FindEntry(m_Level);
	if (m_pLevelDataEntry == NULL) return false;
	m_pDataEntry = WarpStoneDataEntryMgr::Instance()->FindEntry(m_Id);
	if (m_pDataEntry == NULL) return false;

	//校正等级经验
	UpLevel();

	return true;
}

void WarpStone::SetOwner(Player* owner)
{
	m_pOwner = owner;
}

UInt32 WarpStone::Unlock()
{
	if (m_pDataEntry == NULL || m_pLevelDataEntry == NULL) return ErrorCode::WARP_STONE_NOT_FOUNT;
	if (GetOwner()->GetLevel() < m_pDataEntry->level) return ErrorCode::WARP_STONE_PLAYER_LEVEL_ERROR;
	if (GetOwner()->GetGoldBoth() < m_pDataEntry->gold) return ErrorCode::WARP_STONE_SILVER_ERROR;
	GetOwner()->RemoveGoldBoth(GetReason(LogSourceType::SOURCE_TYPE_WARP_STONE_UNLOCK, m_Id).c_str(), m_pDataEntry->gold);

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") WarpStone(" << m_Id
		<< ") Unlock success" << LogStream::eos;

	GetOwner()->OnWarpStoneUnlock(m_pDataEntry);

	return ErrorCode::SUCCESS;
}

UInt32 WarpStone::Charge(std::vector<EnergyStoneInfo> energyStone)
{
	UInt32 nRes = CheckCharge(energyStone);
	if (nRes != ErrorCode::SUCCESS) return nRes;

	bool isAddEnergy = false;
	std::string reason = GetReason(SOURCE_TYPE_WARP_STONE_CHARGE, m_Id);
	UInt32 size = energyStone.size();
	for (UInt32 i = 0;i < size;++i)
	{
		//消耗物品
		GetOwner()->RemoveItem(reason.c_str(), energyStone[i].dataEntry->itemId, energyStone[i].num);

		//充能 这里一次消耗掉num个...满级以后可能会有浪费.
		AddEnergy(energyStone[i].num * energyStone[i].dataEntry->energyNum);
		isAddEnergy = true;
	}
	if (isAddEnergy)
	{
		SetDirty();
	}
	return ErrorCode::SUCCESS;
}

WarpStoneType WarpStone::GetType() 
{
	if (m_pDataEntry == NULL) return WarpStoneType::STONE_INVALID;
	return m_pDataEntry->type; 
}

void WarpStone::SetLevel(UInt32 level)
{
	if (GetLevel() >= level) return;

	UInt32 nowSumEnergy = WarpStoneLevelDataEntryMgr::Instance()->GetSumEnergy(GetLevel() - 1);
	nowSumEnergy += GetEnergy();

	UInt32 sumEnergy = WarpStoneLevelDataEntryMgr::Instance()->GetSumEnergy(level - 1);

	if (sumEnergy > nowSumEnergy)
		AddEnergy(sumEnergy - nowSumEnergy);

	SetDirty();
}

UInt32 WarpStone::CheckCharge(std::vector<EnergyStoneInfo> energyStone)
{
	if (m_pLevelDataEntry->energy == 0) return ErrorCode::WARP_STONE_LEVEL_MAX;
	UInt32 size = energyStone.size();
	for (UInt32 i = 0; i < size; ++i)
	{
		if (energyStone[i].dataEntry == NULL) return ErrorCode::ENERGY_STONE_NOT_FOUNT;
		//检查能量石与次元石类型是否匹配.
		if (energyStone[i].dataEntry->type != STONE_COLORLESS && energyStone[i].dataEntry->type != m_pDataEntry->type)
			return ErrorCode::ENERGY_STONE_TYPE_ERROR;
		//检查物品是不是够
		UInt32 itemNum = GetOwner()->GetItemNum(energyStone[i].dataEntry->itemId);
		if (energyStone[i].num > itemNum) return ErrorCode::ENERGY_STONE_NOT_ENOUGH;
	}

	return ErrorCode::SUCCESS;
}

void WarpStone::UpLevel()
{
	while (m_Energy >= m_pLevelDataEntry->energy)
	{
		WarpStoneLevelDataEntry* levelDataEntry = WarpStoneLevelDataEntryMgr::Instance()->FindEntry(m_Level + 1);
		//满级
		if (levelDataEntry == NULL) break;
		m_Energy = levelDataEntry->energy == 0 ? 0 : m_Energy - m_pLevelDataEntry->energy;
		m_Level += 1;
		m_pLevelDataEntry = levelDataEntry;
	}
}

void WarpStone::AddEnergy(UInt32 energy)
{
	if (energy == 0) return;
	if (m_pLevelDataEntry->energy == 0) return;

	UInt32 oldEnergy = m_Energy;
	UInt32 oldLevel = m_Level;

	m_Energy += energy;

	UpLevel();

	if (m_Energy != oldEnergy || oldLevel != m_Level)
	{
		GetOwner()->OnWarpStoneAddEnergy(m_pDataEntry, energy, energy, oldEnergy, m_Energy, oldLevel, m_Level);

		InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") WarpStone(" << m_Id
			<< ") AddEnergy: sumEnergy" << energy << "||" << oldEnergy << "==>" << m_Energy << " || " << oldLevel << " ==>" << m_Level << " succeed!!" << LogStream::eos;
	}
}