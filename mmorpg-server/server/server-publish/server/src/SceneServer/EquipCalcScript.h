#ifndef _SCENE_EQUIPCALC_SCRIPT_H_
#define _SCENE_EQUIPCALC_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

class Player;
class Creature;
class Item;

/**
*@brief װ������ű�
*/
class EquipCalcScript : public Avalon::ScriptFile
{

public:
	EquipCalcScript(Avalon::ScriptState* state);
	~EquipCalcScript();

public: //ǿ��
	
	float GetWpPhyMod(UInt8 subType);		//�õ���������﹥ϵ��
	float GetWpMagMod(UInt8 subType);		//�õ�������𷨹�ϵ��
	float GetWpClQaMod(UInt8 color);		//�õ�����Ʒ��QAϵ��
	float GetWpClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
	float GetWpStrMod(UInt8 strLv);			//�õ�����ǿ��ϵ��
	float GetArmStrMod(UInt8 strLv);		//�õ�����ǿ��ϵ��
	float GetArmClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
	float GetJewStrMod(UInt8 strLv);		//�õ�����ǿ��ϵ��
	float GetJewClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
	float GetCostMod(UInt32 subType);		//�õ�ǿ������ϵ��

public: //��װ
	UInt32 GetNeedSealMat(UInt16 lv, UInt8 color, UInt32 sealCount);		//�õ���װ����������
	
};

#endif

