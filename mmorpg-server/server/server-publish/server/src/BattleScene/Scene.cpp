#include "Scene.h"

#include <AvalonPacket.h>
#include <AvalonXmlDocument.h>
#include <AvalonRandom.h>
#include <CLSceneProtocol.h>
#include <CLGateProtocol.h>
#include <CLRoutineProtocol.h>
#include <cmath>

#include "SceneMgr.h"
#include "SceneObject.h"
#include "SSRouter.h"
#include "Player.h"
#include "CityMonster.h"

#include "ScriptMgr.h"
#include "SceneScript.h"


/**
 *@brief 释放对象访问器
 */
class FreeObjVisitor : public SceneObjectVisitor
{
public:
	FreeObjVisitor(bool bFreePlayer):m_bFreePlayer(bFreePlayer){}

	bool operator()(SceneObject* obj)
	{
		if(!m_bFreePlayer && obj->GetType() == SOT_PLAYER) return true;
		SceneObject::FreeObject(obj);
		return true;
	}

private:
	bool m_bFreePlayer;
};

Scene::Scene()
{
	m_MapID = 0;
	m_Status = SCENE_INIT;
	m_pDataEntry = NULL;
	m_PlayerNum = 0;
	m_OldPlayerNum = 0;
    m_GridHeight = m_GridWidth = 1.0f;

	m_KickTimer.SetInterval(1000);
	m_SecTimer.SetInterval(1000);
	m_WaitPlayerQuitTimer.SetInterval(60 * 1000);
}

Scene::Scene(const Scene& scene)
	:CLScene(scene), m_AStar(scene.m_AStar), m_MapID(scene.m_MapID), m_Status(SCENE_INIT), m_pDataEntry(scene.m_pDataEntry)
{
	m_PlayerNum = 0;

	m_KickTimer.SetInterval(1000);
	m_SecTimer.SetInterval(1000);
}

Scene::~Scene()
{	
	m_WaitDisappearMap.clear();
	for(CreatureTimeMap::iterator iter = m_WaitReviveMap.begin();
		iter != m_WaitReviveMap.end(); ++iter)
	{
		SceneObject::FreeObject(iter->first);
	}
	m_WaitReviveMap.clear();

	FreeObjVisitor freeVisitor1(false);
	m_Objects.VisitObjects(freeVisitor1);
	m_Objects.Clear();

	FreeObjVisitor freeVisitor2(true);
	m_WaitObjects.VisitObjects(freeVisitor2);
	m_WaitObjects.Clear();

	for (auto itr : m_AllMonsters)
	{
		auto monster = itr.second;
		if (monster)
		{
			CL_SAFE_DELETE(monster);
		}
	}
	m_AllMonsters.clear();
	m_CommonMonsters.clear();
	m_PlayerMonsters.clear();
}

bool Scene::Load(UInt32 mapid, const std::string &config)
{
	m_MapID = mapid;

	m_pDataEntry = SceneDataEntryMgr::Instance()->FindEntry(mapid);
	if(m_pDataEntry == NULL) return false;

	SetName(m_pDataEntry->name);

	//加载地图信息
	Avalon::XmlDocument doc(config);
	if(!doc.LoadFile()) return false;

	Avalon::XmlElement* mapNode = doc.RootElement();
	Avalon::XmlElement* floorNode = doc.FirstChildElement(mapNode, "Floor");
	if(floorNode == NULL) return false;

	UInt32 row = 0, col = 0;
	doc.GetAttribute(floorNode,"Col",row);
	doc.GetAttribute(floorNode,"Row",col);
	if(row == 0 || col == 0) return false;
	
	CLScene::Init(row, col, m_pDataEntry->syncRange);

	//加载阻挡点信息
	const char* blockStr = doc.GetText(floorNode);
	if(blockStr == NULL) return false;

	UInt32 num = row * col;
	UInt8 *blockInfo = new UInt8[num];
	memset(blockInfo, 0, num);
	for(UInt32 i = 0; i < num; ++i)
	{
		char hex = *blockStr++;
		if(hex == 0) break;

		blockInfo[i] = Avalon::StringUtil::HexToInt(hex);
		char split = *blockStr++;
		if(split == 0) break;
	}

	if(!m_AStar.Init(row, col, blockInfo))
	{
		delete[] blockInfo;
		return false;
	}
	delete[] blockInfo;

    //加载格子信息
    Avalon::XmlElement* gridSizeNode = doc.FirstChildElement(mapNode, "GirdSize");
    if (gridSizeNode)
    {
        doc.GetAttribute(gridSizeNode, "H", m_GridHeight);
        doc.GetAttribute(gridSizeNode, "W", m_GridWidth);
    }

    //加载出生点位置
    Avalon::XmlElement* birthPosNode = doc.FirstChildElement(mapNode, "Birth");
    if (birthPosNode)
    {
        UInt32 x, y;
        doc.GetAttribute(birthPosNode, "X", x);
        doc.GetAttribute(birthPosNode, "Y", y);

        ScenePos pos(x, y);
        m_BirthPos = pos.GetGridePos(GetGridWidth(), GetGridHeight());
    }

    Avalon::XmlElement* doorNode = doc.FirstChildElement(mapNode, "Door");
    while (doorNode != NULL)
    {
        TransportDoor door;
        UInt32 x, y;
        doc.GetAttribute(doorNode, "DoorID", door.id);
        doc.GetAttribute(doorNode, "DoorStartPosX", x);
        doc.GetAttribute(doorNode, "DoorStartPosY", y);
        doc.GetAttribute(doorNode, "TragetSceneID", door.targetSceneId);
        doc.GetAttribute(doorNode, "TargetDoorID", door.targetDoorId);

        ScenePos pos(x, y);
		door.birthScenePos = pos;
        door.birthPos = pos.GetGridePos(GetGridWidth(), GetGridHeight());
        m_doors.push_back(door);

        doorNode = doc.NextElement(doorNode, "Door");
    }

	return true;
}

