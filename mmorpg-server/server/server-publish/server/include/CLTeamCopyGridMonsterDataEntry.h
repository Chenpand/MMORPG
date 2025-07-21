/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYGRIDMONSTER_DATA_ENTRY_H__
#define __CL_TEAMCOPYGRIDMONSTER_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct GradeMonster
{
	GradeMonster(UInt32 _g, UInt32 _m, UInt32 _b) : grade(_g), monsterType(_m), bornId(_b) {}

	UInt32 grade;
	UInt32 monsterType;
	UInt32 bornId;

	bool operator < (const GradeMonster& gs) const
	{
		if (grade != gs.grade)
		{
			return grade < gs.grade;
		}

		if (monsterType != gs.monsterType)
		{
			return monsterType < gs.monsterType;
		}

		return bornId < gs.bornId;
	}
};

// __CUSTOM_STRUCT_END__

class TeamCopyGridMonsterDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyGridMonsterDataEntry();
	virtual ~TeamCopyGridMonsterDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 难度
	UInt32                                          grade;
	// 怪物类型
	UInt32                                          monsterType;
	// 出生位置
	UInt32                                          bornPos;
	// 移动CD
	UInt32                                          moveCd;
	// 进攻路线
	std::string                                     attackPath;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	// 进攻路线
	std::list<UInt32>								attackPathList;

// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyGridMonsterDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyGridMonsterDataEntry>
	, public Avalon::Singleton<TeamCopyGridMonsterDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyGridMonsterDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	TeamCopyGridMonsterDataEntry* GetGridMonsterData(UInt32 grade, UInt32 monsterType, UInt32 monsterBornId);

private:

	// key->(难度，怪物出生点)
	std::map<GradeMonster, TeamCopyGridMonsterDataEntry*> m_BornMap;

// __CUSTOM_MGR_END__
};

#endif
