#include "GSSceneMgr.h"
#include "GSScene.h"

GSSceneMgr::GSSceneMgr()
{
}

GSSceneMgr::~GSSceneMgr()
{

}

void GSSceneMgr::Init()
{
}

void GSSceneMgr::Final()
{
	CLDeleteVisitor<GSScene> delvisitor;
	m_Scenes.VisitObjects(delvisitor);
	m_Scenes.Clear();
}

bool GSSceneMgr::AddScene(GSScene *scene)
{
	return m_Scenes.AddObject(scene);
}

void GSSceneMgr::RemoveScene(GSScene *scene)
{	
	class OnSceneUnLoadVisitor : public GSPlayerVisitor
	{
	public:
		OnSceneUnLoadVisitor(GSScene* scene):m_pScene(scene){}

		bool operator()(GSPlayer* player)
		{
			if(player->GetScene() == m_pScene)
			{
				m_pScene->RemovePlayer(player);
			}
			return true;
		}

	private:
		//³¡¾°
		GSScene* m_pScene;
	};
	OnSceneUnLoadVisitor visitor(scene);
	scene->VisitPlayers(visitor);

	m_Scenes.RemoveObject(scene);
}

GSScene* GSSceneMgr::FindScene(ObjID_t id)
{
	return m_Scenes.FindObject(id);
}

void GSSceneMgr::OnSceneDisconnected(UInt32 id)
{
	class OnSceneDisconnectedVisitor : public GSSceneVisitor
	{
	public:
		OnSceneDisconnectedVisitor(UInt32 id):m_NodeID(id){}
		~OnSceneDisconnectedVisitor(){}

		bool operator()(GSScene* scene)
		{
			if(scene->GetNodeID() == m_NodeID)
			{
				GSSceneMgr::Instance()->RemoveScene(scene);
				CL_SAFE_DELETE(scene);
			}
			return true;
		}

	private:
		//½Úµãid
		UInt32 m_NodeID;
	};

	OnSceneDisconnectedVisitor visitor(id);
	m_Scenes.VisitObjects(visitor);
}

