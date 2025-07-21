/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYFIELD_DATA_ENTRY_H__
#define __CL_TEAMCOPYFIELD_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <set>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct FieldStatusUpdateCfg
{
	FieldStatusUpdateCfg() : fieldId(0), status(0) {}
	UInt32 fieldId;
	UInt32 status;
};

// __CUSTOM_STRUCT_END__

class TeamCopyFieldDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyFieldDataEntry();
	virtual ~TeamCopyFieldDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 团本难度
	UInt32                                          teamGrade;
	// 地下城Id
	UInt32                                          dungeonId;
	// 类别
	UInt32		                                    presentedType;
	// 据点歼灭条件
	UInt32                                          defeatCond;
	// 据点重生时间
	UInt32                                          rebornTime;
	// 据点解锁条件
	std::vector<UInt32>                             unlockedCond;
	// 据点出现状态
	UInt32                                          appearStatus;
	// 据点状态更新
	std::vector<std::string>                        updateFieldStatus;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 据点状态更新
	std::vector<FieldStatusUpdateCfg>				updateFieldStatusVec;

// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyFieldDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyFieldDataEntry>
	, public Avalon::Singleton<TeamCopyFieldDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyFieldDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 根据出现类型获得据点列表
	void GetFieldList(UInt32 teamGrade, UInt32 appearType, std::vector<TeamCopyFieldDataEntry*>& fieldVec);
	
	// 根据解锁条件，获取据点列表
	void GetUnlockFieldList(UInt32 fieldId, std::vector<TeamCopyFieldDataEntry*>& fieldVec);

	// 获取boss据点
	UInt32 GetBossFieldId(UInt32 teamGrade);

private:

	// boss据点id(key->难度，val->据点id)
	std::map<UInt32, UInt32> m_BossFieldMap;

// __CUSTOM_MGR_END__
};

#endif
