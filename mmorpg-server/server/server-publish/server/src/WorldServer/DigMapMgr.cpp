#include "DigMapMgr.h"
#include "CLErrorCode.h"
#include "CLSystemValueDataEntry.h"

bool DigMapMgr::Init()
{
	m_StartHour = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_REFRESH_START_HOUR);
	m_EndHour = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_REFRESH_END_HOUR);
	m_SupplySum = 18;

	class DigMapDataEntryVisitor : public Avalon::DataEntryVisitor<DigMapDataEntry>
	{
	public:
		DigMapDataEntryVisitor(std::vector<DigMap>& digMaps) :m_DigMaps(digMaps){}

		bool operator()(DigMapDataEntry* dataEntry)
		{
			DigMap digMap;
			if (digMap.Init(dataEntry))
			{
				m_DigMaps.push_back(digMap);
			}
			return true;
		}

	private:
		std::vector<DigMap>& m_DigMaps;
	};

	DigMapDataEntryVisitor visitor(m_DigMaps);
	DigMapDataEntryMgr::Instance()->Visit(visitor);

	Avalon::Date refreshDate = CURRENT_DATE;
	refreshDate.Min(0);
	refreshDate.Sec(0);
	refreshDate.MSec(0);

	if ((UInt32)refreshDate.Hour() < m_StartHour)
	{
		refreshDate.Hour(0);
		m_LastRefreshTime = (UInt32)refreshDate.ToTime().Sec();
		refreshDate.Hour(m_StartHour);
		m_NextRefreshTime = (UInt32)refreshDate.ToTime().Sec();
	}
	else
	{
		m_LastRefreshTime = (UInt32)refreshDate.ToTime().Sec();
		m_NextRefreshTime = m_LastRefreshTime + Avalon::Time::SECS_OF_HOUR;
	}

	Refresh(m_LastRefreshTime, true);

	m_SupplyTime = m_LastRefreshTime + Avalon::Time::SECS_OF_MIN * 15;

	InfoLogStream << "DigMapMgr Init(" << m_NextRefreshTime << " | " << m_SupplyTime << ")." << LogStream::eos;

	return true;
}

UInt32 DigMapMgr::OpenMap(WSPlayer* player, UInt32 mapId, std::vector<DigInfo>& infos, UInt32& playerSize)
{
	if (player == NULL) return ErrorCode::DIG_MAP_PLAYER_NOT_EXIST;
	DigMap* digMap = GetDigMap(mapId);
	if (digMap == NULL) return ErrorCode::DIG_MAP_NOT_EXIST;

	if (player->GetDigMapId() != 0)
	{
		CloseMap(player, player->GetDigMapId());
	}

	UInt32 nRes = digMap->OpenMap(player->GetID(), infos, playerSize);
	if (nRes == ErrorCode::SUCCESS)
	{
		player->SetDigMapId(mapId);
	}
	return nRes;
}

UInt32 DigMapMgr::CloseMap(WSPlayer* player, UInt32 mapId)
{
	if (player == NULL) return ErrorCode::DIG_MAP_PLAYER_NOT_EXIST;
	DigMap* digMap = GetDigMap(mapId);
	if (digMap == NULL) return ErrorCode::DIG_MAP_NOT_EXIST;

	player->SetDigMapId(0);

	return digMap->CloseMap(player->GetID());
}

UInt32 DigMapMgr::WatchDig(UInt32 mapId, UInt32 index, DigDetailInfo& info)
{
	DigMap* digMap = GetDigMap(mapId);
	if (digMap == NULL) return ErrorCode::DIG_MAP_NOT_EXIST;

	return digMap->WatchDig(index, info);
}

UInt32 DigMapMgr::OpenDig(WSPlayer* player, UInt32 mapId, UInt32 digIndex)
{
	DigMap* digMap = GetDigMap(mapId);
	if (digMap == NULL) return ErrorCode::DIG_MAP_NOT_EXIST;

	return digMap->OpenDig(player, digIndex);
}

UInt32 DigMapMgr::GetDigMapInfo(std::vector<DigMapInfo>& infos)
{
	std::vector<DigMap>::iterator iter = m_DigMaps.begin();
	while (iter != m_DigMaps.end())
	{
		DigMapInfo info;
		info.goldDigSize = iter->GetDigNum(DigType::DIG_GLOD, DigStatus::DIG_STATUS_INIT);
		info.silverDigSize = iter->GetDigNum(DigType::DIG_SILVER, DigStatus::DIG_STATUS_INIT);
		info.mapId = iter->GetDigMapId();

		infos.push_back(info);
		++iter;
	}
	return ErrorCode::SUCCESS;
}

