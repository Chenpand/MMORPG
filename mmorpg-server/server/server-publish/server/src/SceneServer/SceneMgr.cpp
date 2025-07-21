#include "SceneMgr.h"

#include <AvalonDefine.h>
#include <CLSceneProtocol.h>
#include <CLGameDefine.h>
#include <CLCityMonsterDataEntry.h>
#include <CLNpcDataEntry.h>
#include <CLRecordClient.h>
#include <CLSyncObjectProtocol.h>

#include "SSApplication.h"
#include "SSRouter.h"
#include "Scene.h"
#include "DungeonEntryScene.h"
#include "SSNetwork.h"
#include "PKPrepareScene.h"
#include "PkScene.h"
#include "ActivityScene.h"
#include "CityMonster.h"
#include "ChiJiPrepareScene.h"
#include "TeamCopyScene.h"
#include "ChampionScene.h"
#include "PerformanceMgr.h"

SceneMgr::SceneMgr()
{
	m_activitySceneId = 0;
	PrintPerformanceCountTimer.SetInterval(60 * 1000);
}

SceneMgr::~SceneMgr()
{
	Final();
}

bool SceneMgr::Init(const Avalon::PropertySet* properties)
{
	if(SERVER_TYPE != ST_WSCENE)
	{
		m_IdGenerator.Init(SERVER_ID * MIN_DYNSCENE_ID, (SERVER_ID + 1) * MIN_DYNSCENE_ID - 1);
	}
	else
	{
		m_IdGenerator.Init((SERVER_ID + 100) * MIN_DYNSCENE_ID, (SERVER_ID + 101) * MIN_DYNSCENE_ID - 1);
	}

	return true;
}

void SceneMgr::Final()
{
	CLDeleteVisitor<Scene> visitor;
	m_Scenes.VisitObjects(visitor);
	m_Scenes.Clear();

	for(SceneMap::iterator iter = m_SceneTemplates.begin();
		iter != m_SceneTemplates.end(); ++iter)
	{
		CL_SAFE_DELETE(iter->second);
	}
	m_SceneTemplates.clear();

	for(SceneSet::iterator iter = m_FreeList.begin();
		iter != m_FreeList.end(); ++iter)
	{
		delete *iter;
	}
	m_FreeList.clear();
}

bool SceneMgr::AddScene(Scene *scene)
{
	if(scene == NULL) return false;

	UInt32 sceneid = scene->GetMapID();
	if(!IsStaticMap(scene->GetType()))
	{
		sceneid = m_IdGenerator.Generate();
	}
	
	scene->SetID(sceneid);

	if(!m_Scenes.AddObject(scene))
	{
		return false;
	}

	return true;
}

void SceneMgr::RemoveScene(Scene *scene)
{
	if(scene == NULL || m_Scenes.FindObject(scene->GetID()) == NULL) return;

	m_Scenes.RemoveObject(scene);
	if(!IsStaticMap(scene->GetType()))
	{
		m_IdGenerator.Release(scene->GetID());
	}
}

void SceneMgr::FreeScene(Scene* scene)
{
	if(scene == NULL || scene->GetType() == SCENE_TYPE_GHOST) return;
	m_FreeList.insert(scene);
}

Scene* SceneMgr::FindScene(UInt32 id)
{
	UInt32 realSceneId = GetRealSceneId(id);
	Scene* scene = m_Scenes.FindObject(realSceneId);
	return scene;
}

Scene* SceneMgr::FindNormalScene(UInt32 id)
{
	Scene* scene = FindScene(id);
	if(scene != NULL && scene->GetStatus() == SCENE_NORMAL)
	{
		return scene;
	}
	return NULL;
}

