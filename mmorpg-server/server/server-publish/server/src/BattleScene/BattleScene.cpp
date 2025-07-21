#include "BattleScene.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "SceneSysNotify.h"

#include <math.h>
#include <udplog_typedef.h>
#include <CLSyncObjectProtocol.h>
#include <CLBattleProtocol.h>
#include <CLChijiSpawnItemDataEntry.h>
#include <CLChiJiTimeDataEntry.h>
#include <CLChiJiPointDataEntry.h>
#include <CLChiJiShopTimeDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLSystemValueDataEntry.h>

#include <CLCrossProtocol.h>
#include <CLBattleDefine.h>

BattlePlayer::BattlePlayer()
{
	m_playerID = 0;
	m_killerID = 0;
	m_protectTimestamp = 0;
	m_playerStatus = BPS_ALIVE;
	m_openingBoxGuid = 0;
	m_openingTimestamp = 0;
	m_enterTimeoutTimestamp = 0;
}

BattlePlayer::~BattlePlayer()
{
	//do nothing
}

BattleInfo::BattleInfo()
{
	m_battleID = 0;
	m_waveID = 0;
	m_blueCenterPoint.SetX(0);
	m_blueCenterPoint.SetY(0);
	m_blueRadius = 0;
	m_whiteCenterPoint.SetX(0);
	m_whiteCenterPoint.SetY(0);
	m_whiteRadius = 0;
	m_nextWhiteCenterPoint.SetX(0);
	m_nextWhiteCenterPoint.SetY(0);
	m_nextWhiteRadius = 0;
	m_poisonInterval = 0;
	m_poisonBegin = 0;
	m_deltaHPPerSec = 0;
}

BattleInfo::~BattleInfo()
{
	//do nothing
}

static ScenePos operator*(const ScenePos& a, float factor)
{
	return ScenePos(UInt32(a.GetX()*factor), UInt32(a.GetY()*factor));
}

static ScenePos operator*(float factor, const ScenePos& a)
{
	return ScenePos(UInt32(a.GetX()*factor), UInt32(a.GetY()*factor));
}

static ScenePos operator+(const ScenePos& a, const ScenePos& b)
{
	return ScenePos(a.GetX() + b.GetX(), a.GetY() + b.GetY());
}

static ScenePos Lerp(const ScenePos& a, const ScenePos& b, float t)
{
	return (1 - t) * a + b * t;
}

static UInt32 Lerp(UInt32 a, UInt32 b, float t)
{
	return UInt32((1 - t) * a + b * t);
}

static UInt32 GetDistance(const ScenePos& a, const ScenePos& b)
{
	UInt64 x = 0;
	UInt64 y = 0;

	if (a.GetX() > b.GetX())
	{
		x = a.GetX() - b.GetX();
	}
	else
	{
		x = b.GetX() - a.GetX();
	}

	if (a.GetY() > b.GetY())
	{
		y = a.GetY() - b.GetY();
	}
	else
	{
		y = b.GetY() - a.GetY();
	}

	return UInt32(sqrt(x*x + y*y));
}

BattleScene::BattleScene()
{
	m_GuidGen = 10000;
	m_TickTimer500ms.SetInterval(500);
}

BattleScene::~BattleScene()
{
	ClearItems();
	m_AllPlayers.clear();
	m_AllBattles.clear();
	m_ResourceNodes.clear();
	m_TransferNodes.clear();
	m_NpcInfos.clear();
}

void BattleScene::OnTick(const Avalon::Time& now)
{
	Scene::OnTick(now);

	if (!m_TickTimer500ms.IsTimeout(now))
	{
		return;
	}

	UInt32 npcSurvivalTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BATTLE_NPC_SURVIVAL_TIME);
	for (auto& iter : m_NpcInfos)
	{
		CLProtocol::SceneBattleNpcNotify npcNotify;

		std::list<BattleNpcInfo>::iterator tt = iter.second.begin();
		for (;tt != iter.second.end();)
		{
			if (tt->npcType == NPC_SUB_TRADER && (tt->beginTime + npcSurvivalTime) < now.Sec()) // 删除Npc
			{
				CLProtocol::BattleNpc battleNpc;
				battleNpc.npcGuid = tt->npcGuid;
				battleNpc.npcId = tt->npcId;
				battleNpc.opType = 0;
				battleNpc.scenePos = *(tt->pos);
				npcNotify.battleNpcList.push_back(battleNpc);

				InfoLogStream << "npc trader expire npcId:" << tt->npcId << " pos x:" << tt->pos->GetX() 
					<< " y:" << tt->pos->GetY() << LogStream::eos;

				tt = iter.second.erase(tt);
			}
			else
			{
				++tt;
			}
		}

		if (!npcNotify.battleNpcList.empty())
		{
			NotifyMsgToAllPlayerByBattleID(iter.first, npcNotify);
		}
	}

	for (auto& iter : m_AllItems)
	{
		UInt32 battleID = iter.first;
		std::vector<SceneItem*>::iterator itemIter = iter.second.begin();
		while (itemIter != iter.second.end())
		{
			SceneItem* item = *itemIter;

			if (item->ownerId != 0 && item->disapperTime <= now.Sec())
			{
				Sys::SendUdpLog("chiji_traps", LOG_CHIJI_TRAPS, battleID, item->ownerId, 3);

				CLProtocol::SceneItemDel notify;
				notify.battleID = battleID;
				notify.guids.push_back(item->guid);
				NotifyMsgToAllPlayerByBattleID(battleID, notify);

				itemIter = iter.second.erase(itemIter);
				continue;
			}

			if (item->openerId != 0 && item->openTime <= now.Sec())
			{
				BattlePlayer* battlePlayer = GetBattlePlayer(battleID, item->openerId);
				if (battlePlayer)
				{
					battlePlayer->m_openingBoxGuid = 0;
					battlePlayer->m_openingTimestamp = 0;
				}

				Player* player = PlayerMgr::Instance()->FindNormalPlayer(item->openerId);
				if (player)
				{
					InfoLogStream << "battle:" << battleID << ", player:" << player->GetID() << ", item:" << item->guid << ", pick up" << LogStream::eos;

					Sys::SendUdpLog("chiji_box", LOG_CHIJI_BOX, player->GetBattleID(), player->GetID(), item->dataId, 3);

					player->AddItem("", item->dataId, 1);

					CLProtocol::SceneBattleOpenBoxRes res;
					res.itemGuid = item->guid;
					res.param = 3;
					res.retCode = ErrorCode::SUCCESS;
					player->SendProtocol(res);

					CLProtocol::SceneItemDel notify;
					notify.battleID = battleID;
					notify.guids.push_back(item->guid);
					NotifyMsgToAllPlayerByBattleID(battleID, notify);

					itemIter = iter.second.erase(itemIter);
					continue;
				}
				else
				{
					item->openerId = 0;
					item->openTime = 0;
				}
			}

			++itemIter;
		}
	}

	for (auto& iter : m_AllPlayers)
	{
		for (auto& player : iter.second)
		{
			if (player.GetStatus() == BPS_OUT)
			{
				continue;
			}

			//规定时间内未进吃鸡
			if (player.m_enterTimeoutTimestamp != 0 && player.m_enterTimeoutTimestamp <= now.Sec())
			{
				player.SetStatus(BPS_OUT);
				player.m_enterTimeoutTimestamp = 0;
				NotifyBattleSomeoneDead(iter.first, player.m_playerID, 0, CHIJI_DEAD_REASON_TIMEOUT);
			}
		}
	}
}

