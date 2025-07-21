#ifndef _CL_SYS_RECORD_DEFINE_H_
#define _CL_SYS_RECORD_DEFINE_H_

#include "CLDefine.h"


// sysrecord事件
enum SysRecordEvent
{
	SYS_RECORD_EVENT_NULL,
	// 神秘商人
	SYS_RECORD_EVENT_MYSTICAL_MERCHANT,
};

// 稀有控制类型
enum RareControlType
{
	RARE_CTRL_INVALID = 0,

	RARE_CTRL_ITEM = 1,
	RARE_CTRL_DROP_SOLUTION = 2,
	// 地下城材料商店
	RARE_CTRL_DUNGEON_MATERIAL_SHOP = 3,
	// 地下城装备商店
	RARE_CTRL_DUNGEON_EQUIP_SHOP = 4,
	// 地下城珠宝商店
	RARE_CTRL_DUNGEON_JEWELRY_SHOP = 5,
};

#endif
