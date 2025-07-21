/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_INSCRIPTIONSYNTHESIS_DATA_ENTRY_H__
#define __CL_INSCRIPTIONSYNTHESIS_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class InscriptionSynthesisDataEntry : public Avalon::DataEntry
{
public:
	InscriptionSynthesisDataEntry();
	virtual ~InscriptionSynthesisDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	static UInt32 MakeKey(UInt32 color, UInt32 synthesisNum);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 铭文品质
	UInt32                                          color;
	// 可放入合成数量
	UInt32                                          synthesisNum;
	// 铭文库（铭文分解合成库对应的组id）
	UInt32                                          generateInscription;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

// __CUSTOM_ENTRYFIELD_END__
};

class InscriptionSynthesisDataEntryMgr : public Avalon::DataEntryMgrBase<InscriptionSynthesisDataEntry>
	, public Avalon::Singleton<InscriptionSynthesisDataEntryMgr>
{
public:
	virtual bool AddEntry(InscriptionSynthesisDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