bool BattleScene::Load(UInt32 mapid, const std::string& config)
{
	//加载地图信息
	Avalon::XmlDocument doc(config);
	if (!doc.LoadFile()) return false;

	Avalon::XmlElement* mapNode = doc.RootElement();
	if (!mapNode)
	{
		return false;
	}

	Avalon::XmlElement* resourceNode = doc.FirstChildElement(mapNode, "Resource");
	while (resourceNode)
	{
		//InfoLogStream << "find resource node" << LogStream::eos;

		UInt32 tmpID = 0;
		UInt32 x = 0;
		UInt32 y = 0;
		
		doc.GetAttribute(resourceNode, "resourceID", tmpID);
		doc.GetAttribute(resourceNode, "X", x);
		doc.GetAttribute(resourceNode, "Y", y);

		//InfoLogStream << "resourceID:" << tmpID << ", x:" << x << ", y:" << y << LogStream::eos;

		ScenePos tmpPos(x, y);
		std::map<UInt32, std::vector<ScenePos>>::iterator tmpIter = m_ResourceNodes.find(tmpID);
		if (tmpIter != m_ResourceNodes.end())
		{
			tmpIter->second.push_back(tmpPos);
		}
		else
		{
			std::vector<ScenePos> tmpVec;
			tmpVec.push_back(tmpPos);

			m_ResourceNodes[tmpID] = tmpVec;
		}

		m_ResourceVec.push_back(tmpPos);

		resourceNode = doc.NextElement(resourceNode, "Resource");
	}

	Avalon::XmlElement* transferNode = doc.FirstChildElement(mapNode, "Transfer");
	while (transferNode)
	{
		//InfoLogStream << "find transfer node" << LogStream::eos;

		UInt32 tmpID = 0;
		UInt32 x = 0;
		UInt32 y = 0;

		doc.GetAttribute(transferNode, "regionId", tmpID);
		doc.GetAttribute(transferNode, "X", x);
		doc.GetAttribute(transferNode, "Y", y);

		//InfoLogStream << "regionId:" << tmpID << ", x:" << x << ", y:" << y << LogStream::eos;

		ScenePos tmpPos(x, y);
		std::map<UInt32, std::vector<ScenePos>>::iterator tmpIter = m_TransferNodes.find(tmpID);
		if (tmpIter != m_TransferNodes.end())
		{
			tmpIter->second.push_back(tmpPos);
		}
		else
		{
			std::vector<ScenePos> tmpVec;
			tmpVec.push_back(tmpPos);

			m_TransferNodes[tmpID] = tmpVec;
		}

		transferNode = doc.NextElement(transferNode, "Transfer");
	}

	return Scene::Load(mapid, config);
}

void BattleScene::ClearItems()
{
	for (std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.begin();
		iter != m_AllItems.end(); ++iter)
	{
		std::vector<SceneItem*>::iterator itemIter = iter->second.begin();
		while (itemIter != iter->second.end())
		{
			SceneItem* item = *itemIter;
			CL_SAFE_DELETE(item);

			itemIter = iter->second.erase(itemIter);
		}
	}
	m_AllItems.clear();
}

