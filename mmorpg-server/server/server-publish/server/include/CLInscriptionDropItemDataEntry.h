/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_INSCRIPTIONDROPITEM_DATA_ENTRY_H__
#define __CL_INSCRIPTIONDROPITEM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class InscriptionDropItemDataEntry : public Avalon::DataEntry
{
public:
	InscriptionDropItemDataEntry();
	virtual ~InscriptionDropItemDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 掉落id，这是个递增的ID，自动生成
	UInt32											id;
	// 掉落组ID
	UInt32                                          groupID;
	// 个数方案
	std::vector<UInt32>                             chooseNumSet;
	// 个数权重
	std::vector<UInt32>                             numProbSet;
	// 掉落类型
	UInt32                                          dataType;
	// 物品ID
	UInt32                                          itemID;
	// 权重
	UInt32                                          itemProb;
	// 物品数量
	UInt32											itemNum;
	// 职业加成
	std::vector<UInt32>								occuAddition;
	// 加成权重
	Int32											occuAdditionProb;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class InscriptionDropItemDataEntryMgr : public Avalon::DataEntryMgrBase<InscriptionDropItemDataEntry>
	, public Avalon::Singleton<InscriptionDropItemDataEntryMgr>
{
public:
	virtual bool AddEntry(InscriptionDropItemDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

typedef Avalon::DataEntryVisitor<InscriptionDropItemDataEntry> InscriptionDropItemDataEntryVisitor;

#endif
