#ifndef _TASK_SCRIPT_H_
#define _TASK_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLObjectDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include <CLErrorCode.h>
#include <CLRelationDefine.h>
#include <CLCityMonsterDataEntry.h>
#include <CLTaskDefine.h>


class Player;
class Npc;
class RewardGroup;
class BaseTask;
class Item;

struct TaskScriptEventBase
{
	virtual UInt64 GetEventKey() = 0;
};

struct EquipRemakeEvent : public TaskScriptEventBase
{
	EquipRemakeEvent() :id(0), remakeType(0) {}
	UInt32	id;
	UInt8	remakeType;
	virtual UInt64 GetEventKey()
	{
		return TET_EQUIP_REMAKE_EVENT;
	}
};
typedef std::vector<EquipRemakeEvent>	EquipRemakeEventVec;

struct GetItemEvent : public TaskScriptEventBase
{
	GetItemEvent() : id(0), quality(0), strengthen(0) {}
	UInt32	id;
	UInt8	quality;
	UInt8	strengthen;
	virtual UInt64 GetEventKey()
	{
		//return id + quality + strengthen;
		return TET_GET_ITEM_EVENT;
	}
};
typedef std::vector<GetItemEvent>		GetItemEventVec;

//added by aprilliu.2016.04.13
struct stKillNpcEvent : public TaskScriptEventBase //��ɱ������¼�
{
	stKillNpcEvent() :id(0), type(0), dungeon(0)
	{
	}

	UInt32	id;				//����id
	UInt8	type;			//��������
	UInt32  dungeon;		//���������ĵ��³�id   added by aprilliu,2016.05.27
	virtual UInt64 GetEventKey()
	{
		return TET_STKILL_NPC_EVENT;
	}
};
typedef	std::vector<stKillNpcEvent>		KillNPCEventList;

//��ɱ�����������
struct KillCityMonsterEvent : public TaskScriptEventBase
{
	KillCityMonsterEvent(CityMonsterType _type, UInt32 _monsterId) :type(_type), monsterId(_monsterId) {}

	CityMonsterType type;	//�����������
	UInt32 monsterId;		//�������ID
	virtual UInt64 GetEventKey()
	{
		return TET_KILL_CITY_MONSTER_EVENT;
	}
};
typedef std::vector<KillCityMonsterEvent>	KillCityMonsterEventList;

//added by huchenhui 2016.09.02
struct StrengthenEquipEvent : public TaskScriptEventBase	//ǿ���¼�
{
	StrengthenEquipEvent() :isRegister(false), isSuccess(false), strengthen(0), isTicket(false) {}

	bool isRegister;	//�Ƿ��ע
	bool isSuccess;		//�Ƿ�ֻ��ע�ɹ�
	UInt8 strengthen;	//��ע��ǿ���ȼ�
	bool isTicket;		//�Ƿ�ʹ��ǿ����
	virtual UInt64 GetEventKey()
	{
		return TET_STRENGTHEN_EQUIP_EVENT;
	}
};

//added by huchenhui 2016.06.29
struct PutEquipEvent : public TaskScriptEventBase
{
	PutEquipEvent() :level(0), strengthen(0), color(0) {}

	UInt16 level;			//����ȼ�
	UInt32 strengthen;		//ǿ���ȼ�
	UInt8 color;			//װ��Ʒ��
	virtual UInt64 GetEventKey()
	{
		return TET_PUT_EQUIP_EVENT;
	}
};
typedef std::vector<PutEquipEvent>	PutEquipEventList;

//added by huchenhui 2018.03.20
struct PutFashionEvent : public TaskScriptEventBase
{
	PutFashionEvent(ItemSubType _subType, UInt8 _color, UInt8 _isTransparent = 0)
		:subType(_subType), color(_color), isTransparent(_isTransparent) { }

	ItemSubType subType;		//������
	UInt8 color;				//Ʒ��
	UInt8 isTransparent;		//�Ƿ�͸��ʱװ
	virtual UInt64 GetEventKey()
	{
		return TET_PUT_FASHION_EVENT;
	}
};
typedef std::vector<PutFashionEvent> PutFashionEventList;

//added by huchenhui 2016.08.22
struct ClearDungeonEvent : public TaskScriptEventBase
{
	ClearDungeonEvent() : id(0), score(LS_C), usedTime(0), reviveCount(0) {}

	UInt32 id;				//����ID
	LevelScore score;		//�ؿ�����
	UInt32 usedTime;		//��ʱ
	UInt32 reviveCount;		//�������
	virtual UInt64 GetEventKey()
	{
		return TET_CLEAR_DUNGEON_EVENT;
	}
};
typedef std::vector<ClearDungeonEvent> ClearDungeonEventList;

struct DeathTowerEvent : public TaskScriptEventBase
{
	DeathTowerEvent() : id(0), layer(0), usedTime(0) {}

	UInt32 id;				//����֮��ID
	UInt32 layer;			//����
	UInt32 usedTime;		//��ʱ
	virtual UInt64 GetEventKey()
	{
		return TET_DEATH_TOWER_EVENT;
	}
};
typedef std::vector<DeathTowerEvent> DeathTowerEventList;

//added by huchenhui 2016.08.23
struct WarpStoneUpLevelEvent : public TaskScriptEventBase
{
	WarpStoneUpLevelEvent() : type(STONE_INVALID), level(0) {}

	WarpStoneType type;
	UInt32 level;
	virtual UInt64 GetEventKey()
	{
		return TET_WARP_STONE_UP_LEVEL_EVENT;
	}
};
typedef std::vector<WarpStoneUpLevelEvent> WarpStoneUpLevelEventList;

//added by huchenhui 2016.08.25
struct PVPEvent : public TaskScriptEventBase
{
	PVPEvent() : type(PK_NORMAL_1V1), raceResult(PKRR_INVALID) {}