void BattleScene::CreateItems(UInt32 battleID, UInt32 waveID)
{
	InfoLogStream << "battle id :" << battleID << ", wave id :" << waveID << LogStream::eos;

	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		//InfoLogStream << "battle id :" << battleID << ", wave id :" << waveID << ", error return 1" <<LogStream::eos;
		return;
	}

	ChiJiTimeDataEntry* timeDataEntry = ChiJiTimeDataEntryMgr::Instance()->FindEntry(waveID);
	if (!timeDataEntry)
	{
		//InfoLogStream << "battle id :" << battleID << ", wave id :" << waveID << ", error return 3" << LogStream::eos;
		return;
	}

	if (timeDataEntry->chickenType == CT_POSION_SCOPE)
	{
		//刷毒圈
		if (battleIter->second.m_blueRadius <= 0)
		{
			CLPosition pos;
			pos.x = GetLength() / 2;
			pos.y =GetWidth() / 2;
			ScenePos tmpPos = ScenePos::GetScenePosByGridPos(pos, GetGridWidth(), GetGridHeight());

			battleIter->second.m_blueCenterPoint = tmpPos;
			battleIter->second.m_blueRadius = timeDataEntry->chickenParameter;

			battleIter->second.m_whiteCenterPoint = tmpPos;
			battleIter->second.m_whiteRadius = timeDataEntry->chickenParameter;
		}
		else
		{
			battleIter->second.m_blueCenterPoint = battleIter->second.m_whiteCenterPoint;
			battleIter->second.m_blueRadius = battleIter->second.m_whiteRadius;

			battleIter->second.m_whiteCenterPoint = battleIter->second.m_nextWhiteCenterPoint;
			battleIter->second.m_whiteRadius = battleIter->second.m_nextWhiteRadius;
		}

		ChiJiTimeDataEntry* nextDataEntry = ChiJiTimeDataEntryMgr::Instance()->FindEntry(timeDataEntry->nextDuquanID);
		if (nextDataEntry && timeDataEntry->nextDuquanID > 0)
		{
			battleIter->second.m_nextWhiteRadius = nextDataEntry->chickenParameter;

			if (nextDataEntry->minX > 0 && nextDataEntry->maxX > 0 && nextDataEntry->minY > 0 && nextDataEntry->maxY > 0)
			{
				UInt32 x = Avalon::Random::RandBetween(nextDataEntry->minX, nextDataEntry->maxX);
				UInt32 y = Avalon::Random::RandBetween(nextDataEntry->minY, nextDataEntry->maxY);
				battleIter->second.m_nextWhiteCenterPoint.SetX(x);
				battleIter->second.m_nextWhiteCenterPoint.SetY(y);
				//InfoLogStream << "battle :" << battleIter->second.m_battleID << ", wave:" << waveID << ", X:" << x << ", Y:" << y << LogStream::eos;
			}
			else
			{
				UInt32 a = Avalon::Random::RandBetween(1, 360);
				UInt32 r = sqrt(Avalon::Random::RandBetween(1, 100)) * (battleIter->second.m_whiteRadius - battleIter->second.m_nextWhiteRadius) / 10;
				UInt32 x = UInt32(double(r) * cos(a*3.1415 / 180) + double(battleIter->second.m_whiteCenterPoint.GetX()));
				UInt32 y = UInt32(double(r) * sin(a*3.1415 / 180) + double(battleIter->second.m_whiteCenterPoint.GetY()));
				battleIter->second.m_nextWhiteCenterPoint.SetX(x);
				battleIter->second.m_nextWhiteCenterPoint.SetY(y);
				//InfoLogStream << "battle :" << battleIter->second.m_battleID << ", wave:" << waveID << ", a:" << a << ", r:" << r << LogStream::eos;
			}
		}
		else
		{
			battleIter->second.m_nextWhiteCenterPoint = battleIter->second.m_whiteCenterPoint;
			battleIter->second.m_nextWhiteRadius = battleIter->second.m_whiteRadius;
		}

		InfoLogStream << "battle :" << battleIter->second.m_battleID << ", wave:" << waveID << ",poison blue:" << battleIter->second.m_blueCenterPoint.GetX() << "," << battleIter->second.m_blueCenterPoint.GetY() << "," << battleIter->second.m_blueRadius << LogStream::eos;
		InfoLogStream << "battle :" << battleIter->second.m_battleID << ", wave:" << waveID << ",poison white:" << battleIter->second.m_whiteCenterPoint.GetX() << "," << battleIter->second.m_whiteCenterPoint.GetY() << "," << battleIter->second.m_whiteRadius << LogStream::eos;

		battleIter->second.m_poisonInterval = timeDataEntry->continueTime;
		battleIter->second.m_poisonBegin = Avalon::Time::CurrentTime().Sec();
		battleIter->second.m_deltaHPPerSec = timeDataEntry->continueInjury;

		CLProtocol::SceneBattleSyncPoisonRing notify;
		notify.battleID = battleID;
		notify.x = battleIter->second.m_whiteCenterPoint.GetX();
		notify.y = battleIter->second.m_whiteCenterPoint.GetY();
		notify.radius = battleIter->second.m_whiteRadius;
		notify.beginTimestamp = 0;
		notify.interval = battleIter->second.m_poisonInterval;
		notify.x1 = battleIter->second.m_nextWhiteCenterPoint.GetX();
		notify.y1 = battleIter->second.m_nextWhiteCenterPoint.GetY();
		notify.radius1 = battleIter->second.m_nextWhiteRadius;
		NotifyMsgToAllPlayerByBattleID(battleID, notify);
	}

	if (timeDataEntry->chickenType != CT_PUT_ITEM)
	{
		InfoLogStream << "battle id :" << battleID << ", wave id :" << waveID << ", end" << LogStream::eos;
		return;
	}

	class ChijiSpawnItemDataVisitor : public Avalon::DataEntryVisitor<ChijiSpawnItemDataEntry>
	{
	public:
		ChijiSpawnItemDataVisitor(UInt32 waveID) : m_waveID(waveID) {}

		bool operator()(ChijiSpawnItemDataEntry* dataEntry)
		{
			if (m_waveID != dataEntry->times)
			{
				return true;
			}

			UInt32 packID = dataEntry->packID;
			std::map<UInt32, std::vector<ChijiSpawnItemDataEntry*>>::iterator packIter = m_itemRandMap.find(packID);
			if (packIter == m_itemRandMap.end())
			{
				std::vector<ChijiSpawnItemDataEntry*> tmpVec;
				tmpVec.push_back(dataEntry);
				m_itemRandMap[packID] = tmpVec;
			}
			else
			{
				packIter->second.push_back(dataEntry);
			}

			return true;
		}

	public:
		UInt32 m_waveID;
		std::map<UInt32, std::vector<ChijiSpawnItemDataEntry*>> m_itemRandMap;
	};

	ChijiSpawnItemDataVisitor visitor(timeDataEntry->chickenParameter);
	ChijiSpawnItemDataEntryMgr::Instance()->Visit(visitor);

	if (0 == timeDataEntry->chickenParameter)
	{
		return;
	}

	//准备位置点集合
	std::vector<ScenePos> positionVec;
	for (auto node : m_ResourceNodes)
	{
		//if ((node.first / 10000 / timeDataEntry->chickenParameter) != 1)
		//{
		//	continue;
		//}
		positionVec.insert(positionVec.end(), node.second.begin(), node.second.end());
	}

	//去掉蓝圈外的随机位置点，下一个刷毒阶段，现在的白圈变蓝圈
	for (std::vector<ScenePos>::iterator pos_iter = positionVec.begin(); pos_iter != positionVec.end();)
	{
		UInt32 distance = GetDistance(*pos_iter, battleIter->second.m_whiteCenterPoint);
		if ((distance + 15000) > battleIter->second.m_whiteRadius)
		{
			DebugLogStream << "the position is out of blue circle, x:" << pos_iter->GetX() << ", y:" << pos_iter->GetY() << LogStream::eos;
			pos_iter = positionVec.erase(pos_iter);
		}
		else
		{
			++pos_iter;
		}
	}

	if (positionVec.size() <= 0)
	{
		DebugLogStream << "battle id :" << battleID << ", wave id :" << waveID << ", error return 5" << LogStream::eos;
		return;
	}

	std::random_shuffle(positionVec.begin(), positionVec.end());
	size_t positionIndex = 0;
	UInt32 round = 0;
	UInt32 offset = 10000;

	std::vector<SceneItem*> tmpItemVec;
	for (auto& packVec : visitor.m_itemRandMap)
	{
		UInt32 randTotal = 0;
		for (auto& pack : packVec.second)
		{
			randTotal += pack->weight;
		}

		UInt32 randNum = Avalon::Random::RandBetween(1, randTotal);
		for (auto& data : packVec.second)
		{
			if (data->weight >= randNum)
			{
				UInt32 itemNum = Avalon::Random::RandBetween(data->minNumber, data->maxNumber);
				if (itemNum <= 0)
				{
					//InfoLogStream << "battle id :" << battleID << ", wave id :" << waveID << ", error return 6" << LogStream::eos;
					continue;
				}

				for (UInt32 i = 0; i < itemNum; ++i)
				{
					ScenePos tmpPos = positionVec[positionIndex];
					switch (round)
					{
					case 0:
						break;
					case 1:
						tmpPos.SetX(tmpPos.GetX() + offset);
						break;
					case 2:
						tmpPos.SetX(tmpPos.GetX() + offset);
						tmpPos.SetY(tmpPos.GetY() - offset);
						break;
					case 3:
						tmpPos.SetY(tmpPos.GetY() - offset);
						break;
					case 4:
						tmpPos.SetX(tmpPos.GetX() - offset);
						tmpPos.SetY(tmpPos.GetY() - offset);
						break;
					case 5:
						tmpPos.SetX(tmpPos.GetX() - offset);
						break;
					case 6:
						tmpPos.SetX(tmpPos.GetX() - offset);
						tmpPos.SetY(tmpPos.GetY() + offset);
						break;
					case 7:
						tmpPos.SetY(tmpPos.GetY() + offset);
						break;
					case 8:
						tmpPos.SetX(tmpPos.GetX() + offset);
						tmpPos.SetY(tmpPos.GetY() + offset);
						break;
					default:
						break;
					}

					SceneItem* item = new SceneItem;
					item->dataId = data->itemID;
					item->pos.x = tmpPos.GetX();
					item->pos.y = tmpPos.GetY();

					//临时调试代码
					item->waveId = battleIter->second.m_waveID;
					item->blueCenter.x = battleIter->second.m_whiteCenterPoint.GetX();
					item->blueCenter.y = battleIter->second.m_whiteCenterPoint.GetY();
					item->blueRadius = battleIter->second.m_whiteRadius;

					DebugLogStream << "battle id :" << battleID << ", item data id:" << item->dataId << ", x:" << item->pos.x << ", y:" << item->pos.y << LogStream::eos;

					tmpItemVec.push_back(item);

					positionIndex = (positionIndex + 1) % positionVec.size();
					if (0 == positionIndex)
					{
						++round;
						DebugLogStream << "round :" << round << LogStream::eos;
					}
				}
			}
			else
			{
				randNum -= data->weight;
			}
		}
	}

	//通知消息
	CLProtocol::SceneItemAdd notify;
	notify.battleID = battleID;
	SceneItemInfo info;
	info.sceneId = MAPID_CHIJI_BATTLE;

	for (auto& tmpItem : tmpItemVec)
	{
		tmpItem->guid = m_GuidGen++;

		DebugLogStream << "battle id :" << battleID << ", drop item guid:" << tmpItem->guid << ", data id:" << tmpItem->dataId << ", x:" << tmpItem->pos.x << ", y:" << tmpItem->pos.y << ", wave id:" << tmpItem->waveId << ", blue_x:" << tmpItem->blueCenter.x << ", blue_y:" << tmpItem->blueCenter.y << ", blue_r:" << tmpItem->blueRadius << LogStream::eos;

		info.items.push_back(*tmpItem);
	}

	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter != m_AllItems.end())
	{
		iter->second.insert(iter->second.end(), tmpItemVec.begin(), tmpItemVec.end());
	}
	else
	{
		m_AllItems[battleID] = tmpItemVec;
	}

	notify.data.push_back(info);
	NotifyMsgToAllPlayerByBattleID(battleID, notify);

	// 刷出Npc商人
	// CreateNpc(battleID, waveID, NPC_SUB_TRADER);
}

