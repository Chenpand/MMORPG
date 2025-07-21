#ifndef __CL_PK_STATISTIC_DEFINE_H__
#define __CL_PK_STATISTIC_DEFINE_H__

#include <AvalonNetStream.h>
#include <CLDefine.h>

struct PkStatisticInfo
{
	PkStatisticInfo()
	{
		totalWinNum = 0;
		totalLoseNum = 0;
		totalNum = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & totalWinNum & totalLoseNum & totalNum;
	}

	// 胜场数
	UInt32 totalWinNum;
	// 负场数
	UInt32 totalLoseNum;
	// 总场数
	UInt32 totalNum;
};

#endif