#include "Dig.h"
#include "CLErrorCode.h"
#include "CLDigProtocol.h"
#include "CLSystemValueDataEntry.h"
#include "CLItemProtocol.h"
#include "udplog_typedef.h"
#include "WSRouter.h"
#include "WSItemTransaction.h"
#include "WSPlayer.h"
#include "CLDigProtocol.h"
#include "DigMapMgr.h"
#include "CLMsgParser.h"
#include "AnnouncementMgr.h"

class OpenDigTransaction : public ItemTransaction
{
public:
	OpenDigTransaction(WSPlayer* player, Dig* dig, UInt32 mapId)
		:ItemTransaction(player),m_Dig(dig), m_MapId(mapId){}
	~OpenDigTransaction(){}

	void OnCheckSucced(WSPlayer* player)
	{
		if (m_Dig != NULL)
		{
			m_Dig->SetOpenDigTransaction(NULL);
		}

		if (player != NULL && m_Dig != NULL)
		{
			CLProtocol::WorldDigOpenRes res;
			res.result = m_Dig->Open(player, res.itemIndex, res.itemId, res.itemNum);

			if (res.result == ErrorCode::SUCCESS)
			{
				RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_DIG, m_MapId, m_Dig->GetIndex()));

				CLProtocol::WorldDigRecordInfoSync recordSync;
				recordSync.info.digIndex = m_Dig->GetIndex();
				recordSync.info.itemId = res.itemId;
				recordSync.info.itemNum = res.itemNum;
				recordSync.info.mapId = m_MapId;
				recordSync.info.playerId = player->GetID();
				recordSync.info.playerName = player->GetName();
				recordSync.info.type = (UInt8)m_Dig->GetDigType();
				DigMapMgr::Instance()->AddDigRecord(recordSync.info);
				DigMapMgr::Instance()->SyncToPlayers(recordSync);

				CLProtocol::WorldDigInfoSync sync;
				sync.mapId = m_MapId;
				m_Dig->GetDigInfo(sync.info);
				DigMapMgr::Instance()->SyncToPlayers(sync, m_MapId);
			}

			player->SendProtocol(res);
		}
		else
		{
			UnLock();
		}
	}

	void OnCheckFailed(WSPlayer* player)
	{
		if (m_Dig != NULL)
		{
			m_Dig->SetOpenDigTransaction(NULL);
		}

		CLProtocol::WorldDigOpenRes res;
		res.result = ErrorCode::DIG_OPEN_ITEM;
		player->SendProtocol(res);
		UnLock();
	}

private:
	Dig* m_Dig;
	UInt32 m_MapId;
};

Dig::Dig(UInt32 index)
: m_Index(index), m_DigType(DigType::DIG_INVALID), m_DigStatus(DigStatus::DIG_STATUS_INVALID), m_LastRefreshTime(0), m_LastStatusTime(0),
m_OpenItemIndex(0), m_OpenItemId(0), m_OpenItemNum(0), m_OpenDigTransaction(NULL)
{
	m_GridNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DIG_GRID_NUM);
}

UInt32 Dig::CheckOpen(WSPlayer* player, UInt32 mapId)
{
	if (GetDigStatus() != DigStatus::DIG_STATUS_INIT) return ErrorCode::DIG_STATUS_ERROR;
	if (GetDigType() == DigType::DIG_INVALID) return ErrorCode::DIG_TYPE_ERROR;
	if (m_OpenDigTransaction != NULL) return ErrorCode::DIG_OPEN_TRANSACTION;

	OpenDigTransaction* trans = new OpenDigTransaction(player, this, mapId);
	m_OpenDigTransaction = trans;
	
	UInt32 itemId = 0;
	if (GetDigType() == DigType::DIG_GLOD)
	{
		itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DIG_GOLD_ITEM_ID);
	}
	else if (GetDigType() == DigType::DIG_SILVER)
	{
		itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_DIG_SILVER_ITEM_ID);
	}

	trans->SetItem(itemId, 1);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << "mapId(" << mapId << ") index(" << GetIndex() << ") open dig transaction failed." << LogStream::eos;
		return ErrorCode::DIG_OPEN_ITEM;
	}

	return ErrorCode::SUCCESS;
}

