/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_SKILLRECOMMEND_DATA_ENTRY_H__
#define __CL_SKILLRECOMMEND_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__

struct JobSkillInfo
{
	// 职业ID
	UInt32 jobID;
	// 技能配置模式
	UInt32 skillConfigMode;

	JobSkillInfo() : jobID(0), skillConfigMode(0) {}

	bool operator < (const JobSkillInfo& info) const
	{
		if (info.jobID < jobID)
			return true;

		if (jobID < info.jobID)
			return false;

		if (info.skillConfigMode < skillConfigMode)
			return true;

		if (skillConfigMode < info.skillConfigMode)
			return false;

		return false;
	}
};

struct SkillDetails
{
	// 技能ID
	UInt32 skillID;
	// 加点优先级
	UInt32 Priority;
	// 技能位置
	UInt32 skillPos;

	SkillDetails() :skillID(0), Priority(0), skillPos(0) {}
};

// 技能推荐信息列表,下标为优先级顺序
typedef std::vector<const SkillDetails*> SkillRecommendVec;

// __CUSTOM_STRUCT_END__

class SkillRecommendDataEntry : public Avalon::DataEntry
{
public:
	SkillRecommendDataEntry();
	virtual ~SkillRecommendDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 职业ID
	UInt32                                          jobID;
	// 技能模式
	UInt32                                          skillMode;
	// 最大推荐等级
	UInt32                                          maxLv;
	// 技能ID
	std::vector<UInt32>                             skillIDVec;
	// 加点优先级
	std::vector<UInt32>                             priorityVec;
	// 技能位置ID
	std::vector<UInt32>                             posIDVec;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class SkillRecommendDataEntryMgr : public Avalon::DataEntryMgrBase<SkillRecommendDataEntry>
	, public Avalon::Singleton<SkillRecommendDataEntryMgr>
{
public:
	virtual bool AddEntry(SkillRecommendDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	SkillRecommendDataEntryMgr();
public:

	// 获得技能推荐列表
	const SkillRecommendVec* GetRecommendSkillVec(UInt32 _jobID, UInt32 _skillMode, UInt32 _playerLv);

private:
	// 增加技能推荐信息
	void _addSkillRecommendInfo(UInt32 _maxLv, JobSkillInfo& _jobInfo, const SkillDetails* _details, UInt32 _skillNum);

private:
	// 每个职业的推荐信息，key最小推荐等级
	typedef std::map<UInt32, SkillRecommendVec>  RecommendInfoMap;
	// 所有职业技能推荐信息,key为职业ID
	typedef std::map<JobSkillInfo, RecommendInfoMap>	JobSkillRecommondInfoMap;

	JobSkillRecommondInfoMap		m_JobSkillRecommondInfoMap;

// __CUSTOM_MGR_END__
};

#endif
