#include "SceneObject.h"

#include <AvalonCompress.h>
#include <CLSyncObjectProtocol.h>
#include <CLMoveProtocol.h>
#include <CLSceneProtocol.h>
#include <CLGateProtocol.h>

#include "Player.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "SSRouter.h"
#include "SceneChampionMgr.h"

UInt64 SceneObject::m_IdSeed = 0;
SceneObject::SceneObjectSet SceneObject::m_FreeObjects;

SceneObject::SceneObject()
{
	REGISTER_SYNC_PROPERTY(m_Name, SOA_NAME, SST_SELF | SST_AROUND | SST_TEAM | SST_WATCH | SST_SCENE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	m_pScene = NULL;
	m_bDayChanged = 0;
	m_bPosChanged = false;
	m_doorId = 0;
}

SceneObject::~SceneObject()
{
}

void SceneObject::SetPos(const CLPosition& pos)
{
	if(IsInScene())
	{
		m_OldPos = GetPos();
		m_bPosChanged = true;
	}
	CLSceneObject::SetPos(pos);
}

bool SceneObject::IsCanSeeMe(SceneObject* obj)
{
	return false;
}

SceneObject* SceneObject::CreateObject(int type)
{
	switch(type)
	{
	case SOT_PLAYER:
		return new Player();
	default:
		break;
	}
	return NULL;
}

void SceneObject::FreeObject(SceneObject* obj)
{
	if(obj == NULL) return;
	InfoLogStream << "object(" << obj->GetID() << ") wait to free..." << LogStream::eos;
	m_FreeObjects.insert(obj);
}

void SceneObject::DestroyObjects()
{
	for(SceneObjectSet::iterator iter = m_FreeObjects.begin();
		iter != m_FreeObjects.end(); ++iter)
	{
		auto obj = *iter;
		if (obj)
		{
			InfoLogStream << "destroy object(" << obj->GetID() << ")..." << LogStream::eos;
		}
		
		delete (*iter);
	}
	m_FreeObjects.clear();
}

//noted by aprilliu 生成scene object的内存id（非永久性的guid）
ObjID_t SceneObject::GenObjID()
{
	++m_IdSeed;
	if(m_IdSeed % 1000 == 0)
	{
		CLProtocol::SceneSyncIdSeed sync;
		sync.idSeed = m_IdSeed;
		Router::SendToWorld(sync);
	}
	return CL_MAKE_TEMP_GUID(m_IdSeed);
}

void SceneObject::SetIDSeed(ObjID_t id)
{
	m_IdSeed = id;
}

bool SceneObject::ChangeScene(UInt32 sceneid, const CLPosition& pos, UInt8 dir, bool bTransZone, UInt32 targetDoorId)
{
	/*if (SERVER_TYPE == ST_WSCENE)
	{
		return false;
	}*/

	if (!IsInScene())
	{
		return false;
	}

	if(GetSceneID() == sceneid)
	{
		return false;
	}

	if (!CanEnterMap(sceneid))
	{
		return false;
	}

	Scene* scene = SceneMgr::Instance()->FindNormalScene(sceneid);
	if(scene != NULL) //目标场景在本服务器
	{
		CLPosition tmpPos(pos);
		if(pos.x == 0 && pos.y == 0 && targetDoorId > 0)
		{
            if (!scene->HasThisDoor(targetDoorId))
            {
                ErrorLogStream << "player(" << this->GetID() << ") target scene(" << sceneid << ") want enter unexist door(" 
                    << targetDoorId << ")." << LogStream::eos;
                return false;
            }
            tmpPos = scene->GetDoorBirthPos(targetDoorId);
		}
		else if (targetDoorId == 0 && pos.x == 0 && pos.y == 0)
		{
			tmpPos = scene->GetBirthPoint();
		}

		if (!scene->IsValidPos(tmpPos))
		{
			//return false;
			tmpPos = scene->GetBirthPoint();
		}
        
		if(GetType() == SOT_PLAYER)
		{
			Player	*pPlayer = (Player*)this;

			if (!scene->CheckPlayerEnter(GetID(), pPlayer->GetLevel()))
			{
				return false;
			}
		}

		//从源场景移除
		GetScene()->RemoveObject(this);

		SetDoorID(targetDoorId);
		SetPos(tmpPos);
		SetDir(dir);

		//加入新场景，一般都会成功
		if (!scene->AddObject(this))
		{
			ErrorLogStream << "player(" << GetID() << ") add to scene(" << scene->GetID() << ") failed." << LogStream::eos;
			return false;
		}

		OnSceneChanged();
	}
	else  
	{	
		//通过world转发到目标场景，占个位置
		CLProtocol::SceneBeginChangeSceneReq req;
		req.info.id = GetID();
		req.info.accid = GetAccID();
		req.info.gateid = GetGateID();
		req.info.srcnode = CL_MAKE_NODEID(ZONE_ID, SERVER_TYPE, SERVER_ID);
		req.info.srcscene = GetScene()->GetID();
		req.info.destscene = sceneid;
		req.objtype = GetType();
		if(GetType() == SOT_PLAYER)
		{
			Player	*pPlayer = (Player*)this;

			req.level = pPlayer->GetLevel();
		}
		req.pos = pos;
		req.dir = dir;
		req.bTransZone = UInt8(bTransZone ? 1 : 0);
        req.dstDoorId = targetDoorId;
		Router::SendToWorld(req);
	}

	return true;
}

void SceneObject::SyncProperty()
{
	Avalon::Packet* syncPacket = MakeSyncPropertyPacket(SST_AROUND, true);
	if(syncPacket != NULL)
	{
		BroadcastExceptMe(syncPacket);
		Player* player = dynamic_cast<Player*>(this);
		if (player != NULL)
		{
			if (player->GetTeamCopyMgr().IsTeamCopy())
			{
				Router::SendToTeamCopy(syncPacket);
			}
			if (SceneChampionMgr::Instance()->IsPlayerInChampion(player->GetID()))
			{
				Router::SendToUnion(USFT_CHAMPION, syncPacket);
			}
		}

		Avalon::Packet::Destroy(syncPacket);
	}
}

void SceneObject::OnHeartbeat(const Avalon::Time& now)
{
	if(m_bPosChanged)
	{
		OnMoved(m_OldPos);
		m_bPosChanged = false;
	}

	if(m_pScene->UpdatePosition(this))
	{
		OnGridChanged();
	}	

	if (CheckDayChange(now))
	{
		OnDayChanged();
	}
}

void SceneObject::OnBeforeEnterScene()
{
}

void SceneObject::OnEnterScene()
{
	m_bPosChanged  = false;

	//如果周围除了自身还有其他玩家
	Avalon::Packet* syncpacket = MakeSyncObjectPacket(true);
	BroadcastExceptMe(syncpacket);
	Avalon::Packet::Destroy(syncpacket);
}

void SceneObject::OnLeaveScene()
{
	m_bPosChanged  = false;

	//如果周围除了自身还有其他玩家
	Avalon::Packet* delpacket = MakeDelObjectPacket();
	BroadcastExceptMe(delpacket);
	Avalon::Packet::Destroy(delpacket);
}


void SceneObject::OnGridChanged()
{
    InfoLogStream << "player(" << GetID() << ") changed grid(" << GetOldGridIndex() << ", " << GetGridIndex() << ") pos(" << GetPos().GetX() << ", " << GetPos().GetY() << ")" << LogStream::eos;

	//如果新区域有玩家
	Avalon::Packet* syncpacket = MakeSyncObjectPacket(false);
	BroadcastNewZone(syncpacket);
	Avalon::Packet::Destroy(syncpacket);

	Avalon::Packet* delpacket = MakeDelObjectPacket();
	BroadcastOldZone(delpacket);
	Avalon::Packet::Destroy(delpacket);
}

void SceneObject::OnMoved(const CLPosition& oldpos)
{
}

void SceneObject::OnDayChanged()
{
}


bool SceneObject::CheckDayChange(const Avalon::Time& now)
{
	return false;
}

class BroadcastAroundVisitor : public PlayerVisitor
{
public:
	BroadcastAroundVisitor(SceneObject* obj, Avalon::Packet* packet, bool bSelf)
		:m_pObject(obj), m_pPacket(packet), m_bSelf(bSelf){}

	bool operator()(Player* player)
	{
		if(!m_bSelf && player == m_pObject) return true;

		if(m_pObject->IsCanSeeMe(player))
		{
			player->SendPacket(m_pPacket);
		}
		return true;
	}
private:
	//object
	SceneObject*	m_pObject;
	//消息包
	Avalon::Packet*	m_pPacket;
	//是否包含自身
	bool			m_bSelf;
};

void SceneObject::Broadcast(Avalon::Protocol& protocol)
{
	if(!IsInScene()) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		Broadcast(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void SceneObject::Broadcast(Avalon::Packet* packet)
{
	if(packet == NULL) return;
	if(!IsInScene()) return;

	if(IsHidden())
	{
		BroadcastAroundVisitor visitor(this, packet, true);
		GetScene()->VisitPlayersAround(GetPos(), visitor);
	}
	else
	{
		CLProtocol::GateBroadcastAround req(packet);
		req.sceneid = GetScene()->GetID();
		req.grid = GetGridIndex();
		req.exceptobj = 0;
		Router::BroadcastToGate(req);
		req.Detach();
	}
}

void SceneObject::BroadcastExceptMe(Avalon::Protocol& protocol)
{
	if(!IsInScene()) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		BroadcastExceptMe(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void SceneObject::BroadcastExceptMe(Avalon::Packet* packet)
{
	if(packet == NULL) return;
	if(!IsInScene()) return;

	if(IsHidden())
	{
		BroadcastAroundVisitor visitor(this, packet, false);
		GetScene()->VisitPlayersAround(GetPos(), visitor);
	}
	else
	{
		CLProtocol::GateBroadcastAround req(packet);
		req.sceneid = GetScene()->GetID();
		req.grid = GetGridIndex();
		req.exceptobj = GetID();
		Router::BroadcastToGate(req);
		req.Detach();
	}
}

void SceneObject::BroadcastNewZone(Avalon::Packet* packet)
{
	if(!IsInScene()) return;

	CLProtocol::GateBroadcastExcept req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid1 = GetGridIndex();
	req.grid2 = GetOldGridIndex();
    req.exceptobj = GetID();
	Router::BroadcastToGate(req);
	req.Detach();
}
	
void SceneObject::BroadcastOldZone(Avalon::Packet* packet)
{
	if(!IsInScene()) return;

	CLProtocol::GateBroadcastExcept req(packet);
	req.sceneid = GetScene()->GetID();
	req.grid1 = GetOldGridIndex();
	req.grid2 = GetGridIndex();
    req.exceptobj = GetID();
	Router::BroadcastToGate(req);
	req.Detach();
}

Avalon::Packet* SceneObject::MakeSyncObjectPacket(bool bEnterScene)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_SCENEOBJECT);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & UInt8(bEnterScene?1:0) & GetSceneID();
		stream & GetID() & UInt8(GetType());
		Encode(stream, SST_AROUND | SST_MOVE, false);
		stream & ObjID_t(0);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return NULL;
	}
	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	return packet;
}

Avalon::Packet* SceneObject::MakeSyncPropertyPacket(UInt32 type, bool bDirty)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_OBJECTPROPERTY);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	bool ret = false;
	try
	{
		stream & GetID();
		ret = Encode(stream, type, bDirty);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return NULL;
	}
	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	if(!ret) Avalon::Packet::Destroy(packet);
	return packet;
}

