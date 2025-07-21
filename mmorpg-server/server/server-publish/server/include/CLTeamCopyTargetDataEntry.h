/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYTARGET_DATA_ENTRY_H__
#define __CL_TEAMCOPYTARGET_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__

#include <set>
#include <CLTeamCopyFieldDataEntry.h>

// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

class TeamCopyFieldDataEntry;

struct TCTargetStageGrade
{
	TCTargetStageGrade() : teamCopyId(0), teamGrade(0), stage(0), targetGrade(0) {}
	TCTargetStageGrade(UInt32 _id, UInt32 _teamGrade, UInt32 _stage, UInt32 _targetGrade) :
		teamCopyId(_id), teamGrade(_teamGrade), stage(_stage), targetGrade(_targetGrade) {}

	// 团本id
	UInt32 teamCopyId;
	// 团本难度
	UInt32 teamGrade;
	// 阶段
	UInt32 stage;
	// 目标难度
	UInt32 targetGrade;

	bool operator<(const TCTargetStageGrade& key) const
	{
		if (teamCopyId != key.teamCopyId)
		{
			return teamCopyId < key.teamCopyId;
		}

		if (teamGrade != key.teamGrade)
		{
			return teamGrade < key.teamGrade;
		}

		if (stage != key.stage)
		{
			return stage < key.stage;
		}

		return targetGrade < key.targetGrade;
	}
};

struct TeamCopyFieldCfg
{
	TeamCopyFieldCfg() : fieldId(0), totalNum(0), entry(NULL) {}

	// 据点id
	UInt32 fieldId;
	// 总次数
	UInt32 totalNum;
	// 据点配置
	const TeamCopyFieldDataEntry* entry;
};

struct TeamCopyTargetCfg
{
	TeamCopyTargetCfg() : id(0), stage(0), targetType(0), nextTarget(0) {}

	// 目标ID
	UInt32 id;
	// 阶段
	UInt32 stage;
	// 目标类型
	UInt32 targetType;
	// 下个目标
	UInt32 nextTarget;
	// 据点列表
	std::vector<TeamCopyFieldCfg> feildVec;
};

struct TeamCopyStageField
{
	TeamCopyStageField() : teamGrade(0), stage(0) {}
	TeamCopyStageField(UInt32 _teamGrade, UInt32 _stage) : teamGrade(_teamGrade), stage(_stage) {}

	// 队伍难度
	UInt32 teamGrade;
	// 阶段
	UInt32 stage;


	bool operator<(const TeamCopyStageField& key) const
	{
		if (teamGrade != key.teamGrade)
		{
			return teamGrade < key.teamGrade;
		}

		return stage < key.stage;
	}
};

// __CUSTOM_STRUCT_END__

class TeamCopyTargetDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyTargetDataEntry();
	virtual ~TeamCopyTargetDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 目标ID
	UInt32                                          id;
	// 团本id
	UInt32                                          teamCopyId;
	// 团本难度
	UInt32                                          teamGrade;
	// 阶段
	UInt32                                          stage;
	// 目标难度
	UInt32                                          targetDifficulty;
	// 目标类型
	UInt32                                          targetType;
	// 目标据点
	std::vector<std::string>                        feildId;
	// 下一个目标
	UInt32                                          nextTarget;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyTargetDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyTargetDataEntry>
	, public Avalon::Singleton<TeamCopyTargetDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyTargetDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	// 查找目标
	const TeamCopyTargetCfg* GetTarget(UInt32 targetId);
	const TeamCopyTargetCfg* GetFirstTarget(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 grade);

	// 根据目标难度和阶段 获取目标列表
	const std::vector<TeamCopyTargetCfg*>* GetTargetList(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 grade);

	// 某难度的阶段是否有某据点
	bool IsHasFieldByStage(UInt32 teamGrade, UInt32 stage, UInt32 fieldId);

	// 某难度是否有某据点
	bool IsHasFieldByStage(UInt32 teamGrade, UInt32 fieldId);

private:

	// key->目标id
	std::map<UInt32, TeamCopyTargetCfg*> m_TargetMap;

	// key->难度、阶段、目标难度
	std::map<TCTargetStageGrade, std::vector<TeamCopyTargetCfg*>> m_TargetStageGradeMap;

	// key->难度阶段，val->据点
	std::map<TeamCopyStageField, std::set<UInt32>> m_GradeStageFieldMap;

	// key->难度，val->据点
	std::map<UInt32, std::set<UInt32>> m_GradeFieldMap;

// __CUSTOM_MGR_END__
};

#endif
