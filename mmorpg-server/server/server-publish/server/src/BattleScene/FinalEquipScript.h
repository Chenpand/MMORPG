#ifndef _SCENE_FINALEQUIP_SCRIPT_H_
#define _SCENE_FINALEQUIP_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

class Player;
class Creature;
class Item;

/**
*@brief �ռ�װ���ű�
*/
class FinalEquipScript : public Avalon::ScriptFile
{
	/**
	*@brief װ��Ч������
	*/
	enum FINALEQUIP_EFFECT_TYPE
	{
		FEE_INVALID = 0,	// ��Ч
		FEE_ADDBUFF = 1,	// ����ʱ��һ�����ʼ�BUFF
		FEE_MODATTR = 2,	// ��������
		FEE_ADDEXP = 3,		// ����ӳ�
		FEE_SCRIPTINIT = 4,	// �ɽű���ʼ������
	};

	/**
	*@brief װ��Ч���ṹ
	*/
	struct FinalEquipEffect
	{
		FINALEQUIP_EFFECT_TYPE	effect;
		Int32					param[3];
	};

	typedef std::map<UInt32, FinalEquipEffect*> FinalEquipEffectMap;

public:
	FinalEquipScript(Avalon::ScriptState* state);
	~FinalEquipScript();

	/**
	*@brief ��ʼ���ű�
	*/
	void Init();

	/**
	*@brief ���װ��Ч��
	*/
	void AddEffect(UInt32 uItemId, FINALEQUIP_EFFECT_TYPE effect, Int32 param1, Int32 param2, Int32 param3);

public:
	/**
	*@brief �Ƿ�Ϊ�ռ�װ��
	*/
	bool IsFinalEquip(UInt32 uItemId);

	/**
	*@brief �����¼�
	*/
	void OnAttack(Player *pAtk, Creature *pDfd);

	/**
	*@brief ��ʼ���ռ�װ��
	*/
	void InitEquip(Item *pItem);

	/**
	*@brief ����ռ�װ��������
	*/
	bool GetEquipAttr(Item *pItem);

	/**
	*@brief ��þ���ӳ�
	*/
	UInt32 GetExpAddFactor(Player *pPlayer);

private:
	// �ռ�װ��Ч����
	FinalEquipEffectMap	m_mapFinalEquipEffect;
};

#endif

