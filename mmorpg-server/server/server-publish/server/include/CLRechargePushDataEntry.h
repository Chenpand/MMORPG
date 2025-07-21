/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_RECHARGEPUSH_DATA_ENTRY_H__
#define __CL_RECHARGEPUSH_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class RechargePushDataEntry : public Avalon::DataEntry
{
public:
	RechargePushDataEntry();
	virtual ~RechargePushDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 推送内容ID
	UInt32                                          id;
	// 推送类型
	UInt32                                          type;
	// 触发前提：
	UInt32                                          lvLower;
	// 触发前提：
	UInt32                                          lvToplimit;
	// 触发前提：
	UInt32                                          vipLvLower;
	// 判定道具ID
	std::string										judgePropID;
	// 判定参数类型
	UInt32                                          parameterType;
	// 天数（单位：天）
	std::string                                     daysBefore;
	// 判定参数1
	UInt32                                          parameter1;
	// 天数（单位：天）
	UInt32                                          recent;
	// 判定参数2
	UInt32                                          parameter2;
	// 推送道具ID及数量
	std::string                                     pushID;
	// 道具购买次数
	UInt32                                          buyTimes;
	// 推送道具一组的购买价格
	UInt32                                          pushPrice;
	// 推送道具一组的原本价格
	UInt32                                          pushBeforePrice;
	// 推送CD（单位：天）
	UInt32											pushCD;
	// 冷却CD（单位：天）
	UInt32											coolCD;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 判定道具ID
	std::vector<UInt32>                             judgePropIDs;
	UInt32											daysBeforMin;
	UInt32											daysBeforeMax;
	UInt32											pushItemDataID;
	UInt32											pushItemCount;
// __CUSTOM_ENTRYFIELD_END__
};

class RechargePushDataEntryMgr : public Avalon::DataEntryMgrBase<RechargePushDataEntry>
	, public Avalon::Singleton<RechargePushDataEntryMgr>
{
public:
	virtual bool AddEntry(RechargePushDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	const RechargePushDataEntry* GetDataEntryByPushId(UInt32 pushId);
// __CUSTOM_MGR_END__
};

#endif
