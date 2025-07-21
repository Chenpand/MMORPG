#include "ReviveQuickBuyTransaction.h"
#include <CLItemDefine.h>
#include <CLQuickBuyProtocol.h>
#include "Player.h"
#include "SSRouter.h"

ReviveQuickBuyTransaction::ReviveQuickBuyTransaction(ObjID_t targetId, UInt32 reviveId)
	: QuickBuyTransaction(::GetReason(SOURCE_TYPE_QUICK_BUY_REVIVE))
{
	m_targetId = targetId;
	m_reviveId = reviveId;
	SetCostByQuickBuyTable(600000006);
}

ReviveQuickBuyTransaction::~ReviveQuickBuyTransaction()
{

}

bool ReviveQuickBuyTransaction::BeginTransaction(Player* player)
{
	if (!player)
	{
		return false;
	}

	if (GetCostItemId() == ITEM_POINT && !player->SecurityLockCheck())
	{
		OnFinish(player, ErrorCode::SECURITY_LOCK_FORBID_OP);
		return false;
	}

	if (!_CheckRes(player))
	{
		OnFinish(player, ErrorCode::QUICK_BUY_NOT_ENOUGH_MONEY);
		return false;
	}

	CLProtocol::SceneQuickBuyTransactionReq req;

	req.roleId = player->GetID();
	req.type = QUICK_BUY_REVIVE;
	req.transactionId = GetID();
	req.param1 = m_targetId;
	req.param2 = m_reviveId;
	Router::SendToWorld(req);

	return true;
}