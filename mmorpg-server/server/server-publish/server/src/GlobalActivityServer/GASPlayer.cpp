#include "GASPlayer.h"

#include <CLSysProtocol.h>
#include <CLGateProtocol.h>
#include <CLItemProtocol.h>
#include <CLErrorCode.h>

#include "GASGameZoneMgr.h"
#include "GASItemTransaction.h"


GASPlayer::GASPlayer()
{
	m_AccId = 0;
	m_ServerZoneId = 0;
	m_pItemTrans = NULL;
	m_RemoveTime = 0;
	m_ItemTransTimeout = 0;
	memset(m_SourceNode, 0, sizeof(m_SourceNode));

	for (int i = 0; i < LOGICTICK_MAX; ++i)
	{
		m_TickTime[i] = CURRENT_TIME.MSec();
	}
}

GASPlayer::~GASPlayer()
{
	CL_SAFE_DELETE(m_pItemTrans);
}

void GASPlayer::SetSourceNode(ServerType serverType, UInt32 node)
{
	if (serverType >= SERVER_TYPE_MAX) return;
	m_SourceNode[serverType] = node;
}

UInt32 GASPlayer::GetSourceNode(ServerType serverType) const
{
	if (serverType >= SERVER_TYPE_MAX) return 0;
	return m_SourceNode[serverType];
}

void GASPlayer::OnTick(const Avalon::Time& now)
{
	//tick类型
	int tickType = LOGICTICK_MAX;
	for (int i = 0; i < LOGICTICK_MAX; ++i)
	{
		if (now.MSec() >= m_TickTime[i] + LOGICTICK_INTERVAL[i])
		{
			tickType = i;
			m_TickTime[i] = now.MSec();
			break;
		}
	}

	if (tickType == LOGICTICK_SECOND)
	{
		if (m_ItemTransTimeout > 0)
		{
			if (--m_ItemTransTimeout == 0)
			{
				CL_SAFE_DELETE(m_pItemTrans);
			}
		}

		if (m_pItemTrans != NULL && m_pItemTrans->m_Status == GASItemTransaction::STATUS_FINAL)
		{
			CL_SAFE_DELETE(m_pItemTrans);
			m_ItemTransTimeout = 0;
		}
	}
}

bool GASPlayer::BeginItemTransaction(GASItemTransaction* transaction)
{
	if (transaction == NULL || transaction->m_Status != GASItemTransaction::STATUS_INIT)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	//当前事务没结束
	if (m_pItemTrans != NULL && m_pItemTrans->m_Status != GASItemTransaction::STATUS_FINAL)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	CL_SAFE_DELETE(m_pItemTrans);
	m_pItemTrans = transaction;
	m_ItemTransTimeout = ITEM_TRANS_TIMEOUT;

	m_pItemTrans->CheckItem();
	return true;
}

void GASPlayer::UnLockAsserts()
{
	CLProtocol::SceneUnlockItemReq req;
	req.playerId = GetID();
	SendToScene(req);
}

void GASPlayer::RemoveItem(ObjID_t itemId)
{
	CLProtocol::SceneRemoveItemObjReq req;
	req.playerId = GetID();
	req.itemId = itemId;
	SendToScene(req);
}

void GASPlayer::OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes)
{
	if (m_pItemTrans == NULL || m_pItemTrans->m_Status != GASItemTransaction::STATUS_CHECK || m_pItemTrans->GetSerial() != serial) return;

	m_pItemTrans->SetCounterTimes(counterTimes);
	if (result == ErrorCode::SUCCESS)
	{
		m_pItemTrans->m_Status = GASItemTransaction::STATUS_CHECKED;
		m_pItemTrans->OnCheckSucced(this);
	}
	else
	{
		m_pItemTrans->m_Status = GASItemTransaction::STATUS_FINAL;
		m_pItemTrans->SetErrorCode(result);
		m_pItemTrans->OnCheckFailed(this);
	}
}

void GASPlayer::SendProtocol(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GASPlayer::SendPacket(Avalon::Packet* packet)
{
	CLProtocol::GateTransmitPacket trans(packet);
	trans.id = GetID();
	SendToGate(trans);
	trans.Detach();
}

void GASPlayer::SendToCenter(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToCenter(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GASPlayer::SendToCenter(Avalon::Packet* packet)
{
	GASGameZone* zone = GASGameZoneMgr::Instance()->FindGASGameZone(GetSourceNode(ST_CENTER));
	if (!zone)
	{
		ErrorLogStream << "can not find connection(" << GetSourceNode(ST_CENTER) << ")." << LogStream::eos;
		return;
	}

	zone->SendPacket(packet);
}

void GASPlayer::SendToAdmin(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = GetSourceNode(ST_ADMIN);
	if (protocol.Encode(trans.packet))
	{
		SendToCenter(trans);
	}
}

void GASPlayer::SendToAdmin(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodes trans(packet);
	trans.destnode = GetSourceNode(ST_ADMIN);
	SendToCenter(trans);
	trans.Detach();
}

void GASPlayer::SendToWorld(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = GetSourceNode(ST_WORLD);
	if (protocol.Encode(trans.packet))
	{
		SendToGate(trans);
	}
}

void GASPlayer::SendToWorld(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodes trans(packet);
	trans.destnode = GetSourceNode(ST_WORLD);
	SendToGate(trans);
	trans.Detach();
}

void GASPlayer::SendToGate(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = GetSourceNode(ST_GATE);
	if (protocol.Encode(trans.packet))
	{
		SendToAdmin(trans);
	}
}

void GASPlayer::SendToGate(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodes trans(packet);
	trans.destnode = GetSourceNode(ST_GATE);
	SendToAdmin(trans);
	trans.Detach();
}

void GASPlayer::SendToScene(Avalon::Protocol& protocol)
{
	CLProtocol::SysTransmitInNodes trans;
	trans.destnode = GetSourceNode(ST_SCENE);
	if (protocol.Encode(trans.packet))
	{
		SendToGate(trans);
	}
}

void GASPlayer::SendToScene(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodes trans(packet);
	trans.destnode = GetSourceNode(ST_SCENE);
	SendToGate(trans);
	trans.Detach();
}