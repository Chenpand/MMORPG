#include "BattleSceneMgr.h"

#include <CLSceneProtocol.h>
#include <CLSceneDefine.h>
#include <CLSceneDataEntry.h>

#include "BattleScene.h"
#include "BSNetwork.h"

BattleSceneMgr::BattleSceneMgr()
{

}

BattleSceneMgr::~BattleSceneMgr()
{

}

bool BattleSceneMgr::Init()
{
	return true;
}

void BattleSceneMgr::Final()
{
	for(SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_Scenes.clear();

	m_SceneServers.clear();
}

BattleScene* BattleSceneMgr::FindBattleScene(UInt32 nodeId)
{
	for (auto scene : m_Scenes)
	{
		if (scene->GetNodeID() == nodeId)
		{
			return scene;
		}
	}

	return NULL;
}

void BattleSceneMgr::OnTick(const Avalon::Time& now)
{
	
}

void BattleSceneMgr::OnSceneCreated(UInt32 nodeId, UInt32 sceneId)
{
	BattleScene* scene = new BattleScene();
	if(!scene->Init(nodeId, sceneId))
	{
		CL_SAFE_DELETE(scene);
		return;
	}
	m_Scenes.push_back(scene);
}

void BattleSceneMgr::OnSceneDestroyed(UInt32 sceneId)
{
	for(SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end(); ++iter)
	{
		BattleScene* scene = *iter;
		if(scene->GetSceneID() == sceneId)
		{
			CL_SAFE_DELETE(scene);
			m_Scenes.erase(iter);
			return;
		}
	}
}

void BattleSceneMgr::OnSceneServerConnected(UInt32 nodeId)
{
	for(SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == nodeId) return;
	}
	m_SceneServers.push_back(nodeId);


	class GetCrossScenesVisitor : public SceneDataEntryVisitor
	{
	public:
		GetCrossScenesVisitor(std::vector<UInt32>& maps)
		:m_Maps(maps){}
		~GetCrossScenesVisitor(){}

		bool operator()(SceneDataEntry* dataEntry)
		{
			return true;
		}

	private:
		std::vector<UInt32>& m_Maps;
	};

	CLProtocol::SceneNotifyLoadmap protocol;
	protocol.idSeed = 1;
	//protocol.maps.push_back(MAPID_CHIJI_SINGLE);
	//protocol.maps.push_back(MAPID_CHIJI_BATTLE);
	//protocol.maps.push_back(MAPID_CHIJI_PREPARE);

	GetCrossScenesVisitor visitor(protocol.maps);
	SceneDataEntryMgr::Instance()->Visit(visitor);
	
	auto conn = BSNetwork::Instance()->FindConnection(nodeId);
	if (conn != NULL)
	{
		CL_SEND_PROTOCOL(conn, protocol);
	}
	
}

void BattleSceneMgr::OnSceneServerDisconnected(UInt32 nodeId)
{
	for (SceneVec::iterator iter = m_Scenes.begin();
		iter != m_Scenes.end();)
	{
		BattleScene* scene = *iter;
		if (scene->GetNodeID() == nodeId)
		{
			iter = m_Scenes.erase(iter);
			CL_SAFE_DELETE(scene);
		}
		else
		{
			++iter;
		}
	}

	for(SceneServerVec::iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == nodeId)
		{
			m_SceneServers.erase(iter);
			return;
		}
	}
}

UInt32 BattleSceneMgr::SelectSceneServer()
{
	UInt32 ret = 0;
	for(SceneServerVec::const_iterator iter = m_SceneServers.begin();
		iter != m_SceneServers.end(); ++iter)
	{
		if(*iter == m_SceneServer)
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
	m_SceneServer = ret;
	return ret;
}

UInt32 BattleSceneMgr::GetChiJiSceneServer()
{
	if (m_SceneServers.size() > 0)
	{
		//返回第一个
		return *(m_SceneServers.begin());
	}
	else
	{
		return 0;
	}
}
