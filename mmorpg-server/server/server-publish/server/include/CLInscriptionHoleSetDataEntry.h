/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_INSCRIPTIONHOLESET_DATA_ENTRY_H__
#define __CL_INSCRIPTIONHOLESET_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class InscriptionHoleSetDataEntry : public Avalon::DataEntry
{
public:
	InscriptionHoleSetDataEntry();
	virtual ~InscriptionHoleSetDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 孔颜色
	UInt32                                          id;
	// 可镶嵌铭文三类型
	std::string                                     thirdType;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	std::vector<UInt32> mountVec;
// __CUSTOM_ENTRYFIELD_END__
};

class InscriptionHoleSetDataEntryMgr : public Avalon::DataEntryMgrBase<InscriptionHoleSetDataEntry>
	, public Avalon::Singleton<InscriptionHoleSetDataEntryMgr>
{
public:
	virtual bool AddEntry(InscriptionHoleSetDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
