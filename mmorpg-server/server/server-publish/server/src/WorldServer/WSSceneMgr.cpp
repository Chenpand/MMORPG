#include "WSSceneMgr.h"

#include <AvalonFile.h>
#include <AvalonXmlDocument.h>
#include <CLSceneProtocol.h>
#include "WSNetwork.h"
#include "WSPlayerMgr.h"
#include "WSActivityMgr.h"

WSSceneMgr::WSSceneMgr()
{
	m_BattleSceneNode = 0;
}

WSSceneMgr::~WSSceneMgr()
{
}

bool WSSceneMgr::Init()
{
	Avalon::XmlDocument* doc = NULL;

	doc = new Avalon::XmlDocument("../SceneList.xml");
	if(!doc->LoadFile())
	{
		CL_SAFE_DELETE(doc);

		std::string config = CONFIG_PATH;
		doc = new Avalon::XmlDocument(config + "SceneList.xml");
		if(!doc->LoadFile())
		{
			CL_SAFE_DELETE(doc);
			return false;
		}
	}

	Avalon::XmlElement* scenenode= doc->FirstChildElement(doc->RootElement(),"sceneserver");
	while(scenenode != NULL)
	{
		UInt32 sceneid = 0;
		if(doc->GetAttribute(scenenode,"id",sceneid))
		{
			MapList maplist;
			Avalon::XmlElement* mapnode = doc->FirstChildElement(scenenode, "map");
			while(mapnode != NULL)
			{
				UInt32 mapid = 0;
				if(doc->GetAttribute(mapnode,"id",mapid))
				{
					maplist.push_back(mapid);
				}
				mapnode = doc->NextElement(mapnode,"map");
			}
			m_SceneCfg.insert(std::make_pair(sceneid,maplist));

			std::map<UInt32, UInt32> publicCopys;
			mapnode = doc->FirstChildElement(scenenode, "publiccopy");
			while(mapnode != NULL)
			{
				UInt32 mapid = 0;
				UInt32 level = 0;
				if(doc->GetAttribute(mapnode, "id", mapid))
				{
					doc->GetAttribute(mapnode, "level", level);
					publicCopys.insert(std::make_pair(mapid, level));
				}
				mapnode = doc->NextElement(mapnode, "publiccopy");
			}
			m_PublicCopyCfg.insert(std::make_pair(sceneid, publicCopys));

            MapList dungeonEntries;
            mapnode = doc->FirstChildElement(scenenode, "dungeonentry");
            while (mapnode != NULL)
            {
                UInt32 mapid = 0;
                if (doc->GetAttribute(mapnode, "id", mapid))
                {
                    dungeonEntries.push_back(mapid);
                }
                mapnode = doc->NextElement(mapnode, "dungeonentry");
            }
            m_DungeonEntries[sceneid] = dungeonEntries;
		}

		scenenode = doc->NextElement(scenenode,"sceneserver");
	}
	CL_SAFE_DELETE(doc);
	return true;
}

void WSSceneMgr::Final()
{
	CLDeleteVisitor<WSScene> visitor;
	m_Scenes.VisitObjects(visitor);
	m_Scenes.Clear();

	for(int i = 0; i < SCENE_TYPE_MAX; ++i)
	{
		m_DynScenes[i].clear();
	}

	m_Npcs.clear();
}

bool WSSceneMgr::AddScene(WSScene* scene)
{
	if(scene == NULL) return false;

	if(FindScene(scene->GetID()) != NULL) return false;
	
	if(IsDynMap(scene->GetType()))
	{
		if(SCENE_TYPE_PUBLICCOPY == scene->GetType())
		{
			if(GetPublicDynScene(scene->GetMapID(), scene->GetLevel()) != NULL)
				return false;
			m_PublicDynScenes.push_back(scene);
		}
		else if(scene->GetOwner() != 0)
		{
			if(SCENE_TYPE_TEAMCOPY == scene->GetType())
			{	
				if(GetDynScene(scene->GetType(), scene->GetOwner(), scene->GetMapID(), UInt8(scene->GetLevel())) != NULL)
					return false;
			}
			else
			{
				if(GetDynScene(scene->GetType(), scene->GetOwner(), scene->GetMapID()) != NULL)
					return false;
			}
			m_DynScenes[scene->GetType()][scene->GetOwner()].push_back(scene);
		}
	}
	m_Scenes.AddObject(scene);
	
	return true;
}