bool SceneMgr::LoadScene(Avalon::UInt32 mapid)
{
	SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(mapid);
	if(dataEntry == NULL || !IsStaticMap(dataEntry->type)) return false;

	char path[256];
	memset(path, 0, sizeof(path));
	avalon_snprintf(path,255,"%sScene/%u/",CONFIG_PATH,mapid);

	std::string scenepath = path;
	std::string configpath = scenepath + "Config.xml";
	
	Scene* scene = CreateSceneByType((SceneType)dataEntry->type);
    if (!scene)
    {
        ErrorLogStream << "create scene(" << mapid << ") type(" << dataEntry->type << ")instance failed." << LogStream::eos;
        return false;
    }

	if(!scene->Load(mapid,configpath))
	{
		FreeScene(scene);
		return false;
	}

	if(!AddScene(scene))
	{
		FreeScene(scene);
		return false;
	}
	
	//注册场景到world
	CLProtocol::WorldRegisterSceneReq req;
	req.sceneid = scene->GetID();
	req.mapid = scene->GetMapID();
	req.name = scene->GetName();
	req.type = scene->GetType();
	req.level = scene->GetLevel();
	req.creater = 0;
	req.pos = dataEntry->revivePos[0];
	
	if(SERVER_TYPE != ST_WSCENE)	Router::SendToWorld(req);
	else	Router::SendToCross(req);

	if (scene->GetType() == SCENE_TYPE_ACTIVITY)
	{
		m_activitySceneId = scene->GetID();
	}

	return true;
}

Scene* SceneMgr::CreateSceneByType(SceneType type)
{
    if (type == SCENE_TYPE_NORMAL)
    {
        return new Scene();
    }
    else if (type == SCENE_TYPE_SINGLE)
    {
        return new SingleScene();
    }
	else if (type == SCENE_TYPE_PK_PREPARE)
	{
		return new PKPrepareScene();
	}
	else if (type == SCENE_TYPE_PK)
	{
		return new PKScene();
	}
	else if (type == SCENE_TYPE_ACTIVITY)
	{
		return new ActivityScene();
	}
	else if (type == SCENE_TYPE_CHIJI_PREPARE)
	{
		return new ChiJiPrepareScene();
	}
	else if (type == SCENE_TYPE_TEAM_COPY)
	{
		return new TeamCopyScene();
	}
	else if (type == SCENE_TYPE_CHAMPION)
	{
		return new ChampionScene();
	}
	else if (type == SCENE_TYPE_LOST_DUNGEON)
	{
		return new Scene();
	}
    else
    {
        return NULL;
    }
}

bool SceneMgr::LoadDungeonEntryScene(UInt32 mapid)
{
    SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(mapid);
    if (dataEntry == NULL || dataEntry->type != SCENE_TYPE_DUNGEON_ENTRY) return false;

    char path[256];
    memset(path, 0, sizeof(path));
    avalon_snprintf(path, 255, "%sScene/%u/", CONFIG_PATH, mapid);

    std::string scenepath = path;
    std::string configpath = scenepath + "Config.xml";

    DungeonEntryScene* scene = new DungeonEntryScene();
    if (!scene->Load(mapid, configpath))
    {
        FreeScene(scene);
        return false;
    }

    if (!AddScene(scene))
    {
        FreeScene(scene);
        return false;
    }

    //注册场景到world
    CLProtocol::WorldRegisterSceneReq req;
    req.sceneid = scene->GetID();
    req.mapid = scene->GetMapID();
    req.name = scene->GetName();
    req.type = SCENE_TYPE_DUNGEON_ENTRY;
    req.level = scene->GetLevel();
    req.creater = 0;
    req.pos = dataEntry->revivePos[0];

    if (SERVER_TYPE != ST_WSCENE)	Router::SendToWorld(req);
    else	Router::SendToCross(req);

    return true;
}

void SceneMgr::VisitScenes(SceneVisitor& visitor)
{
	m_Scenes.VisitObjects(visitor);
}

