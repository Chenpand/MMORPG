#ifndef _SCENE_FINALEQUIP_SCRIPT_H_
#define _SCENE_FINALEQUIP_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

class Player;
class Creature;
class Item;

/**
*@brief 终极装备脚本
*/
class FinalEquipScript : public Avalon::ScriptFile
{
	/**
	*@brief 装备效果类型
	*/
	enum FINALEQUIP_EFFECT_TYPE
	{
		FEE_INVALID = 0,	// 无效
		FEE_ADDBUFF = 1,	// 攻击时有一定几率加BUFF
		FEE_MODATTR = 2,	// 附加属性
		FEE_ADDEXP = 3,		// 经验加成
		FEE_SCRIPTINIT = 4,	// 由脚本初始化属性
	};

	/**
	*@brief 装备效果结构
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
	*@brief 初始化脚本
	*/
	void Init();

	/**
	*@brief 添加装备效果
	*/
	void AddEffect(UInt32 uItemId, FINALEQUIP_EFFECT_TYPE effect, Int32 param1, Int32 param2, Int32 param3);

public:
	/**
	*@brief 是否为终极装备
	*/
	bool IsFinalEquip(UInt32 uItemId);

	/**
	*@brief 攻击事件
	*/
	void OnAttack(Player *pAtk, Creature *pDfd);

	/**
	*@brief 初始化终极装备
	*/
	void InitEquip(Item *pItem);

	/**
	*@brief 获得终极装备的属性
	*/
	bool GetEquipAttr(Item *pItem);

	/**
	*@brief 获得经验加成
	*/
	UInt32 GetExpAddFactor(Player *pPlayer);

private:
	// 终极装备效果表
	FinalEquipEffectMap	m_mapFinalEquipEffect;
};

#endif

