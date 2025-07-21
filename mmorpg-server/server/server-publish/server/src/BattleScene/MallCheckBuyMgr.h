#ifndef _MALL_CHECK_BUY_MGR_
#define _MALL_CHECK_BUY_MGR_


#include <CLMallProtocol.h>
#include <CLCounterNameDefine.h>

class Player;


class MallCheckBuyInfo
{
public:
	MallCheckBuyInfo();
	~MallCheckBuyInfo();

public:
	void Init(const CLProtocol::WorldMallCheckBuy &req);

	void SetOwner(Player* owner){ m_Owner = owner; }
	Player* GetOwner() const { return m_Owner; }

	/**
	*@brief �����̳���Ʒ
	*/
	void OnBuy();

private:
	/**
	*@brief �Ƿ�����Ұ����
	*/
	bool _IsPlayerBindGiftPack();

	/**
	*@brief �Ƿ��޹�
	*/
	bool _IsLimitBuy(const std::string& mallItemIdStr);

	/**
	*@brief ��õ��߶�Ӧ������
	*/
	UInt32 _GetItemNum(UInt32 itemDataId);

	/**
	*@brief �����߷��͵����
	*/
	void _AddItemsToPlayer(const std::string& itemReason, std::vector<UInt64>& itemUids);

	/**
	*@brief �����߷��͵��ʼ�
	*/
	UInt32 _AddItemsToMail(const std::string& reason, const std::vector<UInt32>& mailItems);

	/**
	*@brief �����߷��͸��������(�ʼ���ʽ)
	*/
	void _SendMailToAddonPayPlayer(UInt64 tarId, UInt32 mailItem);

	//��ȡ����name
	std::string _GetCounterName();

	/**
	*@brief ������idƴ���ַ���
	*/
	std::string _GetItemStr(const std::vector<UInt32>& mailItems);

	/**
	*@brief �Ƿ�ʱװ
	*/
	bool _IsFashion(UInt8 itemSubType);

	/**
	*@brief ��ȡ�̳����յĴ�����
	*/
	UInt32 _GenMallUtilDiscountRate(Player* player, UInt32 baseRate);

private:
	//ӵ����
	Player*					m_Owner;
	// ���id
	ObjID_t					m_PlayerId;
	// �˺�id
	UInt32					m_AccId;
	// �������
	UInt32					m_Cost;
	// ��������
	UInt8					m_MoneyType;
	// �̳���Ʒid
	UInt32					m_MallItemId;
	// ����
	UInt16					m_MallItemNum;
	// ����id��
	std::vector<UInt32>		m_ItemDataId;
	// ����id-->����
	std::map<UInt32, UInt32> m_ItemData;
	// ��õ��ߵ����id
	ObjID_t					m_Receiver;
	// �޹�ˢ������
	MallLimitBuyType		m_MallLimitBuyType;
	// �޹�����
	UInt32					m_LimitNum;
	// ���޹�����
	UInt32					m_TotalLimitNum;
	// ��Ʒ����
	UInt8					m_GoodsType;
	// ������õ�vip����
	UInt16					m_VipScore;
	// ������
	UInt32					m_DiscountRate;
	// �ۿ�ȯid
	UInt32					m_DiscountCouponId;
};

#endif