void SceneMgr::OnTick(const Avalon::Time& now)
{
	static UInt64 s_lastTickTime = 0;
	if (now.MSec() <= s_lastTickTime + SSApplication::Instance()->GetSceneMgrTickIntervalMs())
	{
		return;
	}
	s_lastTickTime = now.MSec();

	class TickVisitor : public SceneVisitor
	{
	public:
		TickVisitor(const Avalon::Time& now):m_Now(now){}

		bool operator()(Scene* scene)
		{
			switch (scene->GetStatus())
			{
			case SCENE_INIT:
				break;
			case SCENE_QUIT:
			case SCENE_NORMAL:
				{
					scene->OnTick(m_Now);
				}
				break;
			}

			return true;
		}

	private:
		Avalon::Time m_Now;
	};

	class PlayerCount
	{
	public:
		PlayerCount()
		{
// 			PerformanceCountMgr::ShowCount(PLAYERSYNCSELFDATA);
// 			PerformanceCountMgr::ShowCount(PLAYERSYNCSELFDATAHASADDR);
// 			PerformanceCountMgr::ShowCount(PLAYERNUM);

			int callSyncSelfDataCount = PerformanceCountMgr::GetCount(PLAYERSYNCSELFDATA);
			int hasDataCount = PerformanceCountMgr::GetCount(PLAYERSYNCSELFDATAHASADDR);
			int totalPlayerNum = PerformanceCountMgr::GetCount(PLAYERNUM);
			int playerTickCount = PerformanceCountMgr::GetCount(PLAYER_TICK);
			int sceneTickCount = PerformanceCountMgr::GetCount(SCENE_TICK);

			InfoLogStream << "[ShowPerf]: call SyncSelfData" << " num(" << callSyncSelfDataCount << ") has data(" << hasDataCount 
							<< ") totalPlayerNum(" << totalPlayerNum << ") playerTickCount(" << playerTickCount << ") sceneTickCount(" 
							<< sceneTickCount << ")." << LogStream::eos;

		};
		~PlayerCount()
		{
			PerformanceCountMgr::CleanCount(PLAYERSYNCSELFDATA);
			PerformanceCountMgr::CleanCount(PLAYERSYNCSELFDATAHASADDR);
			PerformanceCountMgr::CleanCount(PLAYERNUM);
			PerformanceCountMgr::CleanCount(PLAYER_TICK);
			PerformanceCountMgr::CleanCount(SCENE_TICK);
		};
	};

	if (PrintPerformanceCountTimer.IsTimeout(now))
	{
		PlayerCount playerCount;
	}
	TickVisitor visitor(now);
	VisitScenes(visitor);

	for(SceneSet::iterator iter = m_FreeList.begin();
		iter != m_FreeList.end(); ++iter)
	{
		delete *iter;
	}
	m_FreeList.clear();

	PerformanceCountMgr::SetCount(SCENE_TICK, PerformanceCountMgr::GetCount(SCENE_TICK) + 1);
}

/*void SceneMgr::GenActivityCityMonster()
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			auto monsterGroup = CityMonsterGenerateDataEntryMgr::Instance()->FindMonsterSolutionByTypeAndScene(CITY_MONSTER_ACTIVITY, scene->GetMapID());
			if (monsterGroup == NULL)
			{
				return true;
			}

			// 计算刷怪的个数
			UInt32 num = Avalon::Random::RandBetween(monsterGroup->minNum, monsterGroup->maxNum);

			// 获取所有的坐标点
			auto allPos = CityMonsterPositionDataEntryMgr::Instance()->GetAllPos(monsterGroup->sceneId, monsterGroup->posGroupId);

			// 先把目前已有的怪次数填满
			UInt32 curNum = 0;
			scene->VisitCommonCityMonsters([&](CityMonster* monster) -> bool {
				if (monster && monster->GetMonsterType() == CITY_MONSTER_ACTIVITY)
				{
					curNum++;
					auto npc = NpcDataEntryMgr::Instance()->FindEntry(monster->GetDataID());
					if (!npc)
					{
						return true;
					}

					monster->SetRemainTimes(npc->times);
					InfoLogStream << "[CityMonster] refresh " << monster->ToString() << LogStream::eos;

					// 因为npc坐标是唯一的，这里需要把已经用过的坐标点删掉
					allPos.erase(monster->GetPos());
				}
				return true;
			});

			InfoLogStream << "[CityMonster]scene(" << scene->GetID() << ") current monster(" << curNum << ") need gen monster(" << num << ")." << LogStream::eos;

			if (num <= curNum)
			{
				return true;
			}

			SceneNpcInfo info;
			info.sceneId = scene->GetMapID();

			UInt32 needNum = num - curNum;
			for (UInt32 i = 0; i < needNum; i++)
			{
				// 没有坐标点了
				if (allPos.empty())
				{
					InfoLogStream << "[CityMonster]scene(" << scene->GetID() << ") no pos." << LogStream::eos;
					return true;
				}

				auto monsterDataId = monsterGroup->RandMonster();
				auto npc = NpcDataEntryMgr::Instance()->FindEntry(monsterDataId);
				if (!npc)
				{
					continue;
				}

				NpcPos pos;
				{
					// 随机生成一个坐标
					UInt32 index = Avalon::Random::RandBetween(0, allPos.size() - 1);
					auto itr = allPos.begin();
					for (UInt32 i = 0; i < index; i++)
					{
						itr++;
					}

					pos = *itr;
					allPos.erase(itr);
				}

				UInt32 dungeonId = npc->dungeonId;
				UInt32 times = npc->times;
				auto monster = scene->SummonCityMonster(CLRecordClient::Instance()->GenGuid(), 0, npc->name, monsterGroup->monsterType, monsterDataId, pos, dungeonId, times, false);
				if (monster)
				{
					info.npcs.push_back(monster->GetInfo());
				}
			}

			if (!info.npcs.empty())
			{
				protocol.data.push_back(info);
			}

			return true;
		}
		CLProtocol::SceneNpcAdd		protocol;
	};
	
	InfoLogStream << "[CityMonster]start to gen city monsters..." << LogStream::eos;

	CityMonsterSceneVisitor vistor;
	VisitScenes(vistor);

	// 广播给所有玩家
	if (!vistor.protocol.data.empty())
	{
		Router::BroadcastToPlayer(vistor.protocol);
	}
}*/