void DigMapMgr::Refresh(UInt32 refreshTime, bool isInit)
{
	InfoLogStream << "DigMapMgr Refresh(" << refreshTime << ")." << LogStream::eos;

	Avalon::Time nowTime(((UInt64)refreshTime) * 1000);
	Avalon::Date nowDate(nowTime);
	UInt32 hour = nowDate.Hour();
	if (hour == 0) hour = 24;

	std::vector<DigMap>::iterator iter = m_DigMaps.begin();
	while (iter != m_DigMaps.end())
	{
		iter->RandomDigNum();
		if (isInit || iter->IsGoldRefreshHour(hour))
		{
			iter->Refresh(refreshTime, DigType::DIG_GLOD);
		}
		if (isInit || iter->IsSilverRefreshHour(hour))
		{
			iter->Refresh(refreshTime, DigType::DIG_SILVER);
		}
		iter->SyncRefreshToPlayers();
		++iter;
	}
}


void DigMapMgr::Supply(UInt32 supplyTime)
{
	InfoLogStream << "DigMapMgr Supply(" << supplyTime << ")." << LogStream::eos;

	//UInt32 goldDigSum = 0;
	//UInt32 goldOpenSum = 0;
	UInt32 silverDigSum = 0;
	UInt32 silverOpenSum = 0;
	std::vector<DigMap>::iterator iter = m_DigMaps.begin();
	while (iter != m_DigMaps.end())
	{
		//UInt32 goldDigNum = iter->GetDigNum(DigType::DIG_GLOD, DigStatus::DIG_STATUS_INIT);
		//UInt32 goldOpenNum = iter->GetDigNumByOpenTime(DigType::DIG_GLOD, supplyTime, Avalon::Time::SECS_OF_MIN * 15);
		UInt32 silverDigNum = iter->GetDigNum(DigType::DIG_SILVER, DigStatus::DIG_STATUS_INIT);
		UInt32 silverOpenNum = iter->GetDigNumByOpenTime(DigType::DIG_SILVER, supplyTime, Avalon::Time::SECS_OF_MIN * 15);

		//goldDigSum += goldDigNum;
		silverDigSum += silverDigNum;
		//goldOpenSum += goldOpenNum;
		silverOpenSum += silverOpenNum;
		++iter;
	}

	//UInt32 supplyGoldNum = GetSupplyNum(goldDigSum, goldOpenSum);

	//UInt32 baseGoldNum = supplyGoldNum / m_DigMaps.size();
	//UInt32 remainGoldNum = supplyGoldNum % m_DigMaps.size();

	/*
	if (baseGoldNum > 0)
	{
	for (UInt32 i = 0; i < m_DigMaps.size(); ++i)
	{
	m_DigMaps[i].Supply(DigType::DIG_GLOD, baseGoldNum, supplyTime);
	}
	}

	for (UInt32 i = 0; i < remainGoldNum; ++i)
	{
	m_DigMaps[i].Supply(DigType::DIG_GLOD, 1, supplyTime);
	}
	*/

	UInt32 supplySilverNum = GetSupplyNum(silverDigSum, silverOpenSum);
	UInt32 baseSilverNum = supplySilverNum / m_DigMaps.size();
	UInt32 remainSilverNum = supplySilverNum % m_DigMaps.size();

	InfoLogStream << "DigMapMgr Supply silverDigNum(" << silverDigSum << ") silverOpenSum(" << silverOpenSum << ") supplySilverNum("
		<< supplySilverNum << ") baseNum(" << baseSilverNum << ") remainNum(" << remainSilverNum << ")" << LogStream::eos;

	if (baseSilverNum > 0)
	{
		for (UInt32 i = 0; i < m_DigMaps.size(); ++i)
		{
			m_DigMaps[i].Supply(DigType::DIG_SILVER, baseSilverNum, supplyTime);
		}
	}

	for (UInt32 i = 0; i < remainSilverNum; ++i)
	{
		m_DigMaps[i].Supply(DigType::DIG_SILVER, 1, supplyTime);
	}
}

void DigMapMgr::SyncToPlayers(Avalon::Protocol& sync, UInt32 mapId)
{
	for (UInt32 i = 0; i < m_DigMaps.size(); ++i)
	{
		if (m_DigMaps[i].GetDigMapId() == mapId)
		{
			m_DigMaps[i].SyncToPlayers(sync);
			break;
		}
	}
}

void DigMapMgr::SyncToPlayers(Avalon::Protocol& sync)
{
	for (UInt32 i = 0; i < m_DigMaps.size(); ++i)
	{
		m_DigMaps[i].SyncToPlayers(sync);
	}
}

void DigMapMgr::AddDigRecord(DigRecordInfo info)
{
	ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(info.itemId);
	if (itemDataEntry == NULL) return;
	if (itemDataEntry->color <= ItemQuality::ITEM_QUALITY_PURPLE) return;

	UInt32 recordMaxCount = 100;
	m_DigRecords.push_back(info);
	if (m_DigRecords.size() > recordMaxCount)
	{
		std::vector<DigRecordInfo>::iterator iter = m_DigRecords.begin();
		m_DigRecords.erase(iter);
	}
}

UInt32 DigMapMgr::GetDigRecords(std::vector<DigRecordInfo>& infos)
{
	infos = m_DigRecords;
	return ErrorCode::SUCCESS;
}

