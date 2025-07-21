/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_OVERLONDDEVICEVALUE_DATA_ENTRY_H__
#define __CL_OVERLONDDEVICEVALUE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//自定义头文件	
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举	
// __CUSTOM_STRUCT_BEGIN__
struct OverlondProbabilityData
{
	//激化等级
	UInt32	sharpenLv;
	//权重
	UInt32	weight;
};
// __CUSTOM_STRUCT_END__

class OverlondDeviceValueDataEntry : public Avalon::DataEntry
{
public:
	OverlondDeviceValueDataEntry();
	virtual ~OverlondDeviceValueDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口	
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	//ID
	UInt32                                        id;
	//道具ID
	UInt32                                        itemDataID;
	//激化等级
	UInt32                                        sharpenLv;
	//权重
	UInt32                                        weight;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class OverlondDeviceValueDataEntryMgr : public Avalon::DataEntryMgrBase<OverlondDeviceValueDataEntry>
	, public Avalon::Singleton<OverlondDeviceValueDataEntryMgr>
{
public:
	virtual bool AddEntry(OverlondDeviceValueDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
public:
	UInt32 RandSharpenLevel(UInt32 itemDataID, UInt8 lvl);

private:
	std::map<UInt32, std::vector<OverlondProbabilityData>> probabilityMap;
// __CUSTOM_MGR_END__
};

#endif