void SceneMgr::GenActivityCityMonster()
{
	// 先把原来的清掉
	ClearActivityCityMonster();

	// 根据表格生成怪物
	CLProtocol::SceneNpcAdd		protocol;

	std::map<UInt32, std::set<NpcPos>> usedPos;

	auto monsterGroups = CityMonsterGenerateDataEntryMgr::Instance()->FindMonsterSolutionByType(CITY_MONSTER_ACTIVITY);
	for (auto monsterGroup : monsterGroups)
	{
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

		std::set<SceneNpcPos> allPos;
		for (auto sceneId : monsterGroup->sceneIds)
		{
			auto scene = this->FindScene(sceneId);
			if (!scene)
			{
				ErrorLogStream << "scene(" << sceneId << ") gen city monster(" << monsterGroup->id << ") failed, scene not exist." << LogStream::eos;
				continue;
			}

			// 获取所有的坐标点
			auto tmpPoses = CityMonsterPositionDataEntryMgr::Instance()->GetAllPos(sceneId, monsterGroup->posGroupId);
			for (auto& pos : tmpPoses)
			{
				if (usedPos[sceneId].find(pos) == usedPos[sceneId].end())
				{
					SceneNpcPos scenePos;
					scenePos.sceneId = sceneId;
					scenePos.pos = pos;
					allPos.insert(scenePos);
				}
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

			auto scene = this->FindScene(sceneId);
			if (!scene)
			{
				ErrorLogStream << "scene(" << sceneId << ") gen city monster(" << monsterGroup->id << ") failed, scene not exist." << LogStream::eos;
				continue;
			}

			auto monsterDataId = monsterGroup->RandMonster(sceneId);
			auto npc = NpcDataEntryMgr::Instance()->FindEntry(monsterDataId);
			if (!npc)
			{
				continue;
			}

			UInt32 dungeonId = npc->dungeonId;
			UInt32 times = npc->times;
			auto monster = scene->SummonCityMonster(CLRecordClient::Instance()->GenGuid(), 0, npc->name, monsterGroup->monsterType, monsterDataId, pos, dungeonId, times, false);
			if (monster)
			{
				protocol.AddNpc(sceneId, monster->GetInfo());
				usedPos[sceneId].insert(pos);
			}
		}
	}

	// 广播给所有玩家
	if (!protocol.data.empty())
	{
		Router::BroadcastToPlayer(protocol);
	}
}

void SceneMgr::ClearActivityCityMonster()
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		CityMonsterSceneVisitor() {}

		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			std::vector<CityMonster*> monsters;
			scene->VisitCommonCityMonsters([&](CityMonster* monster) {
				if (monster && monster->GetMonsterType() == CITY_MONSTER_ACTIVITY)
				{
					monsters.push_back(monster);
					protocol.guids.push_back(monster->GetID());
				}
				return true;
			});

			for (auto monster : monsters)
			{
				scene->DelCityMonster(monster, false);
			}

			return true;
		}

		CLProtocol::SceneNpcDel		protocol;
	};

	CityMonsterSceneVisitor vistor;
	VisitScenes(vistor);

	// 广播给所有玩家
	if (!vistor.protocol.guids.empty())
	{
		Router::BroadcastToPlayer(vistor.protocol);
	}
}

