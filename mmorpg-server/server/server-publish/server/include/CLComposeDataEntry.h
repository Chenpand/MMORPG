#ifndef _CL_COMPOSE_DATAENTRY_H_
#define _CL_COMPOSE_DATAENTRY_H_

#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <map>

#define MAKE_COMPOSE_KEY(ItemId, Quality) ((ItemId << 4) + Quality)

/**
 *@brief 合成数据项
 */
struct ComposeDataEntry  : public Avalon::DataEntry
{
	typedef std::map<UInt32, UInt32> RateMap;

public:
	ComposeDataEntry();
	~ComposeDataEntry();

public:
	UInt32 GetKey() const { return MAKE_COMPOSE_KEY(itemid, quality); }

	bool Read(Avalon::DataRow& row);

public:
	//道具id
	UInt32	itemid;
	//品质
	UInt8	quality;
	//最大合成数量
	UInt32	maxNum;
	//材料
	UInt32	materials[MAX_ITEM_COMPOSE_MATERIAL_NUM];
	//材料数
	UInt32	materialNum[MAX_ITEM_COMPOSE_MATERIAL_NUM];
	//分阶段概率配置
	RateMap	rates;
};

typedef Avalon::DataEntryMgr<ComposeDataEntry> ComposeDataEntryMgr;

#endif