UInt32 Dig::Open(WSPlayer* player, UInt32& index, UInt32& itemId, UInt32& itemNum)
{
	UInt32 weightSum = 0;
	std::vector<DigItem>::iterator iter = m_DigItemPools.begin();
	while (iter != m_DigItemPools.end())
	{
		weightSum += iter->m_Weight;
		++iter;
	}

	UInt32 randomNum = Avalon::Random::RandBetween(1, weightSum);

	for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
	{
		if (randomNum > m_DigItemPools[i].m_Weight)
		{
			randomNum = randomNum - m_DigItemPools[i].m_Weight;
		}
		else
		{
			//随到了
			m_OpenItemIndex = i;
			index = i;
			itemId = m_DigItemPools[i].m_RewardId;
			SetDigStatus(DigStatus::DIG_STATUS_OPEN);
			SetLastStatusTime((UInt32)CURRENT_TIME.Sec());

			bool isReward = false;
			bool isAnn = false;
			if (m_DigItemPools[i].m_RewardId != 0)
			{
				isReward = DigItemRandomDataEntryMgr::Instance()->GetRandomItem(m_DigItemPools[i].m_RewardId, itemId, itemNum, isAnn);
			}

			if (!isReward)
			{
				itemId = m_DigItemPools[i].m_ItemId;
				itemNum = 1;
			}

			SetOpenItemId(itemId);
			SetOpenItemNum(itemNum);

			UInt32 giveItemId = 0;
			UInt32 giveItemNum = 0;

			if (GetDigType() == DigType::DIG_GLOD)
			{
				giveItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_GOLD_BUY_ITEM_ID);
				giveItemNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_GOLD_BUY_ITEM_NUM);
			}
			else if (GetDigType() == DigType::DIG_SILVER)
			{
				giveItemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_SILVER_BUY_ITEM_ID);
				giveItemNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_DIG_SILVER_BUY_ITEM_NUM);
			}

			//发奖
			CLProtocol::SceneAddRewardReq req;
			req.reason = GetReason(LogSourceType::SOURCE_TYPE_DIG_ADD_REWARD, itemId);
			req.notice = (UInt8)false;
			req.roleId = player->GetID();
			req.rewards.push_back(ItemReward(itemId, itemNum, 0, 0));
			req.rewards.push_back(ItemReward(giveItemId, giveItemNum, 0, 0));
			Router::BroadcastToScene(req);
			

			//发公告
			if (isAnn)
			{
				UInt32 announId = 0;
				if (GetDigType() == DigType::DIG_GLOD)
				{
					announId = 87;
				}
				else if (GetDigType() == DigType::DIG_SILVER)
				{
					announId = 88;
				}

				std::string itemLinkStr;
				std::string playerLinkStr;

				if (MsgPackage::GetPlayerMsgTag(playerLinkStr, player->GetID(), player->GetName(), player->GetOccu(), player->GetLevel()))
				{
					if (MsgPackage::GetItemMsgTag(itemLinkStr, 0, itemId, 0))
					{
						std::string systemLinkStr;
						MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_DIG, 0);
						UInt32 startTime = ((UInt32)CURRENT_TIME.Sec()) + 10;
						AnnouncementMgr::Instance()->PublicAnnouncement(0, announId, startTime, playerLinkStr, itemLinkStr, itemNum, systemLinkStr);
					}
				}
			}
			break;
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 Dig::GMOpen(WSPlayer* player, UInt32& index, UInt32& itemId, UInt32& itemNum)
{
	UInt32 weightSum = 0;
	std::vector<DigItem>::iterator iter = m_DigItemPools.begin();
	while (iter != m_DigItemPools.end())
	{
		weightSum += iter->m_Weight;
		++iter;
	}

	UInt32 randomNum = Avalon::Random::RandBetween(1, weightSum);

	for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
	{
		if (randomNum > m_DigItemPools[i].m_Weight)
		{
			randomNum = randomNum - m_DigItemPools[i].m_Weight;
		}
		else
		{
			//随到了
			m_OpenItemIndex = i;
			index = i;
			itemId = m_DigItemPools[i].m_RewardId;
			SetDigStatus(DigStatus::DIG_STATUS_OPEN);
			SetLastStatusTime((UInt32)CURRENT_TIME.Sec());

			bool isReward = false;
			bool isAnn = false;
			if (m_DigItemPools[i].m_RewardId != 0)
			{
				isReward = DigItemRandomDataEntryMgr::Instance()->GetRandomItem(m_DigItemPools[i].m_RewardId, itemId, itemNum, isAnn);
			}

			if (!isReward)
			{
				itemId = m_DigItemPools[i].m_ItemId;
				itemNum = 1;
			}

			SetOpenItemId(itemId);
			SetOpenItemNum(itemNum);

			break;
		}
	}

	return ErrorCode::SUCCESS;
}