void BattleScene::DestroyBattle(UInt32 battleID)
{
	//删除物品
	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter == m_AllItems.end())
	{
		return;
	}

	std::vector<SceneItem*>::iterator itemIter = iter->second.begin();
	while (itemIter != iter->second.end())
	{
		SceneItem* item = *itemIter;
		CL_SAFE_DELETE(item);
		
		itemIter = iter->second.erase(itemIter);
	}

	m_AllItems.erase(iter);

	//删除玩家
	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return;
	}

	for (auto& battlePlayer : playersIter->second)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(battlePlayer.m_playerID);
		if (player && battleID == player->GetBattleID())
		{
			InfoLogStream << "player:" << player->GetID() << ", leave battle:" << battleID << LogStream::eos;
			player->SetBabttleID(0);
		}
	}

	m_AllPlayers.erase(playersIter);

	std::map<UInt32, BattleInfo>::iterator infoIter = m_AllBattles.find(battleID);
	if (infoIter != m_AllBattles.end())
	{
		m_AllBattles.erase(infoIter);
	}

	m_WaitPlayerQuitTimer.SetLastTime(0);
	m_WaitPlayerQuitTimer.SetInterval(60 * 1000);

	// 删除Npc
	auto npcIter = m_NpcInfos.find(battleID);
	if (npcIter != m_NpcInfos.end())
	{
		m_NpcInfos.erase(npcIter);
	}
}

void BattleScene::InitBattle(UInt32 battleID, std::vector<UInt64>& playerIDs)
{
	std::map<UInt32, std::vector<BattlePlayer>>::iterator iter = m_AllPlayers.find(battleID);
	if (iter != m_AllPlayers.end())
	{
		return;
	}

	std::vector<BattlePlayer> tmpPlayerVec;

	for (auto id : playerIDs)
	{

		InfoLogStream << "battle(" << battleID << ") add player(" << id << ")." << LogStream::eos;

		BattlePlayer tmpPlayer;
		tmpPlayer.m_playerID = id;
		tmpPlayer.m_enterTimeoutTimestamp = UInt32(Avalon::Time::CurrentTime().Sec()) + 60;

		tmpPlayerVec.push_back(tmpPlayer);

		Player* player = PlayerMgr::Instance()->FindPlayer(id);
		if (player)
		{
			// 上一局先清退
			if (player->GetBattleID() > 0)
			{
				NotifyBattleSomeoneDead(player->GetBattleID(), player->GetID(), 0, CHIJI_DEAD_REASON_TIMEOUT);
			}

			InfoLogStream << "battle(" << battleID << ") delete prev player(" << player->GetID() << ")." << LogStream::eos;
			PlayerMgr::Instance()->DelPlayer(player);
		}
	}

	m_AllPlayers[battleID] = tmpPlayerVec;

	BattleInfo tmpInfo;
	tmpInfo.m_battleID = battleID;

	m_AllBattles[battleID] = tmpInfo;
}

void BattleScene::GetSceneItemList(UInt32 battleID, UInt64 playerID, CLProtocol::SceneItemList& itemList)
{
	InfoLogStream << "battle id:" << battleID << ", player id:" << playerID << LogStream::eos;

	if (!BeInBattle(battleID, playerID))
	{
		InfoLogStream << "not in battle" << LogStream::eos;
		return;
	}

	SceneItemInfo info;
	info.sceneId = MAPID_CHIJI_BATTLE;

	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter != m_AllItems.end())
	{
		for (auto itemIter : iter->second)
		{
			info.items.push_back(*itemIter);
		}

		//InfoLogStream << "item size :" << (Int32)info.items.size() << LogStream::eos;
	}

	itemList.infoes.push_back(info);
}

