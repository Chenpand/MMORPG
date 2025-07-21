/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYROLL_DATA_ENTRY_H__
#define __CL_TEAMCOPYROLL_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLItemDefine.h>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct TeamTypeGrade
{
	TeamTypeGrade(UInt32 _tt, UInt32 _tg) : teamType(_tt), teamGrade(_tg) {}

	bool operator < (const TeamTypeGrade& ttg) const
	{
		if (teamType != ttg.teamType)
		{
			return teamType < ttg.teamType;
		}

		return teamGrade < ttg.teamGrade;
	}

	UInt32 teamType;
	UInt32 teamGrade;
};

// __CUSTOM_STRUCT_END__

class TeamCopyRollDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyRollDataEntry();
	virtual ~TeamCopyRollDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 队伍类型（团本ID）
	UInt32                                          teamType;
	// 队伍难度（团本难度）
	UInt32                                          teamGrade;
	// 奖励次数
	UInt32                                          rewardNum;
	// 触发概率(万分比)
	UInt32                                          triggerRate;
	// 掉落id
	UInt32                                          dropId;
	// 转换道具
	std::string                                     convertItem;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 转换奖励
	std::vector<ItemRewardBase>						convertItemVec;

// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyRollDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyRollDataEntry>
	, public Avalon::Singleton<TeamCopyRollDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyRollDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	TeamCopyRollDataEntryMgr();

	TeamCopyRollDataEntry* FindEntry(UInt32 teamType, UInt32 teamGrade);

	// roll次数
	UInt32 GetRollRewardNum(UInt32 teamType, UInt32 teamGrade);

	void OnReload();

	std::vector<UInt32> GetRollPointVec() { return m_RollPointVec; }

private:

	std::vector<UInt32> m_RollPointVec;

	std::map<TeamTypeGrade, TeamCopyRollDataEntry*> m_RollMap;

// __CUSTOM_MGR_END__
};

#endif