UInt32 Dig::Refresh(UInt32 refreshTime, DigType digType)
{
	if (m_OpenDigTransaction != NULL) return ErrorCode::DIG_OPEN_TRANSACTION;

	DigType tmpDigType = DigType::DIG_INVALID;
	if (digType != DigType::DIG_INVALID)
	{
		tmpDigType = digType;
		
	}
	else if (GetDigType() != DigType::DIG_INVALID)
	{
		tmpDigType = GetDigType();
	}
	else
	{
		return ErrorCode::DIG_TYPE_ERROR;
	}

	std::map<UInt32, DigItemPoolDataEntry*> digItemPoolDataEntryMap;
	UInt32 itemNum = 0;
	DigItemPoolDataEntryMgr::DigItemPoolGroupMap digItemPoolGroup;

	if (!DigItemPoolDataEntryMgr::Instance()->GetDigItemPoolType(tmpDigType, digItemPoolGroup)) return ErrorCode::DIG_TYPE_ERROR;

	DigItemPoolDataEntryMgr::DigItemPoolGroupMap::iterator iter = digItemPoolGroup.begin();

	while (iter != digItemPoolGroup.end())
	{
		if (iter->second.digItemPools.size() > 0)
		{
			digItemPoolDataEntryMap[iter->first]= iter->second.digItemPools[0];
			itemNum += iter->second.digItemPools[0]->itemNum;
		}
		else
		{
			digItemPoolDataEntryMap[iter->first] = NULL;
		}
		++iter;
	}

	iter = digItemPoolGroup.begin();
	while (iter != digItemPoolGroup.end())
	{
		UInt32 randomNum = Avalon::Random::RandBetween(1, iter->second.weight);

		if (digItemPoolDataEntryMap[iter->first] != NULL)
		{
			for (UInt32 i = 0; i < iter->second.digItemPools.size(); ++i)
			{
				UInt32 boxWeight = iter->second.digItemPools[i]->boxWeight;

				if (randomNum > boxWeight)
				{
					randomNum = randomNum - boxWeight;
				}
				else
				{
					UInt32 nowItemNum = itemNum - digItemPoolDataEntryMap[iter->first]->itemNum + iter->second.digItemPools[i]->itemNum;
					if (nowItemNum <= m_GridNum)
					{
						digItemPoolDataEntryMap[iter->first] = iter->second.digItemPools[i];
						itemNum = nowItemNum;
					}
					break;
				}
			}
		}
		++iter;
	}

	//最后不够.从优先级低的开始填
	if (itemNum < m_GridNum)
	{
		DigItemPoolDataEntryMgr::DigItemPoolGroupMap::reverse_iterator rIter = digItemPoolGroup.rbegin();
		while (rIter != digItemPoolGroup.rend())
		{
			if (digItemPoolDataEntryMap[rIter->first] != NULL)
			{
				UInt32 needItemNum = m_GridNum - (itemNum - digItemPoolDataEntryMap[rIter->first]->itemNum);
				DigItemPoolDataEntry* nowDataEntry = digItemPoolDataEntryMap[rIter->first];
				for (UInt32 i = 0; i < rIter->second.digItemPools.size(); ++i)
				{
					if (rIter->second.digItemPools[i]->itemNum == needItemNum)
					{
						nowDataEntry = rIter->second.digItemPools[i];
						break;
					}
					else if (rIter->second.digItemPools[i]->itemNum < needItemNum && rIter->second.digItemPools[i]->itemNum > nowDataEntry->itemNum)
					{
						nowDataEntry = rIter->second.digItemPools[i];
					}
				}

				itemNum = itemNum - digItemPoolDataEntryMap[rIter->first]->itemNum + nowDataEntry->itemNum;
				digItemPoolDataEntryMap[rIter->first] = nowDataEntry;
			}
			else
			{
				ErrorLogStream << "DIG Refresh first(" << rIter->first << ") add item failed." << LogStream::eos;
			}

			if (itemNum == m_GridNum)
				break;

			++rIter;
		}
	}

	if (itemNum != m_GridNum)
	{
		return ErrorCode::DIG_RANDOM_ERROR;
	}

	/*
	if (tmpDigType == DigType::DIG_GLOD)
	{
		std::map<UInt32, DigItemPoolDataEntry*>::iterator tmpIter = digItemPoolDataEntryMap.begin();
		while (tmpIter != digItemPoolDataEntryMap.end())
		{
			InfoLogStream << " Watch Refresh Glod Dig Group(" << tmpIter->first << ") Item(" << tmpIter->second->itemId << "|" << tmpIter->second->itemNum << ")" << LogStream::eos;
			++tmpIter;
		}
	}
	*/

	UInt32 nRes = RandomSlot(digItemPoolGroup, digItemPoolDataEntryMap);
	if (nRes != ErrorCode::SUCCESS)
	{
		return nRes;
	}

	/*
	if (tmpDigType == DigType::DIG_GLOD)
	{
		for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
		{
			InfoLogStream << " Watch Refresh RandomSlot Item(" << m_DigItemPools[i].m_ItemId << ")" << LogStream::eos;
		}
	}
	*/

	SetLastRefreshTime(refreshTime);
	SetLastStatusTime(refreshTime);
	SetOpenItemIndex(0);
	SetOpenItemId(0);
	SetOpenItemNum(0);
	SetDigStatus(DigStatus::DIG_STATUS_INIT);
	SetDigType(tmpDigType);
	return ErrorCode::SUCCESS;
}