SceneItem* BattleScene::GetSceneItemByGuid(UInt32 battleID, UInt64 playerID, UInt64 itemID)
{
	if (!BeInBattle(battleID, playerID))
	{
		return NULL;
	}

	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter != m_AllItems.end())
	{
		for (auto itemIter : iter->second)
		{
			if (itemIter->guid == itemID)
			{
				DebugLogStream << "battle id:" << battleID << ", get item guid:" << itemIter->guid << ", data id:" << itemIter->dataId << ", x:" << itemIter->pos.x << ", y:" << itemIter->pos.y << ", wave id:" << itemIter->waveId << ", blue_x:" << itemIter->blueCenter.x << ", blue_y:" << itemIter->blueCenter.y << ", blue_r:" << itemIter->blueRadius << LogStream::eos;
				return itemIter;
			}
		}
	}

	return NULL;
}

void BattleScene::DeleteSceneItemByGuid(UInt32 battleID, UInt64 itemID)
{
	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter != m_AllItems.end())
	{
		for (std::vector<SceneItem*>::iterator itemIter = iter->second.begin(); itemIter != iter->second.end(); ++ itemIter)
		{
			SceneItem* item = *itemIter;
			if (item->guid == itemID)
			{
				iter->second.erase(itemIter);
				CL_SAFE_DELETE(item);
				break;
			}
		}
	}

	CLProtocol::SceneItemDel notify;
	notify.battleID = battleID;
	notify.guids.push_back(itemID);

	NotifyMsgToAllPlayerByBattleID(battleID, notify);
}

bool BattleScene::BeInBattle(UInt32 battleID, UInt64 playerID)
{
	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return false;
	}

	bool find = false;
	for (auto& battlePlayer : playersIter->second)
	{
		if (battlePlayer.m_playerID == playerID)
		{
			find = true;
			break;
		}
	}

	return find;
}

void BattleScene::KillSomeone(UInt32 battleID, UInt64 playerID, UInt64 killerID, Int32 winnerRemainHp, Int32 winnerRemainMp, UInt64 otherID)
{
	if (killerID != 0)
	{
		BattlePlayer* battlePlayer = GetBattlePlayer(battleID, playerID);
		if (!battlePlayer)
		{
			return;
		}

		BattlePlayer* killerPlayer = GetBattlePlayer(battleID, killerID);
		if (!killerPlayer)
		{
			return;
		}

		battlePlayer->m_killerID = killerID;
		battlePlayer->SetStatus(BPS_OUT);
		InfoLogStream << "battle:" << battleID << ", player:" << battlePlayer->m_playerID << ", be killed" << LogStream::eos;

		killerPlayer->m_protectTimestamp = Avalon::Time::CurrentTime().Sec() + 30;
		killerPlayer->SetStatus(BPS_ALIVE);

		NotifyBattleSomeoneDead(battleID, playerID, killerID, CHIJI_DEAD_REASON_BE_KILLED);

		Player* dstPlayer = PlayerMgr::Instance()->FindNormalPlayer(playerID);
		if (dstPlayer)
		{
			dstPlayer->SetStatus(CREATURE_STATUS_DEAD);
			dstPlayer->GetBuffMgr().RemoveBuff(400000001);
		}

		Player* srcPlayer = PlayerMgr::Instance()->FindNormalPlayer(killerID);
		if (srcPlayer)
		{
			//做个简单的血量保护
			if (winnerRemainHp <= 0)
			{
				winnerRemainHp = 1;
			}

			// 修改剩余血量
			srcPlayer->SetChijiHp(winnerRemainHp);
			srcPlayer->SetChijiMp(winnerRemainMp);
			//增加一个无敌buff
			srcPlayer->AddBuff(400000000);
			//去掉pk的buff
			srcPlayer->GetBuffMgr().RemoveBuff(400000001);

			srcPlayer->SetStatus(CREATURE_STATUS_NORMAL);

			InfoLogStream << "player(" << srcPlayer->GetID() << ") hp(" << srcPlayer->GetChijiHp() << ") mp(" << srcPlayer->GetChijiMp() << ")." << LogStream::eos;
		}
		else
		{
			ErrorLogStream << "battle(" << battleID << ") srcPlayer(" << killerID << ") not exist." << LogStream::eos;
			return;
		}

		UInt64 idBegin = 100;
		for (auto& item : battlePlayer->m_items)
		{
			item.guid = idBegin++;
			InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", killer:" << killerID << ", guid:" << item.guid << ", item:" << item.itemTypeId << ", count:" << item.num << LogStream::eos;
		}

		CLProtocol::SceneBattleNotifySpoilsItem notify;
		notify.battleID = battleID;
		notify.playerID = playerID;
		notify.detailItems = battlePlayer->m_items;

		for (auto& item : notify.detailItems)
		{
			InfoLogStream << "item:" << item.guid << "," << item.itemTypeId << "," << item.num << LogStream::eos;
		}

		srcPlayer->SendProtocol(notify);
	}
	else
	{
		//平局处理
		BattlePlayer* battlePlayer = GetBattlePlayer(battleID, playerID);
		if (!battlePlayer)
		{
			return;
		}

		BattlePlayer* otherPlayer = GetBattlePlayer(battleID, otherID);
		if (!otherPlayer)
		{
			return;
		}

		battlePlayer->SetStatus(BPS_OUT);
		InfoLogStream << "battle:" << battleID << ", player:" << battlePlayer->m_playerID << ", be drew" << LogStream::eos;

		otherPlayer->SetStatus(BPS_OUT);
		InfoLogStream << "battle:" << battleID << ", player:" << otherPlayer->m_playerID << ", be drew" << LogStream::eos;

		NotifyBattleSomeoneDead(battleID, playerID, killerID, CHIJI_DEAD_REASON_BE_KILLED);
		NotifyBattleSomeoneDead(battleID, otherID, killerID, CHIJI_DEAD_REASON_BE_KILLED);

		Player* dstPlayer = PlayerMgr::Instance()->FindNormalPlayer(playerID);
		if (dstPlayer)
		{
			dstPlayer->SetStatus(CREATURE_STATUS_DEAD);
			dstPlayer->GetBuffMgr().RemoveBuff(400000001);
		}

		Player* srcPlayer = PlayerMgr::Instance()->FindNormalPlayer(otherID);
		if (srcPlayer)
		{
			srcPlayer->SetStatus(CREATURE_STATUS_DEAD);
			srcPlayer->GetBuffMgr().RemoveBuff(400000001);
		}
	}
}

void BattleScene::OnWaveNotify(UInt32 battleID, UInt32 waveID)
{
	std::map<UInt32, BattleInfo>::iterator tmpIter = m_AllBattles.find(battleID);
	if (tmpIter == m_AllBattles.end())
	{
		return;
	}

	tmpIter->second.m_waveID = waveID;

	CreateItems(tmpIter->second.m_battleID, tmpIter->second.m_waveID);

	ChiJiTimeDataEntry* timeDataEntry = ChiJiTimeDataEntryMgr::Instance()->FindEntry(waveID);
	if (NULL == timeDataEntry)
	{
		return;
	}

	// 刷NPC怪物
	/*if (timeDataEntry->chickenType == ChickenType::CT_CREATE_NPC)
	{
		CreateNpc(battleID, waveID, NPC_SUB_MONSTER);
	}*/
}

