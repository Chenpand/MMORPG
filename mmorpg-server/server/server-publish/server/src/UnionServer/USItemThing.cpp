#include "USItemThing.h"

#include "USPlayer.h"

USItemThing::USItemThing(USPlayer* player) : m_Player(player)
{
	m_SerialId = 0;
	m_FunType = USFT_NONE;
}

USItemThing::~USItemThing()
{

}

void USItemThing::StartThing()
{
	if (NULL == m_Player) return;

	m_TimeOutStamp = CURRENT_TIME.Sec() + ITEM_THING_TIME_OUT;
	m_SerialId = GenItemThingSerialId();

	CLProtocol::SceneItemThingStartReq req;
	req.roleId = m_Player->GetID();
	req.serialId = m_SerialId;
	req.servType = ST_UNION;
	req.param = m_FunType;
	req.reason = m_Reason;
	req.itemCost = m_ItemCost;
	m_Player->SendToScene(req);
}

void USItemThing::CommitThing(UInt64 thingId, bool isCommit)
{
	if (NULL == m_Player) return;

	CLProtocol::SceneItemThingCommitReq req;
	req.roleId = m_Player->GetID();
	req.itemThingGuid = thingId;
	req.servType = ST_UNION;
	req.param = m_FunType;
	req.isCommit = isCommit ? 1 : 0;
	m_Player->SendToScene(req);
}

bool USItemThing::CheckTimeOut(const Avalon::Time& now)
{
	if (m_TimeOutStamp != 0 && now.Sec() > m_TimeOutStamp)
	{
		return true;
	}

	return false;
}

UInt64 USItemThing::GenItemThingSerialId()
{
	static UInt64 serialId;
	++serialId;
	return (UInt64(SERVER_TYPE) << 59) + (UInt64(SERVER_ID) << 55) + ((CURRENT_TIME.Sec()) << 20) + serialId;
}
	 
void USItemThing::OnTimeOut(USPlayer* player)
{
	CLProtocol::SceneItemThingStartCancel req;
	req.roleId = m_Player->GetID();
	req.serialId = m_SerialId;
	req.servType = ST_UNION;
	req.param = GetFunType();
	m_Player->SendToScene(req);
}