void Dig::GetDigInfo(DigInfo& info)
{
	info.changeStatusTime = GetLastStatusTime();
	info.index = GetIndex();
	info.status = GetDigStatus();
	info.type = GetDigType();
	info.openItemId = GetOpenItemId();
	info.openItemNum = GetOpenItemNum();
	info.refreshTime = GetLastRefreshTime();
}

void Dig::GetDigDetailInfo(DigDetailInfo& info)
{
	GetDigInfo(info.simpleInfo);
	for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
	{
		DigItemInfo itemInfo;
		itemInfo.itemId = m_DigItemPools[i].m_ItemId;
		info.digItems.push_back(itemInfo);
	}
}

UInt32 Dig::RandomSlot(DigItemPoolDataEntryMgr::DigItemPoolGroupMap& digItemPoolGroup, std::map<UInt32, DigItemPoolDataEntry*>& digItemPoolDataEntryMap)
{
	InitDigItemPools();

	DigItemPoolDataEntryMgr::DigItemPoolGroupMap::iterator iter = digItemPoolGroup.begin();
	while (iter != digItemPoolGroup.end())
	{
		std::vector<UInt32> randomVec;
		DigItemPoolDataEntry* digItemPoolDataEntry = digItemPoolDataEntryMap[iter->first];
		if (digItemPoolDataEntry != NULL && GetRandomIndex(randomVec, digItemPoolDataEntry))
		{
			UInt32 itemNum = digItemPoolDataEntry->itemNum;
			for (UInt32 i = 0; i < itemNum; ++i)
			{
				if (randomVec.size() == 0)
				{
					PushSlot(digItemPoolDataEntry);
					continue;
				}

				UInt32 randomVecIndex = Avalon::Random::RandBetween(0, randomVec.size() - 1);
				UInt32 randomIndex = randomVec[randomVecIndex];

				if (randomIndex != 0)
				{
					RemoveRandomIndex(randomVec, randomIndex - 1);
				}
				if (randomIndex != m_GridNum - 1)
				{
					RemoveRandomIndex(randomVec, randomIndex + 1);
				}
				RemoveRandomIndex(randomVec, randomIndex);

				m_DigItemPools[randomIndex].m_DigItemType = digItemPoolDataEntry->digItemType;
				m_DigItemPools[randomIndex].m_ItemId = digItemPoolDataEntry->itemId;
				m_DigItemPools[randomIndex].m_Weight = digItemPoolDataEntry->singleDigWeight;
				m_DigItemPools[randomIndex].m_RewardId = digItemPoolDataEntry->rewardId;
			}
		}
		++iter;
	}
	return ErrorCode::SUCCESS;
}