void BattleScene::OnShopWaveNotify(UInt32 battleID, UInt32 shopWave)
{
	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return;
	}

	ChiJiShopTimeDataEntry* shopData = ChiJiShopTimeDataEntryMgr::Instance()->FindEntry(shopWave);
	if (NULL == shopData)
	{
		ErrorLogStream << "not find chiji shop time config id:" << shopWave << LogStream::eos;
		return;
	}

	UInt32 nextTime = 0;
	ChiJiShopTimeDataEntry* nextShopData = ChiJiShopTimeDataEntryMgr::Instance()->FindEntry(shopWave + 1);
	if (nextShopData != NULL)
	{
		nextTime = nextShopData->startTime - shopData->startTime;
	}

	InfoLogStream << "chiji shop battleId:" << battleID << " shopWave:" << shopWave << " nextTime:" << nextTime << LogStream::eos;

	for (auto& battlePlayer : playersIter->second)
	{
		Player* srcPlayer = PlayerMgr::Instance()->FindPlayer(battlePlayer.m_playerID);
		if (srcPlayer)
		{
			if (srcPlayer->GetBattleID() != battleID)
			{
				continue;
			}

			// 第一波商店，先重置一下数据
			if (shopWave == 1)
			{
				srcPlayer->GetShopMgr().ResetChiJiData();
			}

			srcPlayer->GetShopMgr().OnRefreshChiJiShopID(shopData->shopID, nextTime);
		}
	}
}

BattlePlayer* BattleScene::GetBattlePlayer(UInt32 battleID, UInt64 playerID)
{
	//InfoLogStream << "battle:" << battleID << ", player:" << playerID << LogStream::eos;

	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return NULL;
	}

	for (auto& battlePlayer : playersIter->second)
	{
		if (battlePlayer.m_playerID == playerID)
		{
			//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", status:" << UInt32(battlePlayer.m_playerStatus) << LogStream::eos;
			return &battlePlayer;
		}
	}

	return NULL;
}

const ScenePos* BattleScene::RandPositionByRegionID(UInt32 regionID)
{
	std::map<UInt32, std::vector<ScenePos>>::iterator tmpIter = m_TransferNodes.find(regionID);
	if (tmpIter == m_TransferNodes.end() || 0 == tmpIter->second.size())
	{
		InfoLogStream << "error region id :" << regionID << LogStream::eos;
		return NULL;
	}

	size_t index = Avalon::Random::RandBetween(0, Int32(tmpIter->second.size()-1));
	if (index < 0 || index >= tmpIter->second.size())
	{
		InfoLogStream << "error index :" << (Int32)index << LogStream::eos;
		return NULL;
	}

	return &(tmpIter->second[index]);
}

void BattleScene::NotifyMsgToAllPlayerByBattleID(UInt32 battleID, Avalon::Protocol& protocol, bool includeDead)
{
	//InfoLogStream << "send notify to player, msg id:" << protocol.GetProtocolID() << ", battle:" << battleID << LogStream::eos;

	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return;
	}

	for (auto& battlePlayer : playersIter->second)
	{
		if (battlePlayer.GetStatus() == BPS_OUT && !includeDead)
		{
			//InfoLogStream << "send notify to player1:" << battlePlayer.m_playerID << ", msg id:" << protocol.GetProtocolID() << ", battle:" << battleID << LogStream::eos;
			continue;
		}

		auto player = PlayerMgr::Instance()->FindNormalPlayer(battlePlayer.m_playerID);
		if (player && battleID == player->GetBattleID())
		{
			InfoLogStream << "send notify to player:" << battlePlayer.m_playerID << ", msg id:" << protocol.GetProtocolID() << ", battle:" << battleID << LogStream::eos;
			player->SendProtocol(protocol);
		}
		else
		{
			//InfoLogStream << "send notify to player3:" << battlePlayer.m_playerID << ", msg id:" << protocol.GetProtocolID() << ", battle:" << battleID << LogStream::eos;
		}
	}
}

void BattleScene::NotifyBattleSomeoneDead(UInt32 battleID, UInt64 playerID, UInt64 killerID, UInt32 reason)
{
	CLProtocol::SceneBattleNotifySomeoneDead notify;
	notify.battleID = battleID;
	notify.playerID = playerID;
	notify.killerID = killerID;
	notify.reason = reason;
	notify.suvivalNum = 0;
	Router::SendToBattle(notify);
}

void BattleScene::CheckPlayerInPoisonRing(UInt32 battleID, UInt64 playerID, const ScenePos& pos)
{
	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		return;
	}

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerID);
	if (!player)
	{
		return;
	}

	BattlePlayer* tmpPlayer = GetBattlePlayer(battleID, playerID);
	if (!tmpPlayer || tmpPlayer->GetStatus() == BPS_OUT)
	{
		player->GetBuffMgr().RemoveBuff(400000001);
		player->GetBuffMgr().RemoveBuff(402000003);
		return;
	}

	if (tmpPlayer->GetStatus() == BPS_PK)
	{
		//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", is in pk." << LogStream::eos;

		player->GetBuffMgr().RemoveBuff(402000003);
		return;
	}

	BattleInfo& info = battleIter->second;

	UInt32 now = Avalon::Time::CurrentTime().Sec();
	if (now <= info.m_poisonBegin || info.m_poisonInterval <= 0 || now <= tmpPlayer->m_protectTimestamp)
	{
		player->GetBuffMgr().RemoveBuff(402000003);
		return;
	}

	if (now > tmpPlayer->m_protectTimestamp)
	{
		player->GetBuffMgr().RemoveBuff(400000000);
	}

	UInt32 radius = 0;
	UInt32 distance = 0;
	ScenePos center;
	if ((now - info.m_poisonBegin) >= info.m_poisonInterval)
	{
		radius = info.m_whiteRadius;
		center = info.m_whiteCenterPoint;
	}
	else
	{
		UInt32 deltaTime = now - info.m_poisonBegin;
		float factor = float(deltaTime) / float(info.m_poisonInterval);

		//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", delta time:" << deltaTime << ", interval:" << info.m_poisonInterval << ", factor" << factor << LogStream::eos;

		radius = Lerp(info.m_blueRadius, info.m_whiteRadius, factor);
		center = Lerp(info.m_blueCenterPoint, info.m_whiteCenterPoint, factor);
	}

	distance = GetDistance(pos, center);

	//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", player pos : " << pos.GetX() << ", " << pos.GetY() << LogStream::eos;
	//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", center pos:" << center.GetX() << "," << center.GetY() << LogStream::eos;
	//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", radius:" << radius << ", distance:" << distance << LogStream::eos;

	if (distance <= radius)
	{
		player->GetBuffMgr().RemoveBuff(402000003);
		return;
	}

	player->AddBuff(402000003);

	if ((Int32)info.m_deltaHPPerSec >= player->GetChijiHp())
	{
		//毒死了
		player->SetChijiHp(0);
		player->SetStatus(CREATURE_STATUS_DEAD);
		player->SendNoWarToBattle();
		tmpPlayer->SetStatus(BPS_OUT);

		InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", poison dead" << LogStream::eos;
		NotifyBattleSomeoneDead(battleID, playerID, 0, CHIJI_DEAD_REASON_POISIONED);
	}
	else
	{
		//InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", player poison lose blood:" << info.m_deltaHPPerSec << LogStream::eos;
		//掉血了
		player->SetChijiHp(player->GetChijiHp() - info.m_deltaHPPerSec);
	}
}

