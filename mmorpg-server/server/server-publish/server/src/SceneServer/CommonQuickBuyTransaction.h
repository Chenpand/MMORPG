#ifndef __COMMON_QUICK_BUY_TRANSACTION_H__
#define __COMMON_QUICK_BUY_TRANSACTION_H__

#include "QuickBuyTransaction.h"

/*
	ͬ����ٹ������̣����ǹ�����Ʒ������
*/
class CommonQuickBuyTransaction : public QuickBuyTransaction
{
public:
	CommonQuickBuyTransaction(UInt32 id, UInt32 num);
	virtual ~CommonQuickBuyTransaction();

protected:
	/**
	*@brief ����ʼ�ɹ�
	*/
	virtual bool BeginTransaction(Player* player);

	/**
	*@brief ��ʼ�����������
	*/
	virtual void OnBuy(Player* player);

private:
	UInt32			m_ItemId;
	UInt32			m_Num;
};

#endif