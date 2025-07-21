/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_TEAMCOPYGRIDPARAM_DATA_ENTRY_H__
#define __CL_TEAMCOPYGRIDPARAM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct GradeSource
{
	GradeSource() : grade(0), sourceId(0) {}
	GradeSource(UInt32 _g, UInt32 _s) : grade(_g), sourceId(_s) {}

	UInt32 grade;
	UInt32 sourceId;

	bool operator < (const GradeSource& gs) const
	{
		if (grade == gs.grade)
		{
			return sourceId < gs.sourceId;
		}

		return grade < gs.grade;
	}
};

// __CUSTOM_STRUCT_END__

class TeamCopyGridParamDataEntry : public Avalon::DataEntry
{
public:
	TeamCopyGridParamDataEntry();
	virtual ~TeamCopyGridParamDataEntry();

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
	// 资源ID
	UInt32                                          resId;
	// 类型
	UInt32                                          gridType;
	// 参数
	std::string                                     param;
	// 参数2
	std::string                                     param2;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__

	std::vector<UInt32>								paramVec;
	std::vector<UInt32>								paramVec2;

// __CUSTOM_ENTRYFIELD_END__
};

class TeamCopyGridParamDataEntryMgr : public Avalon::DataEntryMgrBase<TeamCopyGridParamDataEntry>
	, public Avalon::Singleton<TeamCopyGridParamDataEntryMgr>
{
public:
	virtual bool AddEntry(TeamCopyGridParamDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__

	std::vector<UInt32> GetParam(UInt32 grade, UInt32 sourceId);
	std::vector<UInt32> GetParam2(UInt32 grade, UInt32 sourceId);

private:

	std::map<GradeSource, TeamCopyGridParamDataEntry*> m_GradeGridSourceMap;

// __CUSTOM_MGR_END__
};

#endif
