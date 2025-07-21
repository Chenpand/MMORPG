#include "LostBattleScene.h"
#include <CLCityMonsterDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLRecordClient.h>
#include <CLSyncObjectProtocol.h>
#include <CLLostDungeonResourcesDataEntry.h>
#include <CLSyncObjectProtocol.h>
#include <CLLostDungeonProtocol.h>
#include <CLBattleDefine.h>
#include <CLLostDungeonBattleDataEntry.h>
#include <CLLostDungeonMissionDataEntry.h>
#include <CLDungeonProtocol.h>
#include <CLBattleProtocol.h>

#include "Player.h"
#include "PlayerMgr.h"
#include "CityMonster.h"
#include "SSRouter.h"
#include "SceneMgr.h"

LostBattleScene::LostBattleScene()
{
	m_IsDelayClosing = false;
	m_IsGuardDelayClosing = false;
	m_IsRefreshKillFixNumTask = false;
}

LostBattleScene::~LostBattleScene()
{
	//删除资源点
	for (auto& resItem : m_AllItems)
	{
		CL_SAFE_DELETE(resItem);
	}
	m_AllItems.clear();
	m_TransferNodes.clear();


	CLProtocol::BattleDestoryRes res;
	res.battleType = BTY_DILAO;
	res.battleId = GetBattleId();
	Router::SendToBattle(res);

	InfoLogStream << "lost dungeon battle is destroyed, battle id : " << this->GetBattleId()
		<< ", scene id : " << this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
}

bool LostBattleScene::Load(UInt32 mapid, const std::string& config)
{
	if (!Scene::Load(mapid, config))
	{
		return false;
	}

	//加载地图信息
	Avalon::XmlDocument doc(config);
	if (!doc.LoadFile()) return false;

	Avalon::XmlElement* mapNode = doc.RootElement();
	if (!mapNode)
	{
		return false;
	}

	Avalon::XmlElement* transferNode = doc.FirstChildElement(mapNode, "Transfer");
	while (transferNode)
	{
		UInt32 tmpID = 0;
		UInt32 x = 0;
		UInt32 y = 0;

		doc.GetAttribute(transferNode, "regionId", tmpID);
		doc.GetAttribute(transferNode, "X", x);
		doc.GetAttribute(transferNode, "Y", y);

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

	return true;
}

const ScenePos* LostBattleScene::RandPosition()
{
	if (m_TransferNodes.size() == 0)
	{
		return NULL;
	}

	std::map<UInt32, std::vector<ScenePos>>::iterator tmpIter = m_TransferNodes.begin();

	size_t index = Avalon::Random::RandBetween(0, Int32(m_TransferNodes.size() - 1));
	for (UInt32 i = 0; i < index; ++index)
	{
		tmpIter++;
	}
	
	index = Avalon::Random::RandBetween(0, Int32(tmpIter->second.size() - 1));
	if (index < 0 || index >= tmpIter->second.size())
	{
		InfoLogStream << "error index :" << (Int32)index << LogStream::eos;
		return NULL;
	}

	return &(tmpIter->second[index]);
}

void LostBattleScene::Init()
{
	GenMonsters();
	GenResources();
	GenDugeonEntrySceneDynamics();

	UInt32  destroyDelayTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_DELAY_TIME);
	destroyDelayTime = (destroyDelayTime == 0) ? 10 : destroyDelayTime;
	UInt32  destroyDelayGuardTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_DESTRUCTION_TIME);
	destroyDelayGuardTime = (destroyDelayGuardTime == 0) ? 20 : destroyDelayGuardTime;
	m_DestroyDelayTimer.SetInterval(destroyDelayTime * 60 * 1000);
	m_DestroyDelayGuardTimer.SetInterval(destroyDelayGuardTime * 60 * 1000);
}

