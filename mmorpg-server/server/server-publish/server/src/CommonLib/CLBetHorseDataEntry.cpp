#include "CLBetHorseDataEntry.h"
#include "CLBetHorseDefine.h"

BetHorseCfgEntry::BetHorseCfgEntry()
{
	id = 0;
	stakeMax = 0;
	needShooterNum = 0;
	refreshOddsInterval = 0;
	stakeEndTime = 0;
	adjustEndTime = 0;
	bulletID = 0;
	startStakePublic = 0;
	endStakePublic = 0;
	battleStartPublic = 0;
	openRewardPublic = 0;
	mysteryRate = 0;
	baseWinRate = 0;
	initOddsMin = 0;
	initOddsMax = 0;
}

bool BetHorseCfgEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	stakeMax = row.ReadUInt32();
	needShooterNum = row.ReadUInt32();

	refreshOddsInterval = row.ReadUInt32() * 60;
	stakeEndTime = row.ReadUInt32() * 60;
	adjustEndTime = row.ReadUInt32() * 60;

	// 每天开始时间到凌晨0相隔的秒数
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");

		for (std::string& iter : params)
		{
			UInt32 hour = std::atoi(iter.c_str());
			startTimeSet.insert(hour * 60 * 60);
		}
	}

	bulletID = row.ReadUInt32();
	startStakePublic = row.ReadUInt32();
	endStakePublic = row.ReadUInt32();
	battleStartPublic = row.ReadUInt32();
	openRewardPublic = row.ReadUInt32();

	// 押注结束提醒时间
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");

		for (std::string& iter : params)
		{
			UInt32 hour = std::atoi(iter.c_str());
			endWarnTimeSet.insert(hour * 60);
		}
	}

	mysteryRate = row.ReadUInt32();
	baseWinRate = row.ReadUInt32();
	initOddsMin = row.ReadUInt32();
	initOddsMax = row.ReadUInt32();

	if (stakeMax == 0 || needShooterNum == 0 || refreshOddsInterval == 0
		|| stakeEndTime == 0 || adjustEndTime == 0 || bulletID == 0
		|| startTimeSet.size() == 0)
	{
		ErrorLogStream << "bet horse config error !!!" << LogStream::eos;
		return false;
	}

	return true;
}

// ***********************************************

BetHorseShooterEntry::BetHorseShooterEntry()
{
	shooterId = 0;
	mysteryRate = 0;
	appearRate = 0;
	occupation = 0;
}

bool BetHorseShooterEntry::Read(Avalon::DataRow& row)
{
	shooterId = row.ReadUInt32();
	mysteryRate = row.ReadUInt32();
	appearRate = row.ReadUInt32();
	shooterName = row.ReadString();
	occupation = row.ReadUInt32();

	return true;
}

BetHorseShooterEntryMgr::BetHorseShooterEntryMgr()
{
	mysteryTotalRate = 0;
	appearTotalRate = 0;
}

bool BetHorseShooterEntryMgr::AddEntry(BetHorseShooterEntry* entry)
{
	if (entry->shooterId == 0)
		return true;

	if (!Avalon::DataEntryMgrBase<BetHorseShooterEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->mysteryRate != 0)
	{
		mysteryTotalRate += entry->mysteryRate;
		mysteryRateMap[mysteryTotalRate] = entry->shooterId;
	}

	if (entry->appearRate != 0)
	{
		appearTotalRate += entry->appearRate;
		appearRateMap[appearTotalRate] = entry->shooterId;
	}

	shooterEntryMap[entry->GetKey()] = entry;

	return true;
}

UInt32 BetHorseShooterEntryMgr::GetRandMysteryShooter(UInt32 _appearMysteryRate)
{
	UInt32 rand_num = Avalon::Random::RandBetween(0, RATE_EXPAND_NUM);
	
	// 没有随机出神秘射手
	if (rand_num > _appearMysteryRate)
		return 0;

	// 随机出具体的神秘射手
	UInt32 mystery_num = Avalon::Random::RandBetween(0, mysteryTotalRate);
	auto iter = mysteryRateMap.upper_bound(mystery_num);
	if (iter == mysteryRateMap.end())
		return 0;

	return iter->second;
}

