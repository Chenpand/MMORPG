#ifndef _ACTIVITY_SCRIPT_H_
#define _ACTIVITY_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include "Player.h"
#include "ActiveTask.h"

/**
 *@brief 活动脚本
 */
class ActivityScript : public Avalon::ScriptFile
{
public:
	typedef std::vector<UInt32>	IdVec;
	struct ShopBuyEvent
	{
		ShopBuyEvent() : shopId(0), shopItemId(0){}
		ShopBuyEvent(UInt32 _shopId, UInt32 _shopItemId) :shopId(_shopId), shopItemId(_shopItemId){}

		UInt32 shopId;
		UInt32 shopItemId;
	};
	typedef std::vector<ShopBuyEvent> ShopBuyEventList;

public:
	ActivityScript(UInt32 id, Avalon::ScriptState* state);
	~ActivityScript();

	void Init();
	
	//获得道具活动
	void BindGetItemEvent(UInt32 id);
	bool CheckGetItemEvent(UInt32 id) const;

	//获得称号
	void BindGetTitleEvent();
	bool CheckGetTitleEvent() const;

	//升级活动
	void BindLevelUpEvent();
	bool CheckLevelUpEvent() const;

	//签到活动
	void BindDayChange();
	bool CheckDayChange();

	//累计在线时长活动
	void BindTickEvent();
	bool CheckTickEvent() const;

	//通关地下城活动
	void BindClearDungeonEvent(UInt32 id);
	bool CheckClearDungeonEvent(UInt32 id) const;

	//pk段位
	void BindPkLvUpEvent();
	bool CheckPkLvUpEvent();

	//死亡塔
	void BindDeathTower();
	bool CheckDeathTower();

	//穿戴装备
	void BindWearEquip();
	bool CheckWearEquip();

	//次元石升级
	void BindWarpStoneUpLevel();
	bool CheckWarpStoneUpLevel();

	//黑市购买
	void BindBuyBlackShop();
	bool CheckBuyBlackShop();

	//装备强化
	void BindEquipStrenth();
	bool CheckEquipStrenth();

	//决斗场
	void BindPk();
	bool CheckPk();

	//随从洗炼
	void BindRetinueChangeSkill();
	bool CheckRetinueChangeSkill();

	//随从升星
	void BindRetinueUpStarEvent();
	bool CheckRetinueUpStarEvent();

	//绑定随从升级事件
	void BindRetinueUpLevelEvent();
	bool CheckRetinueUpLevelEvent();

	//穿戴时装
	void BindWearFashionEquip();
	bool CheckWearFashionEquip();

	//开罐子
	void BindOpenJar();
	bool CheckOpenJar();

	//附魔
	void BindAddMagic();
	bool CheckAddMagic();

	//通关深渊
	void BindAddClearHell();
	bool CheckAddClearHell();

	//充值
	void BindCharge();
	bool CheckCharge();

	//品级调整
	void BindRandQL();
	bool CheckRandQL();
	
	/**
	*@brief 商店购买
	*/
	void BindShopBuyEvent(UInt32 shopId, UInt32 itemId);
	bool CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const;

	/**
	*@brief 绑定购买商城道具事件
	*/
	void BindBuyMallItemEvent(UInt32 mallItemid);
	bool CheckBuyMallItemEvent(UInt32 mallItemid) const;

public: //脚本事件
	void OnAccepted(Player* player, ActiveTask* task);
	bool OnSubmitted(Player* player, ActiveTask* task, bool bRp);
	void OnGetItem(Player* player, ActiveTask* task, UInt32 itemId, UInt32 num);
	void OnRmItem(Player* player, ActiveTask* task, UInt32 itemId, UInt32 num);
	void OnDayChange(Player* player, ActiveTask* task, ActiveTask* preTask, UInt32 offlineDay);
	void OnLvUpChange(Player* player, ActiveTask* task);
	void OnMinTick(Player* player, ActiveTask* task);
	void OnClearDungeon(Player* player, ActiveTask* task, UInt32 id);
	void OnPkLvUp(Player* player, ActiveTask* task, UInt8 type, UInt8 lv);
	void OnDeathTower(Player* player, ActiveTask* task, UInt32 towerId, UInt32 layer, UInt32 usedTime);
	void OnWarpStoneUpLevel(Player* player, ActiveTask* task, UInt32 type, UInt32 lv);
	void OnBuyBlackShop(Player* player, ActiveTask* task);
	void OnWearEquip(Player* player, ActiveTask* task);
	void OnRetinueChangeSkill(Player* player, ActiveTask* task, UInt32 buffId);
	void OnEquipStrenth(Player* player, ActiveTask* task);
	void OnPk(Player* player, ActiveTask* task);
	void OnWearFashionEquip(Player* player, ActiveTask* task, UInt32 num);
	void OnOpenJar(Player* player, ActiveTask* task);
	void OnAddMagic(Player* player, ActiveTask* task);
	void OnGetTitle(Player* player, ActiveTask* task);
	void OnClearHell(Player* player, ActiveTask* task, UInt32 dungeonId, UInt32 usedTime);
	void OnRetinueUpStar(Player* player, ActiveTask* task, UInt32 retinueId, UInt8 starLv);
	void OnRetinueUpLevel(Player* player, ActiveTask* task, UInt32 retinueId, UInt8 lv);
	void OnCharge(Player* player, ActiveTask* task, UInt32 value);
	void OnRandQL(Player* player, ActiveTask* task);
	void OnShopBuy(Player* player, ActiveTask* pTask, UInt32 shopId, UInt32 shopItemId, UInt32 num);
	void OnBuyMallItem(Player *pPlayer, ActiveTask* pTask, UInt32 mallItemId, UInt32 OnBuyMallItem);
private:
	//获取道具事件列表
	std::vector<UInt32>	m_getItemEvents;
	bool	m_dayChangeEvent;
	bool	m_lvupChangeEvent;
	bool	m_tickEvent;
	std::vector<UInt32>	m_clearDungeonEvents;
	bool	m_pkLvUpEvent;
	bool	m_deathTower;
	bool	m_wearEquip;
	bool	m_wrapStoneLvUp;
	bool	m_buyBlackShop;
	bool	m_equipStrenth;
	bool	m_pk;
	bool	m_retinueChangeSkill;
	bool	m_wearFashion;
	bool	m_openJar;
	bool	m_addMagic;
	bool	m_getTitle;
	bool	m_clearHell;
	bool	m_retinueUpStar;
	bool	m_retinueLvUp;
	bool	m_charge;
	bool	m_randQl;

	//商店购买 pxr
	ShopBuyEventList	m_ShopBuyEvents;
	//购买商城道具事件列表 pxr
	IdVec	m_buyMallItemEvents;
};

#endif