void Scene::ScriptSetName(const char* name)
{
	if(name != NULL)
	{
		CLScene::ScriptSetName(name);

		CLProtocol::SceneSyncName sync;
		sync.name = name;
		Broadcast(sync);
	}
}


UInt32 Scene::GetMajorCity() const
{
	return m_pDataEntry->majorCity;
}

bool Scene::HasThisDoor(UInt32 doorId)
{
    for (auto& door : m_doors)
    {
        if (door.id == doorId)
        {
            return true;
        }
    }

    return false;
}

CLPosition Scene::GetDoorBirthPos(UInt32 doorId)
{
    for (auto& door : m_doors)
    {
        if (door.id == doorId)
        {
            return door.birthPos;
        }
    }

    return CLPosition();
}

ScenePos Scene::GetDoorBirthScenePos(UInt32 doorId)
{
	for (auto& door : m_doors)
	{
		if (door.id == doorId)
		{
			return door.birthScenePos;
		}
	}

	return ScenePos();
}

CLPosition Scene::GetBirthPoint()
{
    return m_BirthPos;
}

float Scene::GetGridHeight()
{
    return m_GridHeight;
}

float Scene::GetGridWidth()
{
    return m_GridWidth;
}

void Scene::SyncPlayerNumToWorldServer()
{
	UInt32 playerNum = m_PlayerNum;
	if(playerNum != m_OldPlayerNum)
	{
		m_OldPlayerNum = playerNum;
		CLProtocol::SceneSyncPlayerNum sync;
		sync.sceneId = GetID();
		sync.playerNum = playerNum;
		Router::SendToWorld(sync);
	}	
}

void Scene::BroadcastNotify(UInt16 type, UInt8 color, const char* word)
{
	if(word == NULL) return;

	class NotifyVisitor : public PlayerVisitor
	{
	public:
		NotifyVisitor(UInt16 type, UInt8 color, const char* word)
			:m_Type(type), m_Color(color), m_pWord(word){}
		~NotifyVisitor(){}

		bool operator()(Player* player)
		{
			player->Notify(m_Type, m_Color, m_pWord);
			return true;
		}

	private:
		UInt16	m_Type;
		UInt8	m_Color;
		const char*	m_pWord;
	};
	NotifyVisitor visitor(type, color, word);
	VisitPlayers(visitor);
}

UInt32 Scene::GetWaitPlayerNum()
{
	class GetWaitPlayerNumVisitor : public SceneObjectVisitor
	{
	public:
		GetWaitPlayerNumVisitor():m_Num(0){}

		bool operator()(SceneObject* obj)
		{
			if(obj->GetType() == SOT_PLAYER)
			{
				++m_Num;
			}
			return true;
		}

		UInt32 GetNum() const{ return m_Num; }

	private:
		UInt32 m_Num;
	};
	GetWaitPlayerNumVisitor visitor;
	m_WaitObjects.VisitObjects(visitor);
	return visitor.GetNum();
}

bool Scene::AddObject(SceneObject *obj)
{
	if(obj == NULL || obj->IsInScene()) return false;

	if(!IsValidPos(obj->GetPos()))
	{
		CLPosition tmppos = obj->GetPos();
		if(!FindRandPos(tmppos, SCENE_BLOCK_PHYSICS))
		{
			ErrorLogStream << "find rand pos failed when add object to scene:" << GetID() << " " << tmppos.ToString() << LogStream::eos; 
		}
		obj->SetPos(tmppos);
	}

	if(!m_Objects.AddObject(obj)) return false;

	obj->ClearDirty();

	obj->SetScene(this);
	obj->OnBeforeEnterScene();
	RegisterObject(obj);
	obj->OnEnterScene();

	return true;
}

