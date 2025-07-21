/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_BEADCONVERTDROP_DATA_ENTRY_H__
#define __CL_BEADCONVERTDROP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

class BeadConvertDropDataEntry;

struct BeadConvertDropCfg
{
	BeadConvertDropCfg() : totalWeight(0) {}

	UInt32 totalWeight;
	std::map<UInt32, BeadConvertDropDataEntry*> dropMap;
};

// __CUSTOM_STRUCT_END__

class BeadConvertDropDataEntry : public Avalon::DataEntry
{
public:
	BeadConvertDropDataEntry();
	virtual ~BeadConvertDropDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID（无意义）
	UInt32                                          id;
	// 所属掉落包ID
	UInt32                                          dropPackID;
	// 道具ID
	UInt32                                          itemID;
	// 道具Num
	UInt32                                          itemNum;
	// 获得权重
	UInt32                                          weight;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class BeadConvertDropDataEntryMgr : public Avalon::DataEntryMgrBase<BeadConvertDropDataEntry>
	, public Avalon::Singleton<BeadConvertDropDataEntryMgr>
{
public:
	virtual bool AddEntry(BeadConvertDropDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取转换奖励
	void GetConvertReward(UInt32 dropId, UInt32& itemId, UInt32& itemNum);

private:

	std::map<UInt32, BeadConvertDropCfg> m_ConvertDropCfg;

// __CUSTOM_MGR_END__
};

#endif
