/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYFLOP_DATA_ENTRY_H__
#define __CL_TEAMCOPYFLOP_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct TeamCopyFlopKey
{
	TeamCopyFlopKey() : teamCopyID(0), teamGrade(0), stage(0), param1(0) {}
	TeamCopyFlopKey(UInt32 _id, UInt32 _grade, UInt32 _stage, UInt32 _param) 
		: teamCopyID(_id), teamGrade(_grade), stage(_stage), param1(_param) {}

	// 团本ID
	UInt32 teamCopyID;
	// 团本难度
	UInt32 teamGrade;
	// 阶段
	UInt32 stage;
	UInt32 param1;

	bool operator<(const TeamCopyFlopKey& key) const
	{
		if (teamCopyID != key.teamCopyID)
		{
			return teamCopyID < key.teamCopyID;
		}

		if (teamGrade != key.teamGrade)
		{
			return teamGrade < key.teamGrade;
		}

		if (stage != key.stage)
		{
			return stage < key.stage;
		}

		return param1 < key.param1;
	}
};

// __CUSTOM_STRUCT_END__

class TeamCopyFlopDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyFlopDataEntry();
	virtual ~TeamCopyFlopDataEntry();

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
	// 阶段（3：大阶段）
	UInt32                                          stage;
	// 团本2表示奖励类型（1:普通奖励，2：更好的奖励）
	UInt32                                          param;
	// 奖励内容
	UInt32                                          dropId;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyFlopDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyFlopDataEntry>
	, public Avalon::Singleton<TeamCopyFlopDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyFlopDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	const TeamCopyFlopDataEntry* GetFlopReward(UInt32 copyId, UInt32 teamGrade, UInt32 stage, UInt32 param = 0);

private:

	std::map<TeamCopyFlopKey, TeamCopyFlopDataEntry*> m_FlopMap;

// __CUSTOM_MGR_END__
};

#endif
