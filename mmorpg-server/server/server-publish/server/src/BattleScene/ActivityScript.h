#ifndef _ACTIVITY_SCRIPT_H_
#define _ACTIVITY_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include "Player.h"
#include "ActiveTask.h"

/**
 *@brief ��ű�
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
	
	//��õ��߻
	void BindGetItemEvent(UInt32 id);
	bool CheckGetItemEvent(UInt32 id) const;

	//��óƺ�
	void BindGetTitleEvent();
	bool CheckGetTitleEvent() const;

	//�����
	void BindLevelUpEvent();
	bool CheckLevelUpEvent() const;

	//ǩ���
	void BindDayChange();
	bool CheckDayChange();

	//�ۼ�����ʱ���
	void BindTickEvent();
	bool CheckTickEvent() const;

	//ͨ�ص��³ǻ
	void BindClearDungeonEvent(UInt32 id);
	bool CheckClearDungeonEvent(UInt32 id) const;

	//pk��λ
	void BindPkLvUpEvent();
	bool CheckPkLvUpEvent();

	//������
	void BindDeathTower();
	bool CheckDeathTower();

	//����װ��
	void BindWearEquip();
	bool CheckWearEquip();

	//��Ԫʯ����
	void BindWarpStoneUpLevel();
	bool CheckWarpStoneUpLevel();

	//���й���
	void BindBuyBlackShop();
	bool CheckBuyBlackShop();

	//װ��ǿ��
	void BindEquipStrenth();
	bool CheckEquipStrenth();

	//������
	void BindPk();
	bool CheckPk();

	//���ϴ��
	void BindRetinueChangeSkill();
	bool CheckRetinueChangeSkill();

	//�������
	void BindRetinueUpStarEvent();
	bool CheckRetinueUpStarEvent();

	//����������¼�
	void BindRetinueUpLevelEvent();
	bool CheckRetinueUpLevelEvent();

	//����ʱװ
	void BindWearFashionEquip();
	bool CheckWearFashionEquip();

	//������
	void BindOpenJar();
	bool CheckOpenJar();

	//��ħ
	void BindAddMagic();
	bool CheckAddMagic();

	//ͨ����Ԩ
	void BindAddClearHell();
	bool CheckAddClearHell();

	//��ֵ
	void BindCharge();
	bool CheckCharge();

	//Ʒ������
	void BindRandQL();
	bool CheckRandQL();
	
	/**
	*@brief �̵깺��
	*/
	void BindShopBuyEvent(UInt32 shopId, UInt32 itemId);
	bool CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const;

	/**
	*@brief �󶨹����̳ǵ����¼�
	*/
	void BindBuyMallItemEvent(UInt32 mallItemid);
	bool CheckBuyMallItemEvent(UInt32 mallItemid) const;

public: //�ű��¼�
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
	//��ȡ�����¼��б�
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

	//�̵깺�� pxr
	ShopBuyEventList	m_ShopBuyEvents;
	//�����̳ǵ����¼��б� pxr
	IdVec	m_buyMallItemEvents;
};

#endif
