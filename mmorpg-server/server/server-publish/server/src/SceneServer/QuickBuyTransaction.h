#ifndef __QUICK_BUY_TRANSACTION_H__
#define __QUICK_BUY_TRANSACTION_H__

/*
	快速购买事务接口
*/

#include <CLDefine.h>
#include <AvalonObjectPool.h>

class Player;
class QuickBuyTransaction
{
	AVALON_DEFINE_OBJECTPOOL(QuickBuyTransaction, Avalon::NullMutex)
public:
	QuickBuyTransaction(std::string reason);
	virtual ~QuickBuyTransaction();

	inline UInt32 GetID() const { return m_id; }

	inline UInt32 GetCostItemId() const{ return m_costItemId; }

	inline void SetCost(UInt32 costItemId, UInt32 num) 
	{ 
		m_costItemId = costItemId;
		m_costNum = num;
	}

	inline const std::string& GetReason() const { return m_Reason; }

public:

	/**
	*@brief 通过快速购买表设置消耗
	*/
	bool SetCostByQuickBuyTable(UInt32 id);

	/**
	*@brief 事务是否超时
	*/
	bool IsTimeout(const Avalon::Time& now);

	/**
	*@brief 事务开始成功
	*/
	virtual bool BeginTransaction(Player* player) = 0;

	/**
	*@brief 事务开始结束
	*/
	virtual bool OnFinish(Player* player, UInt32 result);

protected:

	/**
	*@brief 开始购买道具流程
	*/
	virtual void OnBuy(Player* player) {};

	/**
	*@brief 执行失败
	*/
	virtual void OnFailure(Player* player, UInt32 errorCode) {};

protected:
	/**
	*@brief 检测需要的资源够不够（货币、道具。。。）
	*/
	bool _CheckRes(Player* player);

	/**
	*@brief 消耗资源（货币、道具。。。）
	*/
	bool _ConsumeRes(Player* player, const char* reason);

private:
	// 唯一ID
	UInt32			m_id;
	// 购买消耗的货币
	UInt32			m_costItemId;
	// 货币数量
	UInt32			m_costNum;
	// 原因
	std::string		m_Reason;
};

#endif