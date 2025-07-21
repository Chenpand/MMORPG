#include "CLDeathTowerAwardDataEntry.h"

DeathTowerAwardDataEntry::DeathTowerAwardDataEntry()
{

}

DeathTowerAwardDataEntry::~DeathTowerAwardDataEntry()
{

}

bool DeathTowerAwardDataEntry::Read(Avalon::DataRow& row)
{
	floor = row.ReadUInt32();
	exp = row.ReadUInt32();
	warriorSoul = row.ReadUInt32();
	dropSet = row.ReadUInt32();
	awardItemId = row.ReadUInt32();
	awardItemNum = row.ReadUInt32();
	levelLimit = row.ReadUInt16();

	return true;
}