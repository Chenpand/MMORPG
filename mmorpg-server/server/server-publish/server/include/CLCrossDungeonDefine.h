#ifndef __CL_CROSS_DUNGEON_DEFINE_H__
#define __CL_CROSS_DUNGEON_DEFINE_H__

#include <AvalonProtocol.h>

// 地下城结束原因
enum CrossDungeonEndReason
{
    CDER_INVALID,
	CDER_TEAMCOPY_FILED_DESTORY, //团本据点被歼灭
	CDER_TEAMCOPY_SETTLE,        //团本结算
};

// 地下城结束结果
enum CrossDungeonEndResult
{
	CDERT_INVALID,
	CDERT_FAIL,		//失败
	CDERT_SUCC,		//成功
};

#endif
