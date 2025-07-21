/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ANTIADDICTTIME_DATA_ENTRY_H__
#define __CL_ANTIADDICTTIME_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class AntiAddictTimeDataEntry : public Avalon::DataEntry
{
public:
	AntiAddictTimeDataEntry();
	virtual ~AntiAddictTimeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 是否假日
	UInt32                                          isHoliday;
	// 年龄
	UInt32                                          age;
	// 游戏时长(0不限时长)
	UInt32                                          gameTime;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class AntiAddictTimeDataEntryMgr : public Avalon::DataEntryMgrBase<AntiAddictTimeDataEntry>
	, public Avalon::Singleton<AntiAddictTimeDataEntryMgr>
{
public:
	virtual bool AddEntry(AntiAddictTimeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取沉迷时间
	UInt32 GetAntiAddictTime(bool isHoliday, UInt32 age);

private:

	// 沉迷时间(key->日期类型(key->年龄，val->沉迷时间))
	std::map<UInt32, std::map<UInt32, UInt32>> m_AntiAddictTimeMap;

// __CUSTOM_MGR_END__
};

#endif