UInt32 DigMapMgr::GMOpenDig(WSPlayer* player, UInt32 mapId)
{
	if (mapId == 0)
	{
		std::vector<DigMap>::iterator iter = m_DigMaps.begin();
		while (iter != m_DigMaps.end())
		{
			DigMap* digMap = &(*iter);
			if (digMap != NULL)
			{
				for (UInt32 i = 0; i < 15; ++i)
				{
					DigRecordInfo info;
					if (digMap->GMOpenDig(player, i, info) == ErrorCode::SUCCESS)
					{
						AddDigRecord(info);
					}
				}
			}
			++iter;
		}
	}
	else
	{
		DigMap* digMap = GetDigMap(mapId);
		if (digMap == NULL) return ErrorCode::DIG_MAP_NOT_EXIST;

		for (UInt32 i = 0; i < 15; ++i)
		{
			DigRecordInfo info;
			if (digMap->GMOpenDig(player, i, info) == ErrorCode::SUCCESS)
			{
				AddDigRecord(info);
			}
		}
	}
	return ErrorCode::SUCCESS;
}

UInt32 DigMapMgr::GMRandomOpenDig(WSPlayer* player, DigType digType, UInt32& itemId, UInt32& itemNum)
{
	Dig testDig(0);
	UInt32 nRes = testDig.Refresh((UInt32)CURRENT_TIME.Sec(), digType);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}
	UInt32 tmpIndex = 0;
	nRes = testDig.GMOpen(player, tmpIndex, itemId, itemNum);
	return nRes;
}

void DigMapMgr::OnTick(Avalon::Time now)
{
	UInt32 nowSec = (UInt32)now.Sec();

	if (m_SupplyTime <= nowSec)
	{
		UInt32 lastSupplyTime = m_SupplyTime;
		//离上一次临时补充≥10分钟
		m_SupplyTime = m_SupplyTime + Avalon::Time::SECS_OF_MIN * 10;

		//离下一次整点刷新时间≥5分钟
		if (m_SupplyTime + Avalon::Time::SECS_OF_MIN * 5 > m_NextRefreshTime)
		{
			m_SupplyTime = m_SupplyTime + Avalon::Time::SECS_OF_MIN * 20;
		}
		else
		{
			Avalon::Time tmpSupplyTime(((UInt64)lastSupplyTime * 1000));
			Avalon::Date tmpSupplyDate(tmpSupplyTime);
			if ((UInt32)tmpSupplyDate.Hour() < m_StartHour)
			{
				m_SupplyTime = m_NextRefreshTime + Avalon::Time::SECS_OF_MIN * 15;
			}
			else
			{
				Supply(lastSupplyTime);
			}
		}
		InfoLogStream << "OnTick DigMapMgr Supply(" << lastSupplyTime << "|" << m_SupplyTime << "|" << m_NextRefreshTime << ")." << LogStream::eos;
	}

	if (m_NextRefreshTime <= nowSec)
	{
		Avalon::Time nowTime(((UInt64)m_NextRefreshTime) * 1000);
		Avalon::Date nowDate(nowTime);
		m_LastRefreshTime = m_NextRefreshTime;
		if ((UInt32)nowDate.Hour() >= m_StartHour)
		{
			m_NextRefreshTime = m_NextRefreshTime + Avalon::Time::SECS_OF_HOUR;
		}
		else
		{
			m_NextRefreshTime = m_NextRefreshTime + Avalon::Time::SECS_OF_HOUR * m_StartHour;
		}

		Refresh(m_LastRefreshTime);

		//离上一次整点刷新≥15分钟
		m_SupplyTime = m_LastRefreshTime + Avalon::Time::SECS_OF_MIN * 15;

		InfoLogStream << "OnTick DigMapMgr Refresh(" << m_LastRefreshTime << "|" << m_SupplyTime << "|" << m_NextRefreshTime << ")." << LogStream::eos;
	}
}

void DigMapMgr::OnPlayerOnline(WSPlayer* player)
{
	if (player->GetDigMapId() == 0) return;
	CloseMap(player, player->GetDigMapId());
}

void DigMapMgr::OnPlayerOffline(WSPlayer* player)
{
	if (player->GetDigMapId() == 0) return;
	CloseMap(player, player->GetDigMapId());
}

UInt32 DigMapMgr::GetSupplyNum(UInt32 remainDigNum, UInt32 openDigNum)
{
	if (remainDigNum > m_SupplySum) return 0;
	
	UInt32 supplyMinNum = (UInt32)((m_SupplySum - remainDigNum) * 0.3);
	if (supplyMinNum > openDigNum) supplyMinNum = openDigNum;

	UInt32 supplyMaxNum = (UInt32)((m_SupplySum - remainDigNum) * 0.6);
	if (supplyMaxNum > openDigNum) supplyMaxNum = openDigNum;

	UInt32 supplyNum = Avalon::Random::RandBetween(supplyMinNum, supplyMaxNum);

	return supplyNum;
}

DigMap* DigMapMgr::GetDigMap(UInt32 mapId)
{
	std::vector<DigMap>::iterator iter = m_DigMaps.begin();
	while (iter != m_DigMaps.end())
	{
		if (iter->GetDigMapId() == mapId)
			return &(*iter);
		++iter;
	}
	return NULL;
}
