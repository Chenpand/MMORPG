/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_INSCRIPTIONEXTRACT_DATA_ENTRY_H__
#define __CL_INSCRIPTIONEXTRACT_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class InscriptionExtractDataEntry : public Avalon::DataEntry
{
public:
	InscriptionExtractDataEntry();
	virtual ~InscriptionExtractDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 铭文品质
	UInt32                                          color;
	// 摘取材料_数量
	std::string                                     extractItemConsume;
	// 摘取概率(千分比)
	UInt32											extractProbability;
	// 是否可摘取
	UInt32											isExtract;
	// 封装时铭文摘取概率(千分比)
	UInt32											encapsuleProbability;
	// 碎裂材料_数量
	std::string										destroyConsume;
	// 碎裂概率(千分比)
	UInt32											destroyProbability;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	//材料信息
	UInt32 matId;
	UInt32 matNum;
	//碎裂材料信息
	UInt32 destroyMatId;
	UInt32 destroyMatNum;
// __CUSTOM_ENTRYFIELD_END__
};

class InscriptionExtractDataEntryMgr : public Avalon::DataEntryMgrBase<InscriptionExtractDataEntry>
	, public Avalon::Singleton<InscriptionExtractDataEntryMgr>
{
public:
	virtual bool AddEntry(InscriptionExtractDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