void Scene::RemoveObject(SceneObject* obj)
{
	if(obj == NULL || obj->GetScene() != this) return;
	
	if(m_Objects.FindObject(obj->GetID()) == obj)
	{ 
		obj->OnLeaveScene();

		UnRegisterObject(obj);

		m_Objects.RemoveObject(obj);
	}
	
	m_WaitObjects.RemoveObject(obj);
	obj->SetScene(NULL);
}

SceneObject* Scene::FindObject(ObjID_t id)
{
	if(id == 0) return NULL;
	return m_Objects.FindObject(id);
}

Creature* Scene::FindCreature(ObjID_t id)
{
	SceneObject* obj = FindObject(id);
	if(obj != NULL && IsCreature(obj->GetType())) return (Creature*)obj;
	return NULL;
}

Player* Scene::FindPlayer(ObjID_t id)
{
	SceneObject* obj = FindObject(id);
	if(obj != NULL && obj->GetType() == SOT_PLAYER) return (Player*)obj;
	return NULL;
}

bool Scene::AddWaitObject(SceneObject* obj)
{
	if(obj == NULL || obj->GetScene() != NULL) return false;

	if(m_Objects.FindObject(obj->GetID()) != NULL) return false;
	if(!m_WaitObjects.AddObject(obj)) return false;

	obj->SetScene(this);
	return true;;
}

UInt32 Scene::GetWaitObjectNum() const
{
	return m_WaitObjects.GetNum();
}

void Scene::MoveOutWaitList(SceneObject* obj)
{
	if(obj == NULL || obj->GetScene() != this) return;
	if(m_WaitObjects.FindObject(obj->GetID()) == NULL) return;

	Scene* scene = this;

	if(!scene->m_Objects.AddObject(obj)) return;

	m_WaitObjects.RemoveObject(obj);
	obj->SetScene(scene);

	obj->ClearDirty();

	obj->OnBeforeEnterScene();
	scene->RegisterObject(obj);
	obj->OnEnterScene();
}

SceneObject* Scene::FindWaitObject(ObjID_t id)
{
	return m_WaitObjects.FindObject(id);
}

void Scene::ClearItems()
{
	class ClearItemVisitor : public SceneObjectVisitor
	{
	public:
		ClearItemVisitor(){}

		bool operator()(SceneObject* obj)
		{
			if(obj->GetType() != SOT_ITEM) return true;

			Scene* scene = obj->GetScene();
			obj->GetScene()->RemoveObject(obj);
			obj->SetScene(scene);
			SceneObject::FreeObject(obj);
			return true;
		}
	private:
	};
	ClearItemVisitor visitor;
	VisitSceneObjects(visitor);
}

void Scene::OnSceneInit()
{
	//通知网关注册这个场景
	CLProtocol::GateRegisterScene gatereq;
	gatereq.sceneid = GetID();
	gatereq.name = GetName();
	gatereq.row = GetLength();
	gatereq.col = GetWidth();
	gatereq.syncRange = GetSyncRange();
	gatereq.sceneDataId = GetMapID();
	gatereq.sceneType = GetType();
	Router::BroadcastToGate(gatereq);
	
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnSceneInit(this);
	}
}

void Scene::OnSceneQuit()
{
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnSceneQuit(this);
	}

	//销毁所有npc
	FreeObjVisitor freeVisitor1(false);
	m_Objects.VisitObjects(freeVisitor1);
	m_Objects.Clear();

	FreeObjVisitor freeVisitor2(true);
	m_WaitObjects.VisitObjects(freeVisitor2);
	m_WaitObjects.Clear();

	//通知网关注销这个场景
	CLProtocol::GateUnregisterScene gatereq;
	gatereq.sceneid = GetID();
	Router::BroadcastToGate(gatereq);
}

void Scene::OnSceneUnload()
{
}

void Scene::OnBeforePlayerEnter(Player* player)
{
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnBeforePlayerEnter(player);
	}
}

void Scene::OnBeforeNpcEnter(Npc* npc)
{
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnBeforeNpcEnter(npc);
	}
}

void Scene::OnPlayerEnter(Player* player)
{
	GameInfoLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") enter scene("
		<< GetMapID() << "," << GetName() << ")." << LogStream::eos;

	++m_PlayerNum;

	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnPlayerEnter(player);
	}
}

void Scene::OnPlayerLeave(Player* player)
{
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script != NULL)
	{
		script->OnPlayerLeave(player);
	}

	--m_PlayerNum;

	GameInfoLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ") leave scene("
		<< GetMapID() << "," << GetName() << ")." << LogStream::eos;
}

