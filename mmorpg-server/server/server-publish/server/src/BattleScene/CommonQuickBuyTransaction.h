#ifndef __COMMON_QUICK_BUY_TRANSACTION_H__
#define __COMMON_QUICK_BUY_TRANSACTION_H__

#include "QuickBuyTransaction.h"

/*
	同意快速购买流程，就是购买物品到背包
*/
class CommonQuickBuyTransaction : public QuickBuyTransaction
{
public:
	CommonQuickBuyTransaction(UInt32 id, UInt32 num);
	virtual ~CommonQuickBuyTransaction();

protected:
	/**
	*@brief 事务开始成功
	*/
	virtual bool BeginTransaction(Player* player);

	/**
	*@brief 开始购买道具流程
	*/
	virtual void OnBuy(Player* player);

private:
	UInt32			m_ItemId;
	UInt32			m_Num;
};

#endif