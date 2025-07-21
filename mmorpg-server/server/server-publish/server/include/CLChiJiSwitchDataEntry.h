/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJISWITCH_DATA_ENTRY_H__
#define __CL_CHIJISWITCH_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct ChiJiSwitch
{

	ChiJiSwitch() : startTime(0), endTime(0) {}
	// 开始时间
	UInt32 startTime;
	// 结束时间
	UInt32 endTime;
};

// __CUSTOM_STRUCT_END__

class ChiJiSwitchDataEntry : public Avalon::DataEntry
{
public:
	ChiJiSwitchDataEntry();
	virtual ~ChiJiSwitchDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 周几
	UInt32                                          id;
	// 开启时间
	std::string                                     openTime;
	// 关闭时间
	std::string                                     closeTime;
	// 是否启用
	UInt32                                          isOpen;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ChiJiSwitchDataEntryMgr : public Avalon::DataEntryMgrBase<ChiJiSwitchDataEntry>
	, public Avalon::Singleton<ChiJiSwitchDataEntryMgr>
{
public:
	virtual bool AddEntry(ChiJiSwitchDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const ChiJiSwitch* GetChiJiSwitch(UInt32 week);

private:
	std::map<UInt32, ChiJiSwitch> m_ChiJiSwitchMap;
		
// __CUSTOM_MGR_END__
};

#endif
