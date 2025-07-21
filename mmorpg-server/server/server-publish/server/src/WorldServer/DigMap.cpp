#include "DigMap.h"
#include "CLErrorCode.h"
#include "CLDigProtocol.h"
#include "WSPlayerMgr.h"

bool DigMap::Init(DigMapDataEntry* dataEntry)
{
	if (dataEntry == NULL) return false;
	m_DigMapDataEntry = dataEntry;

	for (UInt32 i = 0; i < m_DigMapDataEntry->digMaxNum; ++i)
	{
		m_Digs.push_back(Dig(i));
	}

	return true;
}

UInt32 DigMap::OpenMap(ObjID_t playerId, std::vector<DigInfo>& infos, UInt32& playerSize)
{
	for (UInt32 i = 0; i < m_Digs.size(); ++i)
	{
		DigInfo info;
		m_Digs[i].GetDigInfo(info);
		infos.push_back(info);
	}

	std::vector<ObjID_t>::iterator iter = std::find(m_Players.begin(), m_Players.end(), playerId);
	if (iter == m_Players.end())
	{
		m_Players.push_back(playerId);
	}

	playerSize = m_Players.size();

	SyncPlayerSize();

	return ErrorCode::SUCCESS;
}

UInt32 DigMap::CloseMap(ObjID_t playerId)
{
	std::vector<ObjID_t>::iterator iter = std::find(m_Players.begin(), m_Players.end(), playerId);
	if (iter != m_Players.end())
	{
		m_Players.erase(iter);
	}

	SyncPlayerSize();

	return ErrorCode::SUCCESS;
}

UInt32 DigMap::WatchDig(UInt32 index, DigDetailInfo& info)
{
	UInt32 digSize = m_Digs.size();

	if (digSize > 0 && index > digSize - 1) return ErrorCode::DIG_NOT_EXIST;

	m_Digs[index].GetDigDetailInfo(info);

	return ErrorCode::SUCCESS;
}

UInt32 DigMap::OpenDig(WSPlayer* player, UInt32 index)
{
	UInt32 digSize = m_Digs.size();

	if (digSize > 0 && index > digSize - 1) return ErrorCode::DIG_NOT_EXIST;
	return m_Digs[index].CheckOpen(player, GetDigMapId());
}

void DigMap::RandomDigNum()
{
	//每次刷新时，会随机2个值，银箱子的随机值范围为6-8；金箱子的随机值范围为6-8；且 银箱子+金箱子 数量在15以内,金箱子优先级高
	m_GoldDigNum = Avalon::Random::RandBetween(m_DigMapDataEntry->goldDigMinNum, m_DigMapDataEntry->goldDigMaxNum);
	m_SilverDigNum = Avalon::Random::RandBetween(m_DigMapDataEntry->silverDigMinNum, m_DigMapDataEntry->silverDigMaxNum);

	if (m_GoldDigNum + m_SilverDigNum > m_DigMapDataEntry->digMaxNum)
	{
		UInt32 moreDigNum = (m_GoldDigNum + m_SilverDigNum) - m_DigMapDataEntry->digMaxNum;
		UInt32 tmpDigNum = moreDigNum / 2;
		UInt32 remainDigNum = moreDigNum % 2;
		if (remainDigNum == 0)
		{
			m_GoldDigNum = m_GoldDigNum - tmpDigNum;
			m_SilverDigNum = m_SilverDigNum - tmpDigNum;
		}
		else
		{
			m_GoldDigNum = m_GoldDigNum - tmpDigNum;
			m_SilverDigNum = m_SilverDigNum - tmpDigNum - remainDigNum;
		}
	}
}

