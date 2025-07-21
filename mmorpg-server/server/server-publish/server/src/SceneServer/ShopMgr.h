#ifndef _SCENE_SHOP_MGR_H_
#define _SCENE_SHOP_MGR_H_

#include "Shop.h"
#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLItemProtocol.h>
#include <CLSysRecordProtocol.h>
#include <CLSysRecordDefine.h>


class Player;

class ShopMgr
{
public:
	ShopMgr();
	~ShopMgr();

	//���á����������
	void SetOwner(Player *pPlayer) { m_pOwner = pPlayer; }
	Player* GetOwner() const { return m_pOwner; }

	//��ѯ�̵�
	UInt32 OnQueryItem(UInt8 shopId, UInt8 cache);
	//�������
	UInt32 OnBuy(const CLProtocol::SceneShopBuy& protocol, UInt16& newNum, UInt32& leaseEndTimeStamp);
	//ˢ���̵�
	UInt32 OnRefreshShop(UInt8 shopId);
	//�����̵�
	bool LoadShop(Shop* shop);
	//������Ʒ
	void LoadShopItem(ShopItem* shopItem);
	//��ȡ�̵�
	Shop* GetShop(UInt8 shopId);
	//ɾ���̵�
	void DeleteShop(UInt8 shopId);
	//��ȡ��Ʒ
	ShopItem* GetShopItem(UInt8 shopId, UInt32 shopItemId);
	//��ѯ�̵�ʣ��ˢ�´���
	void OnQueryRefreshNum(UInt8 shopId, UInt8& restNum, UInt8& maxNum);

	void OnDayChange();
	void OnOnline(bool bDayChange);
	//����ֿ��̵�ص�
	void BuyGuildStorageCb(UInt32 retCode, UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt32 costId, UInt32 costNum);

public:
	/**
	*@brief �����л�
	*/
	void OnSceneChanged();

	/**
	*@brief ���³ǽ���
	*/
	void OnDungeonRaceEnd(UInt32 dungeonId, UInt32 mysticalMerchantId);

public:
	/**
	*@brief ��ѯsysrecord���ݷ���
	*/
	void OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& protocol);

private:
	UInt32 BuyNormalMarket(UInt32 shopItemId, UInt16 num);
	UInt32 BuyBlackMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum);
	UInt32 BuyGuildStorage(UInt8 shopId, UInt32 shopItemId, UInt16 num);
	UInt32 BuyDungeonMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum);
	UInt32 BuyLeaseMarket(UInt8 shopId, UInt32 shopItemId, UInt16 num, UInt16& newNum, UInt32& leaseEndTimeStamp);

	// ����ʱ�����������
	bool OnRemoveExtraItemOnBuy(ShopItemDataEntry* shopItemDataEntry, const std::string& reason);

	//�Ƿ��ǹ����̵�
	bool IsNeedRefresh(Shop* shop);

	//ˢ���̵�����б�
	Shop* RefreshShop(UInt32 shopId, bool first, UInt32 version);
	//ͬ���ͻ���
	void SyncClient(Shop* shop, bool cache = false);
	//ͬ��DB
	void SyncDB(Shop* shop);
	//����ܷ���
	UInt32 CheckBuy(ShopItem* shopItem, UInt16 num, UInt32& cost, ItemDataEntry*& costItem);
	//����ˢ������Ļ���
	UInt16 GetRefreshCost(Shop* shop);
	//�ܷ��ֶ�ˢ��
	bool _CanManualRefresh(ShopRefreshType type);

	// ����ܷ���
	UInt32 CheckOtherCost(ShopItem* shopItem, UInt16 num);
	// �ۻ���
	UInt32 RemoveOtherCost(ShopItem* shopItem, const std::string& reason, UInt16 num);

	//�����������
	UInt32 _RandMysticalMerchant();

	//����������Ʒ��������
	void _UpdateMysticalMerchantShopItemNum(const CLProtocol::SceneSysRecordDataRes& res);

private:// �̵�����ˢ��
	/**
	*@brief ˢ�������Ƿ����
	*/
	bool _IsNeedRefreshOnRefreshCycle(Shop* shop, UInt32 refreshCycleType);

	/**
	*@brief �����̵�δˢ��ҳǩ�е���Ʒ����
	*/
	bool _KeepUnRefreshShopItemNum(Shop* shop, UInt32 unRefreshCycleType);

private:
	// ������
	Player		*m_pOwner;
	// �̵��б�
	std::map<ObjID_t, Shop*>	m_shopList;
};

#endif