	PkType type;
	PkRaceResult raceResult;
	virtual UInt64 GetEventKey()
	{
		return TET_PVP_EVENT;
	}
};
typedef std::vector<PVPEvent> PVPEventList;

struct RetinueChangeSkillEvent : public TaskScriptEventBase
{
	UInt32 skillId;
	UInt8 skillLevel;
	virtual UInt64 GetEventKey()
	{
		return TET_RETINUE_CHANGE_SKILL_EVENT;
	}
};
typedef std::vector<RetinueChangeSkillEvent> RetinueChangeSkillEventList;

struct RetinueUpLevelEvent : public TaskScriptEventBase
{
	UInt32 retinueId;
	UInt8 retinueLevel;
	virtual UInt64 GetEventKey()
	{
		return TET_RETINUE_UP_LEVEL_EVENT;
	}
};
typedef std::vector<RetinueUpLevelEvent> RetinueUpLevelEventList;

struct SetTaskItemEvent : public TaskScriptEventBase
{
	SetTaskItemEvent() :quality(ITEM_QUALITY_INVLID), level(0), itemId(0), num(0) {}
	ItemQuality quality;
	UInt32 level;
	UInt32 itemId;
	UInt32 num;
	virtual UInt64 GetEventKey()
	{
		return TET_SET_TASK_ITEM_EVENT;
	}
};
typedef std::vector<SetTaskItemEvent> SetTaskItemEventList;

struct ActiveTaskEvent : public TaskScriptEventBase
{
	ActiveTaskEvent() :templateId(0), taskId(0) {}

	UInt32 templateId;
	UInt32 taskId;
	virtual UInt64 GetEventKey()
	{
		return TET_ACTIVE_TASK_EVENT;
	}
};
typedef std::vector<ActiveTaskEvent> ActiveTaskEventList;

struct ShopBuyEvent : public TaskScriptEventBase
{
	ShopBuyEvent() : shopId(0), shopItemId(0) {}
	ShopBuyEvent(UInt32 _shopId, UInt32 _shopItemId) :shopId(_shopId), shopItemId(_shopItemId) {}

	UInt32 shopId;
	UInt32 shopItemId;
	virtual UInt64 GetEventKey()
	{
		return TET_SHOP_BUY_EVENT;
	}
};
typedef std::vector<ShopBuyEvent> ShopBuyEventList;

struct OpenMagicJarEvent : public TaskScriptEventBase
{
	OpenMagicJarEvent() :type(JT_NONE), combo(0) {}
	OpenMagicJarEvent(JarType _type, UInt32 _combo) :type(_type), combo(_combo) {}

	JarType type;
	UInt32 combo;
	virtual UInt64 GetEventKey()
	{
		return TET_OPEN_MAGIC_JAR_EVENT;
	}
};
typedef std::vector<OpenMagicJarEvent> OpenMagicJarEventList;

struct FeedPetEvent : public TaskScriptEventBase
{
	FeedPetEvent() :type(PetFeedType::PET_FEED_INVALID) {}

	PetFeedType type;
	virtual UInt64 GetEventKey()
	{
		return TET_FEED_PET_EVENT;
	}
};
typedef std::vector<FeedPetEvent> FeedPetEventList;

struct AddPetEvent : public TaskScriptEventBase
{
	AddPetEvent() : petType(PetType::PET_TYPE_INVALID), petQuality(PetQuality::PET_QUALITY_INVALID) {}

	PetType petType;
	PetQuality petQuality;
	virtual UInt64 GetEventKey()
	{
		return TET_ADD_PET_EVENT;
	}
};
typedef std::vector<AddPetEvent> AddPetEventList;

struct PetUpLevelEvent : public TaskScriptEventBase
{
	PetUpLevelEvent() : petType(PetType::PET_TYPE_INVALID), petQuality(PetQuality::PET_QUALITY_INVALID), level(0) {}

	PetType petType;
	PetQuality petQuality;
	UInt16 level;
	virtual UInt64 GetEventKey()
	{
		return TET_PET_UP_LEVEL_EVENT;
	}
};
typedef std::vector<PetUpLevelEvent> PetUpLevelEventList;

struct RelationEvent : public TaskScriptEventBase
{
	RelationEvent() : type(RelationType(0)) {}

	RelationType type;
	virtual UInt64 GetEventKey()
	{
		return TET_RELATION_EVENT;
	}
};
typedef std::vector<RelationEvent> RelationEventList;

struct EquitScoreEvent : public TaskScriptEventBase
{
	EquitScoreEvent() : subType(ItemSubType::ST_ITEM_INVALID), quality(ItemQuality::ITEM_QUALITY_INVLID), score(0) {}

	ItemSubType subType;
	ItemQuality quality;
	UInt32 score;
	virtual UInt64 GetEventKey()
	{
		return TET_EQUIT_SCORE_EVENT;
	}
};
typedef std::vector<EquitScoreEvent> EquitScoreEventList;


struct UseItemEvent : public TaskScriptEventBase
{
	virtual UInt64 GetEventKey()
	{
		return TET_USE_ITEM;
	}
};

struct ChangeMoneyEvent : public TaskScriptEventBase
{
	virtual UInt64 GetEventKey()
	{
		return TET_CHANGE_MONEY;
	}
};

struct MovedEvent : public TaskScriptEventBase
{
	virtual UInt64 GetEventKey()
	{
		return TET_MOVED;
	}
};


/**
 *@brief task�ű� noted by aprilliu, ����һ������ű���ͨ������lua�ű���ĺ��� ʵ�������߼�������TaskScript::OnAccepted()���ǵ��ýű����function OnAccepted(player)��ʵ�ֵ�
 */
class TaskScript
	: public Avalon::ScriptFile
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::vector<UInt32>	IdVec;



public:
	TaskScript(UInt32 id, Avalon::ScriptState* state);
	~TaskScript();