std::vector<SceneNpcInfo> SceneMgr::GetSceneNpcInfo(Player* player)
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		CityMonsterSceneVisitor(Player* player) : player_(player) {}

		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			SceneNpcInfo info = scene->GetNpcInfo(player_);
			if (info.npcs.empty())
			{
				return true;
			}

			infoes.push_back(info);

			return true;
		}

		Player*						player_;
		std::vector<SceneNpcInfo>	infoes;
	};

	CityMonsterSceneVisitor vistor(player);
	VisitScenes(vistor);
	return vistor.infoes;
}

CityMonster* SceneMgr::FindCityMonster(ObjID_t id)
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		CityMonsterSceneVisitor(ObjID_t id) : id_(id), monster_(NULL) {}

		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			auto monster = scene->FindCityMonster(id_);
			if (monster)
			{
				monster_ = monster;
				return false;
			}

			return true;
		}

		ObjID_t id_;
		CityMonster* monster_;
	};

	CityMonsterSceneVisitor vistor(id);
	VisitScenes(vistor);
	return vistor.monster_;
}

void SceneMgr::RemoveTaskCityMonster(ObjID_t owner, ObjID_t id)
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		CityMonsterSceneVisitor(ObjID_t owner, ObjID_t id) : owner_(owner), id_(id) {}

		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			auto monster = scene->FindCityMonster(owner_, id_);
			if (monster)
			{
				scene->DelCityMonster(monster);
				return false;
			}

			return true;
		}

		ObjID_t owner_;
		ObjID_t id_;
	};

	CityMonsterSceneVisitor vistor(owner, id);
	VisitScenes(vistor);
}

void SceneMgr::RemovePersionalCityMonsters(ObjID_t owner, bool notify)
{
	class CityMonsterSceneVisitor : public CLVisitor<Scene>
	{
	public:
		CityMonsterSceneVisitor(ObjID_t owner, bool notify) : owner_(owner), notify_(notify) {}

		bool operator()(Scene* scene)
		{
			if (!scene)
			{
				return true;
			}

			scene->DelPersonalCityMonsters(owner_, notify_);

			return true;
		}

		ObjID_t owner_;
		bool notify_;
	};

	CityMonsterSceneVisitor vistor(owner, notify);
	VisitScenes(vistor);
}

void SceneMgr::OnGateConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = SSNetwork::Instance()->FindConnection(id);
	if(conn == NULL) return;

	class OnConnectedVisitor : public SceneVisitor
	{
	public:
		OnConnectedVisitor(Avalon::NetConnectionPtr conn):m_pConn(conn){}

		bool operator()(Scene* scene)
		{
			if(scene->GetStatus() != SCENE_NORMAL) return true;

			CLProtocol::GateRegisterScene gatereq;
			gatereq.sceneid = scene->GetID();
			gatereq.name = scene->GetName();
			gatereq.row = scene->GetLength();
			gatereq.col = scene->GetWidth();
			gatereq.syncRange = scene->GetSyncRange();
			gatereq.sceneDataId = scene->GetMapID();
			gatereq.sceneType = scene->GetType();
			CL_SEND_PROTOCOL(m_pConn, gatereq);

			return true;
		}

	private:
		Avalon::NetConnectionPtr m_pConn;
	};
	OnConnectedVisitor visitor(conn);
	VisitScenes(visitor);
}

void SceneMgr::OnCrossConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = SSNetwork::Instance()->FindConnection(id);
	if (conn == NULL) return;

	class OnConnectedVisitor : public SceneVisitor
	{
	public:
		OnConnectedVisitor(Avalon::NetConnectionPtr conn) :m_pConn(conn){}

		bool operator()(Scene* scene)
		{
			CLProtocol::WorldRegisterSceneReq req;
			req.sceneid = scene->GetID();
			req.mapid = scene->GetMapID();
			req.name = scene->GetName();
			req.type = scene->GetType();
			req.level = scene->GetLevel();
			req.creater = 0;
			req.pos = scene->GetDataEntry()->revivePos[0];
			Router::SendToCross(req);

			return true;
		}

	private:
		Avalon::NetConnectionPtr m_pConn;
	};
	OnConnectedVisitor visitor(conn);
	VisitScenes(visitor);
}
