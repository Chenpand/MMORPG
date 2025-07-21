#include "ActivityScript.h"
#include "Player.h"

ActivityScript::ActivityScript(UInt32 id, Avalon::ScriptState *state)
	: Avalon::ScriptFile(state)
{
	SetId(id);
	m_dayChangeEvent = false;
	m_lvupChangeEvent = false;
	m_tickEvent = false;
	m_pkLvUpEvent = false;
	m_deathTower = false;
	m_wearEquip = false;
	m_wrapStoneLvUp = false;
	m_buyBlackShop = false;
	m_equipStrenth = false;
	m_pk = false;
	m_retinueChangeSkill = false;
	m_wearFashion = false;
	m_openJar = false;
	m_addMagic = false;
	m_getTitle = false;
	m_clearHell = false;
	m_retinueUpStar = false;
	m_retinueLvUp = false;
	m_charge = false;
	m_randQl = false;
}

ActivityScript::~ActivityScript()
{
}

void ActivityScript::Init()
{
	Call<void>("Init", this);
}

void ActivityScript::OnAccepted(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnAccepted", player, task);
	}
}

bool ActivityScript::OnSubmitted(Player* player, ActiveTask* task, bool bRp)
{
	if (task)
	{
		return Call<bool>("OnSubmitted", player, task, bRp);
	}

	return false;
}

void ActivityScript::BindGetItemEvent(UInt32 id)
{
	m_getItemEvents.push_back(id);
}

bool ActivityScript::CheckGetItemEvent(UInt32 id) const
{
	for (size_t i = 0; i < m_getItemEvents.size(); ++i)
	{
		if (0 == m_getItemEvents[i] || m_getItemEvents[i] == id)
		{
			return true;
		}
	}

	return false;
}

void ActivityScript::OnGetItem(Player* player, ActiveTask* task, UInt32 itemId, UInt32 num)
{
	if (task)
	{
		Call<void>("OnGetItem", player, task, itemId, num);
	}
}

void ActivityScript::OnRmItem(Player* player, ActiveTask* task, UInt32 itemId, UInt32 num)
{
	if (task)
	{
		Call<void>("OnRmItem", player, task, itemId, num);
	}
}

void ActivityScript::BindDayChange()
{
	m_dayChangeEvent = true;
}

bool ActivityScript::CheckDayChange()
{
	return m_dayChangeEvent;
}

void ActivityScript::OnDayChange(Player* player, ActiveTask* task, ActiveTask* preTask, UInt32 offlineDay)
{
	if (task)
	{
		Call<void>("OnDayChange", player, task, preTask, offlineDay);
	}
}

void ActivityScript::BindLevelUpEvent()
{
	m_lvupChangeEvent = true;
}

bool ActivityScript::CheckLevelUpEvent() const
{
	return m_lvupChangeEvent;
}

void ActivityScript::OnLvUpChange(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnLvUpChange", player, task);
	}
}

void ActivityScript::OnMinTick(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnMinTick", player, task);
	}
}

void ActivityScript::BindTickEvent()
{
	m_tickEvent = true;
}

bool ActivityScript::CheckTickEvent() const
{
	return m_tickEvent;
}

void ActivityScript::BindClearDungeonEvent(UInt32 id)
{
	m_clearDungeonEvents.push_back(id);
}

bool ActivityScript::CheckClearDungeonEvent(UInt32 id) const
{
	for (size_t i = 0; i < m_clearDungeonEvents.size(); ++i)
	{
		if (m_clearDungeonEvents[i] == id)
		{
			return true;
		}
	}

	return false;
}

void ActivityScript::OnClearDungeon(Player* player, ActiveTask* task, UInt32 id)
{
	if (task)
	{
		Call<void>("OnClearDungeon", player, task, id);
	}
}

void ActivityScript::OnPkLvUp(Player* player, ActiveTask* task, UInt8 type, UInt8 lv)
{
	if (task)
	{
		Call<void>("OnPkLvUp", player, task, type, lv);
	}
}

void ActivityScript::BindPkLvUpEvent()
{
	m_pkLvUpEvent = true;
}

bool ActivityScript::CheckPkLvUpEvent()
{
	return m_pkLvUpEvent;
}

void ActivityScript::BindDeathTower()
{
	m_deathTower = true;
}

bool ActivityScript::CheckDeathTower()
{
	return m_deathTower;
}

void ActivityScript::BindWearEquip()
{
	m_wearEquip = true;
}

bool ActivityScript::CheckWearEquip()
{
	return m_wearEquip;
}

void ActivityScript::BindWarpStoneUpLevel()
{
	m_wrapStoneLvUp = true;
}

bool ActivityScript::CheckWarpStoneUpLevel()
{
	return m_wrapStoneLvUp;
}

void ActivityScript::BindBuyBlackShop()
{
	m_buyBlackShop = true;
}

bool ActivityScript::CheckBuyBlackShop()
{
	return m_buyBlackShop;
}

void ActivityScript::OnDeathTower(Player* player, ActiveTask* task, UInt32 towerId, UInt32 layer, UInt32 usedTime)
{
	if (task)
	{
		Call<void>("OnDeathTower", player, task, towerId, layer, usedTime);
	}
}

void ActivityScript::OnWarpStoneUpLevel(Player* player, ActiveTask* task, UInt32 type, UInt32 lv)
{
	if (task)
	{
		Call<void>("OnWarpStoneUpLevel", player, task, type, lv);
	}
}

void ActivityScript::OnWearEquip(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnWearEquip", player, task);
	}
}

void ActivityScript::OnBuyBlackShop(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnBuyBlackShop", player, task);
	}
}

void ActivityScript::BindEquipStrenth()
{
	m_equipStrenth = true;
}

