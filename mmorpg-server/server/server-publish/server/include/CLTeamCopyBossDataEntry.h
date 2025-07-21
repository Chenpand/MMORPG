/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYBOSS_DATA_ENTRY_H__
#define __CL_TEAMCOPYBOSS_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct TeamCopyBossKey
{
	TeamCopyBossKey() : teamCopyID(0), teamGrade(0), stage(0) {}
	TeamCopyBossKey(UInt32 _id, UInt32 _grade, UInt32 _stage) : teamCopyID(_id), teamGrade(_grade), stage(_stage) {}

	// 团本ID
	UInt32 teamCopyID;
	// 团本难度
	UInt32 teamGrade;
	// 阶段
	UInt32 stage;

	bool operator<(const TeamCopyBossKey& key) const
	{
		if (teamCopyID != key.teamCopyID)
		{
			return teamCopyID < key.teamCopyID;
		}

		if (teamGrade != key.teamGrade)
		{
			return teamGrade < key.teamGrade;
		}

		return stage < key.stage;
	}
};
// __CUSTOM_STRUCT_END__

class TeamCopyBossDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyBossDataEntry();
	virtual ~TeamCopyBossDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 团本ID
	UInt32                                          teamCopyID;
	// 团本难度
	UInt32                                          teamGrade;
	// 阶段
	UInt32                                          stage;
	// 出现据点
	UInt32                                          appearField;
	// 关闭据点
	UInt32                                          closeField;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyBossDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyBossDataEntry>
	, public Avalon::Singleton<TeamCopyBossDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyBossDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const TeamCopyBossDataEntry* GetBossData(UInt32 copyId, UInt32 teamGrade, UInt32 stage);

private:

	std::map<TeamCopyBossKey, TeamCopyBossDataEntry*> m_BossMap;

// __CUSTOM_MGR_END__
};

#endif
