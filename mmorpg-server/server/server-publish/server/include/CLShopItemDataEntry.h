#ifndef _CL_SHOP_ITEM_DATAENTRY_H_
#define _CL_SHOP_ITEM_DATAENTRY_H_

#include "CLItemDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <bitset>

/**
 *@brief 商品数据项
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
	//商品id
	UInt32		shopItemId;
	//名字
	std::string shopItemName;
	//道具id
	UInt32		itemId;
	//商品类别
	UInt32		subType;
	//归属商店id
	UInt32		shopId;
	// 能否使用等价物品
	bool		useEqualItem;
	//花费货币Id
	UInt32		costItemId;
	//花费数量
	UInt32		costNum;
	//其他消耗
	std::vector<ItemReward> otherItemCost;
	//能否vip出售
	UInt8		vip;
	//是否vip限定
	UInt8		vipLimite;
	//限次出售
	Int16		numLimite;
	//限次重置
	UInt8		limiteOnce;
	//一组数量
	UInt32		grpNum;
	//上架权重
	UInt32		weight;
	//额外限制条件，enum ShopItemExlimite
	UInt32		exLimite;
	//额外限制值
	UInt32		exLimiteVal;
	//额外道具消耗
	std::vector<ItemReward> extraItemCost;
	//角色等级限制
	std::vector<UInt16> playerLevelLimit;
	//vip等级限制
	std::vector<UInt8> vipLevelLimit;
	//地下城难度限制
	std::vector<UInt8> dungeonHardLimit;
	//地下城子类型限制
	std::vector<UInt8> dungeonSubTypeLimit;
	//地下城id限制
	std::vector<UInt32> dungeonIdLimit;
	//折扣率
	std::vector<UInt32> discountRate;
	//折扣率权重
	std::vector<UInt32> discountRateWeight;
	//是否生成时属性适配
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
*@brief 商店数据项
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
	//商店id
	UInt32						shopId;
	//商店名称
	std::string					name;
	//商店类型
	ShopType					shopType;
	//辅助参数
	UInt32						helpId;
	//是否公会商店
	UInt8						isGuildShop;
	//刷新方式
	ShopRefreshType				refresh;
	//页签
	std::vector<UInt32>			subTypes;
	//对应页签刷新周期
	std::vector<UInt32>			refreshCycle;
	//页签-->页签刷新周期
	std::map<UInt32, UInt32>	refreshCycleSub;
	//刷新消耗
	std::vector<UInt32>			refreshCost;
	//刷新消耗上限
	UInt32						refreshCostMax;
	//刷新增长率
	UInt32						refreshIncRate;
	//刷新时间
	std::vector<UInt32>			refreshTimes;
	//刷新总权重
	UInt32						onSaleNum;
	//是否时限商品1
	UInt8						isLimiteGood1;
	//是否时限商品2
	UInt8						isLimiteGood2;
	//vip专属信息
	std::vector<ShopVipInfo>	vipInfos;
	//版本
	UInt32						version;
	// 稀有控制类型
	UInt32						rareControlType;
	//绑定类型(0:角色绑定,1:账号绑定)
	UInt8						bindType;
	//是否荣誉折扣
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
*@brief 神秘商人数据项
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
	// 商店id
	UInt32 shopId;
	// 权重
	UInt32 weight;
};

typedef Avalon::DataEntryMgr<MysticalMerchantDataEntry> MysticalMerchantDataEntryMgr;


#endif