void BetHorseShooterEntryMgr::RandShooterVec(UInt32 _randNum, std::vector<UInt32>& _shooterVec)
{
	std::set<UInt32> tmp_set;
	if ( !_shooterVec.empty())
		tmp_set.insert(*_shooterVec.begin());

	while (_shooterVec.size() < _randNum)
	{
		UInt32 rand_rate = Avalon::Random::RandBetween(0, appearTotalRate);

		auto iter = appearRateMap.upper_bound(rand_rate);
		if (iter == appearRateMap.end())
			continue;

		auto iter1 = tmp_set.find(iter->second);
		if (iter1 == tmp_set.end())
		{
			tmp_set.insert(iter->second);
			_shooterVec.push_back(iter->second);
		}
	}
}

//****************************************

BetHorseMapEntry::BetHorseMapEntry()
{
	mapId = 0;
	mapPhase = 0;
	nextMap = 0;
}

bool BetHorseMapEntry::Read(Avalon::DataRow& row)
{
	mapId = row.ReadUInt32();
	mapPhase = row.ReadUInt32();
	nextMap = row.ReadUInt32();

	return true;
}

//***************************************

BetHorseRandEntry::BetHorseRandEntry()
{
	weatherType = 0;
	weatherRate = 0;
	terrainType = 0;
	terrainRate = 0;
	statusType = 0;
	statusRate = 0;
}

bool BetHorseRandEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	weatherType = row.ReadUInt32();
	weatherRate = row.ReadUInt32();
	terrainType = row.ReadUInt32();
	terrainRate = row.ReadUInt32();
	statusType = row.ReadUInt32();
	statusRate = row.ReadUInt32();

	return true;
}

BetHorseRandEntryMgr::BetHorseRandEntryMgr()
{
	weatherTotalRate = 0;
	terrainTotalRate = 0;
	statusTotalRate = 0;
}

bool BetHorseRandEntryMgr::AddEntry(BetHorseRandEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BetHorseRandEntry>::AddEntry(entry))
	{
		return false;
	}

	// 类型或者概率为0 则不参与随机
	if (entry->weatherType != 0 && entry->weatherRate != 0)
	{
		weatherTotalRate += entry->weatherRate;
		weatherRateMap[weatherTotalRate] = entry->weatherType;
	}

	if (entry->terrainType != 0 && entry->terrainRate != 0)
	{
		terrainTotalRate += entry->terrainRate;
		terrainRateMap[terrainTotalRate] = entry->terrainType;
	}

	if (entry->statusType != 0 && entry->statusRate != 0)
	{
		statusTotalRate += entry->statusRate;
		statusRateMap[statusTotalRate] = entry->statusType;
	}

	return true;
}

UInt32 BetHorseRandEntryMgr::GetRandWeather()
{
	UInt32 weather = Avalon::Random::RandBetween(0, weatherTotalRate);

	auto iter = weatherRateMap.upper_bound(weather);
	if (iter == weatherRateMap.end())
		return WEATHER_RAIN;

	return iter->second;
}

UInt32 BetHorseRandEntryMgr::GetRandTerrain()
{
	UInt32 terrain = Avalon::Random::RandBetween(0, terrainTotalRate);

	auto iter = terrainRateMap.upper_bound(terrain);
	if (iter == terrainRateMap.end())
		return TERRAIN_MOUNTAIN;

	return iter->second;
}

UInt32 BetHorseRandEntryMgr::GetRandStatus()
{
	UInt32 status = Avalon::Random::RandBetween(0, statusTotalRate);

	auto iter = statusRateMap.upper_bound(status);
	if (iter == statusRateMap.end())
		return SHOOTER_STATUS_INSTABLE;

	return iter->second;
}

//***********************************

BetHorseShooterAttrEntry::BetHorseShooterAttrEntry()
{
	id = 0;
	shooter_1 = 0;
	shooter_2 = 0;
	winRate = 0;
}


bool BetHorseShooterAttrEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	shooter_1 = row.ReadUInt32();
	shooter_2 = row.ReadUInt32();
	winRate = row.ReadInt32();

	return true;
}


bool BetHorseShooterStatusEntryMgr::AddEntry(BetHorseShooterAttrEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->shooter_1 == 0 || entry->shooter_2 == 0)
	{
		ErrorLogStream << "bet horse ShooterStatusEntry config error !!!" << LogStream::eos;
		return false;
	}

	BetHorseShooterAttrGroup attrGroup;
	attrGroup.shooterAttr_1 = entry->shooter_1;
	attrGroup.shooterAttr_2 = entry->shooter_2;

	shooterRateMap[attrGroup] = entry->winRate;

	return true;
}