void BattleScene::SyncAllDataToPlayer(UInt32 battleID, UInt64 playerID)
{
	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		return;
	}

	std::map<UInt32, std::vector<BattlePlayer>>::iterator playersIter = m_AllPlayers.find(battleID);
	if (playersIter == m_AllPlayers.end())
	{
		return;
	}

	for (auto& battlePlayer : playersIter->second)
	{
		if (battlePlayer.m_playerID != playerID)
		{
			continue;
		}

		auto player = PlayerMgr::Instance()->FindNormalPlayer(battlePlayer.m_playerID);
		if (!player || battleID != player->GetBattleID())
		{
			continue;
		}

		InfoLogStream << "send all data to player:" << playerID << ", battle:" << battleID << LogStream::eos;

		CLProtocol::SceneBattleSyncPoisonRing notify;
		notify.battleID = battleID;
		notify.x = battleIter->second.m_whiteCenterPoint.GetX();
		notify.y = battleIter->second.m_whiteCenterPoint.GetY();
		notify.radius = battleIter->second.m_whiteRadius;
		notify.beginTimestamp = Avalon::Time::CurrentTime().Sec() - battleIter->second.m_poisonBegin;
		notify.interval = battleIter->second.m_poisonInterval;
		notify.x1 = battleIter->second.m_nextWhiteCenterPoint.GetX();
		notify.y1 = battleIter->second.m_nextWhiteCenterPoint.GetY();
		notify.radius1 = battleIter->second.m_nextWhiteRadius;
		player->SendProtocol(notify);

		CLProtocol::SceneItemList itemList;
		GetSceneItemList(battleID, playerID, itemList);
		player->SendProtocol(itemList);
	}
}

void BattleScene::LeaveBattle(UInt32 battleID, UInt64 playerID)
{
	OnOpenBoxBeInterrupted(battleID, playerID);

	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		return;
	}

	BattlePlayer* tmpPlayer = GetBattlePlayer(battleID, playerID);
	if (!tmpPlayer || tmpPlayer->GetStatus() == BPS_OUT)
	{
		return;
	}

	tmpPlayer->SetStatus(BPS_OUT);

	InfoLogStream << "battle:" << battleID << ", player:" << playerID << ", leave battle" << LogStream::eos;
	NotifyBattleSomeoneDead(battleID, playerID);
}

void BattleScene::UpdatePlayerItemDataVector(UInt32 battleID, UInt64 playerID, std::vector<DetailItem>& itemDatas)
{
	BattlePlayer* tmpPlayer = GetBattlePlayer(battleID, playerID);
	if (tmpPlayer)
	{
		tmpPlayer->m_items.clear();
		tmpPlayer->m_items = itemDatas;

		tmpPlayer->SetStatus(BPS_PK);
	}
}

void BattleScene::CreateNpc(UInt32 battleID, UInt32 waveID, UInt32 npcSubType)
{
	CLProtocol::SceneBattleNpcNotify npcNotify;

	UInt32 randNum = 1;
	if (npcSubType == NPC_SUB_MONSTER)
	{
		randNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BATTLE_NPC_MONSTER_NUM);
	}
	else if (npcSubType == NPC_SUB_TRADER)
	{
		randNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BATTLE_NPC_TRADER_NUM);
	}

	if (randNum == 0)
	{
		ErrorLogStream << " npc rand num error num:"<< randNum << LogStream::eos;
		return;
	}

	Sys::SendUdpLog("chiji_npc", LOG_CHIJI_NPC, battleID, npcSubType, CHIJI_NPC_CREATE, randNum, 0, 0);

	UInt32 npcNum = Avalon::Random::RandBetween(1, randNum);
	while (npcNum--)
	{
		const ScenePos* randPos = GetWhiteCirclePoint(battleID, true);
		if (NULL == randPos)
		{
			ErrorLogStream << "rand pos error! npc subtype:" << npcSubType << LogStream::eos;
			return;
		}

		UInt32 npcId = NpcDataEntryMgr::Instance()->GetRandNpcIdByType(npcSubType);
		if (npcId == 0)
		{
			ErrorLogStream << "rand npcid error! npc subtype:" << npcSubType << LogStream::eos;
			return;
		}

		BattleNpcInfo npcInfo;
		npcInfo.npcGuid = m_GuidGen++;
		npcInfo.npcType = npcSubType;
		npcInfo.npcId = npcId;
		npcInfo.beginTime = UInt32(CURRENT_TIME.Sec());
		npcInfo.pos = randPos;
		AddNpc(battleID, npcInfo);

		CLProtocol::BattleNpc battleNpc;
		battleNpc.npcGuid = npcInfo.npcGuid;
		battleNpc.npcId = npcId;
		battleNpc.opType = 1;
		battleNpc.scenePos = *randPos;

		npcNotify.battleNpcList.push_back(battleNpc);

		InfoLogStream << "battleID:" << battleID << " create npc " << npcSubType << " id:" << npcId << " waveId:" << waveID <<
			" pos x:" << randPos->GetX() << " y:" << randPos->GetY() << LogStream::eos;
	}

	NotifyMsgToAllPlayerByBattleID(battleID, npcNotify);
}

const BattleNpcInfo* BattleScene::GetBattleNpcInfo(UInt32 battleID, UInt64 npcGuid)
{
	auto iter = m_NpcInfos.find(battleID);
	if (iter == m_NpcInfos.end())
	{
		return NULL;
	}

	for (auto& it : iter->second)
	{
		if (it.npcGuid == npcGuid)
		{
			return &it;
		}
	}

	return NULL;
}

