/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ULTIMATECHALLENGEDUNGEON_DATA_ENTRY_H__
#define __CL_ULTIMATECHALLENGEDUNGEON_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class UltimateChallengeDungeonDataEntry : public Avalon::DataEntry
{
public:
	UltimateChallengeDungeonDataEntry();
	virtual ~UltimateChallengeDungeonDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
	//buff随机信息
	struct BuffRandomInfo
	{
		UInt32 buffId;
		UInt32 weight;
	};
	
	//材料信息
	struct MatInfo
	{
		UInt32 matId;
		UInt32 num;
	};

	//鼓舞buff信息
	struct InspireBuffInfo
	{
		UInt32 buffId;
		UInt32 day;
	};

	UInt32 RandExceptBuffId(UInt32 buffId);
	UInt32 GetNextInspireBuffId(UInt32 day);
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// 层数
	UInt32                                          level;
	// 地下城ID
	UInt32                                          dungeonID;
	// 随机buff（buffID_权重）
	std::string                                     randomBuff;
	// 限制时间（秒）
	UInt32                                          limitTime;
	// 刷新关卡效果消耗
	std::string                                     expendItem;
	// 鼓舞buff
	std::string                                     courageBuff;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
	std::vector<BuffRandomInfo> buffRandomInfoVec;
	MatInfo dungeonBuffRefreshMat;
	std::vector<InspireBuffInfo> inspireBuffInfoVec;
// __CUSTOM_ENTRYFIELD_END__
};

class UltimateChallengeDungeonDataEntryMgr : public Avalon::DataEntryMgrBase<UltimateChallengeDungeonDataEntry>
	, public Avalon::Singleton<UltimateChallengeDungeonDataEntryMgr>
{
public:
	virtual bool AddEntry(UltimateChallengeDungeonDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
	UltimateChallengeDungeonDataEntryMgr();

	// 最高层数
	UInt32 topLevel;
// __CUSTOM_MGR_END__
};

#endif