void LostBattleScene::Destroy(UInt8 reason)
{
	InfoLogStream << "begin destroy battle scene id : " << GetBattleId() << ", reason : " << (UInt32)reason 
		<< ", BattlePlayerNum : " << BattlePlayerNum() <<LogStream::eos;
	if (LDCBR_BAT_DYNC_ADMIN == reason) //战场动态管理
	{
		InfoLogStream << "lost dungeon battle player is destroy by dynamic admin, battle id : " << this->GetBattleId()
			<< ", scene id : " << this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
		m_KickTimer.SetInterval(1000);
		this->SetStatus(SCENE_QUIT);
	}
	else if (LDCBR_ACT_CLOSE == reason) //活动结束
	{
		if (BattlePlayerNum() == 0) //没有人
		{
			InfoLogStream << "lost dungeon active is over, battle player num is 0, change status to SCENE_QUIT, battle id : " << this->GetBattleId()
				<< ", scene id : "<< this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
			m_KickTimer.SetInterval(1000);
			this->SetStatus(SCENE_QUIT);
		}
		else //有人则延迟关闭
		{
			InfoLogStream << "lost dungeon active is over, battle player num is not 0, enter delay destroy stage, battle id : " << this->GetBattleId()
				<< ", scene id : " << this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
			BroadcastNotifyById(4800021);
			m_DestroyDelayTimer.SetLastTime(0);
			m_DestroyDelayGuardTimer.SetLastTime(0);
			//this->SetStatus(SCENE_WAIT_KICK);
			this->SetDelayClose(true);
		}
	}
	else
	{
		ErrorLogStream << "Destroy battle scene err, reason is err : " << (UInt32)reason << LogStream::eos;
	}
}

void LostBattleScene::KickPlayersOut()
{
	class KickPlayersVisitor : public PlayerVisitor
	{
	public:
		KickPlayersVisitor(LostBattleScene* scene)
			:m_pScene(scene) {}

		bool operator()(Player* player)
		{
			m_pScene->KickPlayerOut(player);
			return true;
		}
	private:
		LostBattleScene*	m_pScene;
	};

	KickPlayersVisitor visitor(this);
	VisitPlayers(visitor);
}

void LostBattleScene::KickPlayerOut(Player* player)
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

	// 回到原来的城镇
	CLProtocol::BattleExitSceneReq exit;
	exit.roleId = player->GetID();
	exit.battleType = BTY_DILAO;
	exit.result = LDBR_OVER;
	exit.battleId = GetBattleId();
	player->SetLostDungLeaveTag(true);
	Router::SendToSelfScene(player->GetSrcSceneNodeID(), exit);

	InfoLogStream << "kick player(" << player->GetID() << ") back to self scene." << LogStream::eos;

	m_KickTimer.SetLastTime(0);
	m_KickTimer.SetInterval(10000);
}

void LostBattleScene::OnKickPlayerTimeout()
{
	class RemovePlayersVisitor : public PlayerVisitor
	{
	public:
		RemovePlayersVisitor(LostBattleScene* scene)
			:m_pScene(scene) {}

		bool operator()(Player* player)
		{
			InfoLogStream << "scene(" << m_pScene->GetID() << ", " << m_pScene->GetMapID() << ") kick player(" << player->GetID() << ") timeout, jest delete player." << LogStream::eos;
			m_pScene->RemoveObject(player);
			player->SetGameStatus(PS_DESTORY);
			return true;
		}
	private:
		LostBattleScene*	m_pScene;
	};

	RemovePlayersVisitor vistor(this);
	VisitPlayers(vistor);
}

UInt64 LostBattleScene::GenResourceId()
{
	// 生成唯一的ID,括号内为相应字段的位数
	// NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 id = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return id;
}

