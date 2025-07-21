#ifndef _CL_DIG_DEFINE_H_
#define _CL_DIG_DEFINE_H_

#include "CLDefine.h"

enum DigType
{
	//无效
	DIG_INVALID = 0,
	//金挖宝点
	DIG_GLOD = 1,
	//银挖宝点
	DIG_SILVER = 2,
};

enum DigItemType
{
	DIT_INVALID = 0,

	//普通
	DIT_NORMAL = 1,

	//极品
	DIT_HIGHEST_GRADE = 2,
};

enum DigStatus
{
	//无效
	DIG_STATUS_INVALID = 0,

	//初始状态
	DIG_STATUS_INIT = 1,

	//打开状态
	DIG_STATUS_OPEN = 2,

};

struct DigInfo
{
	DigInfo():index(0), type(0), status(0), refreshTime(0), changeStatusTime(0), openItemId(0), openItemNum(0) { }
	~DigInfo() { }

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & index & type & status & refreshTime & changeStatusTime & openItemId & openItemNum;
	}

	UInt32 index;
	UInt8 type;
	UInt8 status;
	UInt32 refreshTime;
	UInt32 changeStatusTime;
	UInt32 openItemId;
	UInt32 openItemNum;
};

struct DigItemInfo
{
	DigItemInfo() :itemId(0), itemNum(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & itemId & itemNum;
	}

	UInt32 itemId;
	UInt32 itemNum;
};

struct DigDetailInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & simpleInfo & digItems;
	}

	DigInfo simpleInfo;
	std::vector<DigItemInfo> digItems;
};

struct DigMapInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & mapId & goldDigSize & silverDigSize;
	}

	UInt32 mapId;

	UInt32 goldDigSize;

	UInt32 silverDigSize;
};

struct DigRecordInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & mapId & digIndex & type & playerId & playerName & itemId & itemNum;
	}

	UInt32 mapId;
	UInt32 digIndex;
	UInt8 type;
	ObjID_t playerId;
	std::string playerName;
	UInt32 itemId;
	UInt32 itemNum;
};


#endif //_CL_DIG_DEFINE_H_