void Dig::PushSlot(DigItemPoolDataEntry* dataEntry)
{
	for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
	{
		if (m_DigItemPools[i].m_DigItemType == DigItemType::DIT_INVALID)
		{
			m_DigItemPools[i].m_DigItemType = dataEntry->digItemType;
			m_DigItemPools[i].m_ItemId = dataEntry->itemId;
			m_DigItemPools[i].m_Weight = dataEntry->singleDigWeight;
			m_DigItemPools[i].m_RewardId = dataEntry->rewardId;
			break;
		}
	}
}

bool Dig::GetRandomIndex(std::vector<UInt32>& randomVec, DigItemPoolDataEntry* digItemPoolDataEntry)
{
	if (digItemPoolDataEntry == NULL) return false;
	UInt32 digItemType = digItemPoolDataEntry->digItemType;
	UInt32 digItemId = digItemPoolDataEntry->itemId;

	for (UInt32 i = 0; i < m_DigItemPools.size(); ++i)
	{
		if (digItemType == DigItemType::DIT_HIGHEST_GRADE)
		{
			//不是最后一个
			if (m_DigItemPools[i].m_DigItemType == DigItemType::DIT_INVALID)
			{
				bool isPush = true;

				if (i != 0)
				{
					if (m_DigItemPools[i - 1].m_DigItemType == DigItemType::DIT_HIGHEST_GRADE)
					{
						isPush = false;
					}
				}
				if (i != m_DigItemPools.size() - 1)
				{
					if (m_DigItemPools[i + 1].m_DigItemType == DigItemType::DIT_HIGHEST_GRADE)
					{
						isPush = false;
					}
				}
				if (isPush)
				{
					randomVec.push_back(i);
				}
			}
		}
		else if (digItemType == DigItemType::DIT_NORMAL)
		{
			if (m_DigItemPools[i].m_DigItemType == DigItemType::DIT_INVALID)
			{
				bool isPush = true;
				if (i != 0)
				{
					if (m_DigItemPools[i - 1].m_ItemId == digItemId)
					{
						isPush = false;
					}
				}
				if (i != m_DigItemPools.size() - 1)
				{
					if (m_DigItemPools[i + 1].m_ItemId == digItemId)
					{
						isPush = false;
					}
				}
				if (isPush)
				{
					randomVec.push_back(i);
				}
			}
		}
		else
		{
			ErrorLogStream << " RandomSlot digItemType(" << digItemType << ") is invalid" << LogStream::eos;
			return false;
		}
	}
	return true;
}

void Dig::RemoveRandomIndex(std::vector<UInt32>& randomVec, UInt32 index)
{
	std::vector<UInt32>::iterator iter = std::find(randomVec.begin(), randomVec.end(), index);
	if (iter != randomVec.end())
	{
		randomVec.erase(iter);
	}
}

void Dig::InitDigItemPools()
{
	m_DigItemPools.clear();
	for (UInt32 i = 0; i < m_GridNum; ++i)
	{
		DigItem digItem(i);
		m_DigItemPools.push_back(digItem);
	}
}