void Scene::OnTick(const Avalon::Time& now)
{
	/**
	 *@brief 心跳
	 */
	class OnHeartbeatVisitor : public SceneObjectVisitor
	{
	public:
		OnHeartbeatVisitor(const Avalon::Time& now):m_Now(now), m_PlayerNum(0){}

		bool operator()(SceneObject* obj)
		{
			if(!obj->IsInScene()) return true;
			if(obj->GetScene() == NULL) return true;

			if(obj->GetType() == SOT_PLAYER) ++ m_PlayerNum;
			else if(obj->GetScene()->GetType() == SCENE_TYPE_PRIVATESCENE 
				&& obj->GetScene()->GetPlayerNum() == 0) return true;

			obj->OnHeartbeat(m_Now);
			if(obj->IsInScene())
			{
				obj->SyncProperty();
				obj->ClearInvalidData();
			}
			return true;
		}

		UInt32 GetPlayerNum() const{ return m_PlayerNum; }
	private:
		//时间
		Avalon::Time	m_Now;
		//玩家数
		UInt32			m_PlayerNum;
	};
	OnHeartbeatVisitor heartbeatVisitor(now);
	VisitSceneObjects(heartbeatVisitor);

	UInt32 playerNum = heartbeatVisitor.GetPlayerNum();

	if(GetStatus() == SCENE_QUIT)
	{
		if (m_KickTimer.IsTimeout(now))
		{
			//先踢出玩家
			KickPlayersOut();
			OnKickPlayerTimeout();
		}

		if (playerNum == 0 && m_WaitObjects.GetNum() == 0)
		{
			InfoLogStream << "scene(" << GetID() << ", " << GetMapID() << ") quit." << LogStream::eos;

			//没有玩家且没有切场景obj，移除场景
			OnSceneQuit();
			SceneMgr::Instance()->RemoveScene(this);
			SceneMgr::Instance()->FreeScene(this);
			return;
		}
	}
	else if (GetStatus() == SCENE_WAIT_KICK)
	{
		if (m_WaitPlayerQuitTimer.IsTimeout(now))
		{
			InfoLogStream << "scene(" << GetID() << ", " << GetMapID() << ") wait player quit timeout." << LogStream::eos;
			SetStatus(SCENE_QUIT);
		}
	}
	else if(GetStatus() == SCENE_NORMAL)
	{
		//同步人数到worldserver
		SyncPlayerNumToWorldServer();
	}
	
	if(m_SecTimer.IsTimeout(now))
	{
		SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
		if(script != NULL)
		{
			script->OnTick(this, now.MSec());
		}

		// 检查怪物是否已经要消失了
		std::vector<CityMonster*> needRemoveMonsters;
		for (auto itr : m_CommonMonsters)
		{
			auto monster = itr.second;
			if (monster && monster->GetRemainTimes() == 0 && monster->GetReviveTimeStamp() == 0)
			{
				needRemoveMonsters.push_back(monster);
			}
		}

		for (auto itr : m_PlayerMonsters)
		{
			auto& monsters = itr.second;
			for (auto monster : monsters)
			{
				if (monster && monster->GetRemainTimes() == 0)
				{
					needRemoveMonsters.push_back(monster);
				}
			}
		}

		for (auto monster : needRemoveMonsters)
		{
			DelCityMonster(monster);
		}
	}
}

CityMonster* Scene::SummonCityMonster(ObjID_t guid, ObjID_t owner, const std::string& name,
	UInt8 type, UInt32 dataId, NpcPos pos, UInt32 dungeonId, UInt32 remainTimes, bool notify,
	UInt32 battleId, UInt32 battleDyncSceneId, UInt32 reviveTimeSpace)
{
	auto monster = FindCityMonster(owner, guid);
	if (monster)
	{
		InfoLogStream << "monster(" << guid << ", " << dataId << ") is already exist." << LogStream::eos;
		return monster;
	}

	monster = new CityMonster();
	monster->SetID(guid);
	monster->SetName(name);
	monster->SetOwnerID(owner);
	monster->SetSceneID(GetMapID());
	monster->SetMonsterType((CityMonsterType)type);
	monster->SetDataID(dataId);
	monster->SetPos(pos);
	monster->SetDungeonID(dungeonId);
	monster->SetRemainTimes(remainTimes);
	monster->SetTotalTimes(remainTimes);
	monster->SetBattleId(battleId);
	monster->SetBattleDyncSceneId(battleDyncSceneId);
	monster->SetReviveTimeSpace(reviveTimeSpace);

	AddCityMonster(monster, notify);

	return monster;
}

SceneNpcInfo Scene::GetNpcInfo(Player* player)
{
	SceneNpcInfo info;
	info.sceneId = GetMapID();

	// 获取通用的
	for (auto itr : m_CommonMonsters)
	{
		auto monster = itr.second;
		if (!monster)
		{
			continue;
		}

		info.npcs.push_back(monster->GetInfo());
	}

	// 获取玩家私有的
	do 
	{
		if (!player)
		{
			break;
		}

		auto playerItr = m_PlayerMonsters.find(player->GetID());
		if (playerItr == m_PlayerMonsters.end())
		{
			break;
		}

		auto& monsters = playerItr->second;
		for (auto monster : monsters)
		{
			if (!monster)
			{
				continue;
			}

			info.npcs.push_back(monster->GetInfo());
		}
	} while (0);

	return info;
}

