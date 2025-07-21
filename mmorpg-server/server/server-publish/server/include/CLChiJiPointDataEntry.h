/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJIPOINT_DATA_ENTRY_H__
#define __CL_CHIJIPOINT_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ChiJiPointDataEntry : public Avalon::DataEntry
{
public:
	ChiJiPointDataEntry();
	virtual ~ChiJiPointDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 必掉资源（不填就是不掉）
	std::vector<UInt32>                             packIDs;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ChiJiPointDataEntryMgr : public Avalon::DataEntryMgrBase<ChiJiPointDataEntry>
	, public Avalon::Singleton<ChiJiPointDataEntryMgr>
{
public:
	virtual bool AddEntry(ChiJiPointDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	//
	std::vector<ChiJiPointDataEntry*> GetPositionVecByPackID(UInt32 packID);
// __CUSTOM_MGR_END__
};

#endif
