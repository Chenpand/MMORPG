#include "CLSceneDataEntry.h"
#include <AvalonRandom.h>

SceneDataEntry::SceneDataEntry()
{
    id = 0;
    type = 0;
    level = 0;
    option = 0;
    syncRange = 4;
    majorCity = 0;
    maxPlayerNum = 0;
    reviveTime = 0;
    recoverPercent = 0;
    memset(revivePos, 0, sizeof(revivePos));
    enterX = 0;
    enterY = 0;
    dailytimes = 0;
    itemId = 0;
	levelLimit = 0;
}

SceneDataEntry::~SceneDataEntry()
{
}

const CLPosition& SceneDataEntry::GetRevivePos() const
{
	int num = 0;
	for(int i = 0; i < 3; ++ i)
	{
		if(revivePos[i].x == 0 || revivePos[i].y == 0) break;
		++num;
	}
	if(num <= 1) return revivePos[0];

	return revivePos[Avalon::Random::RandBetween(0, num - 1)];
}

bool SceneDataEntry::Read(Avalon::DataRow& row)
{
    id = row.ReadUInt32();
    name = row.ReadString();
    birthCity = row.ReadUInt32();
    type = row.ReadUInt8();
	subType = (SceneSubType)row.ReadUInt32();
    syncRange = row.ReadUInt32();

    std::string dungeonsStr = row.ReadString();
    if (dungeonsStr != "-")
    {
        std::vector<std::string> dungeonIdStrs;
        Avalon::StringUtil::Split(dungeonsStr, dungeonIdStrs, "|");
        for (auto dungeonIdStr : dungeonIdStrs)
        {
            UInt32 dungeonId = Avalon::StringUtil::ConvertToValue<UInt32>(dungeonIdStr);
            dungeonList.push_back(dungeonId);
        }
    }
	levelLimit = row.ReadUInt16();
	dungeonId = row.ReadUInt32();

	/*id = row.ReadUInt32();
	name = row.ReadString();
	type = row.ReadUInt8();
	level = row.ReadUInt16();
	option = row.ReadUInt16();
	syncRange = row.ReadUInt32();
	majorCity = row.ReadUInt32();
	maxPlayerNum = row.ReadUInt32();
	reviveTime = row.ReadUInt32();
	recoverPercent = row.ReadUInt32();
	for(int i = 0; i < 3; ++i)
	{
		revivePos[i].x = row.ReadUInt16();
		revivePos[i].y = row.ReadUInt16();
	}
	enterX = row.ReadUInt16();
	enterY = row.ReadUInt16();
	dailytimes = row.ReadUInt8();
	itemId = row.ReadUInt32();*/
	return true;
}


