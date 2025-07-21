#include "ItemScript.h"


ItemScript::ItemScript(UInt32 id, Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	SetId(id);
}

ItemScript::~ItemScript()
{
}

void ItemScript::OnBeginUseItem(Player* player, Item* item, Creature* target)
{
	Call<void>("OnBeginUseItem", player, item, target);
}

void ItemScript::OnSuspandUseItem(Player* player, Item* item, Creature* target)
{
	Call<void>("OnSuspandUseItem", player, item, target);
}

UInt32 ItemScript::OnUseSelf(Player* player, Item* item)
{
	return Call<UInt32>("OnUseSelf", player, item);
}

bool ItemScript::OnUseTarget(Player* player, Item* item, Creature* target)
{
	return Call<bool>("OnUseTarget", player, item, target);
}

bool ItemScript::CheckUseSelf(Player* player)
{
	return Call<bool>("CheckUseSelf", player);
}

bool ItemScript::CheckUseTarget(Player* player, Creature* target)
{
	return Call<bool>("CheckUseTarget", player, target);
}


UInt32 ItemScript::OnDecompose(Player* player, Item* item)
{
	return Call<UInt32>("OnDecompose", player, item);
}
