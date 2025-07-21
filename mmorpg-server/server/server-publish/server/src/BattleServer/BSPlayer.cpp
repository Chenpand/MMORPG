#include "BSPlayer.h"

#include <CLSysProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLLostDungeonDefine.h>

#include "GameZoneMgr.h"
#include "BSNetwork.h"

BSPlayer::BSPlayer()
{
	m_accId = 0;
	m_SourceSceneNodeId = 0;
	m_SrcZoneId = 0;
	m_dstBSSceneNodeId = 0;
	m_lostState = LDBPS_NONE;
	m_lostRaceId = 0;
	m_protectTimeStamp = 0;
}

BSPlayer::~BSPlayer()
{
}

void BSPlayer::SetSourceSceneNodeId(UInt32 sourceSceneNodeId)
{
	m_SourceSceneNodeId = sourceSceneNodeId;
}

void BSPlayer::SetProtectTimeStamp(UInt32 timeStamp) 
{ 
	m_protectTimeStamp = timeStamp;
	SyncEventToBScene(SET_LOST_DUNG_PROT_TIME, m_protectTimeStamp);
}

void BSPlayer::SendToBScene(Avalon::Protocol& protocol)
{
	BSNetwork::Instance()->SendToBScene(GetDstSceneNodeId(), protocol);
}

bool BSPlayer::SendToSelfScene(Avalon::Protocol& protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(this->GetSrcZoneId());
	if (!zone)
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find zone(" << this->GetSrcZoneId() << ") when send protocol(" << protocol.GetProtocolID() << ")." << LogStream::eos;
		return false;
	}

	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = this->GetSourceSceneNodeId();
	if (protocol.Encode(trans.packet))
	{
		zone->SendProtocol(trans);
	}
	return true;
}

bool BSPlayer::sendToClient(Avalon::Protocol& protocol)
{
	auto zone = GameZoneMgr::Instance()->FindGameZoneByID(this->GetSrcZoneId());
	if (!zone)
	{
		ErrorLogStream << "[TCS_PROTOCOL] cant find zone(" << this->GetSrcZoneId() << ") when send protocol(" << protocol.GetProtocolID() << ")." << LogStream::eos;
		return false;
	}

	zone->SendToPlayer(GetID(), protocol);
	return true;
}

void BSPlayer::SyncEventToBScene(SyncEventType type, ObjID_t param1, ObjID_t param2)
{
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = GetID();
	protocol.type = type;
	protocol.param1 = param1;
	protocol.param2 = param2;
	SendToBScene(protocol);
}

void BSPlayer::SetLostState(UInt8 state, bool pvp)
{ 
	if (m_lostState == state)
	{
		return;
	}
	m_lostState = (LostDungeonPlayerState)state;
	if (m_lostState == LDBPS_NORAML)
	{
		SyncEventToBScene(SET_LOST_DUNG_BATTLE_ST, LDPBT_NORMAL);
	}
	else if (m_lostState == LDBPS_RACING || m_lostState == LDBPS_MATCHING)
	{
		if (pvp)
		{
			SyncEventToBScene(SET_LOST_DUNG_BATTLE_ST, LDPBT_BATTLE_PVP);
		}
		else
		{
			SyncEventToBScene(SET_LOST_DUNG_BATTLE_ST, LDPBT_BATTLE_PVE);
		}
		
	}
}

bool BSPlayer::IsCanPK()
{
	return m_lostState == LDBPS_NORAML;
}

bool BSPlayer::IsCanPKed()
{
	return m_lostState == LDBPS_NORAML;
}