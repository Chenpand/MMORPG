#ifndef _ITEM_SCRIPT_H_
#define _ITEM_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLDefine.h>

class Item;
class Player;
class Creature;

/**
 *@brief item�ű�
 */
class ItemScript : public Avalon::ScriptFile
{
public:
	ItemScript(UInt32 id, Avalon::ScriptState* state);
	~ItemScript();

public:
	/**
	 *@brief ��ʼʹ�ú��ж�ʹ�õ���
	 */
	void OnBeginUseItem(Player* player, Item* item, Creature* target);
	void OnSuspandUseItem(Player* player, Item* item, Creature* target);

	/**
	 *@brief ʹ��
	 */
	UInt32 OnUseSelf(Player* player, Item* item);
	bool OnUseTarget(Player* player, Item* item, Creature* target);

	//�ֽ�
	UInt32 OnDecompose(Player* player, Item* item);

	/**
	 *@brief ����ܷ�ʹ��
	 */
	bool CheckUseSelf(Player* player);
	bool CheckUseTarget(Player* player, Creature* target);

};

#endif
