/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_WEEKSIGN_DATA_ENTRY_H__
#define __CL_WEEKSIGN_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include <CLItemDataEntry.h>

// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
enum 
{
	WEEK_REFRESH = 1,	// 周刷新
	WEEK_MONTH = 2,	// 月刷新
};

struct WeekSignItem
{
	WeekSignItem() : rewardUid(0), rewardId(0), rewardNum(0), rewardWeight(0), serverLimit(0), roleLimit(0)
		, isBigReward(0), notifyId(0), superLink(0), refreshType(0), isLimit(false), itemEntry(NULL) {}

	// 奖品ID：
	UInt32 rewardUid;
	// 奖励物品
	UInt32 rewardId;
	// 奖励数量
	UInt32 rewardNum;
	// 权重
	UInt32 rewardWeight;
	// 全服限制数量(0不限量)
	UInt32 serverLimit;
	// 角色限制数量(0不限量)
	UInt32 roleLimit;
	// 是否大奖
	UInt32 isBigReward;
	// 公告ID
	UInt32 notifyId;
	// 超链接
	UInt32 superLink;
	// 刷新类型
	UInt32 refreshType;
	// 是否限制
	bool isLimit;
	// vip概率(百分比)
	std::map<UInt32, UInt32> vipRate;
	// 物品配置
	ItemDataEntry* itemEntry;
};

struct ActItem
{
	ActItem() : actType(0), itemUid(0) {}
	ActItem(UInt32 type, UInt32 id) : actType(type), itemUid(id) {}

	UInt32 actType;
	UInt32 itemUid;

	bool operator < (const ActItem& pa) const
	{
		if (actType < pa.actType)
		{
			return true;
		}
		else if (actType == pa.actType)
		{
			return itemUid < pa.itemUid;
		}

		return false;
	}
};

// __CUSTOM_STRUCT_END__

class WeekSignDataEntry : public Avalon::DataEntry
{
public:
	WeekSignDataEntry();
	virtual ~WeekSignDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// id
	UInt32                                          id;
	// 活动类型
	UInt32                                          opActType;
	// 奖励物品
	UInt32                                          rewardId;
	// 奖励数量
	UInt32                                          rewardNum;
	// 权重
	UInt32                                          rewardWeight;
	// 全服限制数量(0不限量)
	UInt32                                          serverLimit;
	// 角色限制数量(0不限量)
	UInt32                                          roleLimit;
	// VIP获得概率(不填的vip表示不限制，万分比)
	std::vector<std::string>                        vipRate;
	// 是否大奖
	UInt32                                          isBigReward;
	// 公告ID
	UInt32                                          notifyId;
	// 公告立即前往超链
	UInt32                                          superLink;
	// 刷新类型
	UInt32                                          refreshType;
	// 奖品ID：
	UInt32                                          rewardUid;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class WeekSignDataEntryMgr : public Avalon::DataEntryMgrBase<WeekSignDataEntry>
	, public Avalon::Singleton<WeekSignDataEntryMgr>
{
public:
	virtual bool AddEntry(WeekSignDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const WeekSignItem* GetWeekSignItem(UInt32 actType, UInt32 vipLvl);
	const WeekSignItem* GetWeekSignTrashItem(UInt32 actType);
	const WeekSignItem* FindWeekSignItem(UInt32 actType, UInt32 itemUid);

	void ReSortWeekSignCfg(UInt32 actType);
	void SetLimitWeekSignItem(UInt32 actType, UInt32 itemUid, UInt32 num);
	void ResetSortWeekSignCfg(UInt32 actType);

private:

	typedef std::map<UInt32, WeekSignItem*> WeekSignItemMap;
	typedef std::map<UInt32, WeekSignItemMap> WeekSignCfgMap;

	// 周签到奖励
	WeekSignCfgMap m_WeekSignCfgMap;
	// 周签到奖励权重
	std::map<UInt32, UInt32>  m_ActWeightMap;

	typedef std::vector<WeekSignItem*> WeekSignItemVec;
	typedef std::map<UInt32, WeekSignItemVec> WeekSignCfgMap2;

	// 必出垃圾
	WeekSignCfgMap2 m_WeekSignTrashMap;

	// 所有奖励物品
	std::map<ActItem, WeekSignItem*>	m_ItemCfg;

// __CUSTOM_MGR_END__
};

#endif
