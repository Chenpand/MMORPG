/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDEMBLEM_DATA_ENTRY_H__
#define __CL_GUILDEMBLEM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class GuildEmblemDataEntry : public Avalon::DataEntry
{
public:
	GuildEmblemDataEntry();
	virtual ~GuildEmblemDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 等级
	UInt32                                          id;
	// 需要荣耀殿堂等级
	UInt32                                          needHonourLevel;
	// 消耗ID
	std::vector<UInt32>                             costId;
	// 消耗数量
	std::vector<UInt32>                             costNum;
	// 使用的等价道具
	std::vector<UInt32>                             useEqual;
	// 徽记名称
	std::string                                     name;
	// 徽记阶段
	UInt32                                          stageLevel;
	// 头衔
	UInt32                                          titleId;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildEmblemDataEntryMgr : public Avalon::DataEntryMgrBase<GuildEmblemDataEntry>
	, public Avalon::Singleton<GuildEmblemDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildEmblemDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	GuildEmblemDataEntry * GetMaxStageEmblemDataEntry(UInt32 level);
	//key -> HonourLevel , 每个荣耀殿堂等级的最高阶
	std::map<UInt32, GuildEmblemDataEntry*> emblemDataEntryByHonourLevel;
// __CUSTOM_MGR_END__
};

#endif