void Scene::AddCityMonster(CityMonster* monster, bool notify)
{
	if (!monster)
	{
		return;
	}

	if (monster->GetOwnerID() > 0)
	{
		m_PlayerMonsters[monster->GetOwnerID()].push_back(monster);
	}
	else
	{
		m_CommonMonsters[monster->GetID()] = monster;
	}

	m_AllMonsters[monster->GetID()] = monster;

	monster->OnCreated(notify);

	DebugLogStream << "scene(" << GetMapID() << ") create monster(" << monster->ToString() << ")." << LogStream::eos;
}

void Scene::DelCityMonster(CityMonster* monster, bool notify)
{
	if (!monster)
	{
		return;
	}

	DebugLogStream << "scene(" << GetMapID() << ") remove monster(" << monster->ToString() << ")." << LogStream::eos;

	m_AllMonsters.erase(monster->GetID());

	do 
	{
		if (monster->GetOwnerID() > 0)
		{
			auto itr = m_PlayerMonsters.find(monster->GetOwnerID());
			if (itr == m_PlayerMonsters.end())
			{
				ErrorLogStream << "Scene(" << GetMapID() << ") can't find monster(" << monster->ToString() << ")." << LogStream::eos;
				break;
			}

			auto& monsters = itr->second;
			monsters.erase(remove(monsters.begin(), monsters.end(), monster), monsters.end());

			if (monsters.empty())
			{
				m_PlayerMonsters.erase(itr);
			}
		}
		else
		{
			m_CommonMonsters.erase(monster->GetID());
		}
	} while (0);
	

	monster->OnRemoved(notify);
	delete monster;
}

void Scene::DelPersonalCityMonsters(ObjID_t owner, bool notify)
{
	auto itr = m_PlayerMonsters.find(owner);
	if (itr == m_PlayerMonsters.end())
	{
		return;
	}

	auto monsters = itr->second;
	for (auto monster : monsters)
	{
		DelCityMonster(monster, notify);
	}
}

CityMonster* Scene::FindCityMonster(ObjID_t owner, ObjID_t id)
{
	if (owner == 0) 
	{
		auto itr = m_CommonMonsters.find(id);
		return itr == m_CommonMonsters.end() ? NULL : itr->second;
	}
	else
	{
		auto playerItr = m_PlayerMonsters.find(owner);
		if (playerItr == m_PlayerMonsters.end())
		{
			return NULL;
		}

		auto& monsters = playerItr->second;
		for (auto monster : monsters)
		{
			if (monster && monster->GetID() == id)
			{
				return monster;
			}
		}
	}

	return NULL;
}

CityMonster* Scene::FindCityMonster(ObjID_t id)
{
	auto itr = m_AllMonsters.find(id);
	return itr == m_AllMonsters.end() ? NULL : itr->second;
}

void Scene::VisitCommonCityMonsters(const std::function<bool(CityMonster*)>& F)
{
	for (auto itr : m_CommonMonsters)
	{
		auto monster = itr.second;
		if (!monster)
		{
			continue;
		}

		if (!F(monster))
		{
			return;
		}
	}
}

void Scene::SetBlock(const CLPosition& pos, UInt8 flag)
{
	m_AStar.SetBlock(pos, flag);
}

void Scene::UnsetBlock(const CLPosition& pos, UInt8 flag)
{
	m_AStar.UnsetBlock(pos, flag);
}

bool Scene::CheckBlock(const CLPosition& pos, UInt8 flag)
{
	return m_AStar.CheckBlock(pos, flag);
}

bool Scene::CheckBlock(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag)
{
	if(pos1 == pos2)
	{
		return m_AStar.CheckBlock(pos1, flag);
	}

	CLPosition tarpos;
	if(!FindPosReachable(pos1, pos2, flag, tarpos)) return false;

	return tarpos != pos2;
}

bool Scene::FindPosNearby(const CLPosition& pos, CLPosition& tarpos, UInt8 flag)
{
	if(!IsValidPos(pos)) return false;

	for(Int32 i = 0; i <= NONEGIRD_SEARCH_RANGE; ++i)
	{
		if(FindRandPosOnCircle(pos,i,tarpos,flag)) return true;
	}

	return false;
}

