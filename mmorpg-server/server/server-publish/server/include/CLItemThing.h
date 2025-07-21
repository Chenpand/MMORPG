#ifndef _CL_ITEM_THING_H_
#define _CL_ITEM_THING_H_

#include <CLDefine.h>

// ����ʱʱ��
#define ITEM_THING_TIME_OUT		(5)

// ��������
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

	// ��ȯ
	UInt32 point;
	// ���
	UInt32 gold;
};


#endif // !_CL_ITEM_THING_H_

