/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJISKILL_DATA_ENTRY_H__
#define __CL_CHIJISKILL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <set>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ChiJiSkillDataEntry : public Avalon::DataEntry
{
public:
	ChiJiSkillDataEntry();
	virtual ~ChiJiSkillDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 技能ID
	UInt32                                          id;
	// 最大等级
	UInt32                                          maxLvl;
	// 对应职业
	std::string                                     job;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	std::vector<UInt32>		jobVec;

// __CUSTOM_ENTRYFIELD_END__
};

class ChiJiSkillDataEntryMgr : public Avalon::DataEntryMgrBase<ChiJiSkillDataEntry>
	, public Avalon::Singleton<ChiJiSkillDataEntryMgr>
{
public:
	virtual bool AddEntry(ChiJiSkillDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	void GetSkillVec(UInt32 jobId, UInt32 skillNum, std::set<UInt32>& exceptSet, std::vector<UInt32>& skillVec);

private:

	// (key->职业id，技能列表)
	std::map<UInt32, std::vector<UInt32>> m_JobSkillMap;

// __CUSTOM_MGR_END__
};

#endif