bool Scene::FindPosReachable(const CLPosition& pos1, const CLPosition& pos2, UInt8 flag, CLPosition& tarpos)
{
	if(pos1 == pos2)
	{
		if(!m_AStar.CheckBlock(pos1, flag))
		{
			tarpos = pos1;
			return true;
		}
		return false;
	}

	Int32 offsetX = Int32(pos2.x) - Int32(pos1.x);
	Int32 offsetY = Int32(pos2.y) - Int32(pos1.y);

	tarpos = pos1;
	if(std::abs(offsetX) < std::abs(offsetY))
	{
		Real32 deltaX = Real32(offsetX) / Real32(offsetY);
		Int32 daltaY = offsetY / std::abs(offsetY);

		while(tarpos.y != pos2.y)
		{
			Int32 y = tarpos.y + daltaY;
			if(y < 0) return true;
			Int32 x = pos1.x + (Int32)std::floor(deltaX * (y - Int32(pos1.y)) + 0.5f);
			if(x < 0) return true;

			CLPosition pos;
			pos.x = UInt16(x);
			pos.y = UInt16(y);
			if(m_AStar.CheckBlock(pos, flag)) return true;
			tarpos = pos;
		}
	}
	else
	{
		Int32 daltaX = offsetX / std::abs(offsetX);
		Real32 deltaY = Real32(offsetY) / Real32(offsetX);

		while(tarpos.x != pos2.x)
		{
			
			Int32 x = tarpos.x + daltaX;
			if(x < 0) return true;
			Int32 y = pos1.y + (Int32)std::floor(deltaY * (x - Int32(pos1.x)) + 0.5f);
			if(y < 0) return true;

			CLPosition pos;
			pos.x = UInt16(x);
			pos.y = UInt16(y);
			if(m_AStar.CheckBlock(pos, flag)) return true;
			tarpos = pos;
		}
	}

	return true;
}

bool Scene::FindRandPos(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag)
{
	std::vector<Int32> radiuses;
	for(Int32 i = 0; i <= radius; ++i)
	{
		radiuses.push_back(i);
	}
	std::random_shuffle(radiuses.begin(), radiuses.end());
	for(std::vector<Int32>::iterator iter = radiuses.begin();
		iter != radiuses.end(); ++iter)
	{
		if(FindRandPosOnCircle(center, *iter, pos, blockFlag))
		{
			return true;
		}
	}
	return false;
}

bool Scene::FindRandPos(CLPosition& pos, UInt8 blockFlag, UInt32 edgeWidth)
{
	if(m_AStar.GetXSize() < (2 * edgeWidth + 1) || m_AStar.GetYSize() < (2 * edgeWidth + 1)) return false;

	int i = 0;
	while(i++ < 50)
	{
		pos.x = Avalon::Random::RandBetween(edgeWidth, m_AStar.GetXSize() - edgeWidth - 1);
		pos.y = Avalon::Random::RandBetween(edgeWidth, m_AStar.GetYSize() - edgeWidth - 1);
		if(!IsValidPos(pos)) continue;
		if(!m_AStar.CheckBlock(pos, blockFlag)) return true;
	}

	return FindPosNearby(pos, pos, blockFlag);
}

bool Scene::FindRandPosOnCircle(const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag)
{
	if(radius <= 0)
	{		
		if(m_AStar.CheckBlock(center, blockFlag)) return false;
		pos = center;
		return true;
	}

	int x = int(center.x) - radius;
	int y = int(center.y) - radius;
	int beginIndex = Avalon::Random::RandBetween(0, 8 * radius - 1);
	int index = beginIndex;
	do
	{
		x = int(center.x) - radius;
		y = int(center.y) - radius;

		if(index <= int(2 * radius))	//上边
		{
			x += index;
		}
		else if(index > int(2 * radius) && index <= int(4 * radius)) //右边
		{
			x += 2 * radius;
			y += (index - 2 * radius);
		}
		else if(index > int(4 * radius) && index <= int(6 * radius)) //下边
		{
			x += 6 * radius - index;
			y += 2 * radius;
		}
		else //左边
		{
			y += 8 * radius - index;
		}

		if(x >= 0 && y >= 0)
		{
			CLPosition tmppos;
			tmppos.x = UInt16(x);
			tmppos.y = UInt16(y);
			if(!m_AStar.CheckBlock(tmppos, blockFlag))
			{
				pos = tmppos;
				return true;
			}
		}

		++index;
		index = index % (8 * radius);
	}
	while(index != beginIndex);

	return false;
}