public:

	const char*  GetScriptParam(){ return m_TaskParams.c_str(); }
	void SetScriptParam(std::string scriptParam) { m_TaskParams = scriptParam; };
	
	/**
	 *@brief �󶨻�õ����¼�, Ҳ����ζ��Ҫ��עɾ�������¼�(ɾ��������صĵ��ߣ���һ�����������),������������״̬�����̬��Ϊδ���״̬)
	 */
	void BindGetItemEvent(UInt32 id, UInt8 quality = 0, UInt8 strengthen = 0);
	void BindGetItemQualityEvent(UInt32 id, UInt8 quality);
	void BindGetItemStrengthenEvent(UInt32 id, UInt8 strengthen);
	bool CheckGetItemEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const;

	//added by aprilliu, 2016.04.18  �������󶨺�����ʵ�ڲ����ǵ���BindGetItemEvent��BindGetItemQualityEventʵ�ֵģ���ɾ������ʱ�� �Ƿ��ע�õ��߻��ǵ���CheckGetItemEvent��ʵ�ֵ�
	void BindRmItemEvent(UInt32 itemId);
	void BindItemChangeEvent(UInt32 itemId, UInt8 quality);
	bool CheckItemChangeEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const; //��ȫ���Ե���CheckGetItemEvent������Ƿ��ע�õ��ߣ���Ϊ�������Ĵ��⻹���¶����˸ú���

	/**
	 *@brief ��ɱ�������¼�
	 */
	void BindKillNpcEvent(UInt32 id,UInt32 dungeon);
	void BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon);
	void BindKillLevelNpcEvent();
	bool IsBindKillLevelNpcEvent() const;
	bool CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const;

	void BindKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId);
	bool CheckKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid) const;

	/**
	 *@brief ��ʹ�õ����¼�
	 */
	void BindUseItemEvent(UInt32 id);
	bool CheckUseItemEvent(UInt32 id) const;

	/**
	*@brief ��������Ϸ���¼�
	*/
	void BindChangeMoneyEvent(UInt32 type);
	bool CheckChangeMoneyEvent(UInt32 type) const;

	/**
	 *@brief ��װ�������¼�
	 */
	void BindRemakeEquipEvent(UInt32 id, UInt8 type);
	bool CheckRemakeEquipEvent(UInt32 id, UInt8 type) const;

	/**
	*@brief ��ǿ��װ���¼� adder by huchenhui 2016.06.24
	*/
	void BindStrengthenEquipEvent(bool isSuccess = false, UInt8 strengthen = 0);
	bool CheckStrengthenEquipEvent(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen) const;

	/**
	*@brief �󶨷ֽ�װ���¼�  adder by huchenhui 2016.06.24
	*/
	void BindDecomposeEquipEvent();
	bool CheckDecomposeEquipEvent() const;

	/**
	*@brief ����װ���¼�  adder by huchenhui 2016.06.29
	*/
	void BindPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen);
	bool CheckPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen) const;

	/**
	*@brief ����ʱװ�¼�  adder by huchenhui 2018.03.20
	*/
	void BindPutFashionEvent(ItemSubType subType, UInt8 color, UInt8 isTransparent = 0);
	bool CheckPutFashionEvent(ItemSubType subType, UInt8 color, UInt8 isTransparent) const;

	/**
	 *@brief �󶨻��ͳɹ��¼�
	 */
	void BindEscortSuccedEvent(UInt32 id);
	bool CheckEscortSuccedEvent(UInt32 id) const;
	const IdVec& GetEscortSuccedEvents() const{ return m_EscortSuccedEvents; }

	/**
	 *@brief ���������
	 */
	void BindTriggerZone(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
	bool CheckInTriggerZone(UInt32 mapid, const CLPosition& pos);
	const std::vector<CLZone>& GetTriggerZones() const { return m_TriggerZones; }

	void BindEnterZoneEvent(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
	bool CheckInEventZone(UInt32 mapid, const CLPosition& pos);

	/**
	 *@brief ��������
	 */
	void BindLevelupEvent(UInt16 level){ m_LevelupEvent = level; }
	bool CheckLevelupEvent(UInt16 level){ return m_LevelupEvent != 0 && level >= m_LevelupEvent; }

	/**
	 *@brief �󶨲�׽�����б�
	 */
	void BindCatchPetEvent(UInt32 id);
	bool CheckCatchPetEvent(UInt32 id) const;

	/**
	*@brief ��������
	*/
	void BindAddRetinueEvent(UInt32 quality = 0);
	bool CheckAddRetinueEvent(RetinueQuality quality) const;

	/**
	*@brief �����ϴ���¼�
	*/
	void BindRetinueChangeSkillEvent(UInt32 buffId = 0);
	bool CheckRetinueChangeSkillEvent(UInt32 buffId) const;

	/**
	*@brief ����������¼�
	*/
	void BindRetinueUpLevelSkillEvent(UInt32 id, UInt8 level);
	bool CheckRetinueUpLevelSkillEvent(UInt32 id, UInt8 level);

	/**
	*@brief ����������¼�
	*/
	void BindRetinueUpStarEvent(UInt32 id, UInt8 star);
	bool CheckRetinueUpStarEvent(UInt32 id, UInt8 star);

	/**
	 *@brief �󶨸���ͨ���¼�(���³�ͨ��)
	 */
	void BindCopyScenePassedEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	//added by aprilliu, �ڲ�����BindCopyScenePassedEvent
	//m by huchenhui 2016.08.22	����score����ʱ�޶�
	void BindClearDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	void BindClearDungeonEventByRevive(UInt32 dungeonId, UInt32 reviveCount);
	bool CheckCopyScenePassedEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief �󶨸���ͨ���¼�(���³�ͨ��,��ע����)
	*/
	void BindClearDungeonEventCareScore(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckCopyScenePassedCareScoreEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief ����ӵ��³�ͨ���¼�
	*/
	void BindTeamClearDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckTeamClearDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief �󶨻����ͨ���¼�(����³�ͨ��)
	*/
	void BindClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief ����ӻ����ͨ���¼�(����³�ͨ��)
	*/
	void BindTeamClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckTeamClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	 *@brief ��ͨ����ͨ���¼�
	 */
	void BindBabelPassedEvent(UInt32 id);
	bool CheckBabelPassedEvent(UInt32 id) const;

	/**
	*@brief �󶨹����̳ǵ����¼�
	*/
	void BindBuyMallItemEvent(UInt32 mallItemid);
	bool CheckBuyMallItemEvent(UInt32 mallItemid) const;

	/**
	 *@brief �󶨼��벿���¼�
	 */
	void BindEnterTribeEvent(){ m_bEnterTribeEvnet = true; }
	bool CheckEnterTribeEvent() const{ return m_bEnterTribeEvnet; }

	/**
	 *@brief  �󶨾�������¼�                                                                    
	 */
	void BindStoryEndEvent(UInt32 id);
	bool CheckStoryEndEvent(UInt32 id) const;

	/**
	 *@brief ������ɹ��¼�                                                                     
	 */
	void BindTaskSuccedEvent(UInt32 id){ m_TaskSuccId = id; }
	bool CheckTaskSuccedEvent(UInt32 id) const{ return m_TaskSuccId == id; }

	//added by aprilliu, 2016.04.18 �������ύ�ɹ����¼�
	void BindSubmitTaskSucceedEvent(UInt32 taskId);
	bool CheckSubmitTaskSucceedEvent(UInt32 taskId) const;

	/**
	*@brief �󶨻����ɹ��¼�
	*/
	void BindSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId);
	bool CheckSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId);

	/**
	*@brief ��ѭ������ɹ��¼�
	*/
	void BindSubmitCycleTaskSucceedEvent(UInt32 taskId);
	bool CheckSubmitCycleTaskSucceedEvent(UInt32 taskId);

	/**
	 *@brief ������ʧ���¼�
	 */
	void BindTaskFailedEvent(UInt32 id){ m_TaskFailedId = id; }
	bool CheckTaskFailedEvent(UInt32 id) const{ return m_TaskFailedId == id; } 

	/**
	 *@brief �������¼�
	 */
	void BindDiedEvent(){ m_bDiedEvent = true; }
	bool CheckDiedEvent() const{ return m_bDiedEvent; } 

	/**
	 *@brief ���ճ������ύ�¼�
	 */
	void BindDailyTaskSubmittedEvent(UInt32 taskId);
	bool CheckDailyTaskSubmittedEvent(UInt32 taskId) const;

	/**
	*@brief ����ʱ�������
	*/
	void BindTimeEvent(UInt32 uCompleteTime) { m_uCompleteTime = uCompleteTime; }
	UInt32 GetTimeEvent() const { return m_uCompleteTime; }

	/** 
	 *@brief ��ȡ������
	 */
	UInt32 GetTaskExp(Player* player, UInt8 quality);
	RewardGroup* GetTaskRewards(Player* player, UInt8 quality); 

	/**
	*@brief ��ͳ����
	*/
	void BindPVPEvent(PkType type, PkRaceResult raceResult = PKRR_INVALID);
	bool CheckPVPEvent(PkType type, PkRaceResult raceResult) const;

	/**
	*@brief תְ
	*/
	void BindChangeOccuEvent();
	bool CheckChangeOccuEvent() const;

	/**
	*@brief ��ħ
	*/
	void BindAddMagicEvent();
	bool CheckAddMagicEvent() const;

	/**
	*@brief �ϳɸ�ħ��
	*/
	void BindAddMagicCardEvent();
	bool CheckAddMagicCardEvent() const;

	/**
	*@brief ������
	*/
	void BindOpenMagicJarEvent(JarType type = JarType::JT_NONE, UInt32 combo = 0);
	bool CheckOpenMagicJarEvent(JarType type, UInt32 combo) const;

	/**
	*@brief ��������
	*/
	void BindFriendGiveEvent();
	bool CheckFriendGiveEvent() const;

	/**
	*@brief ��Ԫʯ����
	*/
	void BindWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level);
	bool CheckWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level) const;

	/**
	*@brief ��Ԫʯ����
	*/
	void BindWarpStoneAddEnergyEvent();
	bool CheckWarpStoneAddEnergyEvent() const;

	/**
	*@brief �̵�ˢ��
	*/
	void BindRefreshShopEvent();
	bool CheckRefreshShopEvent() const;

	/**
	*@brief �̵깺��
	*/
	void BindShopBuyEvent(UInt32 shopId, UInt32 itemId);
	bool CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const;

	/**
	*@brief ����ƣ��
	*/
	void BindRemoveFatigueEvent(UInt16 fatigue);
	bool CheckRemoveFatigueEvent(UInt16 fatigue) const;

	/**
	*@brief ��ʼ��Ԩ
	*/
	void BindBeginHellEvent(UInt32 id);
	bool CheckBeginHellEvent(UInt32 id);

	/**
	*@brief ͨ����Ԩ
	*/
	void BindClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score = LevelScore::LS_C);
	bool CheckClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief ���ͨ����Ԩ
	*/
	void BindTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score = LevelScore::LS_C);
	bool CheckTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief ����֮����ʼ
	*/
	void BindDeathTowerBeginEvent(UInt32 towerId);
	bool CheckDeathTowerBeginEvent(UInt32 towerId);

	/**
	*@brief ����֮����ʼɨ��
	*/
	void BindDeathTowerWipeoutBeginEvent();
	bool CheckDeathTowerWipeoutBeginEvent();

	/**
	*@brief ����֮��
	*/
	void BindDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime);
	bool CheckDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief ����
	*/
	void BindAwakenEvent();
	bool CheckAwakenEvent();

	/**
	*@brief ʹ�ø����
	*/
	void BindReviveCoinEvent();
	bool CheckReviveCoinEvent();

	/**
	*@brief ������ �ϼ�
	*/
	void BindAddAuctionEvent();
	bool CheckAddAuctionEvent();

	/**
	*@brief ������ �۳�
	*/
	void BindSellAuctionEvent();
	bool CheckSellAuctionEvent();

	/**
	*@brief ������ ����
	*/
	void BindBuyAuctionEvent();
	bool CheckBuyAuctionEvent();

	/**
	*@brief �ύ������Ʒ
	*/
	void BindSetTaskItemEvent(ItemQuality quality, UInt32 level);
	void BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num);
	bool CheckSetTaskItemEvent(ItemQuality quality, UInt32 level, UInt32 itemId, UInt32& num);

	/**
	*@brief ιʳ����
	*/
	void BindFeedPetEvent(PetFeedType type);
	bool CheckFeedPetEvent(PetFeedType type);

	/**
	*@brief ��ó���
	*/ 
	void BindAddPetEvent(PetType petType, PetQuality petQuality);
	bool CheckAddPetEvent(PetType petType, PetQuality petQuality);

	/**
	*@brief ��������
	*/
	void BindPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level);
	bool CheckPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief ���ӹ�ϵ
	*/
	void BindAddRelationEvent(RelationType type);
	bool CheckAddRelationEvent(RelationType type);

	/**
	*@brief ������
	*/
	void BindSendHornEvent();
	bool CheckSendHornEvent();

	/**
	*@brief װ������
	*/
	void BindCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score);
	bool CheckCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief ��Ƕ����
	*/
	void BindAddPreciousBeadEvent();
	bool CheckAddPreciousBeadEvent();

	/**
	*@brief ���빤��
	*/
	void BindJoinGuildEvent();
	bool CheckJoinGuildEvent();

	/**
	*@brief ���ӹ������
	*/
	void BindIncGuildBattleScoreEvent();
	bool CheckIncGuildBattleScoreEvent();

	/**
	*@brief ռ�����
	*/
	void BindGuildOccupyTerritoryEvent(UInt32 terrId);
	bool CheckGuildOccupyTerritoryEvent(UInt32 terrId);

	/**
	*@brief ������
	*/
	void BindWinWudaodahuiPkEndEvent(UInt32 winNum);
	bool CheckWinWudaodahuiPkEndEvent(UInt32 winNum);

	/**
	*@brief ��չ����
	*/
	void BindEnlargePackageEvent();
	bool CheckEnlargePackageEvent();

	/**
	*@brief ��չ�ֿ�
	*/
	void BindEnlargeStorageEvent();
	bool CheckEnlargeStorageEvent();

	/**
	*@brief  ���Ѷ�����
	*/
	void BindIncIntimacyEvent();
	bool CheckIncIntimacyEvent();

	/**
	*@brief  ��������ճ��������(��Ծ��)
	*/
	void BindIncDailyTaskScoreEvent();
	bool CheckIncDailyTaskScoreEvent();

	/**
	*@brief  ��ͽ
	*/
	void BindApprentDiscipleEvent();
	bool CheckApprentDiscipleEvent();

	/**
	*@brief ͽ�ܳ�ʦ
	*/
	void BindDiscipleFinschEvent();
	bool CheckDiscipleFinschEvent();

	/**
	*@brief �����ռ�����
	*/
	void BindGuildEmblemEvent();
	bool CheckGuildEmblemEvent();

	/**
	 *@brief ͨ���ű�
	 */
	void BindClearTeamCopyEvent();
	bool CheckClearTeamCopyEvent();