void WSSceneMgr::RemoveScene(WSScene *scene)
{
	if(IsDynMap(scene->GetType()))
	{
		if(SCENE_TYPE_PUBLICCOPY == scene->GetType())
		{
			for(PublicDynSceneVec::iterator iter = m_PublicDynScenes.begin(); iter != m_PublicDynScenes.end(); iter++)
			{
				if(*iter == scene)
				{
					m_PublicDynScenes.erase(iter);
					break;
				}
			}
		}
		else if(scene->GetOwner() != 0)
		{
			DynSceneMap::iterator iter = m_DynScenes[scene->GetType()].find(scene->GetOwner());
			if(iter != m_DynScenes[scene->GetType()].end())
			{
				for(std::vector<WSScene*>::iterator sceneIter = iter->second.begin();
					sceneIter != iter->second.end(); ++sceneIter)
				{
					if(*sceneIter == scene)
					{
						iter->second.erase(sceneIter);
						break;
					}
				}
				if(iter->second.empty())
				{
					m_DynScenes[scene->GetType()].erase(iter);
				}
			}
		}
	}

	scene->OnDestroyed();

	m_Scenes.RemoveObject(scene);
}

WSScene* WSSceneMgr::FindScene(UInt32 id)
{
	id = GetRealSceneId(id);
	return m_Scenes.FindObject(id);
}

void WSSceneMgr::VisitScenes(WSSceneVisitor& visitor)
{
	m_Scenes.VisitObjects(visitor);
}

UInt32 WSSceneMgr::SelectDynSceneServer(UInt32 nodeId)
{
	if(m_SceneServers.find(nodeId) == m_SceneServers.end()) return 0;

	UInt32 playerNum = WSPlayer::GetServerPlayerNum(nodeId);
	UInt32 minNum = playerNum;
	UInt32 minNumNode = nodeId;
	for(std::set<UInt32>::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(WSPlayer::GetServerPlayerNum(*iter) < minNum) minNumNode = *iter;
	}

	if(playerNum > 3000 && minNum + 1000 < playerNum)
	{
		return minNumNode;
	}
	return nodeId;
}

UInt32 WSSceneMgr::SelectBattleSceneServer()
{
	UInt32 ret = 0;
	for(std::set<UInt32>::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == m_BattleSceneNode)
		{
			if(++iter != m_SceneServers.end())
			{
				ret = *iter;
			}
			break;
		}
	}
	if(ret == 0 && !m_SceneServers.empty())
	{
		ret = *(m_SceneServers.begin());
	}
	m_BattleSceneNode = ret;
	return ret;
}

UInt32 WSSceneMgr::SelectRandSceneServer()
{
	std::vector<UInt32> servers(m_SceneServers.begin(), m_SceneServers.end());
	if(servers.empty()) return 0;

	return servers[Avalon::Random::RandBetween(0, servers.size() - 1)];
}

WSScene* WSSceneMgr::GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId, UInt16 level)
{
	if(!IsDynMap(type)) return NULL;

	DynSceneMap::iterator iter = m_DynScenes[type].find(owner);
	if(iter != m_DynScenes[type].end())
	{
		for(std::vector<WSScene*>::iterator sceneIter = iter->second.begin();
			sceneIter != iter->second.end(); ++sceneIter)
		{
			if((*sceneIter)->GetMapID() == mapId && (*sceneIter)->GetLevel() == level) return *sceneIter;
		}
	}
	return NULL;
}

WSScene* WSSceneMgr::GetDynScene(UInt8 type, ObjID_t owner, UInt32 mapId)
{
	if(!IsDynMap(type)) return NULL;

	DynSceneMap::iterator iter = m_DynScenes[type].find(owner);
	if(iter != m_DynScenes[type].end())
	{
		for(std::vector<WSScene*>::iterator sceneIter = iter->second.begin();
			sceneIter != iter->second.end(); ++sceneIter)
		{
			if((*sceneIter)->GetMapID() == mapId) return *sceneIter;
		}
	}
	return NULL;
}

WSScene* WSSceneMgr::GetPublicDynScene(UInt32 uMapId, UInt16 level)
{
	for(PublicDynSceneVec::iterator iter = m_PublicDynScenes.begin();
		iter != m_PublicDynScenes.end(); iter++)
	{
		WSScene	*pScene = *iter;
		if(pScene->GetMapID() == uMapId && pScene->GetLevel() == level)
			return pScene;
	}

	return NULL;
}

void WSSceneMgr::SetIdSeed(UInt32 nodeid, ObjID_t idseed)
{
	m_IdSeeds[nodeid] = idseed;
}

void WSSceneMgr::RegisterNpc(UInt32 npcId, UInt32 mapId, const CLPosition& pos)
{
	NpcPosInfo info;
	info.mapId = mapId;
	info.pos = pos;
	m_Npcs[npcId].push_back(info);
}

