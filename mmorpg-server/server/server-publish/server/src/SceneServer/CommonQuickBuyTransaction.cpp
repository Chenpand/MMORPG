#include "CommonQuickBuyTransaction.h"
#include <CLQuickBuyDataEntry.h>
#include <CLErrorCode.h>
#include <CLQuickBuyProtocol.h>
#include <CLMallShopMultiIDataEntry.h>
#include <udplog_typedef.h>
#include "Player.h"


CommonQuickBuyTransaction::CommonQuickBuyTransaction(UInt32 id, UInt32 num)
: QuickBuyTransaction(::GetReason(SOURCE_TYPE_QUICK_BUY_COMMON, id, num)), m_ItemId(id), m_Num(num)
{
	
}

CommonQuickBuyTransaction::~CommonQuickBuyTransaction()
{

}

bool CommonQuickBuyTransaction::BeginTransaction(Player* player)
{
	if (!player)
	{
		return false;
	}

	if (m_Num == 0)
	{
		OnFinish(player, ErrorCode::QUICK_BUY_INVALID_NUM);
		return false;
	}

	auto data = QuickBuyDataEntryMgr::Instance()->FindEntry(m_ItemId);
	if (!data)
	{
		OnFinish(player, ErrorCode::QUICK_BUY_INVALID_ITEM);
		return false;
	}

	if (data->costItemId == ITEM_POINT && !player->SecurityLockCheck())
	{
		OnFinish(player, ErrorCode::SECURITY_LOCK_FORBID_OP);
		return false;
	}

	SetCost(data->costItemId, data->costNum * m_Num);

	if (!_CheckRes(player))
	{
		OnFinish(player, ErrorCode::QUICK_BUY_NOT_ENOUGH_MONEY);
		return false;
	}

	if (!player->CheckAddItem(m_ItemId, m_Num))
	{
		OnFinish(player, ErrorCode::QUICK_BUY_BAG_FULL);
		return false;
	}

	OnFinish(player, ErrorCode::SUCCESS);
	return false;
}

void CommonQuickBuyTransaction::OnBuy(Player* player)
{
	if (!player)
	{
		return;
	}

	auto data = QuickBuyDataEntryMgr::Instance()->FindEntry(m_ItemId);
	if (data)
	{
		if (data->costItemId == ITEM_POINT)
		{
			UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE);
			UInt8 multiple = 0;
			UInt32 endTime = 0;
			MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(m_ItemId, multiple, endTime);
			player->AddMallPoint(GetReason().c_str(), m_Num * data->costNum * (data->multiple + multiple) * rate);
		}
	}

	player->AddItem(GetReason().c_str(), m_ItemId, m_Num);
}