public: //�ű��¼�
	/**
	 *@brief ��ʼ��
	 */
	void Init();

	/**
	 *@brief ����Ƿ�ɽ�
	 */
	bool CheckAccept(Player* player);

	/**
	 *@brief �ӵ���������
	 */
	void OnAccepted(Player* player, BaseTask* task);

	/**
	 *@brief �ύ����
	 */
	void OnSubmitted(Player* player, BaseTask* task);

	/**
	 *@brief ȡ������
	 */
	void OnAbandomed(Player* player, BaseTask* task);

	/**
	*@brief ��������Ƿ����
	*/
	void CheckFinish(Player* player, BaseTask* task);

	/**
	 *@brief ��õ���
	 */
	void OnGetItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task);

	/**
	*@brief ɾ������   ���õ���������OnGetItem,OnRmItem �ϲ���OnChangeItem
	*/
	void OnRmItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task);

	/**
	 *@brief ʹ�õ���
	 */
	void OnUseItem(Player* player, UInt32 itemid, BaseTask* task);

	/**
	*@brief ��Ϸ�ұ仯�¼�  added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(Player* player, UInt32 type, Int32 num, BaseTask* task);

	/**
	 *@brief ɱ��npc�¼�
	 */
	void OnKillNpc(Player* player, UInt32 npcid, UInt8 type, BaseTask* task, UInt32 dungeonId);

	/**
	*@brief ɱ������ָ������
	*/
	void OnKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid);

	/**
	*@brief ɱ������ָ������
	*/
	void OnTeamKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds);

	/**
	 *@brief ���������
	 */
	void OnEnterZone(Player* player);

	/**
	 *@brief ����װ��
	 */
	void OnRemakeEquip(Player* player, BaseTask* task, UInt32 id, UInt8 type);

	/**
	*@brief ����װ������
	*/
	void OnCalculateValuedScore(Player* player, BaseTask* task, ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief ��Ƕ����
	*/
	void OnAddPreciousBead(Player* player, BaseTask* task);

	/**
	*@brief ǿ��װ�� Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(Player* player, BaseTask* task, UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen);

	/**
	*@brief �ֽ�װ�� Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip(Player* player, BaseTask* task);

	/**
	*@brief ����װ�� Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(Player* player, BaseTask* task, UInt16 level, UInt32 subType, UInt32 strength);

	/**
	*@brief ����ʱװ Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(Player* player, BaseTask* task, ItemSubType subType, UInt32 color);

	/**
	 *@brief �������
	 */
	void OnCatchPet(Player* player, UInt32 id);

	/**
	*@brief ������  added by huchenhui 2016.08.04
	*/
	void OnAddRetinue(Player* player, BaseTask* task, RetinueQuality quality);

	/**
	*@brief ���ϴ��  added by huchenhui 2016.08.20
	*/
	void OnRetinueChangeSkill(Player* player, BaseTask* task, UInt32 buffId);

	/**
	*@brief �������  added by huchenhui 2016.11.08
	*/
	void OnRetinueUpLevel(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueLevel);

	/**
	*@brief �������  added by huchenhui 2016.12.07
	*/
	void OnRetinueUpStar(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueStar);

	/**
	 *@brief ���ͳɹ�
	 */
	void OnEscortSucced(Player* player, Npc* npc);

	/**
	 *@brief ����ʧ��
	 */
	void OnEscortFailed(Player* player, UInt32 npcId);

	/**
	 *@brief ����Ŀ�걻ɱ��
	 */
	void OnEscortTargetKilled(Npc* npc, Player* killer);

	/**
	 *@brief �������
	 */
	void OnPlayerLevelup(Player* player, BaseTask* task);

	/**
	 *@brief ����ͨ��
	 */
	void OnCopyScenePassed(Player* player, UInt32 id, BaseTask* task);

	//added by aprilliu. 2016.04.14. 
	//���³�(����)ͨ�غ�Ļص�. dungeonId - ���³�id(�����Ѷ���Ϣ), beated - ��������, areas - ͨ�ط�����
	void OnClearDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas);

	//����³�(����)ͨ�غ�Ļص�. dungeonId - ���³�id(�����Ѷ���Ϣ), beated - ��������, areas - ͨ�ط�����
	void OnClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas);

	/**
	*@brief ���ͨ�ص��³�
	*/
	void OnTeamClearDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���ͨ�ػ���³�
	*/
	void OnTeamClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���ͨ����Ԩ���³�
	*/
	void OnTeamClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief ���pvp
	*/
	void OnTeamPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status, std::vector<ObjID_t>& teammateIds);

	/**
	 *@brief ͨ����ͨ��
	 */
	void OnBabelPassed(Player* player, UInt32 id);

	/**
	*@brief �̳ǹ������
	*/
	void OnBuyMallItem(Player *pPlayer, BaseTask* pTask, UInt32 mallItemId, UInt32 OnBuyMallItem);

	/**
	 *@brief ���벿��
	 */
	void OnEnterTribe(Player* pPlayer);

	/**
	 *@brief �������
	 */
	void OnStoryEnd(Player* player, UInt32 id, BaseTask* task);

	/**
	 *@brief ����ʱ
	 */
	void OnTimeout(Player* player);

	/**
	 *@brief ����ɹ��¼�
	 */
	void OnTaskSucced(Player* player, UInt32 taskId);

	//added by aprilliu, 2016.04.18 �ɹ��ύ�����¼�
	void OnSubmitTaskSucceed(Player* player, UInt32 taskId, BaseTask* task);

	/**
	*@brief �����ɹ��¼� by huchenhui
	*/
	void OnSubmitActiveTaskSucceed(Player* player, BaseTask* task, UInt32 templateId, UInt32 taskId);

	/**
	*@brief ѭ������ɹ��¼� by huchenhui
	*/
	void OnSubmitCycleTaskSucceed(Player* player, BaseTask* task, UInt32 taskId);

	/**
	 *@brief ����ʧ���¼�
	 */
	void OnTaskFailed(Player* player, UInt32 taskId);

	/**
	 *@brief �����¼�
	 */
	void OnOwnerDied(Player* player);

	/**
	 *@brief �ճ������ύ
	 */
	void OnDailyTaskSubmitted(Player* player,BaseTask* task, UInt8 type);

	/**
	*@brief PVP�����ύ
	*/
	void OnPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status);

	/**
	*@brief תְ�¼�
	*/
	void OnChangeOccu(Player* player, BaseTask* task);

	/**
	*@brief ��ħ�¼�
	*/
	void OnAddMagic(Player* player, BaseTask* task);

	/**
	*@brief �ϳɸ�ħ��
	*/
	void OnAddMagicCard(Player* player, BaseTask* task);

	/**
	*@brief �������¼�
	*/
	void OnOpenMagicJar(Player* player, BaseTask* task, JarType type, UInt32 combo);
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	/**
	*@brief ���������¼�
	*/
	void OnFriendGive(Player* player, BaseTask* task);

	/**
	*@brief ��Ԫʯ�����¼�
	*/
	void OnWarpStoneUpLevel(Player* player, BaseTask* task, WarpStoneType type, UInt32 level);

	/**
	*@brief ��Ԫʯ�����¼�
	*/
	void OnWarpStoneAddEnergy(Player* player, BaseTask* task, UInt32 energy);

	/**
	*@brief ˢ���̵��¼�
	*/
	void OnRefreshShop(Player* player, BaseTask* task);

	/**
	*@brief �̵깺���¼�
	*/
	void OnShopBuy(Player* player, BaseTask* pTask, UInt32 shopId, UInt32 shopItemId, UInt32 num);

	/**
	*@brief ����ƣ��ֵ�¼�
	*/
	void OnRemoveFatigue(Player* player, BaseTask* task, UInt16 usedFatigue);

	/**
	*@brief ��ʼ��Ԩ�¼�
	*/
	void OnBeginHell(Player* player, BaseTask* task, UInt32 dungeonId);

	/**
	*@brief ͨ����Ԩ�¼�
	*/
	void OnClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime);

	/**
	*@brief ����֮����ʼ�¼�
	*/
	void OnDeathTowerBegin(Player* player, BaseTask* task, UInt32 towerId);

	/**
	*@brief ����֮��ɨ����ʼ�¼�
	*/
	void OnDeathTowerWipeoutBegin(Player* player, BaseTask* task);

	/**
	*@brief ����֮���¼�
	*/
	void OnDeathTower(Player* player, BaseTask* task, UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief �����¼�
	*/
	void OnAwaken(Player* player, BaseTask* task, AwakenStatus status);

	/**
	*@brief �����
	*/
	void OnReviveCoin(Player* player, BaseTask* task, bool isSelf);

	/**
	*@brief ������ �ϼ�
	*/
	void OnAddAuction(Player* player, BaseTask* task);

	/**
	*@brief ������ �۳�
	*/
	void OnSellAuction(Player* player, BaseTask* task, UInt32 money);

	/**
	*@brief ������ ����
	*/
	void OnBuyAuction(Player* player, BaseTask* task, UInt32 money);

	/**
	*@brief �ύ������Ʒ
	*/
	void OnSetTaskItem(Player* player, BaseTask* task, Item* item, UInt32 itemId, UInt32 num);

	/**
	*@brief ����ιʳ
	*/
	void OnFeedPet(Player* player, BaseTask* task, PetFeedType type);

	/**
	*@brief ��ó���
	*/
	void OnAddPet(Player* player, BaseTask* task, PetType petType, PetQuality petQuality);

	/**
	*@brief ��������
	*/
	void OnPetUpLevel(Player* player, BaseTask* task, PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief ����������ϵ
	*/
	void OnAddRelation(Player* player, BaseTask* task, RelationType type);

	/**
	*@brief ������
	*/
	void OnSendHorn(Player* player, BaseTask* task);

	/**
	*@brief ���빤��
	*/
	void OnJoinGuild(Player* player, BaseTask* task);

	/**
	*@brief ���ӹ������
	*/
	void OnIncGuildBattleScore(Player* player, BaseTask* task, UInt32 score);

	/**
	*@brief ռ�����
	*/
	void OnGuildOccupyTerritory(Player* player, BaseTask* task, UInt32 terrId);

	/**
	*@brief ������
	*/
	void OnWinWudaodahuiPkEnd(Player* player, BaseTask* task, UInt32 winNum);

	/**
	*@brief ��չ����
	*/
	void OnEnlargePackage(Player* player, BaseTask* task, UInt32 size);

	/**
	*@brief ��չ�ֿ�
	*/
	void OnEnlargeStorage(Player* player, BaseTask* task, UInt32 size);

	/**
	*@brief ���Ѷ�
	*/
	void OnIncIntimacy(Player* player, ObjID_t targetId, BaseTask* task, UInt32 incIntimacy, UInt32 finalIntimacy);

	/**
	*@brief ��������ճ��������(��Ծ��)
	*/
	void OnIncDailyTaskScore(Player* player, BaseTask* task, UInt32 totalDailyTaskScore);

	/**
	*@brief ��ͽ�¼�
	*/
	void OnApprentDisciple(Player* player, BaseTask* task);

	/**
	*@brief ͽ�ܳ�ʦ�¼�
	*/
	void OnDiscipleFinsch(Player* player, BaseTask* task);

	/**
	 *@brief ����ռ�����
	 */
	void OnGuildEmblemUp(Player* player, BaseTask* task, UInt32 lvl);

	/**
	 *@brief ���³�(����)ͨ�غ�Ļص�. dungeonId - ���³�id(�����Ѷ���Ϣ), score - ����
	 */
	void OnClearDungeonCareScore(Player* player, BaseTask* task, UInt32 dungeonId, LevelScore score);

	/**
	 *@brief ͨ���ű�
	 */
	void OnClearTeamCopy(Player* player, BaseTask* task, UInt32 teamType, UInt32 grade, UInt32 costTime);

private:
	std::string m_TaskParams;

	//��ȡ�����¼��б�
	GetItemEventVec	m_getItemEvents;
	//ɱ��npc�¼��б�
	KillNPCEventList	m_killNpcEvents;
	//ɱ����������¼��б�
	KillCityMonsterEventList	m_KillCityMonsterEvents;
	//ɱ����npc�ȼ�����
	bool	m_bLevelNpcCond;
	//ʹ�õ����¼��б�
	IdVec	m_useItemEvents;
	//��Ϸ���¼��б�
	IdVec	m_ChangeMoneyEvents;
	//����װ���¼��б�
	EquipRemakeEventVec	m_remakeEquipEvents;
	//��׽�����¼��б�
	IdVec	m_catchPetEvents;
	//���ͳɹ��¼��б�
	IdVec	m_EscortSuccedEvents;
	//����ͨ���¼��б�
	ClearDungeonEventList	m_CopyScenePassedEvents;
	//��ʼ��Ԩ�¼��б�		added by huchenhui
	IdVec	m_BeginHellEvents;
	//ͨ����Ԩ�¼��б�		added by huchenhui
	ClearDungeonEventList	m_ClearHellEvents;
	//ͨ�ػ���³��¼��б�
	ClearDungeonEventList	m_ClearActivityDungeonEvents;
	//���ͨ�ظ����¼��б�
	ClearDungeonEventList	m_TeamCopyScenePassedEvents;
	//���ͨ����Ԩ�¼��б�
	ClearDungeonEventList	m_TeamClearHellEvents;
	//���ͨ�ػ���³��¼��б�
	ClearDungeonEventList	m_TeamActivityDungeonEvents;

	//����ͨ���¼��б�(��ע����)
	ClearDungeonEventList	m_CopyScenePassedCareScoreEvents;

	//ͨ����ͨ��ʱ���б�
	IdVec	m_BabelPassedEvents;
	//�����̳ǵ����¼��б�
	IdVec	m_buyMallItemEvents;
	//��������¼��б�
	IdVec	m_StoryEndEvents;
	//�ճ������¼��б�
	IdVec	m_DailyTaskSubmittedEvents;
	//�Ƿ��PVP�¼�  added by huchenhui 2016.08.05
	PVPEventList m_PVPEvent;


	//����װ�����¼�		added huchenhui 2016.06.29
	PutEquipEventList	m_PutEquipEvents;

	//����ʱװ���¼�		added huchenhui 2018.03.20
	PutFashionEventList m_PutFashionEvents;

	//ʯͷ����		added huchenhui 2016.08.23
	WarpStoneUpLevelEventList m_WarpStoneUpLevelEvents;

	//����֮��	added by huchenhui
	DeathTowerEventList m_DeathTowerEvents;

	//����֮����ʼ	added by huchenhui
	IdVec	m_DeathTowerBeginEvents;

	//����֮��ɨ����ʼ  added by huchenhui
	bool	m_bDeathTowerWipeoutBeginEvent;

	//��ɻ����
	ActiveTaskEventList	m_SubmitActiveTaskSucceedEvents;

	//�̵깺��	added by huchenhui
	ShopBuyEventList	m_ShopBuyEvents;

	//���ѭ������
	IdVec	m_SubmitCycleTaskSucceedEvents;

	//�����ϴ���¼�	added by huchenhui
	IdVec	m_RetinueChangeSkillEvents;

	//�Ƿ����������¼�	added by huchenhui
	RetinueUpLevelEventList m_bRetinueUpLevelEvents;

	//�Ƿ����������¼�	added by huchenhui
	RetinueUpLevelEventList m_RetinueUpStarEvents;

	//�Ͻ�������Ʒ
	SetTaskItemEventList m_SetTaskItemEvents;

	//ǿ���¼�	added by huchenhui
	StrengthenEquipEvent m_StrengthenEquipEvent;

	//��������
	std::vector<CLZone>	m_TriggerZones;
	//������
	std::vector<CLZone>	m_BindZones;

	//ιʳ����
	FeedPetEventList m_FeedPetEvents;

	//��ó���
	AddPetEventList m_AddPetEvents;

	//��������
	PetUpLevelEventList m_PetUpLevelEvents;

	//��ҹ�ϵ
	RelationEventList m_RelationEvents;

	//�����¼��ȼ�
	UInt16	m_LevelupEvent;

	//�Ƿ�󶨽��벿���¼�
	bool	m_bEnterTribeEvnet;

	//������ɹ��¼�
	UInt32	m_TaskSuccId;
	//������ʧ���¼�
	UInt32	m_TaskFailedId;

	//added by aprilliu, 2016.04.18
	IdVec m_SubmitTaskSucceedIdList;

	//�Ƿ�������¼�
	bool	m_bDiedEvent;

	//���ճ������ύ�¼�
	UInt8	m_TaskSubmittedType;

	//����ʱ�����������ʱ�䣨�룩
	UInt32	m_uCompleteTime;

	//��ʹ�õ�ƣ��ֵ	added by huchenhui
	UInt16	m_uUsedFatigue;

	//װ������		added by huchenhui
	EquitScoreEventList m_EquipScoreEventList;

	//�Ƿ񿪹����¼�		added by huchenhui
	OpenMagicJarEventList m_OpenMagicJarComboEventList;

	//�Ƿ���������¼�	added by huchenhui
	RetinueQuality m_AddRetinueEvent;

	//������				added by huchenhui
	UInt32 m_uWudaodahuiWinNumEvent;

	//ռ�����					added by huchenhui
	IdVec m_GuildOccupyTerritoryEvent;

	//�Ƿ��װ��ǿ���¼�	added by huchenhui
	bool m_bStrengthenEquipEvent;

	//�Ƿ��װ���ֽ��¼�	added by huchenhui
	bool m_bDecomposeEquipEvent;

	//�Ƿ��תְ�¼�		added by huchenhui
	bool m_bChangeOccuEvent;

	//�Ƿ�󶨸�ħ�¼�		added by huchenhui
	bool m_bAddMagicEvent;

	//�Ƿ�󶨺ϳɸ�ħ���¼�	added by huchenhui
	bool m_bAddMagicCardEvent;

	//�Ƿ���������¼�		added by huchenhui
	bool m_bFriendGiveEvent;

	//�Ƿ�ˢ���̵��¼�		added by huchenhui
	bool m_bRefreshShopEvent;

	//�Ƿ�����¼�			added by huchenhui
	bool m_bAwakenEvent;

	//��Ԫʯ����			added by huchenhui
	bool m_bAddEnergyEvent;

	//�����				added by huchenhui
	bool m_bReviveCoinEvent;

	//������ �ϼ�			added by huchenhui
	bool m_bAddAuctionEvent;

	//������ �۳�			added by huchenhui
	bool m_bSellAuctionEvent;

	//������ ����			added by huchenhui
	bool m_bBuyAuctionEvent;

	//��Ƕ����				added by huchenhui
	bool m_bAddPreciousBeadEvent;

	//������				added by huchenhui
	bool m_bSendHornEvent;

	//���빤��				added by huchenhui
	bool m_bJoinGuildEvent;

	//���ӹ���ս����		added by huchenhui
	bool m_bIncGuildBattleEvent;

	//��չ����				added by huchenhui
	bool m_bEnlargePackageEvent;

	//��չ�ֿ�				added by huchenhui
	bool m_bEnlargeStorageEvent;
	
	//���ܶ�����
	bool m_bIncIntimacyEvent;
	
	//��������ճ��������(��Ծ��)
	bool m_bIncIDailyTaskScoreEvent;

	//��ͽ
	bool m_bApprentDiscipleEvent;

	//ͽ�ܳ�ʦ
	bool m_bDiscipleFinschEvent;

	//����ռ�����
	bool m_bGuildEmblemEvent;

	//�ű�ͨ���¼�
	bool m_bClearTeamCopyEvent;
};


#endif
