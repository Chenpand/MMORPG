/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_GUILDHONOR_DATA_ENTRY_H__
#define __CL_GUILDHONOR_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
//公会荣誉类型
enum GuildHonorChallenge
{
	GUILD_CHALLENGE_TEMACOPY = 1,// 团本
    GUILD_CHALLENGE_BATTLE = 2,// 公会战
	GUILD_CHALLENGE_WUDAO = 3, //赏金武道大会
	GUILD_CHALLENGE_GUILD_DUNGEON = 4 // 公会地下城
};
enum GuildHonorConditionType
{
	GUILD_COND_TYPE, //关卡类型
	GUILD_COND_DIFF, //关卡难度
	GUILD_COND_TIME, //通关时间
	GUILD_COND_MEMBER,//通关人数限制
	GUILD_COND_TEAM,//一个队伍算一次
	GUILD_COND_MAX
};
// __CUSTOM_STRUCT_END__

class GuildHonorDataEntry : public Avalon::DataEntry
{
public:
	GuildHonorDataEntry();
	virtual ~GuildHonorDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// id
	UInt32                                          id;
	// 荣誉组id
	UInt32                                          groupId;
	// 类型2
	UInt32                                          type;
	// 关卡类型|关卡难度|通关时间|最少人数限制
	std::vector<UInt32>                             conds;
	// 次数
	UInt32                                          count;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GuildHonorDataEntryMgr : public Avalon::DataEntryMgrBase<GuildHonorDataEntry>
	, public Avalon::Singleton<GuildHonorDataEntryMgr>
{
public:
	virtual bool AddEntry(GuildHonorDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
