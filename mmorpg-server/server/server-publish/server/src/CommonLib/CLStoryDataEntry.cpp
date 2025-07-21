#include "CLStoryDataEntry.h"

StoryDataEntry::StoryDataEntry()
{

}

StoryDataEntry::~StoryDataEntry()
{

}

bool StoryDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	mapId = row.ReadUInt32();
	preStory = row.ReadUInt32();
	std::istringstream stream(row.ReadString());
	char split = 0;
	while(stream)
	{
		ItemReward item;
		item.DecodeString(stream);
		if(item.id == 0) break;
		rewards.push_back(item);
		stream >> split;
	}
	return true;
}
