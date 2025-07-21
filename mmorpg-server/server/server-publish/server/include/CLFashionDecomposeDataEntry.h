/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_FASHIONDECOMPOSE_DATA_ENTRY_H__
#define __CL_FASHIONDECOMPOSE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class FashionDecomposeDataEntry : public Avalon::DataEntry
{
public:
	FashionDecomposeDataEntry();
	virtual ~FashionDecomposeDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 MakeKey(UInt32 subType, UInt32 thirdType, UInt32 color);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 子类别
	UInt32											subType;
	// 三类别
	UInt32											thirdType;
	// 颜色
	UInt32                                          color;
	// 特殊时装id
	UInt32                                          fashionID;
	// 掉落组ID
	UInt32                                          groupID;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class FashionDecomposeDataEntryMgr : public Avalon::DataEntryMgrBase<FashionDecomposeDataEntry>
	, public Avalon::Singleton<FashionDecomposeDataEntryMgr>
{
public:
	virtual bool AddEntry(FashionDecomposeDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