NpcPosInfo* WSSceneMgr::FindNpc(UInt32 mapId, UInt32 npcId)
{
	NpcMap::iterator iter = m_Npcs.find(npcId);
	if(iter != m_Npcs.end())
	{
		std::vector<NpcPosInfo>& posVec = iter->second;
		for(std::vector<NpcPosInfo>::iterator posIter = posVec.begin();
			posIter != posVec.end(); ++posIter)
		{
			if(mapId == 0 || mapId == posIter->mapId)
			{
				return &(*posIter);
			}
		}
		if(!posVec.empty())
		{
			return &(*posVec.begin());
		}
	}
	return NULL;
}

void WSSceneMgr::OnSceneConnected(UInt32 id)
{
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(id);
	if(conn == NULL) return;

	if(m_SceneServers.find(id) != m_SceneServers.end()) return;
	m_SceneServers.insert(id);

	//场景连上后通知加载地图
	UInt32 sceneid = CL_GET_SERVERID_FROM_NODEID(id);
	std::map<UInt32,MapList>::iterator iter1 = m_SceneCfg.find(sceneid);
	if(iter1 != m_SceneCfg.end())
	{
		CLProtocol::SceneNotifyLoadmap protocol;
		protocol.idSeed = m_IdSeeds[id];
		protocol.maps = iter1->second;
		CL_SEND_PROTOCOL(conn,protocol);
	}

	//场景连上后创建公共副本
	std::map<UInt32, std::map<UInt32, UInt32> >::iterator iter2 = m_PublicCopyCfg.find(sceneid);
	if(iter2 != m_PublicCopyCfg.end())
	{
		for(std::map<UInt32, UInt32>::iterator mapIter = iter2->second.begin();
			mapIter != iter2->second.end(); ++mapIter)
		{
			CreatePublicDynScene(mapIter->first, mapIter->second, 0, 0);
		}
	}

    //场景连上后通知加载地下城入口地图
    std::map<UInt32, MapList>::iterator iter3 = m_DungeonEntries.find(sceneid);
    if (iter3 != m_DungeonEntries.end())
    {
        CLProtocol::SceneNotifyLoadDungeonEntryMap protocol;
        protocol.idSeed = m_IdSeeds[id];
        protocol.maps = iter3->second;
        CL_SEND_PROTOCOL(conn, protocol);
    }

	// 如果有怪物攻城活动，召唤一下怪物
	if (WSActivityMgr::Instance()->IsInActivity("CityMonster"))
	{
		CLProtocol::SceneSummonNpc summon;
		CL_SEND_PROTOCOL(conn, summon);
	}
}

void WSSceneMgr::OnSceneDisconnected(UInt32 id)
{
	if(m_SceneServers.find(id) == m_SceneServers.end()) return;

	class OnDisconnectedVisitor : public WSSceneVisitor
	{
	public:
		OnDisconnectedVisitor(UInt32 id):m_NodeID(id){}

		bool operator()(WSScene* scene)
		{
			if(scene->GetNodeID() == m_NodeID)
			{
				WSSceneMgr::Instance()->RemoveScene(scene);
				CL_SAFE_DELETE(scene);
			}
			return true;
		}

	private:
		UInt32 m_NodeID;
	};
	OnDisconnectedVisitor visitor(id);
	VisitScenes(visitor);

	m_SceneServers.erase(id);
}

void WSSceneMgr::OnTick(const Avalon::Time& now)
{
}

void WSSceneMgr::BroadcastToSceneServer(Avalon::Packet *pPacket)
{
	for(std::set<UInt32>::iterator iter = m_SceneServers.begin(); iter != m_SceneServers.end(); iter++)
	{
		Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(*iter);
		if(conn == NULL)
			continue;

		conn->SendPacket(pPacket);
	}
}

void WSSceneMgr::CreatePublicDynScene(UInt32 uMapId, UInt16 level, UInt64 uParam, UInt32 uLastTime)
{	
	if(GetPublicDynScene(uMapId, level) != NULL) return;

	UInt32 uNodeId = SelectDynSceneServer(SelectRandSceneServer());
	Avalon::NetConnectionPtr conn = WSNetwork::Instance()->FindConnection(uNodeId);
	if(conn == NULL) return;

	CLProtocol::SceneNotifyLoadDynMap	protocol;
	protocol.mapId = uMapId;
	protocol.type = SCENE_TYPE_PUBLICCOPY;
	protocol.creater = 0;
	protocol.owner = uParam;
	protocol.level = level;
	protocol.lasttime = uLastTime;
	CL_SEND_PROTOCOL(conn, protocol);
}

