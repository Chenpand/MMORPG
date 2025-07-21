/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ACCOUNTSHOPITEM_DATA_ENTRY_H__
#define __CL_ACCOUNTSHOPITEM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include "CLItemDefine.h"
#include "TimeUtil.h"
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
class AccountShopItemDataEntry;
typedef std::vector<AccountShopItemDataEntry*> AccountShopItemDataVec;
// __CUSTOM_STRUCT_END__

class AccountShopItemDataEntry : public Avalon::DataEntry
{
public:
	AccountShopItemDataEntry();
	virtual ~AccountShopItemDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 GetColNum() { return 22; };

	bool CalculateTime();
	// 是否上架
	bool IsOnSale(UInt16 playerLevel);
	bool operator<(const AccountShopItemDataEntry &other) const;
	
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 商品ID
	UInt32                                          id;
	// 商品名称
	std::string                                     shopItemName;
	// 归属商店
	UInt32                                          shopId;
	// 页签类别
	UInt32                                          tabType;
	// 职业类别
	UInt32                                          jobType;
	// 上架道具
	UInt32                                          itemDatId;
	// 上架数量
	UInt32                                          itemNum;
	// 购买消耗
	std::string                                     costItems;
	// 时间计算类型
	UInt32                                          timeCalcType;
	// 开始时间
	std::string                                     startTime;
	// 结束时间
	std::string                                     endTime;
	// 账号刷新类型
	UInt32                                          accountRefreshType;
	// 账号刷新时间点
	std::string                                     accountRefreshTimePoint;
	// 账号限购类型
	UInt32                                          accountLimitBuyType;
	// 账号限购数量
	UInt32                                          accountLimitBuyNum;
	// 角色刷新类型
	UInt32                                          roleRefreshType;
	// 角色刷新时间点
	std::string                                     roleRefreshTimePoint;
	// 角色限购类型
	UInt32                                          roleLimitBuyType;
	// 角色限购数量
	UInt32                                          roleLimitBuyNum;
	// 扩展条件1
	UInt32                                          extensibleCondition;
	// 是否需要预览按钮
	UInt32                                          needPreviewFunc;
	// 商品展示等级限制
	UInt32                                          showLevelLimit;
	// 商品展示排序
	UInt32                                          rank;
	// 单次购买次数限制
	UInt32											buyLimit;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 购买消耗
	ItemRewardVec costItemVec;
	// 账号刷新日期
	TimeUtil::RefreshDate accountRefreshDate;
	// 角色刷新日期
	TimeUtil::RefreshDate roleRefreshDate;
	// 上架时间
	UInt32 onSaleTime;
	// 下架时间
	UInt32 offSaleTime;
// __CUSTOM_ENTRYFIELD_END__
};

class AccountShopItemDataEntryMgr : public Avalon::DataEntryMgrBase<AccountShopItemDataEntry>
	, public Avalon::Singleton<AccountShopItemDataEntryMgr>
{
public:
	virtual bool AddEntry(AccountShopItemDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	bool GetAccountShopItemDatas(UInt32 shopid, AccountShopItemDataVec& datas);

	void OnAddEntry(AccountShopItemDataEntry* entry);

	void OnReload();

private:
	HashMap<UInt32, AccountShopItemDataVec> m_ShopItemsMap;
// __CUSTOM_MGR_END__
};

#endif