void LostBattleScene::GenMonsters()
{
	auto monsterGroups = CityMonsterGenerateDataEntryMgr::Instance()->FindMonsterSolutionByTypeAndSceneId(CITY_MONSTER_LOST_DUNGEON, GetMapID());

	for (auto monsterGroup : monsterGroups)
	{
		if (monsterGroup->sceneIds.size() != 1)
		{
			ErrorLogStream << "lost battle(" << GetBattleId() << "), sceneId(" << GetMapID() << ") gen monsters failed. monsterGroup->sceneIds.size() = "
				<< (UInt32)monsterGroup->sceneIds.size() << LogStream::eos;
			continue;;
		}

		// 计算刷怪的个数
		UInt32 num = Avalon::Random::RandBetween(monsterGroup->minNum, monsterGroup->maxNum);
		struct SceneNpcPos
		{
			bool operator <(const SceneNpcPos& other) const
			{
				if (sceneId != other.sceneId)
				{
					return sceneId < other.sceneId;
				}

				return pos < other.pos;
			}

			UInt32 sceneId;
			NpcPos pos;
		};

		std::set<NpcPos> usedPos;
		std::set<SceneNpcPos> allPos;
		UInt32 sceneId = monsterGroup->sceneIds[0];

		// 获取所有的坐标点
		auto tmpPoses = CityMonsterPositionDataEntryMgr::Instance()->GetAllPos(sceneId, monsterGroup->posGroupId);
		for (auto& pos : tmpPoses)
		{
			if (usedPos.find(pos) == usedPos.end())
			{
				SceneNpcPos scenePos;
				scenePos.sceneId = sceneId;
				scenePos.pos = pos;
				allPos.insert(scenePos);
			}
		}
		UInt32 needNum = num;
		for (UInt32 i = 0; i < needNum; i++)
		{
			// 没有坐标点了
			if (allPos.empty())
			{
				ErrorLogStream << "gen city monster(" << monsterGroup->id << ") failed, no empty pos." << LogStream::eos;
				break;
			}

			NpcPos pos;
			UInt32 sceneId;
			{
				// 随机生成一个坐标
				UInt32 index = Avalon::Random::RandBetween(0, allPos.size() - 1);
				auto itr = allPos.begin();
				for (UInt32 i = 0; i < index; i++)
				{
					itr++;
				}

				sceneId = itr->sceneId;
				pos = itr->pos;
				allPos.erase(itr);
			}
			auto monsterDataId = monsterGroup->RandMonster(sceneId);
			auto npc = NpcDataEntryMgr::Instance()->FindEntry(monsterDataId);
			if (!npc)
			{
				continue;
			}
			UInt32 dungeonId = npc->dungeonId;
			UInt32 times = npc->times;
			UInt32 reviveTimeSpace = npc->reviveTime;
			auto monster = this->SummonCityMonster(CLRecordClient::Instance()->GenGuid(), 0, npc->name, monsterGroup->monsterType,
				monsterDataId, pos, dungeonId, times, false, m_battleId, GetID(), reviveTimeSpace);
			if (monster)
			{
				usedPos.insert(pos);
			}
		}
	}
}

void LostBattleScene::GenResources()
{
	std::vector<LostDungeonResDataEntry*>& datas = LostDungeonResDataEntryMgr::Instance()->GetDatasByMapId(GetMapID());
	std::map<UInt32, std::vector<ScenePos>> allPos;
	for (auto data : datas)
	{
		auto it = m_ResourceNodes.find(data->resGroupId);
		if (it == m_ResourceNodes.end())	continue;
		auto iter = allPos.find(data->resGroupId);
		if (iter == allPos.end())
		{
			allPos[data->resGroupId] = it->second;
		}
	}

	for (auto data : datas)
	{
		if (!data)	continue;
		//随机坐标
		//auto it = m_ResourceNodes.find(data->resGroupId);
		//if (it == m_ResourceNodes.end())	continue;
		auto it = allPos.find(data->resGroupId);
		if (it == allPos.end())	continue;

		std::vector<ScenePos>& posv = it->second;
		if (posv.size() == 0)	continue;
		UInt32 index = Avalon::Random::RandBetween(0, posv.size() -1);
		ScenePos pos = posv[index];
		posv.erase(posv.begin() + index);
		SceneItem* item = new SceneItem;
		item->resourceId = data->id;
		item->resPosGroupId = data->resGroupId;
		item->pos.x = pos.GetX();
		item->pos.y = pos.GetY();
		//随机item
		UInt32 itemIndex = Avalon::Random::RandBetween(0, data->items.size() - 1);
		UInt32 itemId = data->items[itemIndex];
		item->dataId = itemId;
		item->guid = GenResourceId();
		item->refreshTime = (UInt32)CURRENT_TIME.Sec() + data->initRefreshTime;
		item->refreshed = false;
		m_AllItems.push_back(item);
	}
}