Avalon::Packet* SceneObject::MakeSyncPropertyPacket(UInt32 type)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_OBJECTPROPERTY);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	bool ret = false;
	try
	{
		stream & GetID();
		ret = Encode(stream, type);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return NULL;
	}
	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	if(!ret) Avalon::Packet::Destroy(packet);
	return packet;
}

Avalon::Packet* SceneObject::MakeDelObjectPacket()
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_DELETE_SCENEOBJECT);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & GetID() & ObjID_t(0);
	}
	catch(const Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return NULL;
	}
	packet->SetSize(packet->GetSize() + UInt16(stream.GetPos()));
	return packet;
}

Avalon::Packet* SceneObject::MakeChangeSceneDataPacket(ChangeSceneInfo& info)
{
	char databuf[Avalon::Packet::MAX_BODY_SIZE * 2];

	Avalon::NetOutputStream stream((UInt8*)databuf, Avalon::Packet::MAX_BODY_SIZE * 2);
	try
	{
		stream & info;
		Encode(stream, SST_SCENE, false);
		Output(stream);
	}
	catch(Avalon::NetException& exception)
	{
		ErrorLogStream << "encode changescene data failed:" << exception.what()
			<< "!id=" << GetID() << LogStream::eos;
		return NULL;
	}

	size_t compressedLen = Avalon::snappy_maxcompressedlen(stream.GetPos());
	if(compressedLen > Avalon::Packet::MAX_BODY_SIZE * 2)
	{
		ErrorLogStream << "encode changescene data failed:compressed failed!id=" << GetID() << LogStream::eos;
		return NULL;
	}

	char destbuf[Avalon::Packet::MAX_BODY_SIZE * 2];
	size_t len = Avalon::Packet::MAX_BODY_SIZE * 2;
	Avalon::snappy_compress(destbuf, &len, databuf, stream.GetPos());
	if(len > Avalon::Packet::MAX_BODY_SIZE)
	{
		ErrorLogStream << "encode changescene data failed:compressed failed!id=" << GetID() << LogStream::eos;
		return NULL;
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_TRANS_OBJECTDATA);
	avalon_memmove(packet->GetBuffer(), destbuf, len);
	packet->SetSize(UInt16(len));
	return packet;
}

