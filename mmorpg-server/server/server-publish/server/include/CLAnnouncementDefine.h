#ifndef _CL_ANNOUNCEMENT_DEFINE_H_
#define _CL_ANNOUNCEMENT_DEFINE_H_

#include <AvalonNetStream.h>
#include <AvalonObjectPool.h>

/**
*@brief 公告数据
*/
struct AnnouncementData
{
	AVALON_DEFINE_OBJECTPOOL(AnnouncementData, Avalon::NullMutex)

	AnnouncementData() :id(0), begintime(0), interval(0), times(0), isSave(0), gmId(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & begintime & interval & times & content & isSave & gmId;
	}

	UInt32 id;
	UInt32 begintime;
	UInt32 interval;
	UInt32 times;
	std::string content;
	UInt8 isSave;
	UInt32 gmId;
};

#define ANNOUNCE_STRENGTHEN_TEN				10
#define ANNOUNCE_STRENGTHEN_LOWER_BEGIN		11
#define ANNOUNCE_STRENGTHEN_LOWER_END		15

#define ANNOUNCE_STRENGTHEN_MIDDLE_BEGIN	16
#define ANNOUNCE_STRENGTHEN_MIDDLE_END		19

#define ANNOUNCE_STRENGTHEN_LAST			20

#define ANNOUNCE_WARPSTONE_LEVEL_TEN		10
#define ANNOUNCE_WARPSTONE_LEVEL_TWENTY		20
#define ANNOUNCE_WARPSTONE_LEVEL_SIMPLE		2

#define ANNOUNCE_WARPSTONE_SIZE_ONE			1
#define ANNOUNCE_WARPSTONE_SIZE_TWO			2
#define ANNOUNCE_WARPSTONE_SIZE_FIVE		5
#define ANNOUNCE_WARPSTONE_SIZE_TEN			10

#define ANNOUNCE_PVP_MAX_WIN_FIVE			5
#define ANNOUNCE_PVP_MAX_WIN_TEN			10
#define ANNOUNCE_PVP_MAX_WIN_TWENTY			20
#define ANNOUNCE_PVP_MAX_WIN_FIFTY			50


#endif