void LostBattleScene::GenDugeonEntrySceneDynamics()
{
	//加载动态地下城选择场景
	const std::vector<TransportDoor>& doors = GetDoors();
	for (auto door : doors)
	{
		SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(door.targetSceneId);
		if (!dataEntry || dataEntry->type != SCENE_TYPE_DUNGEON_ENTRY_DYNAMIC)
		{
			continue;
		}

		DungeonEntrySceneDynamic* scene = dynamic_cast<DungeonEntrySceneDynamic*>(SceneMgr::Instance()->CreateDungeonEntrySceneDynamic(GetID(), door.id, door.targetSceneId));
		if (scene)
		{
			DungeonEntrySceneDynamicKey key(GetID(), door.id, door.targetSceneId);
			m_DungeonEntrySceneDynamics.insert(key);
			scene->SetMajorCity(GetID());
			scene->SetMajorDoor(door.id);
			scene->SetBattleId(GetBattleId());
		}
	}
}

void LostBattleScene::DelAllDungeonEntrySceneDynamics()
{
	InfoLogStream << "lost dungeon battle destroy all dungeon entry scenes , battle id : " << this->GetBattleId()
		<< ", scene id : " << this->GetID() << LogStream::eos;
	//销毁动态地下城入口场景
	for (auto key : m_DungeonEntrySceneDynamics)
	{
		DungeonEntrySceneDynamic* dungeonEntryScene = dynamic_cast<DungeonEntrySceneDynamic*>(SceneMgr::Instance()->GetDungeonEntrySceneDynamic(key.ownSceneId, key.curDoorId, key.sceneId));
		if (dungeonEntryScene)
		{
			dungeonEntryScene->Destroy();
		}
	}
	m_DungeonEntrySceneDynamics.clear();
}

bool LostBattleScene::VistResourceItems(const std::function<bool(SceneItem*)>& ff)
{
	for (auto item : m_AllItems)
	{
		if (!ff(item))
		{
			return false;
		}
	}
	return true;
}

SceneItem* LostBattleScene::FindResourceItem(UInt64 guid)
{
	for (auto item : m_AllItems)
	{
		if (item->guid == guid)
		{
			return item;
		}
	}
	return NULL;
}

void LostBattleScene::RemoveResourceItem(UInt64 guid)
{
	for (std::vector<SceneItem*>::iterator itemIter = m_AllItems.begin(); itemIter != m_AllItems.end(); ++itemIter)
	{
		SceneItem* item = *itemIter;
		if (item->guid == guid)
		{
			m_AllItems.erase(itemIter);
			CL_SAFE_DELETE(item);
			break;
		}
	}
	CLProtocol::SceneLostDungeonResourceDel notify;
	notify.battleID = GetBattleId();
	notify.guids.push_back(guid);

	Broadcast(notify);
}