Int32 BetHorseShooterStatusEntryMgr::GetShooterStatusWinRate(UInt32 _shooterStatus_1, UInt32 _shooterStatus_2)
{
	BetHorseShooterAttrGroup group;
	group.shooterAttr_1 = _shooterStatus_1;
	group.shooterAttr_2 = _shooterStatus_2;

	auto iter = shooterRateMap.find(group);
	if (iter == shooterRateMap.end())
		return 0;

	return iter->second;
}

//*************************************

BetHorseRateEntry::BetHorseRateEntry()
{
	id = 0;
	weatherType = 0;
	weatherOccupation_1 = 0;
	weatherOccupation_2 = 0;
	weatherWinRate = 0;

	terrainType = 0;
	terrainOccupation_1 = 0;
	terrainOccupation_2 = 0;
	terrainWinRate = 0;
}

bool BetHorseRateEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	weatherType = row.ReadUInt32();
	weatherOccupation_1 = row.ReadUInt32();
	weatherOccupation_2 = row.ReadUInt32();
	weatherWinRate = row.ReadInt32();

	terrainType = row.ReadUInt32();
	terrainOccupation_1 = row.ReadUInt32();
	terrainOccupation_2 = row.ReadUInt32();
	terrainWinRate = row.ReadInt32();

	return true;
}

bool BetHorseRateEntryMgr::AddEntry(BetHorseRateEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BetHorseRateEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->weatherType != 0 && entry->weatherOccupation_1 != 0 && entry->weatherOccupation_2 != 0)
	{
		BetHorseAmbient ambient;
		ambient.ambientType = entry->weatherType;
		ambient.occupation_1 = entry->weatherOccupation_1;
		ambient.occupation_2 = entry->weatherOccupation_2;

		weatherOccupationRateMap[ambient] = entry->weatherWinRate;
	}

	if (entry->terrainType != 0 && entry->terrainOccupation_1 != 0 && entry->terrainOccupation_2 != 0)
	{
		BetHorseAmbient ambient;
		ambient.ambientType = entry->terrainType;
		ambient.occupation_1 = entry->terrainOccupation_1;
		ambient.occupation_2 = entry->terrainOccupation_2;

		terrainOccupationRateMap[ambient] = entry->terrainWinRate;
	}

	return true;
}

Int32 BetHorseRateEntryMgr::GetWeatherOccpationWinRate(UInt32 _weather, UInt32 _occupation_1, UInt32 _occupation_2)
{
	BetHorseAmbient ambient;
	ambient.ambientType = _weather;
	ambient.occupation_1 = _occupation_1;
	ambient.occupation_2 = _occupation_2;

	auto iter = weatherOccupationRateMap.find(ambient);
	if (iter == weatherOccupationRateMap.end())
		return 0;

	return iter->second;
}

Int32 BetHorseRateEntryMgr::GetTerrainOccpationWinRate(UInt32 _terrain, UInt32 _occupation_1, UInt32 _occupation_2)
{
	BetHorseAmbient ambient;
	ambient.ambientType = _terrain;
	ambient.occupation_1 = _occupation_1;
	ambient.occupation_2 = _occupation_2;

	auto iter = terrainOccupationRateMap.find(ambient);
	if (iter == terrainOccupationRateMap.end())
		return 0;

	return iter->second;
}

//**************************

bool BetHorseOccuSuppressEntryMgr::AddEntry(BetHorseShooterAttrEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<BetHorseShooterAttrEntry>::AddEntry(entry))
	{
		return false;
	}

	if (entry->shooter_1 == 0 || entry->shooter_2 == 0)
	{
		ErrorLogStream << "bet horse ShooterStatusEntry config error !!!" << LogStream::eos;
		return false;
	}

	BetHorseShooterAttrGroup attrGroup;
	attrGroup.shooterAttr_1 = entry->shooter_1;
	attrGroup.shooterAttr_2 = entry->shooter_2;

	occuSuppressMap[attrGroup] = entry->winRate;

	return true;
}

Int32 BetHorseOccuSuppressEntryMgr::GetOccuSuppressWinRate(UInt32 _occu_1, UInt32 _occu_2)
{
	BetHorseShooterAttrGroup group;
	group.shooterAttr_1 = _occu_1;
	group.shooterAttr_2 = _occu_2;

	auto iter = occuSuppressMap.find(group);
	if (iter == occuSuppressMap.end())
		return 0;

	return iter->second;
}