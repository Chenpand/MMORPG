/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJITIME_DATA_ENTRY_H__
#define __CL_CHIJITIME_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <set>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
enum ChickenType
{
	CT_CHOOSE_ROLE	= 1,	//选角色
	CT_CHOOSE_JOB	= 2,	//选职业
	CT_PREPARE_TIME	= 3,	//准备阶段
	CT_PUT_ITEM		= 4,	//道具投放
	CT_START_PK		= 5,	//开始PK
	CT_POSION_SCOPE	= 6,
	CT_CREATE_NPC	= 7,
	CT_ENUM_END
};

struct ShopTimeCfg
{
	ShopTimeCfg() : shopId(0), time(0) {}
	UInt32 shopId;
	UInt32 time;
};

// __CUSTOM_STRUCT_END__

class ChiJiTimeDataEntry : public Avalon::DataEntry
{
public:
	ChiJiTimeDataEntry();
	virtual ~ChiJiTimeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 编号ID
	UInt32                                          id;
	// 吃鸡进程类型
	ChickenType                                     chickenType;
	// 参数
	UInt32                                          chickenParameter;
	// 开启时间（秒）
	UInt32                                          startTime;
	// 持续时间（秒）
	UInt32                                          continueTime;
	// 持续掉血伤害（千分比/秒）
	UInt32                                          continueInjury;
	// 下一次毒圈ID
	UInt32                                          nextDuquanID;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	// 最小X
	UInt32                                          minX;
	// 最大X
	UInt32                                          maxX;
	// 最小Y
	UInt32                                          minY;
	// 最大Y
	UInt32											maxY;
// __CUSTOM_ENTRYFIELD_END__
};

class ChiJiTimeDataEntryMgr : public Avalon::DataEntryListMgrBase<ChiJiTimeDataEntry>
	, public Avalon::Singleton<ChiJiTimeDataEntryMgr>
{
public:
	virtual bool AddEntry(ChiJiTimeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
