#include "DSPlayer.h"

#include <CLObjectDefine.h>
#include <CLSysProtocol.h>
#include <CLItemProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLErrorCode.h>

#include "DSRouter.h"
#include "DSItemTransaction.h"

DSPlayer::DSPlayer()
{
	m_SourceSceneId = 0;
	m_ServiceSessionId = 0;
	m_pItemTrans = NULL;
}

DSPlayer::~DSPlayer()
{
	CL_SAFE_DELETE(m_pItemTrans);
}

void DSPlayer::SetServiceSessionId(UInt32 serviceSessionId)
{
	m_ServiceSessionId = serviceSessionId;
}

void DSPlayer::SetSourceSceneId(UInt32 sourceSceneId)
{
	m_SourceSceneId = sourceSceneId;
}

bool DSPlayer::SendToScene(Avalon::Protocol& protocol)
{
#ifdef _SCENE_CONNECT_
	return DSRouter::Instance()->SendToScene(m_SourceSceneId, protocol);
#else
	CLProtocol::SysTransmitInNodes req;
	req.destnode = m_SourceSceneId;
	if (!protocol.Encode(req.packet))
	{
		return false;
	}

	return DSRouter::Instance()->SendToService(m_ServiceSessionId, req);
#endif
}

bool DSPlayer::sendToClient(Avalon::Protocol& protocol)
{
#ifdef _SCENE_CONNECT_
	CLProtocol::SysTransmitToPlayer req;
	req.playerId = GetID();
	if (!protocol.Encode(req.packet))
	{
		return false;
	}
	return DSRouter::Instance()->SendToScene(m_SourceSceneId, req);
#else
	CLProtocol::SysTransmitToPlayer req;
	req.playerId = GetID();
	if (!protocol.Encode(req.packet))
	{
		return false;
	}

	return DSRouter::Instance()->SendToService(m_ServiceSessionId, req);
#endif
}

void DSPlayer::SyncEventToScene(SyncEventType type, ObjID_t param1, ObjID_t param2)
{
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = GetID();
	protocol.type = type;
	protocol.param1 = param1;
	protocol.param2 = param2;
	SendToScene(protocol);
}

bool DSPlayer::BeginItemTransaction(DSItemTransaction* transaction)
{
	if (transaction == NULL || transaction->m_Status != DSItemTransaction::STATUS_INIT)
	{
		CL_SAFE_DELETE(transaction);
		return false;
	}

	//当前事务没结束
	if (m_pItemTrans != NULL && m_pItemTrans->m_Status != DSItemTransaction::STATUS_FINAL)
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

void DSPlayer::UnLockAsserts()
{
	CLProtocol::SceneUnlockItemReq req;
	req.playerId = GetID();
	SendToScene(req);
}

void DSPlayer::RemoveItem(ObjID_t itemId)
{
	CLProtocol::SceneRemoveItemObjReq req;
	req.playerId = GetID();
	req.itemId = itemId;
	SendToScene(req);
}

void DSPlayer::OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes)
{
	if (m_pItemTrans == NULL || m_pItemTrans->m_Status != DSItemTransaction::STATUS_CHECK || m_pItemTrans->GetSerial() != serial) return;

	m_pItemTrans->SetCounterTimes(counterTimes);
	if (result == ErrorCode::SUCCESS)
	{
		m_pItemTrans->m_Status = DSItemTransaction::STATUS_CHECKED;
		m_pItemTrans->OnCheckSucced(this);
	}
	else
	{
		m_pItemTrans->m_Status = DSItemTransaction::STATUS_FINAL;
		m_pItemTrans->SetErrorCode(result);
		m_pItemTrans->OnCheckFailed(this);
	}
}