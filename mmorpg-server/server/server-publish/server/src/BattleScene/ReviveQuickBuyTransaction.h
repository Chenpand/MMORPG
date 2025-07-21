#ifndef __REVIVE_QUICK_BUY_TRANSACTION_H__
#define __REVIVE_QUICK_BUY_TRANSACTION_H__

#include "QuickBuyTransaction.h"

class ReviveQuickBuyTransaction : public QuickBuyTransaction
{
public:
	ReviveQuickBuyTransaction(ObjID_t targetId, UInt32 reviveId);
	virtual ~ReviveQuickBuyTransaction();

protected:
	/**
	*@brief 事务开始成功
	*/
	virtual bool BeginTransaction(Player* player);

private:
	ObjID_t			m_targetId;
	UInt32			m_reviveId;
};

#endif