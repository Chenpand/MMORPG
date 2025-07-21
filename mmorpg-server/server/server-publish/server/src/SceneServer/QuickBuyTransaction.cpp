#include "QuickBuyTransaction.h"
#include <CLErrorCode.h>
#include <CLQuickBuyDataEntry.h>
#include <CLQuickBuyProtocol.h>
#include "Player.h"

QuickBuyTransaction::QuickBuyTransaction(std::string reason)
	: m_Reason(reason)
{
	static UInt32 s_id = 0;
	m_id = ++s_id;
	m_costItemId = 0;
	m_costNum = 0;
}

QuickBuyTransaction::~QuickBuyTransaction()
{

}

bool QuickBuyTransaction::SetCostByQuickBuyTable(UInt32 id)
{
	auto data = QuickBuyDataEntryMgr::Instance()->FindEntry(id);
	if (!data)
	{
		return false;
	}

	SetCost(data->costItemId, data->costNum);
	return true;
}

bool QuickBuyTransaction::IsTimeout(const Avalon::Time& now)
{
	return false;
}

bool QuickBuyTransaction::OnFinish(Player* player, UInt32 result)
{
	if (result == ErrorCode::SUCCESS)
	{
		if (!_ConsumeRes(player, GetReason().c_str()))
		{
			result = ErrorCode::QUICK_BUY_NOT_ENOUGH_MONEY;
		}
		else
		{
			OnBuy(player);
		}
	}
	else
	{
		OnFailure(player, result);
	}

	if (player && result != ErrorCode::SECURITY_LOCK_FORBID_OP)
	{
		CLProtocol::SceneQuickBuyRes res;
		res.result = result;
		player->SendProtocol(res);
	}

	return true;
}

bool QuickBuyTransaction::_CheckRes(Player* player)
{
	if (!player)
	{
		return false;
	}

	if (player->GetItemNum(m_costItemId) < m_costNum)
	{
		return false;
	}

	return true;
}

bool QuickBuyTransaction::_ConsumeRes(Player* player, const char* reason)
{
	if (!player)
	{
		return false;
	}
	
	return player->RemoveItem(reason, m_costItemId, m_costNum);
}