bool Scene::FindRandBlockPosOnCircle( const CLPosition& center, Int32 radius, CLPosition& pos, UInt8 blockFlag )
{
	if(radius <= 0)
	{		
		if(m_AStar.CheckBlock(center, blockFlag)) return false;
		pos = center;
		return true;
	}

	int x = int(center.x) - radius;
	int y = int(center.y) - radius;
	int beginIndex = Avalon::Random::RandBetween(0, 8 * radius - 1);
	int index = beginIndex;
	do
	{
		x = int(center.x) - radius;
		y = int(center.y) - radius;

		if(index <= int(2 * radius))	//上边
		{
			x += index;
		}
		else if(index > int(2 * radius) && index <= int(4 * radius)) //右边
		{
			x += 2 * radius;
			y += (index - 2 * radius);
		}
		else if(index > int(4 * radius) && index <= int(6 * radius)) //下边
		{
			x += 6 * radius - index;
			y += 2 * radius;
		}
		else //左边
		{
			y += 8 * radius - index;
		}

		if(x >= 0 && y >= 0)
		{
			CLPosition tmppos;
			tmppos.x = UInt16(x);
			tmppos.y = UInt16(y);
			if(m_AStar.CheckBlock(tmppos, blockFlag))
			{
				pos = tmppos;
				return true;
			}
		}

		++index;
		index = index % (8 * radius);
	}
	while(index != beginIndex);

	return false;
}

bool Scene::FindPosInDirection(const CLPosition& center, Int32 radius, Int32 dir, CLPosition& pos, UInt8 blockFlag)
{
	if(radius <= 0)
	{		
		if(m_AStar.CheckBlock(center, blockFlag)) return false;
		pos = center;
		return true;
	}

	int beginIndex = 0;
	switch(dir)
	{
		case Avalon::DIR_UP:		beginIndex = radius;		break;
		case Avalon::DIR_DOWN:		beginIndex = 5 * radius;	break;
		case Avalon::DIR_LEFT:		beginIndex = 7 * radius;	break;
		case Avalon::DIR_RIGHT:		beginIndex = 3 * radius;	break;
		case Avalon::DIR_LEFTUP:	beginIndex = 0;				break;
		case Avalon::DIR_RIGHTUP:	beginIndex = 2 * radius;	break;
		case Avalon::DIR_RIGHTDOWN:	beginIndex = 4 * radius;	break;
		case Avalon::DIR_LEFTDOWN:	beginIndex = 6 * radius;	break;
		default:	return false;
	}

	int x = int(center.x) - radius;
	int y = int(center.y) - radius;
	int index = beginIndex;
	do
	{
		x = int(center.x) - radius;
		y = int(center.y) - radius;

		if(index <= int(2 * radius))	//上边
		{
			x += index;
		}
		else if(index > int(2 * radius) && index <= int(4 * radius)) //右边
		{
			x += 2 * radius;
			y += (index - 2 * radius);
		}
		else if(index > int(4 * radius) && index <= int(6 * radius)) //下边
		{
			x += 6 * radius - index;
			y += 2 * radius;
		}
		else //左边
		{
			y += 8 * radius - index;
		}

		if(x >= 0 && y >= 0)
		{
			CLPosition tmppos;
			tmppos.x = UInt16(x);
			tmppos.y = UInt16(y);
			if(!m_AStar.CheckBlock(tmppos, blockFlag))
			{
				pos = tmppos;
				return true;
			}
		}

		++index;
		index = index % (8 * radius);
	}
	while(index != beginIndex);

	return false;
}

bool Scene::FindPath(const CLPosition& startPos, const CLPosition& endPos, std::vector<CLPosition>& path, UInt8 blockFlag)
{
	CLPosition pos;
	if(FindPosNearby(endPos, pos, blockFlag))
	{
		return m_AStar.FindPath(startPos, pos, path, blockFlag);
	}
	return false;
}

void Scene::VisitSceneObjects(SceneObjectVisitor& visitor)
{
	m_Objects.VisitObjects(visitor);
}

void Scene::ScriptVisitPlayers(const char* func)
{
	if(func == NULL) return;
	//调用脚本
	SceneScript* script = ScriptMgr::Instance()->FindSceneScript(GetMapID());
	if(script == NULL) return;

	class ScriptPlayerVisitor : public PlayerVisitor
	{
	public:
		ScriptPlayerVisitor(SceneScript* script, const char* func)
			:m_pScript(script), m_pFuncName(func){}

		bool operator()(Player* player)
		{
			m_pScript->VisitPlayer(m_pFuncName, player);
			return true;
		}
	private:
		SceneScript* m_pScript;
		const char* m_pFuncName;
	};
	ScriptPlayerVisitor visitor(script, func);
	VisitPlayers(visitor);
}

class CastObjVisitor : public CLSceneObjectVisitor
{
public:
	CastObjVisitor(SceneObjectVisitor& visitor):m_Visitor(visitor){}
	~CastObjVisitor(){}

	bool operator()(CLSceneObject* obj)
	{
		SceneObject* sceneobj = dynamic_cast<SceneObject*>(obj);
		if(sceneobj == NULL) return true;

		return m_Visitor(sceneobj);
	}

private:
	//访问器
	SceneObjectVisitor& m_Visitor;
};
void Scene::VisitObjectsAround(const CLPosition& pos, SceneObjectVisitor &visitor)
{
	CastObjVisitor myVisitor(visitor);
	CLScene::VisitObjects(CLScene::GetGridsAround(CLScene::CalculateGridIndex(pos)),myVisitor);
}

