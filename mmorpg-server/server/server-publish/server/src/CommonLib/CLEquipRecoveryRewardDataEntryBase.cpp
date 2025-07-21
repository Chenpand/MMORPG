#include "CLEquipRecoveryRewardDataEntryBase.h"

EquipRecoveryRewardDataEntryBase::EquipRecoveryRewardDataEntryBase()
{
}

EquipRecoveryRewardDataEntryBase::~EquipRecoveryRewardDataEntryBase()
{
}

bool EquipRecoveryRewardDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		return false;
	}	

	iD = row.ReadUInt32();
	integral = row.ReadUInt32();
	jarID = row.ReadUInt32();

	return InternalRead();
}