/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_ARTIFACTJARLOTTERY_DATA_ENTRY_H__
#define __CL_ARTIFACTJARLOTTERY_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLItemProtocol.h>
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
class ArtifactJarLotteryDataEntry;
typedef std::map<UInt32, ArtifactJarLotteryDataEntry*> ArtifactJarLotteryPool;

// __CUSTOM_STRUCT_END__

class ArtifactJarLotteryDataEntry : public Avalon::DataEntry
{
public:
	ArtifactJarLotteryDataEntry();
	virtual ~ArtifactJarLotteryDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 罐子Id
	UInt32                                          jarId;
	// 奖励物品
	UInt32                                          rewardItemId;
	// 数量
	UInt32                                          rewardItemNum;
	// 权重
	UInt32                                          weight;
	// 限量数量，0表示不限量
	UInt32                                          limitNum;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ArtifactJarLotteryDataEntryMgr : public Avalon::DataEntryMgrBase<ArtifactJarLotteryDataEntry>
	, public Avalon::Singleton<ArtifactJarLotteryDataEntryMgr>
{
public:
	virtual bool AddEntry(ArtifactJarLotteryDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
public:

	// 获取奖池
	ArtifactJarLotteryPool GetArtifactJarRewardPool(UInt32 jarId);

	// 获取奖励配置
	void GetArtifactJarLotteryCfg(std::vector<CLProtocol::ArtifactJarLotteryCfg>& cfgVec);

private:
	// key->罐子id
	std::map<UInt32, ArtifactJarLotteryPool> m_ArtifactJarLotteryCfgMap;

// __CUSTOM_MGR_END__
};

#endif
