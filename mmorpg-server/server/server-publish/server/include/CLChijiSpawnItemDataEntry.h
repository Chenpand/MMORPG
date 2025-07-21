/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#ifndef __CL_CHIJISPAWNITEM_DATA_ENTRY_H__
#define __CL_CHIJISPAWNITEM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// 自定义结构、枚举
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class ChijiSpawnItemDataEntry : public Avalon::DataEntry
{
public:
	ChijiSpawnItemDataEntry();
	virtual ~ChijiSpawnItemDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// 自定义接口
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// 第几波出现
	UInt32                                          times;
	// 资源包库
	UInt32                                          packID;
	// 产出道具ID
	UInt32                                          itemID;
	// 权重
	UInt32                                          weight;
	// 最小数量
	UInt32                                          minNumber;
	// 最大数量
	UInt32                                          maxNumber;
	// 地图ID
	UInt32                                          mapID;

// 自定义字段
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class ChijiSpawnItemDataEntryMgr : public Avalon::DataEntryMgrBase<ChijiSpawnItemDataEntry>
	, public Avalon::Singleton<ChijiSpawnItemDataEntryMgr>
{
public:
	virtual bool AddEntry(ChijiSpawnItemDataEntry* entry);

// 自定义接口、字段
// __CUSTOM_MGR_BEGIN__
// __CUSTOM_MGR_END__
};

#endif
