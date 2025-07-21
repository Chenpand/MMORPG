#ifndef _SHOP_H_
#define _SHOP_H_

#include "ShopItem.h"

class Player;

enum ShopSyncDBState
{
	SSDS_NONE = 0,				
	SSDS_INSERT,				//����
	SSDS_UPDATE,				//����
};

class Shop : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Shop, Avalon::NullMutex)
public:
	Shop();
	~Shop();

public:
	static Shop* Create(UInt32 shopId);
	void SetDataEntry(ShopDataEntry* dataEntry){ 
		m_pDataEntry = dataEntry; 
		m_shopid = dataEntry->shopId;
	}
	ShopDataEntry* GetDataEntry() const { return m_pDataEntry; }

	
public:	
	inline UInt32 GetShopId() { return m_shopid; }

	inline UInt32 GetRefreshNum() { return m_refreshNum; }
	inline void SetRefreshNum(UInt32 value) { m_refreshNum = value; }
	inline ShopSyncDBState GetSyncDBState() { return m_syncDBState; }
	inline void SetSyncDBState(ShopSyncDBState state) { m_syncDBState = state; }
	inline UInt8 GetItemNum() { return (UInt8)m_shopItems.size(); }
	inline void SetOwner(ObjID_t owner) { m_owner = owner; }
	inline void SetVersion(UInt32 ver)	{ m_version = ver; }
	inline UInt32 GetVersion()	{ return m_version; }

	void AddShopItem(UInt32 itemId);
	void AddShopItem(ShopItem* item);

	void GetShopItems(std::vector<ShopItem*>& items) { items = m_shopItems; }

	void ClearItem();
	void ClearAllItems();

	ShopItem* GetShopItem(UInt32 shopItemId);

	void SetDungeonId(UInt32 dungeonId) { m_DungeonId = dungeonId; }
	UInt32 GetDungeonId() const { return m_DungeonId; }

public:// �̵�����ˢ��
	/**
	*@brief ���δˢ�µ���Ʒ����
	*/
	void ClearUnRefreshShopItem();

	/**
	*@brief �����̵���δˢ�µ���Ʒ����
	*/
	void KeepUnRefreshShopItemRestNum(UInt32 shopItemId, Int16 shopItemRestNum);

	/**
	*@brief �̵��Ƿ���δˢ�µ���Ʒ
	*/
	bool HasUnRefreshShopItem();

	/**
	*@brief ����δˢ����Ʒʣ������
	*/
	void SetUnRefreshShopItemRestNum();

	/**
	*@brief ���û�ȡˢ�����ڵ�ˢ��״̬
	*/
	void SetRefreshCycleTypeState(UInt32 refreshCycleType, UInt32 refreshCycleState);
	UInt32 GetRefreshCycleState(UInt32 refreshCycleType);

	/**
	*@brief ����ˢ��ʱ��
	*/
	void SetRefreshTime();

	/**
	*@brief ��ȡ��ˢ������ʱ���
	*/
	Avalon::Time GetRefreshTime() { return Avalon::Time(m_refreshTime); }

	/**
	*@brief ��ȡ��ˢ������ʱ���
	*/
	Avalon::Time GetWeekRefreshTime() { return Avalon::Time(m_weekRefreshTime); }

	/**
	*@brief ��ȡ��ˢ������ʱ���
	*/
	Avalon::Time GetMonthRefreshTime() { return Avalon::Time(m_monthRefreshTime); }

private:
	/**
	*@brief �ж�ˢ�·�ʽ״̬�Ƿ��Ǽ���״̬
	*/
	bool _CheckRefreshCycleStateActivated(RefreshCycleType refreshCycleType);

	/**
	*@brief ������һ��ˢ��ʱ���(���뼶)
	*/
	UInt64 CalculateNextRefreshTimestamp(RefreshCycleType refreshCycleType, UInt32 refreshHour);

public:	//��������
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner", m_owner)
	CL_DBPROPERTY("shopid", m_shopid)
	CL_DBPROPERTY("refreshtime", m_refreshTime)
	CL_DBPROPERTY("week_refreshtime", m_weekRefreshTime)
	CL_DBPROPERTY("month_refreshtime", m_monthRefreshTime)
	CL_DBPROPERTY("refreshnum", m_refreshNum)
	CL_DBPROPERTY("version", m_version)

	CL_DBPROPERTY_END()
	
private:
	//�̵�id
	ObjUInt32				m_shopid;
	//���uid
	ObjUInt64				m_owner;
	//ÿ��ˢ��ʱ���			
	ObjUInt64				m_refreshTime;
	//ÿ��ˢ��ʱ���		
	ObjUInt64				m_weekRefreshTime;
	//ÿ��ˢ��ʱ���		
	ObjUInt64				m_monthRefreshTime;
	//ˢ�´���
	ObjUInt32				m_refreshNum;
	//��Ʒ
	std::vector<ShopItem*>  m_shopItems;
	//ͬ��db״̬
	ShopSyncDBState			m_syncDBState;
	//�����
	ShopDataEntry*			m_pDataEntry;
	//�汾
	ObjUInt32				m_version;

	// ˢ�·�ʽ-->ˢ��״̬
	HashMap<UInt32, UInt32> m_RefreshCycleTypeWithState;

	// �̵��������ˢ��ʱδˢ��ҳǩ�е���Ʒ��ʣ������ ��Ʒid-->��Ʒ����
	HashMap<UInt32, Int16> m_UnRefreshShopItemRestNum;

	// ���³�id
	UInt32 m_DungeonId;
};

#endif
