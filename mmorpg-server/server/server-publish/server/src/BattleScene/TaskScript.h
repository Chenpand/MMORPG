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


class Player;
class Npc;
class RewardGroup;
class BaseTask;
class Item;

/**
 *@brief task脚本 noted by aprilliu, 代表一个任务脚本，通过调用lua脚本里的函数 实现任务逻辑，比如TaskScript::OnAccepted()就是调用脚本里的function OnAccepted(player)来实现的
 */
class TaskScript
	: public Avalon::ScriptFile
{
	typedef std::vector<ItemReward> ItemRewardVec;
	typedef std::vector<UInt32>	IdVec;

	struct EquipRemakeEvent
	{
		EquipRemakeEvent():id(0), remakeType(0){}
		UInt32	id;
		UInt8	remakeType;
	};
	typedef std::vector<EquipRemakeEvent>	EquipRemakeEventVec;

	struct GetItemEvent
	{
		GetItemEvent() : id(0), quality(0), strengthen(0) {}
		UInt32	id;
		UInt8	quality;
		UInt8	strengthen;
	};
	typedef std::vector<GetItemEvent>		GetItemEventVec;

	//added by aprilliu.2016.04.13
	struct stKillNpcEvent //击杀怪物的事件
	{
		stKillNpcEvent() :id(0), type(0), dungeon(0)
		{
		}

		UInt32	id;				//怪物id
		UInt8	type;			//怪物类型
		UInt32  dungeon;		//怪物所属的地下城id   added by aprilliu,2016.05.27
	};
	typedef	std::vector<stKillNpcEvent>		KillNPCEventList;

	//击杀城镇怪物类型
	struct KillCityMonsterEvent
	{
		KillCityMonsterEvent(CityMonsterType _type, UInt32 _monsterId) :type(_type), monsterId(_monsterId) {}

		CityMonsterType type;	//城镇怪物类型
		UInt32 monsterId;		//城镇怪物ID
	};
	typedef std::vector<KillCityMonsterEvent>	KillCityMonsterEventList;

	//added by huchenhui 2016.09.02
	struct StrengthenEquipEvent		//强化事件
	{
		StrengthenEquipEvent() :isRegister(false), isSuccess(false), strengthen(0), isTicket(false){}

		bool isRegister;	//是否关注
		bool isSuccess;		//是否只关注成功
		UInt8 strengthen;	//关注的强化等级
		bool isTicket;		//是否使用强化卷
	};

	//added by huchenhui 2016.06.29
	struct PutEquipEvent
	{
		PutEquipEvent() :level(0), strengthen(0), color(0) {}

		UInt16 level;			//需求等级
		UInt32 strengthen;		//强化等级
		UInt8 color;			//装备品质
	};
	typedef std::vector<PutEquipEvent>	PutEquipEventList;

	//added by huchenhui 2018.03.20
	struct PutFashionEvent
	{
		PutFashionEvent(ItemSubType _subType, UInt8 _color):subType(_subType), color(_color) { }

		ItemSubType subType;		//子类型
		UInt8 color;				//品质
	};
	typedef std::vector<PutFashionEvent> PutFashionEventList;

	//added by huchenhui 2016.08.22
	struct ClearDungeonEvent
	{
		ClearDungeonEvent() : id(0), score(LS_C), usedTime(0), reviveCount(0){}

		UInt32 id;				//副本ID
		LevelScore score;		//关卡评分
		UInt32 usedTime;		//用时
		UInt32 reviveCount;		//复活次数
	};
	typedef std::vector<ClearDungeonEvent> ClearDungeonEventList;

	struct DeathTowerEvent
	{
		DeathTowerEvent() : id(0), layer(0), usedTime(0){}

		UInt32 id;				//死亡之塔ID
		UInt32 layer;			//层数
		UInt32 usedTime;		//用时
	};
	typedef std::vector<DeathTowerEvent> DeathTowerEventList;

	//added by huchenhui 2016.08.23
	struct WarpStoneUpLevelEvent
	{
		WarpStoneUpLevelEvent() : type(STONE_INVALID), level(0){}

		WarpStoneType type;
		UInt32 level;
	};
	typedef std::vector<WarpStoneUpLevelEvent> WarpStoneUpLevelEventList;

	//added by huchenhui 2016.08.25
	struct PVPEvent
	{
		PVPEvent() : type(PK_NORMAL_1V1), raceResult(PKRR_INVALID) {}

		PkType type;
		PkRaceResult raceResult;
	};
	typedef std::vector<PVPEvent> PVPEventList;

	struct RetinueChangeSkillEvent
	{
		UInt32 skillId;
		UInt8 skillLevel;
	};
	typedef std::vector<RetinueChangeSkillEvent> RetinueChangeSkillEventList;

	struct RetinueUpLevelEvent
	{
		UInt32 retinueId;
		UInt8 retinueLevel;
	};
	typedef std::vector<RetinueUpLevelEvent> RetinueUpLevelEventList;

	struct SetTaskItemEvent
	{
		SetTaskItemEvent() :quality(ITEM_QUALITY_INVLID), level(0), itemId(0), num(0){}
		ItemQuality quality;
		UInt32 level;
		UInt32 itemId;
		UInt32 num;
	};
	typedef std::vector<SetTaskItemEvent> SetTaskItemEventList;

	struct ActiveTaskEvent
	{
		ActiveTaskEvent() :templateId(0), taskId(0){}

		UInt32 templateId;
		UInt32 taskId;
	};
	typedef std::vector<ActiveTaskEvent> ActiveTaskEventList;

	struct ShopBuyEvent
	{
		ShopBuyEvent() : shopId(0), shopItemId(0){}
		ShopBuyEvent(UInt32 _shopId, UInt32 _shopItemId) :shopId(_shopId), shopItemId(_shopItemId){}

		UInt32 shopId;
		UInt32 shopItemId;
	};
	typedef std::vector<ShopBuyEvent> ShopBuyEventList;

	struct OpenMagicJarEvent
	{
		OpenMagicJarEvent() :type(JT_NONE), combo(0){}
		OpenMagicJarEvent(JarType _type, UInt32 _combo) :type(_type), combo(_combo){}

		JarType type;
		UInt32 combo;
	};
	typedef std::vector<OpenMagicJarEvent> OpenMagicJarEventList;

	struct FeedPetEvent
	{
		FeedPetEvent() :type(PetFeedType::PET_FEED_INVALID){}

		PetFeedType type;
	};
	typedef std::vector<FeedPetEvent> FeedPetEventList;

	struct AddPetEvent
	{
		AddPetEvent() : petType(PetType::PET_TYPE_INVALID), petQuality(PetQuality::PET_QUALITY_INVALID){}

		PetType petType;
		PetQuality petQuality;
	};
	typedef std::vector<AddPetEvent> AddPetEventList;

	struct PetUpLevelEvent
	{
		PetUpLevelEvent() : petType(PetType::PET_TYPE_INVALID), petQuality(PetQuality::PET_QUALITY_INVALID), level(0){}

		PetType petType;
		PetQuality petQuality;
		UInt16 level;
	};
	typedef std::vector<PetUpLevelEvent> PetUpLevelEventList;

	struct RelationEvent
	{
		RelationEvent() : type(RelationType(0)){}

		RelationType type;
	};
	typedef std::vector<RelationEvent> RelationEventList;

	struct EquitScoreEvent
	{
		EquitScoreEvent() : subType(ItemSubType::ST_ITEM_INVALID), quality(ItemQuality::ITEM_QUALITY_INVLID), score(0) {}

		ItemSubType subType;
		ItemQuality quality;
		UInt32 score;
	};
	typedef std::vector<EquitScoreEvent> EquitScoreEventList;

	// ***************爬塔任务******************//

	struct  KillRoleNumEvent
	{
		KillRoleNumEvent() : num(0) {}

		UInt32 num;
	};
	typedef std::vector< KillRoleNumEvent> KillRoleNumEventList;

	struct AliveTimeEvent
	{
		AliveTimeEvent() : aliveTime(0) {}

		UInt32 aliveTime;
	};
	typedef std::vector<AliveTimeEvent> AliveTimeEventList;

	struct ClearLostDungeonEvent
	{
		ClearLostDungeonEvent() : clearNum(0), dungeonId(0) {}

		UInt32 clearNum;
		UInt32 dungeonId;
	};
	typedef std::vector<ClearLostDungeonEvent> ClearLostDungeonEventList;

	struct KillMonsterEvent
	{
		KillMonsterEvent() : killNum(0), monsterId(0) {}

		UInt32 killNum;
		UInt32 monsterId;
	};
	typedef std::vector<KillMonsterEvent> KillMonsterEventList;

	struct GetLostDungeonItemEvent
	{
		GetLostDungeonItemEvent() : itemNum(0), itemId(0) {}

		UInt32 itemNum;
		UInt32 itemId;
	};
	typedef std::vector<GetLostDungeonItemEvent> GetItemEventList;

public:
	TaskScript(UInt32 id, Avalon::ScriptState* state);
	~TaskScript();

public:

	const char*  GetScriptParam(){ return m_TaskParams.c_str(); }
	void SetScriptParam(std::string scriptParam) { m_TaskParams = scriptParam; };
	
	/**
	 *@brief 绑定获得道具事件, 也就意味着要关注删除道具事件(删除任务相关的道具（不一定是任务道具),可能引发任务状态由完成态变为未完成状态)
	 */
	void BindGetItemEvent(UInt32 id, UInt8 quality = 0, UInt8 strengthen = 0);
	void BindGetItemQualityEvent(UInt32 id, UInt8 quality);
	void BindGetItemStrengthenEvent(UInt32 id, UInt8 strengthen);
	bool CheckGetItemEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const;

	//added by aprilliu, 2016.04.18  这两个绑定函数其实内部就是调用BindGetItemEvent，BindGetItemQualityEvent实现的，当删除道具时候 是否关注该道具还是调用CheckGetItemEvent来实现的
	void BindRmItemEvent(UInt32 itemId);
	void BindItemChangeEvent(UInt32 itemId, UInt8 quality);
	bool CheckItemChangeEvent(UInt32 id, UInt8 quality, UInt8 strengthen) const; //完全可以调用CheckGetItemEvent来检查是否关注该道具，但为了命名的达意还是新定义了该函数

	/**
	 *@brief 绑定杀死怪物事件
	 */
	void BindKillNpcEvent(UInt32 id,UInt32 dungeon);
	void BindKillNpcTypeEvent(UInt8 type, UInt32 dungeon);
	void BindKillLevelNpcEvent();
	bool IsBindKillLevelNpcEvent() const;
	bool CheckKillNpcEvent(UInt32 id, UInt8 type, UInt32 dungeon) const;

	void BindKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId);
	bool CheckKillCityMonsterEvent(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid) const;

	/**
	 *@brief 绑定使用道具事件
	 */
	void BindUseItemEvent(UInt32 id);
	bool CheckUseItemEvent(UInt32 id) const;

	/**
	*@brief 绑定消耗游戏币事件
	*/
	void BindChangeMoneyEvent(UInt32 type);
	bool CheckChangeMoneyEvent(UInt32 type) const;

	/**
	 *@brief 绑定装备改造事件
	 */
	void BindRemakeEquipEvent(UInt32 id, UInt8 type);
	bool CheckRemakeEquipEvent(UInt32 id, UInt8 type) const;

	/**
	*@brief 绑定强化装备事件 adder by huchenhui 2016.06.24
	*/
	void BindStrengthenEquipEvent(bool isSuccess = false, UInt8 strengthen = 0);
	bool CheckStrengthenEquipEvent(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen) const;

	/**
	*@brief 绑定分解装备事件  adder by huchenhui 2016.06.24
	*/
	void BindDecomposeEquipEvent();
	bool CheckDecomposeEquipEvent() const;

	/**
	*@brief 穿戴装备事件  adder by huchenhui 2016.06.29
	*/
	void BindPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen);
	bool CheckPutEquipEvent(UInt16 level, UInt8 color, UInt32 strengthen) const;

	/**
	*@brief 穿戴装备事件  adder by huchenhui 2018.03.20
	*/
	void BindPutFashionEvent(ItemSubType subType, UInt8 color);
	bool CheckPutFashionEvent(ItemSubType subType, UInt8 color) const;

	/**
	 *@brief 绑定护送成功事件
	 */
	void BindEscortSuccedEvent(UInt32 id);
	bool CheckEscortSuccedEvent(UInt32 id) const;
	const IdVec& GetEscortSuccedEvents() const{ return m_EscortSuccedEvents; }

	/**
	 *@brief 绑定相关区域
	 */
	void BindTriggerZone(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
	bool CheckInTriggerZone(UInt32 mapid, const CLPosition& pos);
	const std::vector<CLZone>& GetTriggerZones() const { return m_TriggerZones; }

	void BindEnterZoneEvent(UInt32 mapid, UInt16 x1, UInt16 y1, UInt16 x2, UInt16 y2);
	bool CheckInEventZone(UInt32 mapid, const CLPosition& pos);

	/**
	 *@brief 升级条件
	 */
	void BindLevelupEvent(UInt16 level){ m_LevelupEvent = level; }
	bool CheckLevelupEvent(UInt16 level){ return m_LevelupEvent != 0 && level >= m_LevelupEvent; }

	/**
	 *@brief 绑定捕捉宠物列表
	 */
	void BindCatchPetEvent(UInt32 id);
	bool CheckCatchPetEvent(UInt32 id) const;

	/**
	*@brief 绑定添加随从
	*/
	void BindAddRetinueEvent(UInt32 quality = 0);
	bool CheckAddRetinueEvent(RetinueQuality quality) const;

	/**
	*@brief 绑定随从洗练事件
	*/
	void BindRetinueChangeSkillEvent(UInt32 buffId = 0);
	bool CheckRetinueChangeSkillEvent(UInt32 buffId) const;

	/**
	*@brief 绑定随从升级事件
	*/
	void BindRetinueUpLevelSkillEvent(UInt32 id, UInt8 level);
	bool CheckRetinueUpLevelSkillEvent(UInt32 id, UInt8 level);

	/**
	*@brief 绑定随从升星事件
	*/
	void BindRetinueUpStarEvent(UInt32 id, UInt8 star);
	bool CheckRetinueUpStarEvent(UInt32 id, UInt8 star);

	/**
	 *@brief 绑定副本通关事件(地下城通关)
	 */
	void BindCopyScenePassedEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	//added by aprilliu, 内部就是BindCopyScenePassedEvent
	//m by huchenhui 2016.08.22	增加score和用时限定
	void BindClearDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	void BindClearDungeonEventByRevive(UInt32 dungeonId, UInt32 reviveCount);
	bool CheckCopyScenePassedEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief 绑定副本通关事件(地下城通关,关注评分)
	*/
	void BindClearDungeonEventCareScore(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckCopyScenePassedCareScoreEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief 绑定组队地下城通过事件
	*/
	void BindTeamClearDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckTeamClearDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief 绑定活动副本通关事件(活动地下城通关)
	*/
	void BindClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	*@brief 绑定组队活动副本通关事件(活动地下城通关)
	*/
	void BindTeamClearActivityDungeonEvent(UInt32 dungeonId, LevelScore score = LS_C, UInt32 usedTime = 0, UInt32 reviveCount = 0);
	bool CheckTeamClearActivityDungeonEvent(UInt32 id, LevelScore score, UInt32 usedTime, UInt32 reviveCount) const;

	/**
	 *@brief 绑定通天塔通关事件
	 */
	void BindBabelPassedEvent(UInt32 id);
	bool CheckBabelPassedEvent(UInt32 id) const;

	/**
	*@brief 绑定购买商城道具事件
	*/
	void BindBuyMallItemEvent(UInt32 mallItemid);
	bool CheckBuyMallItemEvent(UInt32 mallItemid) const;

	/**
	 *@brief 绑定加入部落事件
	 */
	void BindEnterTribeEvent(){ m_bEnterTribeEvnet = true; }
	bool CheckEnterTribeEvent() const{ return m_bEnterTribeEvnet; }

	/**
	 *@brief  绑定剧情结束事件                                                                    
	 */
	void BindStoryEndEvent(UInt32 id);
	bool CheckStoryEndEvent(UInt32 id) const;

	/**
	 *@brief 绑定任务成功事件                                                                     
	 */
	void BindTaskSuccedEvent(UInt32 id){ m_TaskSuccId = id; }
	bool CheckTaskSuccedEvent(UInt32 id) const{ return m_TaskSuccId == id; }

	//added by aprilliu, 2016.04.18 绑定任务提交成功的事件
	void BindSubmitTaskSucceedEvent(UInt32 taskId);
	bool CheckSubmitTaskSucceedEvent(UInt32 taskId) const;

	/**
	*@brief 绑定活动任务成功事件
	*/
	void BindSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId);
	bool CheckSubmitActiveTaskSucceedEvent(UInt32 templateId, UInt32 taskId);

	/**
	*@brief 绑定循环任务成功事件
	*/
	void BindSubmitCycleTaskSucceedEvent(UInt32 taskId);
	bool CheckSubmitCycleTaskSucceedEvent(UInt32 taskId);

	/**
	 *@brief 绑定任务失败事件
	 */
	void BindTaskFailedEvent(UInt32 id){ m_TaskFailedId = id; }
	bool CheckTaskFailedEvent(UInt32 id) const{ return m_TaskFailedId == id; } 

	/**
	 *@brief 绑定死亡事件
	 */
	void BindDiedEvent(){ m_bDiedEvent = true; }
	bool CheckDiedEvent() const{ return m_bDiedEvent; } 

	/**
	 *@brief 绑定日常任务提交事件
	 */
	void BindDailyTaskSubmittedEvent(UInt32 taskId);
	bool CheckDailyTaskSubmittedEvent(UInt32 taskId) const;

	/**
	*@brief 绑定延时完成任务
	*/
	void BindTimeEvent(UInt32 uCompleteTime) { m_uCompleteTime = uCompleteTime; }
	UInt32 GetTimeEvent() const { return m_uCompleteTime; }

	/** 
	 *@brief 获取任务奖励
	 */
	UInt32 GetTaskExp(Player* player, UInt8 quality);
	RewardGroup* GetTaskRewards(Player* player, UInt8 quality); 

	/**
	*@brief 传统决斗
	*/
	void BindPVPEvent(PkType type, PkRaceResult raceResult = PKRR_INVALID);
	bool CheckPVPEvent(PkType type, PkRaceResult raceResult) const;

	/**
	*@brief 转职
	*/
	void BindChangeOccuEvent();
	bool CheckChangeOccuEvent() const;

	/**
	*@brief 附魔
	*/
	void BindAddMagicEvent();
	bool CheckAddMagicEvent() const;

	/**
	*@brief 合成附魔卡
	*/
	void BindAddMagicCardEvent();
	bool CheckAddMagicCardEvent() const;

	/**
	*@brief 开罐子
	*/
	void BindOpenMagicJarEvent(JarType type = JarType::JT_NONE, UInt32 combo = 0);
	bool CheckOpenMagicJarEvent(JarType type, UInt32 combo) const;

	/**
	*@brief 好友赠送
	*/
	void BindFriendGiveEvent();
	bool CheckFriendGiveEvent() const;

	/**
	*@brief 次元石升级
	*/
	void BindWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level);
	bool CheckWarpStoneUpLevelEvent(WarpStoneType type, UInt32 level) const;

	/**
	*@brief 次元石充能
	*/
	void BindWarpStoneAddEnergyEvent();
	bool CheckWarpStoneAddEnergyEvent() const;

	/**
	*@brief 商店刷新
	*/
	void BindRefreshShopEvent();
	bool CheckRefreshShopEvent() const;

	/**
	*@brief 商店购买
	*/
	void BindShopBuyEvent(UInt32 shopId, UInt32 itemId);
	bool CheckShopBuyEvent(UInt32 shopId, UInt32 itemId) const;

	/**
	*@brief 消耗疲劳
	*/
	void BindRemoveFatigueEvent(UInt16 fatigue);
	bool CheckRemoveFatigueEvent(UInt16 fatigue) const;

	/**
	*@brief 开始深渊
	*/
	void BindBeginHellEvent(UInt32 id);
	bool CheckBeginHellEvent(UInt32 id);

	/**
	*@brief 通关深渊
	*/
	void BindClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score = LevelScore::LS_C);
	bool CheckClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief 组队通过深渊
	*/
	void BindTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score = LevelScore::LS_C);
	bool CheckTeamClearHellEvent(UInt32 id, UInt32 usedTime, LevelScore score);

	/**
	*@brief 死亡之塔开始
	*/
	void BindDeathTowerBeginEvent(UInt32 towerId);
	bool CheckDeathTowerBeginEvent(UInt32 towerId);

	/**
	*@brief 死亡之塔开始扫荡
	*/
	void BindDeathTowerWipeoutBeginEvent();
	bool CheckDeathTowerWipeoutBeginEvent();

	/**
	*@brief 死亡之塔
	*/
	void BindDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime);
	bool CheckDeathTowerEvent(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief 觉醒
	*/
	void BindAwakenEvent();
	bool CheckAwakenEvent();

	/**
	*@brief 使用复活币
	*/
	void BindReviveCoinEvent();
	bool CheckReviveCoinEvent();

	/**
	*@brief 拍卖行 上架
	*/
	void BindAddAuctionEvent();
	bool CheckAddAuctionEvent();

	/**
	*@brief 拍卖行 售出
	*/
	void BindSellAuctionEvent();
	bool CheckSellAuctionEvent();

	/**
	*@brief 拍卖行 购买
	*/
	void BindBuyAuctionEvent();
	bool CheckBuyAuctionEvent();

	/**
	*@brief 提交任务物品
	*/
	void BindSetTaskItemEvent(ItemQuality quality, UInt32 level);
	void BindSetTaskItemByIdEvent(UInt32 itemId, UInt32 num);
	bool CheckSetTaskItemEvent(ItemQuality quality, UInt32 level, UInt32 itemId, UInt32& num);

	/**
	*@brief 喂食宠物
	*/
	void BindFeedPetEvent(PetFeedType type);
	bool CheckFeedPetEvent(PetFeedType type);

	/**
	*@brief 获得宠物
	*/ 
	void BindAddPetEvent(PetType petType, PetQuality petQuality);
	bool CheckAddPetEvent(PetType petType, PetQuality petQuality);

	/**
	*@brief 宠物升级
	*/
	void BindPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level);
	bool CheckPetUpLevelEvent(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief 增加关系
	*/
	void BindAddRelationEvent(RelationType type);
	bool CheckAddRelationEvent(RelationType type);

	/**
	*@brief 发喇叭
	*/
	void BindSendHornEvent();
	bool CheckSendHornEvent();

	/**
	*@brief 装备评分
	*/
	void BindCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score);
	bool CheckCalculateEquipScoreEvent(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief 镶嵌宝珠
	*/
	void BindAddPreciousBeadEvent();
	bool CheckAddPreciousBeadEvent();

	/**
	*@brief 加入工会
	*/
	void BindJoinGuildEvent();
	bool CheckJoinGuildEvent();

	/**
	*@brief 增加工会积分
	*/
	void BindIncGuildBattleScoreEvent();
	bool CheckIncGuildBattleScoreEvent();

	/**
	*@brief 占领领地
	*/
	void BindGuildOccupyTerritoryEvent(UInt32 terrId);
	bool CheckGuildOccupyTerritoryEvent(UInt32 terrId);

	/**
	*@brief 武道大会
	*/
	void BindWinWudaodahuiPkEndEvent(UInt32 winNum);
	bool CheckWinWudaodahuiPkEndEvent(UInt32 winNum);

	/**
	*@brief 扩展包裹
	*/
	void BindEnlargePackageEvent();
	bool CheckEnlargePackageEvent();

	/**
	*@brief 扩展仓库
	*/
	void BindEnlargeStorageEvent();
	bool CheckEnlargeStorageEvent();

	/**
	*@brief  好友度增加
	*/
	void BindIncIntimacyEvent();
	bool CheckIncIntimacyEvent();

	/**
	*@brief  增加玩家日常任务积分(活跃度)
	*/
	void BindIncDailyTaskScoreEvent();
	bool CheckIncDailyTaskScoreEvent();

	/**
	*@brief  收徒
	*/
	void BindApprentDiscipleEvent();
	bool CheckApprentDiscipleEvent();

	/**
	*@brief 徒弟出师
	*/
	void BindDiscipleFinschEvent();
	bool CheckDiscipleFinschEvent();

	/**
	*@brief 公户徽记升级
	*/
	void BindGuildEmblemEvent();
	bool CheckGuildEmblemEvent();

public: //脚本事件
	/**
	 *@brief 初始化
	 */
	void Init();

	/**
	 *@brief 检查是否可接
	 */
	bool CheckAccept(Player* player);

	/**
	 *@brief 接到任务后调用
	 */
	void OnAccepted(Player* player, BaseTask* task);

	/**
	 *@brief 提交任务
	 */
	void OnSubmitted(Player* player, BaseTask* task);

	/**
	 *@brief 取消任务
	 */
	void OnAbandomed(Player* player, BaseTask* task);

	/**
	*@brief 检查任务是否完成
	*/
	void CheckFinish(Player* player, BaseTask* task);

	/**
	 *@brief 获得道具
	 */
	void OnGetItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task);

	/**
	*@brief 删除道具   更好的做法：把OnGetItem,OnRmItem 合并成OnChangeItem
	*/
	void OnRmItem(Player* player, UInt32 itemid, UInt16 num, BaseTask* task);

	/**
	 *@brief 使用道具
	 */
	void OnUseItem(Player* player, UInt32 itemid, BaseTask* task);

	/**
	*@brief 游戏币变化事件  added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(Player* player, UInt32 type, Int32 num, BaseTask* task);

	/**
	 *@brief 杀死npc事件
	 */
	void OnKillNpc(Player* player, UInt32 npcid, UInt8 type, BaseTask* task, UInt32 dungeonId);

	/**
	*@brief 杀死城镇指定怪物
	*/
	void OnKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid);

	/**
	*@brief 杀死城镇指定怪物
	*/
	void OnTeamKillCityMonster(Player* player, BaseTask* task, UInt32 cityMonsterType, UInt32 monsterId, UInt64 monsterGuid, std::vector<ObjID_t>& teammateIds);

	/**
	 *@brief 进入绑定区域
	 */
	void OnEnterZone(Player* player);

	/**
	 *@brief 改造装备
	 */
	void OnRemakeEquip(Player* player, BaseTask* task, UInt32 id, UInt8 type);

	/**
	*@brief 计算装备积分
	*/
	void OnCalculateValuedScore(Player* player, BaseTask* task, ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief 镶嵌宝珠
	*/
	void OnAddPreciousBead(Player* player, BaseTask* task);

	/**
	*@brief 强化装备 Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(Player* player, BaseTask* task, UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen);

	/**
	*@brief 分解装备 Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip(Player* player, BaseTask* task);

	/**
	*@brief 穿戴装备 Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(Player* player, BaseTask* task, UInt16 level, UInt32 subType, UInt32 strength);

	/**
	*@brief 穿戴时装 Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(Player* player, BaseTask* task, ItemSubType subType, UInt32 color);

	/**
	 *@brief 捕获宠物
	 */
	void OnCatchPet(Player* player, UInt32 id);

	/**
	*@brief 添加随从  added by huchenhui 2016.08.04
	*/
	void OnAddRetinue(Player* player, BaseTask* task, RetinueQuality quality);

	/**
	*@brief 随从洗练  added by huchenhui 2016.08.20
	*/
	void OnRetinueChangeSkill(Player* player, BaseTask* task, UInt32 buffId);

	/**
	*@brief 随从升级  added by huchenhui 2016.11.08
	*/
	void OnRetinueUpLevel(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueLevel);

	/**
	*@brief 随从升星  added by huchenhui 2016.12.07
	*/
	void OnRetinueUpStar(Player* player, BaseTask* task, UInt32 retinueId, UInt8 retinueStar);

	/**
	 *@brief 护送成功
	 */
	void OnEscortSucced(Player* player, Npc* npc);

	/**
	 *@brief 护送失败
	 */
	void OnEscortFailed(Player* player, UInt32 npcId);

	/**
	 *@brief 护送目标被杀死
	 */
	void OnEscortTargetKilled(Npc* npc, Player* killer);

	/**
	 *@brief 玩家升级
	 */
	void OnPlayerLevelup(Player* player, BaseTask* task);

	/**
	 *@brief 副本通关
	 */
	void OnCopyScenePassed(Player* player, UInt32 id, BaseTask* task);

	//added by aprilliu. 2016.04.14. 
	//地下城(副本)通关后的回调. dungeonId - 地下城id(包含难度信息), beated - 被击次数, areas - 通关房间数
	void OnClearDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas);

	//活动地下城(副本)通关后的回调. dungeonId - 地下城id(包含难度信息), beated - 被击次数, areas - 通关房间数
	void OnClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas);

	/**
	*@brief 组队通关地下城
	*/
	void OnTeamClearDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 组队通关活动地下城
	*/
	void OnTeamClearActivityDungeon(Player* player, BaseTask* task, UInt32 dungeonId, UInt16 beated, UInt16 areas, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 组队通关深渊地下城
	*/
	void OnTeamClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 组队pvp
	*/
	void OnTeamPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status, std::vector<ObjID_t>& teammateIds);

	/**
	 *@brief 通天塔通关
	 */
	void OnBabelPassed(Player* player, UInt32 id);

	/**
	*@brief 商城购买道具
	*/
	void OnBuyMallItem(Player *pPlayer, BaseTask* pTask, UInt32 mallItemId, UInt32 OnBuyMallItem);

	/**
	 *@brief 进入部落
	 */
	void OnEnterTribe(Player* pPlayer);

	/**
	 *@brief 剧情结束
	 */
	void OnStoryEnd(Player* player, UInt32 id, BaseTask* task);

	/**
	 *@brief 任务超时
	 */
	void OnTimeout(Player* player);

	/**
	 *@brief 任务成功事件
	 */
	void OnTaskSucced(Player* player, UInt32 taskId);

	//added by aprilliu, 2016.04.18 成功提交任务事件
	void OnSubmitTaskSucceed(Player* player, UInt32 taskId, BaseTask* task);

	/**
	*@brief 活动任务成功事件 by huchenhui
	*/
	void OnSubmitActiveTaskSucceed(Player* player, BaseTask* task, UInt32 templateId, UInt32 taskId);

	/**
	*@brief 循环任务成功事件 by huchenhui
	*/
	void OnSubmitCycleTaskSucceed(Player* player, BaseTask* task, UInt32 taskId);

	/**
	 *@brief 任务失败事件
	 */
	void OnTaskFailed(Player* player, UInt32 taskId);

	/**
	 *@brief 死亡事件
	 */
	void OnOwnerDied(Player* player);

	/**
	 *@brief 日常任务提交
	 */
	void OnDailyTaskSubmitted(Player* player,BaseTask* task, UInt8 type);

	/**
	*@brief PVP任务提交
	*/
	void OnPVP(Player* player, BaseTask* task, UInt32 type, UInt32 status);

	/**
	*@brief 转职事件
	*/
	void OnChangeOccu(Player* player, BaseTask* task);

	/**
	*@brief 附魔事件
	*/
	void OnAddMagic(Player* player, BaseTask* task);

	/**
	*@brief 合成附魔卡
	*/
	void OnAddMagicCard(Player* player, BaseTask* task);

	/**
	*@brief 开罐子事件
	*/
	void OnOpenMagicJar(Player* player, BaseTask* task, JarType type, UInt32 combo);
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
	/**
	*@brief 好友赠送事件
	*/
	void OnFriendGive(Player* player, BaseTask* task);

	/**
	*@brief 次元石升级事件
	*/
	void OnWarpStoneUpLevel(Player* player, BaseTask* task, WarpStoneType type, UInt32 level);

	/**
	*@brief 次元石充能事件
	*/
	void OnWarpStoneAddEnergy(Player* player, BaseTask* task, UInt32 energy);

	/**
	*@brief 刷新商店事件
	*/
	void OnRefreshShop(Player* player, BaseTask* task);

	/**
	*@brief 商店购买事件
	*/
	void OnShopBuy(Player* player, BaseTask* pTask, UInt32 shopId, UInt32 shopItemId, UInt32 num);

	/**
	*@brief 消耗疲劳值事件
	*/
	void OnRemoveFatigue(Player* player, BaseTask* task, UInt16 usedFatigue);

	/**
	*@brief 开始深渊事件
	*/
	void OnBeginHell(Player* player, BaseTask* task, UInt32 dungeonId);

	/**
	*@brief 通关深渊事件
	*/
	void OnClearHell(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 usedTime);

	/**
	*@brief 死亡之塔开始事件
	*/
	void OnDeathTowerBegin(Player* player, BaseTask* task, UInt32 towerId);

	/**
	*@brief 死亡之塔扫荡开始事件
	*/
	void OnDeathTowerWipeoutBegin(Player* player, BaseTask* task);

	/**
	*@brief 死亡之塔事件
	*/
	void OnDeathTower(Player* player, BaseTask* task, UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief 觉醒事件
	*/
	void OnAwaken(Player* player, BaseTask* task, AwakenStatus status);

	/**
	*@brief 复活币
	*/
	void OnReviveCoin(Player* player, BaseTask* task, bool isSelf);

	/**
	*@brief 拍卖行 上架
	*/
	void OnAddAuction(Player* player, BaseTask* task);

	/**
	*@brief 拍卖行 售出
	*/
	void OnSellAuction(Player* player, BaseTask* task, UInt32 money);

	/**
	*@brief 拍卖行 购买
	*/
	void OnBuyAuction(Player* player, BaseTask* task, UInt32 money);

	/**
	*@brief 提交任务物品
	*/
	void OnSetTaskItem(Player* player, BaseTask* task, Item* item, UInt32 itemId, UInt32 num);

	/**
	*@brief 宠物喂食
	*/
	void OnFeedPet(Player* player, BaseTask* task, PetFeedType type);

	/**
	*@brief 获得宠物
	*/
	void OnAddPet(Player* player, BaseTask* task, PetType petType, PetQuality petQuality);

	/**
	*@brief 宠物升级
	*/
	void OnPetUpLevel(Player* player, BaseTask* task, PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief 增加上社会关系
	*/
	void OnAddRelation(Player* player, BaseTask* task, RelationType type);

	/**
	*@brief 发喇叭
	*/
	void OnSendHorn(Player* player, BaseTask* task);

	/**
	*@brief 加入工会
	*/
	void OnJoinGuild(Player* player, BaseTask* task);

	/**
	*@brief 增加工会积分
	*/
	void OnIncGuildBattleScore(Player* player, BaseTask* task, UInt32 score);

	/**
	*@brief 占领领地
	*/
	void OnGuildOccupyTerritory(Player* player, BaseTask* task, UInt32 terrId);

	/**
	*@brief 武道大会
	*/
	void OnWinWudaodahuiPkEnd(Player* player, BaseTask* task, UInt32 winNum);

	/**
	*@brief 扩展背包
	*/
	void OnEnlargePackage(Player* player, BaseTask* task, UInt32 size);

	/**
	*@brief 扩展仓库
	*/
	void OnEnlargeStorage(Player* player, BaseTask* task, UInt32 size);

	/**
	*@brief 好友度
	*/
	void OnIncIntimacy(Player* player, ObjID_t targetId, BaseTask* task, UInt32 incIntimacy, UInt32 finalIntimacy);

	/**
	*@brief 玩家增加日常任务积分(活跃度)
	*/
	void OnIncDailyTaskScore(Player* player, BaseTask* task, UInt32 totalDailyTaskScore);

	/**
	*@brief 收徒事件
	*/
	void OnApprentDisciple(Player* player, BaseTask* task);

	/**
	*@brief 徒弟出师事件
	*/
	void OnDiscipleFinsch(Player* player, BaseTask* task);

	/**
	*@brief 公会徽记升级
	*/
	void OnGuildEmblemUp(Player* player, BaseTask* task, UInt32 lvl);

	//地下城(副本)通关后的回调. dungeonId - 地下城id(包含难度信息), score - 评分
	void OnClearDungeonCareScore(Player* player, BaseTask* task, UInt32 dungeonId, LevelScore score);

private:
	std::string m_TaskParams;

	//杀死npc事件列表
	GetItemEventVec	m_getItemEvents;
	//杀死npc事件列表
	KillNPCEventList	m_killNpcEvents;
	//杀死城镇怪物事件列表
	KillCityMonsterEventList	m_KillCityMonsterEvents;
	//杀死的npc等级条件
	bool	m_bLevelNpcCond;
	//使用道具事件列表
	IdVec	m_useItemEvents;
	//游戏币事件列表
	IdVec	m_ChangeMoneyEvents;
	//改造装备事件列表
	EquipRemakeEventVec	m_remakeEquipEvents;
	//捕捉宠物事件列表
	IdVec	m_catchPetEvents;
	//护送成功事件列表
	IdVec	m_EscortSuccedEvents;
	//副本通关事件列表
	ClearDungeonEventList	m_CopyScenePassedEvents;
	//开始深渊事件列表		added by huchenhui
	IdVec	m_BeginHellEvents;
	//通关深渊事件列表		added by huchenhui
	ClearDungeonEventList	m_ClearHellEvents;
	//通关活动地下城事件列表
	ClearDungeonEventList	m_ClearActivityDungeonEvents;
	//组队通关副本事件列表
	ClearDungeonEventList	m_TeamCopyScenePassedEvents;
	//组队通关深渊事件列表
	ClearDungeonEventList	m_TeamClearHellEvents;
	//组队通关活动地下城事件列表
	ClearDungeonEventList	m_TeamActivityDungeonEvents;

	//副本通关事件列表(关注评分)
	ClearDungeonEventList	m_CopyScenePassedCareScoreEvents;

	//通天塔通关时间列表
	IdVec	m_BabelPassedEvents;
	//购买商城道具事件列表
	IdVec	m_buyMallItemEvents;
	//剧情结束事件列表
	IdVec	m_StoryEndEvents;
	//日常任务事件列表
	IdVec	m_DailyTaskSubmittedEvents;
	//是否绑定PVP事件  added by huchenhui 2016.08.05
	PVPEventList m_PVPEvent;


	//穿上装备的事件		added huchenhui 2016.06.29
	PutEquipEventList	m_PutEquipEvents;

	//穿上时装的事件		added huchenhui 2018.03.20
	PutFashionEventList m_PutFashionEvents;

	//石头升级		added huchenhui 2016.08.23
	WarpStoneUpLevelEventList m_WarpStoneUpLevelEvents;

	//死亡之塔	added by huchenhui
	DeathTowerEventList m_DeathTowerEvents;

	//死亡之塔开始	added by huchenhui
	IdVec	m_DeathTowerBeginEvents;

	//死亡之塔扫荡开始  added by huchenhui
	bool	m_bDeathTowerWipeoutBeginEvent;

	//完成活动任务
	ActiveTaskEventList	m_SubmitActiveTaskSucceedEvents;

	//商店购买	added by huchenhui
	ShopBuyEventList	m_ShopBuyEvents;

	//完成循环任务
	IdVec	m_SubmitCycleTaskSucceedEvents;

	//绑定随从洗练事件	added by huchenhui
	IdVec	m_RetinueChangeSkillEvents;

	//是否绑定随从升级事件	added by huchenhui
	RetinueUpLevelEventList m_bRetinueUpLevelEvents;

	//是否绑定随从升星事件	added by huchenhui
	RetinueUpLevelEventList m_RetinueUpStarEvents;

	//上交任务物品
	SetTaskItemEventList m_SetTaskItemEvents;

	//强化事件	added by huchenhui
	StrengthenEquipEvent m_StrengthenEquipEvent;

	//触发区域
	std::vector<CLZone>	m_TriggerZones;
	//绑定区域
	std::vector<CLZone>	m_BindZones;

	//喂食宠物
	FeedPetEventList m_FeedPetEvents;

	//获得宠物
	AddPetEventList m_AddPetEvents;

	//宠物升级
	PetUpLevelEventList m_PetUpLevelEvents;

	//玩家关系
	RelationEventList m_RelationEvents;

	//升级事件等级
	UInt16	m_LevelupEvent;

	//是否绑定进入部落事件
	bool	m_bEnterTribeEvnet;

	//绑定任务成功事件
	UInt32	m_TaskSuccId;
	//绑定任务失败事件
	UInt32	m_TaskFailedId;

	//added by aprilliu, 2016.04.18
	IdVec m_SubmitTaskSucceedIdList;

	//是否绑定死亡事件
	bool	m_bDiedEvent;

	//绑定日常任务提交事件
	UInt8	m_TaskSubmittedType;

	//绑定延时完成任务的完成时间（秒）
	UInt32	m_uCompleteTime;

	//已使用的疲劳值	added by huchenhui
	UInt16	m_uUsedFatigue;

	//装备积分		added by huchenhui
	EquitScoreEventList m_EquipScoreEventList;

	//是否开罐子事件		added by huchenhui
	OpenMagicJarEventList m_OpenMagicJarComboEventList;

	//是否绑定添加随从事件	added by huchenhui
	RetinueQuality m_AddRetinueEvent;

	//武道大会				added by huchenhui
	UInt32 m_uWudaodahuiWinNumEvent;

	//占领领地					added by huchenhui
	IdVec m_GuildOccupyTerritoryEvent;

	//是否绑定装备强化事件	added by huchenhui
	bool m_bStrengthenEquipEvent;

	//是否绑定装备分解事件	added by huchenhui
	bool m_bDecomposeEquipEvent;

	//是否绑定转职事件		added by huchenhui
	bool m_bChangeOccuEvent;

	//是否绑定附魔事件		added by huchenhui
	bool m_bAddMagicEvent;

	//是否绑定合成附魔卡事件	added by huchenhui
	bool m_bAddMagicCardEvent;

	//是否好友赠送事件		added by huchenhui
	bool m_bFriendGiveEvent;

	//是否刷新商店事件		added by huchenhui
	bool m_bRefreshShopEvent;

	//是否觉醒事件			added by huchenhui
	bool m_bAwakenEvent;

	//次元石充能			added by huchenhui
	bool m_bAddEnergyEvent;

	//复活币				added by huchenhui
	bool m_bReviveCoinEvent;

	//拍卖行 上架			added by huchenhui
	bool m_bAddAuctionEvent;

	//拍卖行 售出			added by huchenhui
	bool m_bSellAuctionEvent;

	//拍卖行 购买			added by huchenhui
	bool m_bBuyAuctionEvent;

	//镶嵌宝珠				added by huchenhui
	bool m_bAddPreciousBeadEvent;

	//发喇叭				added by huchenhui
	bool m_bSendHornEvent;

	//加入工会				added by huchenhui
	bool m_bJoinGuildEvent;

	//增加工会战积分		added by huchenhui
	bool m_bIncGuildBattleEvent;

	//扩展背包				added by huchenhui
	bool m_bEnlargePackageEvent;

	//扩展仓库				added by huchenhui
	bool m_bEnlargeStorageEvent;
	
	//亲密度增加
	bool m_bIncIntimacyEvent;
	
	//玩家增加日常任务积分(活跃度)
	bool m_bIncIDailyTaskScoreEvent;

	//招徒
	bool m_bApprentDiscipleEvent;

	//徒弟出师
	bool m_bDiscipleFinschEvent;

	//公会徽记升级
	bool m_bGuildEmblemEvent;


public:
	// 爬塔任务
	
	/**
	*@brief 杀人事件
	*/
	void BindLostDungeonKillRoleNumEvent(UInt32 killNum);
	void OnLostDungeonKillRoleNum(Player* player, BaseTask* task, UInt32 num);
	void OnLostDungeonKillFixRole(Player* player, BaseTask* task, UInt64 dieId);

	/**
	*@brief 战场生存事件
	*/
	void BindLostDungeonAliveTimeEvent(UInt32 minTime);
	void OnLostDungeonAliveTime(Player* player, BaseTask* task, UInt32 time);

	/**
	*@brief 通关地下城事件
	*/
	void BindLostDungeonClearEvent(UInt32 dungeonId, UInt32 clearNum);
	void OnLostDungeonClearNum(Player* player, BaseTask* task, UInt32 num);
	void OnLostDungeonFixClear(Player* player, BaseTask* task, UInt32 dungeonId, UInt32 num);

	/**
	*@brief 杀怪事件
	*/
	void BindLostDungeonKillMonsterEvent(UInt32 monsterId, UInt32 killNum);
	void OnLostDungeonKillFixMonster(Player* player, BaseTask* task, UInt32 monsterId, UInt32 num);

	/**
	*@brief 道具事件
	*/
	void BindLostDungeonGetItemEvent(UInt32 itemId, UInt32 itemNum);
	void OnLostDungeonCollectItem(Player* player, BaseTask* task, UInt32 itemId, UInt32 itemNum);

private:

	// 爬塔任务事件
	// 击杀人数事件列表
	KillRoleNumEventList m_KillRoleNumEventList;
	// 战场生存时间事件列表
	AliveTimeEventList m_AliveTimeEventList;
	// 通关地下城
	ClearLostDungeonEventList m_ClearLostDungeonEventList;
	// 杀怪
	KillMonsterEventList m_KillMonsterEventList;
	// 获得道具
	GetItemEventList m_GetItemEventList;

};


#endif
