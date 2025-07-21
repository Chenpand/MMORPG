#ifndef __QUICK_BUY_TRANSACTION_H__
#define __QUICK_BUY_TRANSACTION_H__

/*
	���ٹ�������ӿ�
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
	*@brief ͨ�����ٹ������������
	*/
	bool SetCostByQuickBuyTable(UInt32 id);

	/**
	*@brief �����Ƿ�ʱ
	*/
	bool IsTimeout(const Avalon::Time& now);

	/**
	*@brief ����ʼ�ɹ�
	*/
	virtual bool BeginTransaction(Player* player) = 0;

	/**
	*@brief ����ʼ����
	*/
	virtual bool OnFinish(Player* player, UInt32 result);

protected:

	/**
	*@brief ��ʼ�����������
	*/
	virtual void OnBuy(Player* player) {};

	/**
	*@brief ִ��ʧ��
	*/
	virtual void OnFailure(Player* player, UInt32 errorCode) {};

protected:
	/**
	*@brief �����Ҫ����Դ�����������ҡ����ߡ�������
	*/
	bool _CheckRes(Player* player);

	/**
	*@brief ������Դ�����ҡ����ߡ�������
	*/
	bool _ConsumeRes(Player* player, const char* reason);

private:
	// ΨһID
	UInt32			m_id;
	// �������ĵĻ���
	UInt32			m_costItemId;
	// ��������
	UInt32			m_costNum;
	// ԭ��
	std::string		m_Reason;
};

#endif