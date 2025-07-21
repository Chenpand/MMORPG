#include "EquipCalcScript.h"
#include "Player.h"
#include "Creature.h"

EquipCalcScript::EquipCalcScript(Avalon::ScriptState* state)
	: Avalon::ScriptFile(state)
{
}

EquipCalcScript::~EquipCalcScript()
{
}

float EquipCalcScript::GetWpPhyMod(UInt8 subType)
{
	return (Call<float>("GetWpPhyMod", subType) / 100);
}

float EquipCalcScript::GetWpMagMod(UInt8 subType)
{
	return (Call<float>("GetWpMagMod", subType) / 100);
}

float EquipCalcScript::GetWpClQaMod(UInt8 color)
{
	return Call<float>("GetWpClQaMod", color);
}

float EquipCalcScript::GetWpClQbMod(UInt8 color)
{
	return (Call<float>("GetWpClQbMod", color) / 100);
}

float EquipCalcScript::GetWpStrMod(UInt8 strLv)
{
	return Call<float>("GetWpStrMod", strLv);
}

float EquipCalcScript::GetArmStrMod(UInt8 strLv)
{
	return (Call<float>("GetArmStrMod", strLv) / 100);
}

float EquipCalcScript::GetArmClQbMod(UInt8 color)
{
	return (Call<float>("GetArmClQbMod", color) / 100);
}

float EquipCalcScript::GetJewStrMod(UInt8 strLv)
{
	return (Call<float>("GetJewStrMod", strLv) / 100);
}

float EquipCalcScript::GetJewClQbMod(UInt8 color)
{
	return (Call<float>("GetJewClQbMod", color) / 100);
}

UInt32 EquipCalcScript::GetNeedSealMat(UInt16 lv, UInt8 color, UInt32 sealCount)
{
	return Call<UInt32>("GetNeedSealMat", lv, color, sealCount);
}

float EquipCalcScript::GetCostMod(UInt32 subType)
{
	if (IsWeapon(subType))
	{
		return (Call<float>("GetCostMod", 1) / 10);
	}
	else if (IsDefend(subType))
	{
		return (Call<float>("GetCostMod", 2) / 10);
	}
	else if (IsOrnaments(subType))
	{
		return (Call<float>("GetCostMod", 3) / 10);
	}

	return 0;
}