void BattleScene::RemoveNpc(UInt32 battleID, UInt64 npcGuid, bool isNotify)
{
	InfoLogStream << "remove npc battleID:" << battleID << " npcId:" << npcGuid << " npcGuid:" << npcGuid << LogStream::eos;

	auto iter = m_NpcInfos.find(battleID);
	if (iter == m_NpcInfos.end())
	{
		return;
	}

	std::list<BattleNpcInfo>::iterator it = iter->second.begin();
	for (; it != iter->second.end();++it)
	{
		if (it->npcGuid == npcGuid)
		{
			if (isNotify)
			{
				CLProtocol::SceneBattleNpcNotify notify;
				CLProtocol::BattleNpc battleNpc;
				battleNpc.npcGuid = it->npcGuid;
				battleNpc.npcId = it->npcId;
				battleNpc.opType = 0;
				battleNpc.scenePos = *(it->pos);
				notify.battleNpcList.push_back(battleNpc);
				NotifyMsgToAllPlayerByBattleID(battleID, notify);
			}

			iter->second.erase(it);

			return;
		}
	}
}

const ScenePos* BattleScene::GetWhiteCirclePoint(UInt32 battleID, bool isHasNpc)
{
	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		return NULL;
	}

	std::random_shuffle(m_ResourceVec.begin(), m_ResourceVec.end());

	for (auto& tt : m_ResourceVec)
	{
		UInt32 distance = GetDistance(tt, battleIter->second.m_blueCenterPoint);
		if (distance > battleIter->second.m_blueRadius)
		{
			continue;
		}

		// 找到蓝圈内的某一点，该点没有Npc
		if (isHasNpc && !PosIsHasNpc(&tt))
		{
			continue;
		}

		return &tt;
	}

	return NULL;
}

void BattleScene::AddNpc(UInt32 battleID, BattleNpcInfo& npcInfo)
{
	auto iter = m_NpcInfos.find(battleID);
	if (iter == m_NpcInfos.end())
	{
		std::list<BattleNpcInfo> tempVec;
		tempVec.push_back(npcInfo);
		m_NpcInfos[battleID] = tempVec;
	}
	else
	{
		iter->second.push_back(npcInfo);
	}
}

bool BattleScene::PosIsHasNpc(const ScenePos* pos)
{
	for (auto& iter : m_NpcInfos)
	{
		for (auto& tt : iter.second)
		{
			if (tt.pos != pos)
			{
				continue;
			}

			return false;
		}
	}

	return true;
}

void BattleScene::KickPlayersOut()
{
	class KickPlayersVisitor : public PlayerVisitor
	{
	public:
		KickPlayersVisitor(BattleScene* scene)
			:m_pScene(scene) {}

		bool operator()(Player* player)
		{
			m_pScene->KickPlayerOut(player);
			return true;
		}
	private:
		BattleScene*	m_pScene;
	};

	KickPlayersVisitor visitor(this);
	VisitPlayers(visitor);
}

void BattleScene::KickPlayerOut(Player* player)
{
	if (player == NULL || player->GetScene() == NULL)
	{
		return;
	}

	if (player->GetScene() != this)
	{
		ErrorLogStream << "player(" << player->GetID() << ") kick from scene(" << GetID() << ") current scene(" << player->GetScene()->GetID() << ")." << LogStream::eos;
		return;
	}

	if (player->GetGameStatus() != PS_NORMAL)
	{
		ErrorLogStream << "player(" << player->GetID() << ") kick from scene(" << GetID() << ") invalid status(" << player->GetGameStatus() << ")." << LogStream::eos;
		return;
	}

	player->SendNoWarToBattle();

	// 回到原来的城镇
	CLProtocol::BattleExitSceneReq exit;
	exit.roleId = player->GetID();
	exit.battleType = BTY_CHIJI;
	Router::SendToSelfScene(player->GetSrcSceneNodeID(), exit);

	InfoLogStream << "kick player(" << player->GetID() << ") back to self scene." << LogStream::eos;

	m_KickTimer.SetLastTime(0);
	m_KickTimer.SetInterval(10000);

	SetStatus(SCENE_QUIT);
}

void BattleScene::OnKickPlayerTimeout()
{
	class RemovePlayersVisitor : public PlayerVisitor
	{
	public:
		RemovePlayersVisitor(BattleScene* scene)
			:m_pScene(scene) {}

		bool operator()(Player* player)
		{
			InfoLogStream << "scene(" << m_pScene->GetID() << ", " << m_pScene->GetMapID() << ") kick player(" << player->GetID() << ") timeout, jest delete player." << LogStream::eos;
			m_pScene->RemoveObject(player);
			player->SetGameStatus(PS_DESTORY);
			return true;
		}
	private:
		BattleScene*	m_pScene;
	};

	RemovePlayersVisitor vistor(this);
	VisitPlayers(vistor);
}
void BattleScene::AddSceneItem(UInt32 battleID, SceneItem* item)
{
	if (!item)
	{
		return;
	}

	std::map<UInt32, std::vector<SceneItem*>>::iterator iter = m_AllItems.find(battleID);
	if (iter == m_AllItems.end())
	{
		return;
	}

	item->guid = m_GuidGen++;
	iter->second.push_back(item);
}

void BattleScene::OnOpenBoxBeInterrupted(UInt32 battleID, UInt64 playerID)
{
	//DebugLogStream << "battle:" << battleID << ", player:" << playerID << ", open box be interrupted" << LogStream::eos;

	std::map<UInt32, BattleInfo>::iterator battleIter = m_AllBattles.find(battleID);
	if (battleIter == m_AllBattles.end())
	{
		return;
	}

	BattlePlayer* tmpPlayer = GetBattlePlayer(battleID, playerID);
	if (!tmpPlayer)
	{
		return;
	}

	if (0 == tmpPlayer->m_openingBoxGuid)
	{
		return;
	}

	SceneItem* tmpItem = GetSceneItemByGuid(battleID, playerID, tmpPlayer->m_openingBoxGuid);
	if (!tmpItem || tmpItem->openerId != playerID)
	{
		return;
	}

	Sys::SendUdpLog("chiji_box", LOG_CHIJI_BOX, battleID, playerID, tmpItem->dataId, 2);

	DebugLogStream << "battle:" << battleID << ", player:" << playerID << ", open box be interrupted, box guid:" << tmpPlayer->m_openingBoxGuid << LogStream::eos;

	tmpPlayer->m_openingBoxGuid = 0;
	tmpPlayer->m_openingTimestamp = 0;

	tmpItem->openerId = 0;
	tmpItem->openTime = 0;

	CLProtocol::SceneBattleOpenBoxRes res;
	res.itemGuid = tmpItem->guid;
	res.param = 2;
	res.retCode = ErrorCode::SUCCESS;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerID);
	if (player)
	{
		player->SendProtocol(res);
	}
}

