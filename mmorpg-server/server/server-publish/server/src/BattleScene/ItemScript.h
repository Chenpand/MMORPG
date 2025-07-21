#ifndef _ITEM_SCRIPT_H_
#define _ITEM_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Item;
class Player;
class Creature;

/**
 *@brief item脚本
 */
class ItemScript : public Avalon::ScriptFile
{
public:
	ItemScript(UInt32 id, Avalon::ScriptState* state);
	~ItemScript();

public:
	/**
	 *@brief 开始使用和中断使用道具
	 */
	void OnBeginUseItem(Player* player, Item* item, Creature* target);
	void OnSuspandUseItem(Player* player, Item* item, Creature* target);

	/**
	 *@brief 使用
	 */
	UInt32 OnUseSelf(Player* player, Item* item);
	bool OnUseTarget(Player* player, Item* item, Creature* target);

	//分解
	UInt32 OnDecompose(Player* player, Item* item);

	/**
	 *@brief 检查能否使用
	 */
	bool CheckUseSelf(Player* player);
	bool CheckUseTarget(Player* player, Creature* target);

};

#endif
