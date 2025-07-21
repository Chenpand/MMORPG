/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ROLEVALUETOTALSCORECALC_DATA_ENTRY_H__
#define __CL_ROLEVALUETOTALSCORECALC_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class RoleValueTotalScoreCalcDataEntry : public Avalon::DataEntry
{
public:
	RoleValueTotalScoreCalcDataEntry();
	virtual ~RoleValueTotalScoreCalcDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__

	UInt32 ConvertToTotalScoreBySingleScore(UInt32 singleScore);

// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 角色评分占比
	UInt32                                          roleValueScoreProb;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class RoleValueTotalScoreCalcDataEntryMgr : public Avalon::DataEntryMgrBase<RoleValueTotalScoreCalcDataEntry>
	, public Avalon::Singleton<RoleValueTotalScoreCalcDataEntryMgr>
{
public:
	virtual bool AddEntry(RoleValueTotalScoreCalcDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	RoleValueTotalScoreCalcDataEntryMgr() :m_MaxKeyInDatas(0) {}

	UInt32 GetMaxKeyInDatas() { return m_MaxKeyInDatas; }

private:
	UInt32 m_MaxKeyInDatas;
// __CUSTOM_MGR_END__
};

#endif
