#ifndef _CL_ITEM_THING_H_
#define _CL_ITEM_THING_H_

#include <CLDefine.h>

// 事务超时时间
#define ITEM_THING_TIME_OUT		(5)

// 事务内容
struct ItemThingCost
{
	ItemThingCost() : point(0), gold(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & point & gold;
	}

	std::string LogStr()
	{
		std::stringstream ss;
		ss << " point:" << point;
		ss << " gold:" << gold;
		return ss.str();
	}

	std::string Serialize()
	{
		std::stringstream ss;
		ss << point << "," << gold;
		return ss.str();
	}

	// 点券
	UInt32 point;
	// 金币
	UInt32 gold;
};


#endif // !_CL_ITEM_THING_H_