void DigMap::Refresh(UInt32 refreshTime, DigType digType)
{
	if (digType == DigType::DIG_INVALID) return;

	std::vector<Dig*> randomDigs;
	std::vector<Dig*> maxRandomDigs;
	UInt32 remainDigNum = 0;
	UInt32 remainRefreshDigNum = 0;

	std::vector<Dig>::iterator iter = m_Digs.begin();
	while (iter != m_Digs.end())
	{
		if (iter->GetDigType() == digType && iter->GetDigStatus() == DigStatus::DIG_STATUS_INIT)
		{
			if (refreshTime >= iter->GetLastRefreshTime() + Avalon::Time::SECS_OF_HOUR * 3)
			{
				UInt32 nRes = iter->Refresh(refreshTime);
				if (nRes != ErrorCode::SUCCESS)
				{
					ErrorLogStream << "dig map(" << iter->GetIndex() << ") refresh nRes(" << nRes << ")." << LogStream::eos;
				}
				else
				{
					remainRefreshDigNum += 1;
				}
			}
			else
			{
				maxRandomDigs.push_back(&(*iter));
			}
			remainDigNum += 1;
		}
		else if (iter->GetDigStatus() != DigStatus::DIG_STATUS_INIT)
		{
			randomDigs.push_back(&(*iter));
		}
		++iter;
	}

	UInt32 maxRandomDigSize = maxRandomDigs.size();
	InfoLogStream << "digType("<< digType <<") remainDigNum(" << remainDigNum << ") m_DigNum(" << m_GoldDigNum << " | " 
		<< m_SilverDigNum <<") remainRefreshDigNum(" << remainRefreshDigNum << ") maxRandomDigSize(" << maxRandomDigSize << ") ." << LogStream::eos;
	UInt32 refreshDigNum = 0;
	if (digType == DigType::DIG_GLOD)
	{
		refreshDigNum = GetRefreshDigNum(remainDigNum, m_GoldDigNum, GetGoldDigMinNum(), remainRefreshDigNum, maxRandomDigs, refreshTime);
	}
	else if (digType == DigType::DIG_SILVER)
	{
		refreshDigNum = GetRefreshDigNum(remainDigNum, m_SilverDigNum, GetSilverDigMinNum(), remainRefreshDigNum, maxRandomDigs, refreshTime);
	}
	
	if (refreshDigNum > 0)
	{
		RefreshDigs(randomDigs, refreshDigNum, refreshTime, digType);
	}
}

void DigMap::Refresh(UInt32 refreshTime)
{
	std::vector<Dig*> randomDigs;
	std::vector<Dig*> maxRandomGoldDigs;
	std::vector<Dig*> maxRandomSilverDigs;
	UInt32 remainGoldDigNum = 0;
	UInt32 remainSilverDigNum = 0;
	UInt32 remainRefreshGoldDigNum = 0;
	UInt32 remainRefreshSilverDigNum = 0;

	std::vector<Dig>::iterator iter = m_Digs.begin();
	while (iter != m_Digs.end())
	{
		if (iter->GetDigStatus() == DigStatus::DIG_STATUS_INIT)
		{
			if (refreshTime >= iter->GetLastRefreshTime() + Avalon::Time::SECS_OF_HOUR * 3)
			{
				//没有被打开的箱子在地图上停留超过2次刷新时，第3次刷新会把箱子内容刷新
				UInt32 nRes = iter->Refresh(refreshTime);
				if (nRes != ErrorCode::SUCCESS)
				{
					ErrorLogStream << "dig map(" << iter->GetIndex() << ") refresh nRes(" << nRes << ")." << LogStream::eos;
				}
				else
				{
					if (iter->GetDigType() == DigType::DIG_GLOD)
					{
						remainRefreshGoldDigNum += 1;
					}
					else if (iter->GetDigType() == DigType::DIG_SILVER)
					{
						remainRefreshSilverDigNum += 1;
					}
				}
			}
			else
			{
				if (iter->GetDigType() == DigType::DIG_GLOD)
				{
					remainGoldDigNum += 1;
					maxRandomGoldDigs.push_back(&(*iter));
				}
				else if (iter->GetDigType() == DigType::DIG_SILVER)
				{
					remainSilverDigNum += 1;
					maxRandomSilverDigs.push_back(&(*iter));
				}
			}
		}
		else
		{
			randomDigs.push_back(&(*iter));
		}

		++iter;
	}

	UInt32 maxRandomGoldDigSize = maxRandomGoldDigs.size();
	InfoLogStream << "remainGoldDigNum(" << remainGoldDigNum << ") m_GoldDigNum(" << m_GoldDigNum << ") remainRefreshGoldDigNum(" 
		<< remainRefreshGoldDigNum << ") maxRandomGoldDigSize(" << maxRandomGoldDigSize << ") ." << LogStream::eos;
	UInt32 refreshGoldDigNum = GetRefreshDigNum(remainGoldDigNum, m_GoldDigNum, GetGoldDigMinNum(), remainRefreshGoldDigNum, maxRandomGoldDigs, refreshTime);
	if (refreshGoldDigNum > 0)
	{
		RefreshDigs(randomDigs, refreshGoldDigNum, refreshTime, DigType::DIG_GLOD);
	}

	UInt32 maxRandomSilverDigSize = maxRandomSilverDigs.size();
	InfoLogStream << "remainSilverDigNum(" << remainSilverDigNum << ") m_SilverDigNum(" << m_SilverDigNum << ") remainRefreshSilverDigNum(" 
		<< remainRefreshSilverDigNum << ") maxRandomSilverDigSize(" << maxRandomSilverDigSize << ") ." << LogStream::eos;
	UInt32 refreshSilverDigNum = GetRefreshDigNum(remainSilverDigNum, m_SilverDigNum, GetSilverDigMinNum(), remainRefreshSilverDigNum, maxRandomSilverDigs, refreshTime);
	if (refreshSilverDigNum > 0)
	{
		RefreshDigs(randomDigs, refreshSilverDigNum, refreshTime, DigType::DIG_SILVER);
	}

	SyncRefreshToPlayers();

}

