#ifndef __CL_DROP_ITEM_DATA_ENTRY_H__
#define __CL_DROP_ITEM_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>
#include <bitset>

enum DropDataType
{
    DDT_ITEM,           // 物品
    DDT_SOLUTION,       // 方案
};

enum DropItemType
{
    DIT_COMMON,         // 普通道具
    DIT_GOLD,           // 金币
    DIT_PINK,           // 粉装
    DIT_TASK,           // 任务物品
	DIT_SS,				// 史诗
	DIT_SS_MATERIAL,	// 珍贵材料
	DIT_BOSS_PINK,		// 领主粉
	DIT_YZZY,			// 宇宙之眼
	DIT_CHEST,			// 翻牌
	DIT_TICKET_SY,		// 深渊票
	DIT_TICKET_YG,		// 远古票
	DIT_YUANGU_PINK,	// 远古粉装
	DIT_BERYL,			// 透明水苍玉
	DIT_BIND_GOLD,		// 绑金
	DIT_XWZY,			// 虚无之源
	DIT_NUM,
};

/**
*@brief 掉落数据项
*/
class DropItemDataEntry : public Avalon::DataEntry
{
public:
    DropItemDataEntry();
    ~DropItemDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // 掉落id，这是个递增的ID，自动生成
    UInt32                  id;
    // 掉落方案ID
    UInt32                  projID;
    // 个数方案
    std::vector<UInt32>     chooseNumSet;
    // 个数权重
    std::vector<UInt32>     numProbSet;
    // 掉落类型
    DropDataType            dataType;
    // 物品ID
    UInt32                  itemID;
    // 权重
    UInt32                  itemProb;
    // 物品数量
    UInt32                  itemMinNum;
    // 物品数量
    UInt32                  itemMaxNum;
    // 是否稀有控制
    bool                    isRareControl;
    // 任务ID
    UInt32                  taskId;
    // 职业加成
    std::vector<UInt32>     occuAddition;
    // 加成权重
    Int32                   occuAdditionProb;
    // 掉落物品类型(DropItemType)
    UInt8                   itemType;
	// 掉落提示
	UInt32					notice;
	// 活动ID
	std::set<UInt32>		activityIds;
	// 标记
	std::bitset<32>         markFlag;
	// vip最小等级
	UInt8					minVipLv;
	// vip最大等级
	UInt8					maxVipLv;
	// vip掉落上限控制id
	UInt32					vipDropLimitId;
	// 额外加成
	UInt32                  extra;
};

typedef Avalon::DataEntryVisitor<DropItemDataEntry> DropItemDataEntryVisitor;
typedef Avalon::DataEntryMgr<DropItemDataEntry> DropItemDataEntryMgr;

#endif