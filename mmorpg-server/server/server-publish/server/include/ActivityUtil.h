#ifndef _ACTIVITY_UTIL_H_
#define _ACTIVITY_UTIL_H_

#include "CLActivityDefine.h"
#include "CLActiveEventDataEntry.h"

namespace ActivityUtil
{

	// 初始化运营活动
	bool InitOpActivityInfo(OpActivityRegInfo& info, const OpActivityDataEntry* data);

	// 自定义生成运营活动完成计数
	bool CustomGenOpActCompleteCount(OpActivityRegInfo& opActInfo);

	// 是否在老兵回归期
	bool IsInVeteranReturnPeriod(UInt32 returnTime);

	// 检测老兵回归
	bool CheckVeteranReturn(UInt32 accId, UInt16 maxLevelInRoles, UInt32 veteranReturnTime, UInt32 accountOfflineTime);
}

#endif