void DigMap::Supply(DigType digType, UInt32 num, UInt32 supplyTime)
{
	std::vector<Dig*> supplyDigs;

	std::vector<Dig>::iterator iter = m_Digs.begin();
	while (iter != m_Digs.end())
	{
		if (iter->GetDigStatus() == DigStatus::DIG_STATUS_INVALID ||
			(iter->GetDigStatus() == DigStatus::DIG_STATUS_OPEN && supplyTime > iter->GetLastStatusTime() + Avalon::Time::SECS_OF_MIN * 10))
		{
			supplyDigs.push_back(&(*iter));
		}
		++iter;
	}

	RefreshDigs(supplyDigs, num, supplyTime, digType);
}

UInt32 DigMap::GMOpenDig(WSPlayer* player, UInt32 index, DigRecordInfo& info)
{
	UInt32 digSize = m_Digs.size();

	if (digSize > 0 && index > digSize - 1) return ErrorCode::DIG_NOT_EXIST;
	UInt32 tmpIndex = 0;
	UInt32 itemId = 0;
	UInt32 itemNum = 0;
	UInt32 nRes = m_Digs[index].Open(player, tmpIndex, itemId, itemNum);

	if (nRes == ErrorCode::SUCCESS)
	{
		info.digIndex = tmpIndex;
		info.itemId = itemId;
		info.itemNum = itemNum;
		info.mapId = GetDigMapId();
		info.playerId = player->GetID();
		info.playerName = player->GetName();
		info.type = m_Digs[index].GetDigType();
	}
	return nRes;
}

UInt32 DigMap::GetDigNum(DigType digType, DigStatus digStatus)
{
	UInt32 result = 0;
	std::vector<Dig>::iterator iter = m_Digs.begin();
	while (iter != m_Digs.end())
	{
		if (iter->GetDigStatus() == digStatus && iter->GetDigType() == digType)
		{
			result += 1;
		}
		++iter;
	}

	return result;
}

UInt32 DigMap::GetDigNumByOpenTime(DigType digType, UInt32 nowTime, UInt32 interval)
{
	UInt32 result = 0;
	std::vector<Dig>::iterator iter = m_Digs.begin();
	while (iter != m_Digs.end())
	{
		if (iter->GetDigType() == digType && iter->GetDigStatus() == DigStatus::DIG_STATUS_OPEN && nowTime > iter->GetLastStatusTime() + interval)
		{
			result += 1;
		}
		++iter;
	}

	return result;
}

