#include "CLEquipRecoScUpPrbWeightDataEntryBase.h"

EquipRecoScUpPrbWeightDataEntryBase::EquipRecoScUpPrbWeightDataEntryBase()
{
}

EquipRecoScUpPrbWeightDataEntryBase::~EquipRecoScUpPrbWeightDataEntryBase()
{
}

bool EquipRecoScUpPrbWeightDataEntryBase::Read(Avalon::DataRow& row)
{
	if(row.GetColNum() != GetColNum())
	{
		return false;
	}	

	iD = row.ReadUInt32();
	upScore = row.ReadUInt32();
	probWeight = row.ReadUInt32();

	return InternalRead();
}
