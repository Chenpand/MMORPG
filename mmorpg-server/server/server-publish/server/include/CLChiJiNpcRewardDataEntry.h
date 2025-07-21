/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJINPCREWARD_DATA_ENTRY_H__
#define __CL_CHIJINPCREWARD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ChiJiNpcRewardDataEntry : public Avalon::DataEntry
{
public:
	ChiJiNpcRewardDataEntry();
	virtual ~ChiJiNpcRewardDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// npcId
	UInt32                                          npcId;
	// 参数(上交装备为品质总和,
	UInt32                                          param;
	// 参数2(血量千分比，
	UInt32                                          param2;
	// 奖励
	std::vector<UInt32>                             rewards;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ChiJiNpcRewardDataEntryMgr : public Avalon::DataEntryMgrBase<ChiJiNpcRewardDataEntry>
	, public Avalon::Singleton<ChiJiNpcRewardDataEntryMgr>
{
public:
	virtual bool AddEntry(ChiJiNpcRewardDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 获取一个奖励
	UInt32 GetRewards(UInt32 npcId, UInt32 param);

	const ChiJiNpcRewardDataEntry* GetChiJiNpcRewardDataEntry(UInt32 npcId, UInt32 param = 0);

private:

	// 吃鸡商人key->(npcId， key->param)
	std::map<UInt32, std::map<UInt32, ChiJiNpcRewardDataEntry*>> m_NpcRewards;

// __CUSTOM_MGR_END__
};

#endif