UInt32 LostBattleScene::OnPlayerPickUpItem(Player* player, UInt64 guid)
{
	if (!player) return ErrorCode::ITEM_DATA_INVALID;
	SceneItem* resItem = FindResourceItem(guid);
	if (!resItem)	return ErrorCode::ITEM_DATA_INVALID;
	ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(resItem->dataId);
	if (NULL == itemEntry)
	{
		InfoLogStream << "player:" << player->GetID() << ", item:" << guid << ", data id:" << resItem->dataId << ", not exist" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (itemEntry->subType == ST_CHIJI_BUFF)
	{
		if (itemEntry->onUseBuff != 0)
		{
			player->AddBuff(itemEntry->onUseBuff);
		}
	}
	
	UInt32 resId = resItem->resourceId;
	UInt32 resPosGroupId = resItem->resPosGroupId;
	RemoveResourceItem(guid);

	//重新刷组内一个资源点
	LostDungeonResDataEntry* data = LostDungeonResDataEntryMgr::Instance()->FindEntry(resId);
	if (data)
	{
		//随机坐标
		auto it = m_ResourceNodes.find(data->resGroupId);
		if (it == m_ResourceNodes.end())	return ErrorCode::SUCCESS;
		std::vector<ScenePos> posv = it->second;
		if (posv.size() == 0)	return ErrorCode::SUCCESS;
		std::vector<ScenePos> usedPosv;
		for (auto item : m_AllItems)
		{
			if (item->resPosGroupId == resPosGroupId)
			{
				ScenePos pos(item->pos.x, item->pos.y);
				usedPosv.push_back(pos);
			}
		}

		//去掉已经生成的坐标
		auto iter = posv.begin();
		for (; iter != posv.end();)
		{
			bool find = false;
			for (auto pos : usedPosv)
			{
				if (pos.GetX() == iter->GetX() 
					&& pos.GetY() == iter->GetY())
				{
					find = true;
					break;
				}
			}
			if (find)
			{
				iter = posv.erase(iter);
				continue;
			}
			else
			{
				++iter;
			}
		}

		if (posv.size() == 0)	return ErrorCode::SUCCESS;

		UInt32 index = Avalon::Random::RandBetween(0, posv.size() - 1);
		ScenePos pos = posv[index];
		SceneItem* item = new SceneItem;
		item->resourceId = data->id;
		item->pos.x = pos.GetX();
		item->pos.y = pos.GetY();
		//随机item
		UInt32 itemIndex = Avalon::Random::RandBetween(0, data->items.size() - 1);
		UInt32 itemId = data->items[itemIndex];
		item->dataId = itemId;
		item->guid = GenResourceId();
		item->refreshTime = (UInt32)CURRENT_TIME.Sec() + data->intervalRefreshTime;
		item->refreshed = false;
		m_AllItems.push_back(item);
	}

	return ErrorCode::SUCCESS;
}

void LostBattleScene::OnPlayerEnter(Player* player)
{
	if (!player)	return;
	Scene::OnPlayerEnter(player);

	//同步小怪
	CLProtocol::SceneNpcList notify;
	SceneNpcInfo info;
	info.sceneId = GetMapID();

	VisitCommonCityMonsters([&](CityMonster* monster) {
		if (!monster || monster->GetReviveTimeStamp() > 0) return true;
		info.npcs.push_back(monster->GetInfo());
		return true;
	});
	notify.infoes.push_back(info);
	player->SendProtocol(notify);

	InfoLogStream << "enter lost dungeon scene" << GetMapID() << " battleId:" << m_battleId 
		<< "playerId:" << player->GetID() << LogStream::eos;

	//同步资源
	CLProtocol::SceneLostDungeonSyncResourcesList syncItems;
	syncItems.battleID = GetBattleId();
	SceneItemInfo itemInfo;
	info.sceneId = GetMapID();
	UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
	VistResourceItems([&](SceneItem* item)
	{
		if (!item) return true;
		if (item->refreshTime > currTm) return true;
		itemInfo.items.push_back(*item);
		return true;
	}
	);

	if (itemInfo.items.size() > 0)
	{
		syncItems.infoes.push_back(itemInfo);
		player->SendProtocol(syncItems);
	}

	player->OnEnterLostDungeon(false);

	m_PlayerSet.insert(player->GetID());

	CLProtocol::BattleLostDungSyncBattleData sync;
	sync.battleId = GetBattleId();
	sync.playerNum = BattlePlayerNum();
	Broadcast(sync);
}

void LostBattleScene::OnTick(const Avalon::Time& now)
{
	Scene::OnTick(now);

	if (m_IsRefreshKillFixNumTask && m_KillFixNumTimer.IsTimeout(now))
	{
		RefreshFixNumTask();
	}

	if (m_IsDelayClosing)
	{
		//延迟关闭时间到了
		if (m_DestroyDelayTimer.IsTimeout(now))
		{
			InfoLogStream << "lost dungeon battle delay destroy timer is timeout, battle id : " << this->GetBattleId()
				<< ", scene id : " << this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
			CLProtocol::SceneNotifyClientLeaveBattle notify;
			Broadcast(notify);
			BroadcastToDungeonEntrys(notify);
			m_IsDelayClosing = false;
			m_IsGuardDelayClosing = true;
		}
	}

	if (m_IsGuardDelayClosing)
	{
		//保底关闭时间到了
		if (m_DestroyDelayGuardTimer.IsTimeout(now))
		{
			InfoLogStream << "lost dungeon battle guard destroy timer is timeout, change status to SCENE_QUIT, battle id : " << this->GetBattleId()
				<< ", scene id : " << this->GetID() << ", floor : " << this->GetFloor() << LogStream::eos;
			m_KickTimer.SetInterval(1000);
			m_KickTimer.SetLastTime(CURRENT_TIME.MSec());
			this->SetStatus(SCENE_QUIT);
			m_IsGuardDelayClosing = false;
		}
	}

	CLProtocol::SceneNpcAdd notify;
	SceneNpcInfo info;
	info.sceneId = GetMapID();

	VisitCommonCityMonsters([&](CityMonster* monster) {
		if (!monster || monster->GetReviveTimeStamp() == 0) return true;
		if (monster->GetReviveTimeStamp() < (UInt32)CURRENT_TIME.Sec())
		{
			monster->SetReviveTimeStamp(0);
			monster->SetRemainTimes(monster->GetTotalTimes());
			info.npcs.push_back(monster->GetInfo());
		}
		return true;
	});
	if (info.npcs.size() > 0)
	{
		notify.data.push_back(info);
		Broadcast(notify);
	}

	CLProtocol::SceneLostDungeonResourceAdd notifyAddItems;
	notifyAddItems.battleID = GetBattleId();

	UInt32 currTm = (UInt32)CURRENT_TIME.Sec();
	VistResourceItems([&](SceneItem* item) {
		if (!item) return true;
		if (item->refreshTime > currTm) return true;
		if (item->refreshed) return true;
		notifyAddItems.AddItem(GetMapID(), *item);
		item->refreshed = true;
		return true;
	}
	);

	if (notifyAddItems.data.size() > 0)
	{
		Broadcast(notifyAddItems);
	}
}

void  LostBattleScene::OnMonsterKilled(CityMonster* monster)
{
	if (!monster) return;
	
	if (monster->GetRemainTimes() == 0)
	{
		UInt32 currTime = (UInt32)CURRENT_TIME.Sec();
		monster->SetReviveTimeStamp(currTime + monster->GetReviveTimeSpace());
		
		CLProtocol::SceneNpcDel del;
		del.guids.push_back(monster->GetID());
		Broadcast(del);
	}
}

void LostBattleScene::OnSceneQuit()
{
	Scene::OnSceneQuit();
	SceneMgr::Instance()->DelDynamicScene(GetBattleId());

	DelAllDungeonEntrySceneDynamics();
}

void LostBattleScene::BroadcastToDungeonEntrys(Avalon::Protocol& protocol)
{
	for (auto key : m_DungeonEntrySceneDynamics)
	{
		DungeonEntrySceneDynamic* dungeonEntryScene = dynamic_cast<DungeonEntrySceneDynamic*>(SceneMgr::Instance()->GetDungeonEntrySceneDynamic(key.ownSceneId, key.curDoorId, key.sceneId));
		if (dungeonEntryScene)
		{
			dungeonEntryScene->Broadcast(protocol);
		}
	}
}

void LostBattleScene::AddTaskAcceptNum(UInt32 taskId)
{
	m_TaskAcceptMap[taskId]++;
}

UInt32 LostBattleScene::GetTaskAcceptNum(UInt32 taskId)
{
	auto iter = m_TaskAcceptMap.find(taskId);
	return iter != m_TaskAcceptMap.end() ? iter->second : 0;
}

void LostBattleScene::RemoveTaskAcceptNum(UInt32 taskId)
{
	auto iter = m_TaskAcceptMap.find(taskId);
	if (iter == m_TaskAcceptMap.end())
		return;

	if (iter->second > 0)
	{
		m_TaskAcceptMap[taskId]--;
	}
}

void LostBattleScene::RefreshFixNumTask()
{
	for (auto& it : m_PlayerSet)
	{
		Player* player = PlayerMgr::Instance()->FindPlayer(it);
		if (NULL == player)
		{
			ErrorLogStream << "not find player:" << it << LogStream::eos;
		}

		if ( !player->GetLostDungeonTaskMgr().IsKillFixNumTask())
		{
			continue;
		}

		if (player->GetLostDungeonTaskMgr().IsFinishTask())
		{
			continue;
		}

		player->SendNotify(4800022);
		player->GetLostDungeonTaskMgr().SetTaskRefresh(true);
	}
}

UInt64 LostBattleScene::RandGetPlayer(UInt64 exceptId)
{
	if (m_PlayerSet.empty() || m_PlayerSet.size() == 1)
	{
		return 0;
	}

	UInt32 randNum = Avalon::Random::RandBetween(0, m_PlayerSet.size() - 1);
	auto iter = m_PlayerSet.begin();
	std::advance(iter, randNum);

	if (*iter == exceptId)
	{
		if (iter == m_PlayerSet.begin())
			return *(++iter);
		else
			return *(--iter);
	}

	return *iter;
}

std::set<UInt64>& LostBattleScene::RandGetPlayerList()
{
	return m_PlayerSet;
}

void LostBattleScene::OnPlayerBeforeEnterBattle(Player* player)
{
	if (!player) return;
	const ScenePos* tmpPos = this->RandPosition();
	if (tmpPos)
	{
		player->SetScenePos(tmpPos->GetX(), tmpPos->GetY());
		player->SetPos(tmpPos->GetGridePos(this->GetGridWidth(), this->GetGridHeight()));
	}
	else
	{
		ErrorLogStream << "player(" << player->GetID() << ") , can't rand a point " << LogStream::eos;
	}
}

void LostBattleScene::OnPlayerEnterBattle(Player* player)
{
	if (!player) return;

	m_PlayerSet.insert(player->GetID());
	if (m_PlayerSet.size() > 1)
	{
		m_IsRefreshKillFixNumTask = false;
	}

	player->SetLostDungeonHpBack(player->GetLostDungeonHp());
	player->SetLostDungeonMpBack(player->GetLostDungeonMp());
	player->SetLostDungeonSpeedBack(player->GetMoveSpeed());
	UInt32 protTm = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_BIRTH_PROTECT_TIME);
	protTm = (protTm == 0) ? 10 : protTm;
	player->SetLostDungProtTime(protTm + (UInt32)CURRENT_TIME.Sec());
	player->SetLastLostBattleSceneId(this->GetID());
	player->SetLostDungLeaveTag(false);

	//加速神器
	player->GetBuffMgr().ClearBuff();
	class Visitor : public ItemVisitor
	{
	public:
		Visitor(Player* player) : m_player(player) {}
		~Visitor() {};
		bool operator()(Item* item)
		{
			if (!item) return true;
			if (item->GetSubType() == ST_LOST_DUNGEON_ADD_SPEED
				&& item->GetDataEntry())
			{
				m_player->AddBuff(item->GetDataEntry()->onUseBuff);
			}

			return true;
		}
	public:
		Player*	m_player;
	};

	Visitor vistior(player);
	player->VisitItems(PACK_LOSTDUNGEON, vistior);
}

