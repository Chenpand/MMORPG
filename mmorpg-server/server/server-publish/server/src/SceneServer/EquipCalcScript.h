#ifndef _SCENE_EQUIPCALC_SCRIPT_H_
#define _SCENE_EQUIPCALC_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

class Player;
class Creature;
class Item;

/**
*@brief 装备计算脚本
*/
class EquipCalcScript : public Avalon::ScriptFile
{

public:
	EquipCalcScript(Avalon::ScriptState* state);
	~EquipCalcScript();

public: //强化
	
	float GetWpPhyMod(UInt8 subType);		//得到武器类别物攻系数
	float GetWpMagMod(UInt8 subType);		//得到武器类别法攻系数
	float GetWpClQaMod(UInt8 color);		//得到武器品质QA系数
	float GetWpClQbMod(UInt8 color);		//得到武器品质QB系数
	float GetWpStrMod(UInt8 strLv);			//得到武器强化系数
	float GetArmStrMod(UInt8 strLv);		//得到防具强化系数
	float GetArmClQbMod(UInt8 color);		//得到防具品质QB系数
	float GetJewStrMod(UInt8 strLv);		//得到首饰强化系数
	float GetJewClQbMod(UInt8 color);		//得到首饰品质QB系数
	float GetCostMod(UInt32 subType);		//得到强化消耗系数

public: //封装
	UInt32 GetNeedSealMat(UInt16 lv, UInt8 color, UInt32 sealCount);		//得到封装所需蜜蜡数
	
};

#endif

