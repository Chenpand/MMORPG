#ifndef _CL_ANNOUNCEMENT_DATAENTRY_H_
#define _CL_ANNOUNCEMENT_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 系统公告 
* 原类名SysNotifyDataEntry,类名有歧义.所以改掉
*/

enum AnnouncementSyncEvent
{
	SyncGuildLog = 1 << 1,//同步公会日志
};
class AnnouncementDataEntry : public Avalon::DataEntry
{
public:
	AnnouncementDataEntry();
	~AnnouncementDataEntry();

	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32	id;

	//首播时间（秒）
	//UInt32	firstTime;

	//内容
	std::string content;

	//播放间隔（秒）
	UInt32	interval;

	//播放次数
	UInt32	times;

	//参数
	UInt32	param;

	//是否同步到公会日志
	UInt64	SyncEventBit;

};

typedef Avalon::DataEntryVisitor<AnnouncementDataEntry> AnnouncementDataEntryVisitor;
typedef Avalon::DataEntryMgr<AnnouncementDataEntry> AnnouncementDataEntryMgr;

#endif