void LostBattleScene::OnPlayerLeaveBattle(Player* player, UInt8 result)
{
	if (!player)	return;
	UInt64 playerID = player->GetID();

	m_PlayerSet.erase(playerID);
	if (m_PlayerSet.size() == 1)
	{
		m_IsRefreshKillFixNumTask = true;
		m_KillFixNumTimer.SetLastTime(CURRENT_TIME.MSec());
		m_KillFixNumTimer.SetInterval(60 * 1000);
	}

	InfoLogStream << "begin leave lost dungeon scene" << GetMapID() << " battleId:" << GetBattleId()
		<< "playerId:" << playerID  <<", result : " << (UInt32)result << LogStream::eos;
	

	RemoveTaskAcceptNum(player->GetLostDungeonTaskMgr().CurTaskId());
	player->GetLostDungeonTaskMgr().NotifyKillMePlayer();
	player->GetLostDungeonTaskMgr().NotifyMeKillPlayer();
	

	//通知BattleServer
	CLProtocol::BattleExitNotify notify;
	notify.roleId = playerID;
	notify.battleId = GetBattleId();
	notify.battleType = BTY_DILAO;
	notify.battleResult = result;
	notify.score = 0;
	notify.btState = player->GetLostDungBatSt();

	if (result == LDBR_PASS)
	{
		auto entry = LostDungeonMissionDataEntryMgr::Instance()->FindEntry(player->GetLostDungeonTaskMgr().CurTaskId());
		if (entry)
		{
			UInt32 addScore = entry->score;
			UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_FADIAN_ITEM);
			if (player->GetItemNum(itemId) > 0)
			{
				UInt32 addPro = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_FADIAN_SCORE_ADD);
				addScore = addScore * (100 + addPro) / 100;
			}
			notify.score = addScore;
		}
		else {
			ErrorLogStream <<"not find LostDungeonMissionDataEntry id : " << player->GetLostDungeonTaskMgr().CurTaskId() 
				<< ", playerID : " << playerID << ", battleId : " << GetBattleId() << LogStream::eos;
		}
	}
	Router::SendToBattle(notify);

	//同步数据
	CLProtocol::BattleLostDungSyncDataToSelfScene syncSelfData;
	syncSelfData.roleId = player->GetID();
	if (result == LDBR_PASS)
	{
		syncSelfData.lostDungeonHp = player->GetLostDungeonHp();
		syncSelfData.lostDungeonMp = player->GetLostDungeonMp();
	}
	else
	{
		syncSelfData.lostDungeonHp = player->GetLostDungeonHpBack();
		syncSelfData.lostDungeonMp = player->GetLostDungeonMpBack();
	}
	player->SendToSelfScene(syncSelfData);

	CLProtocol::BattleLostDungSyncBattleData sync;
	sync.battleId = GetBattleId();
	sync.playerNum = m_PlayerSet.size();
	Broadcast(sync);

	player->SetBabttleID(0);
	player->SetLastLostBattleSceneId(0);
}

bool LostBattleScene::IsBattleExitDoor(UInt32 doorId)
{
	if (doorId == 0) return false;

	auto battleData = LostDungeonBattleDataEntryMgr::Instance()->FindEntry(GetBattleDataId());
	if (!battleData)	return false;

	return battleData->passExitDoorId == doorId;
}

void LostBattleScene::OnPlayerRevive(ObjID_t playerId, ObjID_t targetId, UInt32 reviveId, UInt32 reviveItem, UInt32 reviveItemNum)
{
	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_LOSTDUNG_DUNG_REVIVE_LUNHUI_ITEM);
	if (itemId != reviveItem)
	{
	}
	CLProtocol::DungeonSvrReviveReq req;
	req.playerId = playerId;
	req.targetId = targetId;
	req.reviveId = reviveId;
	req.reviveItem = reviveItem;
	req.reviveItemNum = reviveItemNum;
	req.reviveHp = HP_MP_FULL_VALUE / 2;
	req.reviveMp = HP_MP_FULL_VALUE / 2;

	Router::SendToBattle(req);
}