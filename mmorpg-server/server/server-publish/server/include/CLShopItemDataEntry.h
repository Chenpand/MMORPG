#ifndef _CL_SHOP_ITEM_DATAENTRY_H_
#define _CL_SHOP_ITEM_DATAENTRY_H_

#include "CLItemDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <bitset>

/**
 *@brief ��Ʒ������
 */
class ShopItemDataEntry : public Avalon::DataEntry
{
public:
	ShopItemDataEntry();
	~ShopItemDataEntry();

public:
	UInt32 GetKey() const { return shopItemId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 26; };

public:
	//��Ʒid
	UInt32		shopItemId;
	//����
	std::string shopItemName;
	//����id
	UInt32		itemId;
	//��Ʒ���
	UInt32		subType;
	//�����̵�id
	UInt32		shopId;
	// �ܷ�ʹ�õȼ���Ʒ
	bool		useEqualItem;
	//���ѻ���Id
	UInt32		costItemId;
	//��������
	UInt32		costNum;
	//��������
	std::vector<ItemReward> otherItemCost;
	//�ܷ�vip����
	UInt8		vip;
	//�Ƿ�vip�޶�
	UInt8		vipLimite;
	//�޴γ���
	Int16		numLimite;
	//�޴�����
	UInt8		limiteOnce;
	//һ������
	UInt32		grpNum;
	//�ϼ�Ȩ��
	UInt32		weight;
	//��������������enum ShopItemExlimite
	UInt32		exLimite;
	//��������ֵ
	UInt32		exLimiteVal;
	//�����������
	std::vector<ItemReward> extraItemCost;
	//��ɫ�ȼ�����
	std::vector<UInt16> playerLevelLimit;
	//vip�ȼ�����
	std::vector<UInt8> vipLevelLimit;
	//���³��Ѷ�����
	std::vector<UInt8> dungeonHardLimit;
	//���³�����������
	std::vector<UInt8> dungeonSubTypeLimit;
	//���³�id����
	std::vector<UInt32> dungeonIdLimit;
	//�ۿ���
	std::vector<UInt32> discountRate;
	//�ۿ���Ȩ��
	std::vector<UInt32> discountRateWeight;
	//�Ƿ�����ʱ��������
	UInt8				attriFit;
};

typedef std::map<UInt32, std::vector<ShopItemDataEntry*>>	ShopItemMap;

class ShopItemDataEntryMgr : public Avalon::DataEntryMgrBase<ShopItemDataEntry>
	, public Avalon::Singleton<ShopItemDataEntryMgr>
{
public:
	ShopItemDataEntryMgr();
	~ShopItemDataEntryMgr();

public:
	bool AddEntry(ShopItemDataEntry* dataEntry);
	void GetShopCommonItems(UInt32 shopID, std::vector<ShopItemDataEntry*>& datas);
	void GetShopVipItems(UInt32 shopID, std::vector<ShopItemDataEntry*>& datas);
	void GetShopItemsByOccu(UInt32 shopID, UInt8 oc, std::vector<ShopItemDataEntry*>& datas);
private:
	ShopItemMap m_commonShopItems;
	ShopItemMap m_vipShopItems;
};

/**
*@brief �̵�������
*/

struct ShopVipInfo
{
	ShopVipInfo()
	{
		lv = 0;
		discount = 0;
	}
	UInt8 lv;
	UInt8 discount;
};
class ShopDataEntry : public Avalon::DataEntry
{
public:
	ShopDataEntry();
	~ShopDataEntry();

public:
	UInt32 GetKey() const { return shopId; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 19; };

private:
	void _SplitToVecInt(Avalon::DataRow& row, std::vector<UInt32>& vecInt);
	void _SetRefreshCycleSub();

public:
	//�̵�id
	UInt32						shopId;
	//�̵�����
	std::string					name;
	//�̵�����
	ShopType					shopType;
	//��������
	UInt32						helpId;
	//�Ƿ񹫻��̵�
	UInt8						isGuildShop;
	//ˢ�·�ʽ
	ShopRefreshType				refresh;
	//ҳǩ
	std::vector<UInt32>			subTypes;
	//��Ӧҳǩˢ������
	std::vector<UInt32>			refreshCycle;
	//ҳǩ-->ҳǩˢ������
	std::map<UInt32, UInt32>	refreshCycleSub;
	//ˢ������
	std::vector<UInt32>			refreshCost;
	//ˢ����������
	UInt32						refreshCostMax;
	//ˢ��������
	UInt32						refreshIncRate;
	//ˢ��ʱ��
	std::vector<UInt32>			refreshTimes;
	//ˢ����Ȩ��
	UInt32						onSaleNum;
	//�Ƿ�ʱ����Ʒ1
	UInt8						isLimiteGood1;
	//�Ƿ�ʱ����Ʒ2
	UInt8						isLimiteGood2;
	//vipר����Ϣ
	std::vector<ShopVipInfo>	vipInfos;
	//�汾
	UInt32						version;
	// ϡ�п�������
	UInt32						rareControlType;
	//������(0:��ɫ��,1:�˺Ű�)
	UInt8						bindType;
	//�Ƿ������ۿ�
	UInt32						isHonorDiscount;
};

typedef std::map<UInt32, ShopDataEntry*>	ShopMap;

class ShopDataEntryMgr : public Avalon::DataEntryMgrBase<ShopDataEntry>
	, public Avalon::Singleton<ShopDataEntryMgr>
{
public:
	ShopDataEntryMgr();
	~ShopDataEntryMgr();

	bool AddEntry(ShopDataEntry* dataEntry);

	void GetAccountShops(std::vector<ShopDataEntry*>& shops);

private:
	std::vector<ShopDataEntry*> m_AccountShops;
};

/**
*@brief ��������������
*/
class MysticalMerchantDataEntry : public Avalon::DataEntry
{
public:
	MysticalMerchantDataEntry() {}
	~MysticalMerchantDataEntry() {}

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 3; };

public:
	UInt32 id;
	// �̵�id
	UInt32 shopId;
	// Ȩ��
	UInt32 weight;
};

typedef Avalon::DataEntryMgr<MysticalMerchantDataEntry> MysticalMerchantDataEntryMgr;


#endif
