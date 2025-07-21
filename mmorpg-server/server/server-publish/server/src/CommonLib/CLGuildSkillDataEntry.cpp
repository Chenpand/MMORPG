#include "CLGuildSkillDataEntry.h"

bool GuildSkillDataEntry::Read(Avalon::DataRow& row)
{
	skillId = row.ReadUInt16();
	level = row.ReadUInt8();
	needBattleLevel = row.ReadUInt8();
	contriCost = row.ReadUInt32();
	goldCost = row.ReadUInt32();

	return true;
}