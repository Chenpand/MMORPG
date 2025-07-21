#include "CLEquipSuitDataEntry.h"

EquipSuitDataEntry::EquipSuitDataEntry()
{
	id = 0;
	memset(attrIdByEquipNum, 0, sizeof(attrIdByEquipNum));
	memset(attrIdByEquipNumNew, 0, sizeof(attrIdByEquipNumNew));
}

bool EquipSuitDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	suitLev = row.ReadUInt32();

	// 从两件套开始
	for (size_t i = 2; i <= sizeof(attrIdByEquipNumNew) / sizeof(attrIdByEquipNumNew[0]); i++)
	{
		if (i==7)
		{
			continue;
		}

		auto attr = row.ReadUInt32();

		if (i <= sizeof(attrIdByEquipNum) / sizeof(attrIdByEquipNum[0]))
		{
			if (attr == 0)
			{
				attrIdByEquipNum[i - 1] = attrIdByEquipNum[i - 2];
			}
			else
			{
				attrIdByEquipNum[i - 1] = attr;
			}
		}

		attrIdByEquipNumNew[i - 1] = attr;
	}

	eqScoreAdjust = row.ReadUInt32();
	return true;
}