class CastPlayerVisitor : public CLSceneObjectVisitor
{
public:
	CastPlayerVisitor(PlayerVisitor& visitor):m_Visitor(visitor){}
	~CastPlayerVisitor(){}

	bool operator()(CLSceneObject* obj)
	{
		Player* player = dynamic_cast<Player*>(obj);
		if(player == NULL) return true;
		return m_Visitor(player);
	}

private:
	//访问器
	PlayerVisitor& m_Visitor;
};

void Scene::VisitPlayersAround(const CLPosition& pos, PlayerVisitor &visitor)
{
	CastPlayerVisitor myVisitor(visitor);
	CLScene::VisitPlayers(CLScene::GetGridsAround(CLScene::CalculateGridIndex(pos)), myVisitor);
}

void Scene::VisitOthersAround(const CLPosition& pos,  SceneObjectVisitor& visitor)
{
	CastObjVisitor myVisitor(visitor);
	CLScene::VisitOthers(CLScene::GetGridsAround(CLScene::CalculateGridIndex(pos)), myVisitor);
}

void Scene::VisitPlayersNewZone(const SceneObject* obj, PlayerVisitor& visitor)
{
	if(obj == NULL) return;
	CastPlayerVisitor myVisitor(visitor);
	CLScene::GridIndexVector grids;
	CLScene::GetGridsAroundExcept(obj->GetGridIndex(), obj->GetOldGridIndex(), grids);
	CLScene::VisitPlayers(grids, myVisitor);
}

void Scene::VisitPlayersOldZone(const SceneObject* obj, PlayerVisitor& visitor)
{
	if(obj == NULL) return;
	CastPlayerVisitor myVisitor(visitor);
	CLScene::GridIndexVector grids;
	CLScene::GetGridsAroundExcept(obj->GetOldGridIndex(), obj->GetGridIndex(), grids);
	CLScene::VisitPlayers(grids, myVisitor);
}

void Scene::VisitObjectsNewZone(const SceneObject* obj, SceneObjectVisitor& visitor)
{
	if(obj == NULL) return;
	CastObjVisitor myVisitor(visitor);
	CLScene::GridIndexVector grids;
	CLScene::GetGridsAroundExcept(obj->GetGridIndex(), obj->GetOldGridIndex(), grids);
	CLScene::VisitObjects(grids,myVisitor);
}

void Scene::VisitObjectsOldZone(const SceneObject* obj, SceneObjectVisitor& visitor)
{
	if(obj == NULL) return;
	CastObjVisitor myVisitor(visitor);
	CLScene::GridIndexVector grids;
	CLScene::GetGridsAroundExcept(obj->GetOldGridIndex(),obj->GetGridIndex(),grids);
	CLScene::VisitObjects(grids,myVisitor);
}

bool Scene::IsInBroadcastZone(const SceneObject* obj1, const SceneObject* obj2) const
{
	if(obj1 == NULL || obj2 == NULL) return false;
	const CLScene::GridIndexVector& grids = CLScene::GetGridsAround(obj1->GetGridIndex());
	for(CLScene::GridIndexVector::const_iterator iter = grids.begin();
		iter != grids.end(); ++iter)
	{
		if(*iter == obj2->GetGridIndex()) return true;
	}
	return false;
}

UInt32 Scene::GetPlayerNumAround(const CLPosition& pos) const
{
	return CLScene::GetPlayerNum(CLScene::GetGridsAround(CLScene::CalculateGridIndex(pos)));
}

void Scene::VisitPlayers(CLVisitor<Player>& visitor)
{
	class ScenePlayerVisitor : public SceneObjectVisitor
	{
	public:
		ScenePlayerVisitor(PlayerVisitor& visitor)
			:m_Visitor(visitor){}

		bool operator()(SceneObject* obj)
		{
			if(obj->GetType() == SOT_PLAYER)
			{
				return m_Visitor((Player*)obj);
			}
			return true;
		}
	private:
		PlayerVisitor& m_Visitor;
	};
	ScenePlayerVisitor playerVisitor(visitor);
	VisitSceneObjects(playerVisitor);
}

void Scene::Broadcast(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		Broadcast(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void Scene::Broadcast(Avalon::Packet* packet)
{
	if(packet == NULL) return;

	class BroadcastVisitor : public PlayerVisitor
	{
	public:
		BroadcastVisitor(Avalon::Packet* packet):m_pPacket(packet){}

		bool operator()(Player* player)
		{
			player->SendPacket(m_pPacket);
			return true;
		}
	private:
		Avalon::Packet* m_pPacket;
	};
	BroadcastVisitor visitor(packet);
	VisitPlayers(visitor);
}

