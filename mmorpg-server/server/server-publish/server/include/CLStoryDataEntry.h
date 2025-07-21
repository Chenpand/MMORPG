#ifndef _CL_STORY_DATAENTRY_H_
#define _CL_STORY_DATAENTRY_H_

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

#include "CLItemDefine.h"

/**
 *@brief 剧情数据项                                                                     
 */
class StoryDataEntry : public Avalon::DataEntry
{
public:
	StoryDataEntry();
	~StoryDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 mapId;
	UInt32 preStory;
	std::vector<ItemReward>	rewards;
};

typedef Avalon::DataEntryMgr<StoryDataEntry> StoryDataEntryMgr;

#endif