UInt32 DigMap::GetRefreshDigNum(UInt32 remainDigNum, UInt32 digNum, UInt32 minDigNum, UInt32 remainRefreshDigNum, std::vector<Dig*>& randomDigs, UInt32 refreshTime)
{
	UInt32 result = 0;
	if (remainDigNum == 0)
	{
		//当地图上剩余金 / 银箱子数量为0时，每次刷新出来的箱子数量 = 随机值
		result = digNum;
	}
	else if (remainDigNum <= minDigNum)
	{
		//当地图上剩余金/银箱子数量大于0，且小于最小随机值时，每次刷新出来的箱子数量 = 随机值-剩余箱子数量
		result = digNum - remainDigNum;
	}
	else if (digNum >= remainDigNum)
	{
		//当地图上剩余金/银箱子数量在随机值之间，如果随机值大于剩余值，每次刷新出来的箱子数量 = 随机值-剩余箱子数量；
		result = digNum - remainDigNum;
	}
	else
	{
		//如果随机值小于剩余值，则随机选择 max（【剩余值 -随机值 - 条目1中出现的需要刷新的箱子数量】，0）数量的箱子刷新其内容
		if (remainDigNum - digNum > remainRefreshDigNum)
		{
			UInt32 tmpCount = remainDigNum - digNum - remainRefreshDigNum;
			RefreshDigs(randomDigs, tmpCount, refreshTime);
		}
	}
	return result;
}

void DigMap::GetDigInfos(std::vector<DigInfo>& infos)
{
	for (UInt32 i = 0; i < m_Digs.size(); ++i)
	{
		DigInfo info;
		m_Digs[i].GetDigInfo(info);
		infos.push_back(info);
	}
}

void DigMap::SyncToPlayers(Avalon::Protocol& sync)
{
	for (UInt32 i = 0; i < m_Players.size(); ++i)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_Players[i]);
		if (player != NULL)
		{
			player->SendProtocol(sync);
		}
	}
}

void DigMap::SyncRefreshToPlayers()
{
	CLProtocol::WorldDigRefreshSync sync;
	sync.mapId = GetDigMapId();
	GetDigInfos(sync.infos);

	std::vector<ObjID_t>::iterator playerIter = m_Players.begin();
	if (playerIter != m_Players.end())
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(*playerIter);
		if (player != NULL)
		{
			player->SendProtocol(sync);
		}
	}
}

void DigMap::RefreshDigs(std::vector<Dig*>& randomDigs, UInt32 count, UInt32 refreshTime, DigType digType)
{
	UInt32 randomDigSize = randomDigs.size();

	if (count > randomDigSize)
	{
		ErrorLogStream << " digMap(" << GetDigMapId() << ") tmpCount >= maxRandomDigSize(" << count << "|" << randomDigSize << ") " << LogStream::eos;
		count = randomDigSize;
	}

	while (count > 0)
	{
		UInt32 index = Avalon::Random::RandBetween(1, randomDigSize) - 1;
		std::vector<Dig*>::iterator iter = randomDigs.begin();
		UInt32 tmpIndex = 0;
		while (iter != randomDigs.end())
		{
			if (tmpIndex == index)
			{
				Dig* dig = (*iter);
				if (dig != NULL)
				{
					UInt32 nRes = 0;
					if (digType == DigType::DIG_INVALID)
					{
						nRes = dig->Refresh(refreshTime);
					}
					else
					{
						nRes = dig->Refresh(refreshTime, digType);
					}

					if (nRes != ErrorCode::SUCCESS)
					{
						ErrorLogStream << "RefreshDigs(" << GetDigMapId() << "|" << dig->GetIndex() << "|" 
							<< digType << ") refresh nRes(" << nRes << ")." << LogStream::eos;
					}
				}
				randomDigs.erase(iter);
				break;
			}
			++tmpIndex;
			++iter;
		}
		randomDigSize = randomDigs.size();
		count--;
	}

}

void DigMap::SyncPlayerSize()
{
	UInt32 playerSize = m_Players.size();
	if (playerSize == 0) return;

	CLProtocol::WorldDigPlayerSizeSync sync;
	sync.mapId = GetDigMapId();
	sync.playerSize = playerSize;

	std::vector<ObjID_t>::iterator iter = m_Players.begin();
	while (iter != m_Players.end())
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(*iter);
		if (player != NULL)
		{
			player->SendProtocol(sync);
		}

		++iter;
	}
}