bool ActivityScript::CheckEquipStrenth()
{
	return m_equipStrenth;
}

void ActivityScript::BindPk()
{
	m_pk = true;
}

bool ActivityScript::CheckPk()
{
	return m_pk;
}

void ActivityScript::BindRetinueChangeSkill()
{
	m_retinueChangeSkill = true;
}

bool ActivityScript::CheckRetinueChangeSkill()
{
	return m_retinueChangeSkill;
}

void ActivityScript::BindWearFashionEquip()
{
	m_wearFashion = true;
}

bool ActivityScript::CheckWearFashionEquip()
{
	return m_wearFashion;
}

void ActivityScript::BindOpenJar()
{
	m_openJar = true;
}

bool ActivityScript::CheckOpenJar()
{
	return m_openJar;
}

void ActivityScript::BindAddMagic()
{
	m_addMagic = true;
}

bool ActivityScript::CheckAddMagic()
{
	return m_addMagic;
}

void ActivityScript::OnRetinueChangeSkill(Player* player, ActiveTask* task, UInt32 buffId)
{
	if (task)
	{
		Call<void>("OnRetinueChangeSkill", player, task, buffId);
	}
}

void ActivityScript::OnEquipStrenth(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnEquipStrenth", player, task);
	}
}

void ActivityScript::OnPk(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnPk", player, task);
	}
}

void ActivityScript::OnWearFashionEquip(Player* player, ActiveTask* task, UInt32 num)
{
	if (task)
	{
		Call<void>("OnWearFashionEquip", player, task, num);
	}
}

void ActivityScript::OnOpenJar(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnOpenJar", player, task);
	}
}

void ActivityScript::OnAddMagic(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnAddMagic", player, task);
	}
}

void ActivityScript::BindGetTitleEvent()
{
	m_getTitle = true;
}

bool ActivityScript::CheckGetTitleEvent() const
{
	return m_getTitle;
}

void ActivityScript::OnGetTitle(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnGetTitle", player, task);
	}
}

void ActivityScript::OnClearHell(Player* player, ActiveTask* task, UInt32 dungeonId, UInt32 usedTime)
{
	if (task)
	{
		Call<void>("OnClearHell", player, task, dungeonId, usedTime);
	}
}

void ActivityScript::BindAddClearHell()
{
	m_clearHell = true;
}

bool ActivityScript::CheckAddClearHell()
{
	return m_clearHell;
}

void ActivityScript::BindRetinueUpStarEvent()
{
	m_retinueUpStar = true;
}

bool ActivityScript::CheckRetinueUpStarEvent()
{
	return m_retinueUpStar;
}

void ActivityScript::OnRetinueUpStar(Player* player, ActiveTask* task, UInt32 retinueId, UInt8 starLv)
{
	if (task)
	{
		Call<void>("OnRetinueUpStar", player, task, retinueId, starLv);
	}
}

void ActivityScript::BindRetinueUpLevelEvent()
{
	m_retinueLvUp = true;
}

bool ActivityScript::CheckRetinueUpLevelEvent()
{
	return m_retinueLvUp;
}

void ActivityScript::OnRetinueUpLevel(Player* player, ActiveTask* task, UInt32 retinueId, UInt8 lv)
{
	if (task)
	{
		Call<void>("OnRetinueUpLevel", player, task, retinueId, lv);
	}
}

void ActivityScript::BindCharge()
{
	m_charge = true;
}

bool ActivityScript::CheckCharge()
{
	return m_charge;
}

void ActivityScript::OnCharge(Player* player, ActiveTask* task, UInt32 value)
{
	if (task)
	{
		Call<void>("OnCharge", player, task, value);
	}
}

void ActivityScript::BindRandQL()
{
	m_randQl = true;
}

bool ActivityScript::CheckRandQL()
{
	return m_randQl;
}

void ActivityScript::OnRandQL(Player* player, ActiveTask* task)
{
	if (task)
	{
		Call<void>("OnRandQL", player, task);
	}
}

void ActivityScript::BindShopBuyEvent(UInt32 shopId, UInt32 itemId)
{
	ShopBuyEvent sbEvent(shopId, itemId);
	m_ShopBuyEvents.push_back(sbEvent);
}

bool ActivityScript::CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const
{
	for (ShopBuyEventList::const_iterator iter = m_ShopBuyEvents.begin(); iter != m_ShopBuyEvents.end(); ++iter)
	{
		if ((iter->shopId == 0 || iter->shopId == shopId) &&
			(iter->shopItemId == 0 || iter->shopItemId == itemId))
			return true;
	}
	return false;
}

void ActivityScript::OnShopBuy(Player* player, ActiveTask* pTask, UInt32 shopId, UInt32 shopItemId, UInt32 num)
{
	if (pTask)
	{
		Call<void>("OnShopBuy", player, pTask, shopId, shopItemId, num);
	}
}

void ActivityScript::BindBuyMallItemEvent(UInt32 mallItemid)
{
	m_buyMallItemEvents.push_back(mallItemid);
}

bool ActivityScript::CheckBuyMallItemEvent(UInt32 mallItemid) const
{
	for (IdVec::const_iterator iter = m_buyMallItemEvents.begin(); iter != m_buyMallItemEvents.end(); iter++)
	{
		if (*iter == mallItemid || *iter == 0)
			return true;
	}
	return false;
}

void ActivityScript::OnBuyMallItem(Player *pPlayer, ActiveTask* pTask, UInt32 mallItemId, UInt32 mallItemNum)
{
	//noted by aprilliu, 2016.04.25 暂时没有使用，所以暂时没有添加task参数
	//modified by huchenhui, 2017.01.02	增加task参数
	if (pTask)
	{
		Call<void>("OnBuyMallItem", pPlayer, pTask, mallItemId, mallItemNum);
	}
}