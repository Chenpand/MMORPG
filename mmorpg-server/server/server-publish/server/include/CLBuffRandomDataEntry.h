/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_BUFFRANDOM_DATA_ENTRY_H__
#define __CL_BUFFRANDOM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class BuffRandomDataEntry : public Avalon::DataEntry
{
public:
	BuffRandomDataEntry();
	virtual ~BuffRandomDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// Buff ID
	UInt32                                          buffId;
	// 权重
	UInt32                                          weight;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class BuffRandomDataEntryMgr : public Avalon::DataEntryMgrBase<BuffRandomDataEntry>
	, public Avalon::Singleton<BuffRandomDataEntryMgr>
{
public:
	virtual bool AddEntry(BuffRandomDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	Int32 RandBuff(const std::vector<UInt32>& uids);
// __CUSTOM_MGR_END__
};

#endif
