#include "GSScene.h"
#include "GSInternalNetwork.h"

GSScene::GSScene()
{
	m_sceneDataId = 0;
	m_sceneType = 0;
}

GSScene::~GSScene()
{
}

bool GSScene::Init(UInt32 id, const std::string& name, UInt32 length, 
	UInt32 width, UInt32 syncRange, UInt32 nodeid, UInt32 sceneDataId, UInt8 sceneType)
{
	m_Conn = GSInternalNetwork::Instance()->FindConnection(nodeid);
	if(m_Conn == NULL)
	{
		return false;
	}

	SetID(id);
	SetName(name);

	SetSceneDataId(sceneDataId);
	SetSceneType(sceneType);

	CLScene::Init(length, width, syncRange);
	return true;
}

bool GSScene::AddPlayer(UInt32 gridIndex,GSPlayer *player)
{
	if(player == NULL || player->IsInScene()) return false;

	CLScene::RegisterObject(gridIndex,player);
	player->SetScene(this);
	return true;
}

void GSScene::RemovePlayer(GSPlayer *player)
{
	if(player == NULL || player->GetSceneID() != GetID()) return;

	CLScene::UnRegisterObject(player);
	player->SetScene(NULL);
	return;
}

UInt32 GSScene::GetNodeID() const
{
	if(m_Conn == NULL) return 0;
	return m_Conn->GetID();
}

void GSScene::SetSceneDataId(UInt32 sceneDataId)
{
	m_sceneDataId = sceneDataId;
}

UInt32 GSScene::GetSceneDataId()
{
	return m_sceneDataId;
}

void GSScene::SetSceneType(UInt8 sceneType)
{
	m_sceneType = sceneType;
}

UInt8 GSScene::GetSceneType()
{
	return m_sceneType;
}

class CastPlayerVisitor : public CLSceneObjectVisitor
{
public:
	CastPlayerVisitor(GSPlayerVisitor& visitor):m_Visitor(visitor){}
	~CastPlayerVisitor(){}

	bool operator()(CLSceneObject* obj)
	{
		return m_Visitor(dynamic_cast<GSPlayer*>(obj));
	}

private:
	GSPlayerVisitor& m_Visitor;
};

void GSScene::VisitPlayers(GSPlayerVisitor& visitor)
{
	CastPlayerVisitor myVisitor(visitor);
	CLScene::VisitPlayers(myVisitor);
}

void GSScene::VisitAround(UInt32 grid,GSPlayerVisitor& visitor)
{
	CastPlayerVisitor myVisitor(visitor);
	CLScene::VisitPlayers(CLScene::GetGridsAround(grid),myVisitor);
}

void GSScene::VisitExcept(UInt32 grid1,UInt32 grid2,GSPlayerVisitor& visitor)
{
	CastPlayerVisitor myVisitor(visitor);
	CLScene::GridIndexVector indexs;
	CLScene::GetGridsAroundExcept(grid1,grid2,indexs);
	CLScene::VisitPlayers(indexs,myVisitor);
}

void GSScene::BroadcastAround(UInt32 grid, Avalon::Protocol &protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		BroadcastAround(grid,packet);
	}
	Avalon::Packet::Destroy(packet);
}


class BroadcastVisitor : public GSPlayerVisitor
{
public:
	BroadcastVisitor(Avalon::Packet* packet,ObjID_t except)
		:m_pPacket(packet),m_Except(except)
	{
		m_pCopyPacket = Avalon::Packet::Create();
	}
	~BroadcastVisitor()
	{
		Avalon::Packet::Destroy(m_pCopyPacket);
	}

	bool operator()(GSPlayer* player)
	{
		if(player->GetID() == m_Except) return true;
		m_pPacket->CopyTo(m_pCopyPacket);
		player->SendPacket(m_pCopyPacket);
		return true;
	}

private:
	//消息
	Avalon::Packet* m_pPacket;
	//广播复制的消息
	Avalon::Packet* m_pCopyPacket;
	//排除的玩家
	ObjID_t	m_Except;
};

void GSScene::BroadcastAround(UInt32 grid, Avalon::Packet *packet,ObjID_t except)
{
	BroadcastVisitor myVisitor(packet,except);
	VisitAround(grid,myVisitor);
}

void GSScene::BroadcastExcept(UInt32 grid1, UInt32 grid2, Avalon::Packet *packet, ObjID_t except)
{
	BroadcastVisitor myVisitor(packet, except);
	VisitExcept(grid1, grid2, myVisitor);
}

void GSScene::SendToSceneServer(Avalon::Protocol& protocol)
{
	if(m_Conn == NULL) return;

	CL_SEND_PROTOCOL(m_Conn,protocol);
}

