#ifndef _MALL_HELPER_H_
#define _MALL_HELPER_H_

#include "CLDefine.h"
#include "CLMallDefine.h"


namespace MallHelper
{
	/**
	*@brief 获取商城购买获得物对应的道具ID
	*/
	UInt32 GetMallBuyGotItemDataId(MallBuyGotType type);
}

#endif