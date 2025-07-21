#ifndef _BASE_TASK_MGR_H_
#define _BASE_TASK_MGR_H_

#include <map>
#include <set>
#include <CLObjectProperty.h>
#include <CLTaskDefine.h>
#include <CLDungeonDefine.h>
#include <CLWarpStoneDefine.h>
#include <CLMatchDefine.h>
#include <CLRelationDefine.h>
#include <CLCityMonsterDataEntry.h>

#include "BaseTask.h"

class Player;
class Npc;

class BaseTaskMgr //: public CLComplexProperty
{
public:
	typedef std::map<UInt32, BaseTask*> TaskMap;
	typedef std::set<UInt32>		TaskIdSet;

public:
	BaseTaskMgr();
	virtual ~BaseTaskMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }


public:

	/**
	*@brief 添加一个任务
	*/
	virtual bool AddTask(BaseTask* task);

	/**
	*@brief 加载一个任务
	*/
	bool LoadTask(BaseTask* task);

	/**
	*@brief 查找一个任务
	*/
	BaseTask* FindTask(UInt32 id) const;

	/**
	*@brief 删除一个任务
	*/
	void RemoveTask(BaseTask* task);

	/**
	*@brief 清除所有任务
	*/
	void ClearTask();

	/**
	*@brief 获取任务数
	*/
	virtual UInt32 GetTaskNum() const;

	/**
	*@brief 遍历所有任务
	*/
	void VisitTasks(BaseTaskVisitor& visitor);

	/**
	*@brief 获取任务列表
	*/
	void GetTaskList(std::vector<TaskBriefInfo>& task);

	//added by aprilliu, 2016.04.20 
	//根据地下城id 返回玩家已接的该地下城相关的任务列表
	void GetDungeonTaskList(UInt32 dungeonId, std::vector<UInt32>& taskList);

	virtual bool GetInitTaskBriefInfo(UInt32 id, TaskBriefInfo& info);

	virtual void CheckFinish();

public:

	/**
	*@brief 接受一个任务
	*/
	virtual bool AcceptTask(UInt32 id, UInt8 quality, bool bForce) { return true; }

	/**
	*@brief 提交一个任务
	*/
	virtual bool SubmitTask(BaseTask* task, bool force = false) = 0;

	/**
	*@brief 放弃一个任务
	*/
	virtual bool AbandonTask(BaseTask* task) { return true; };

	/**
	*@brief 是否能够接取一个任务
	*/
	virtual bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false) { return true; }

	/**
	*@brief 获取最大任务数
	*/
	virtual UInt32 GetMaxTaskNum() = 0;

public:
	/**
	*@brief tick事件
	*/
	virtual void OnHeartbeat(const Avalon::Time& now, int tickType) {};

	/**
	*@brief 玩家上线
	*/
	virtual void OnOnline(bool bDayChanged) = 0;

	/**
	*@brief 玩家升级
	*/
	virtual void OnLevelup() {};

	/**
	*@brief 进入第二天
	*/
	virtual void OnDayChanged() {};

	/**
	*@brief 获得道具
	*/
	void OnGetItem(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num);

	//added by aprilliu, 删除道具 (之所以增加改接口是因为 搜集类任务 玩家可以卖出搜集到的道具，这样可能引起任务状态由已完成变为未完成
	void OnRmItem(UInt32 id, UInt8 quality, UInt8 strengthen, UInt16 num);

	/**
	*@brief 使用道具
	*/
	void OnUseItem(UInt32 id);

	/**
	*@brief 游戏币变化  num为正数表示增加, 负数表示消耗   added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(UInt32 type, Int32 num);

	/**
	*@brief 具有归属权的npc死亡
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	*@brief 指定NPC死亡
	*/
	void OnKillCityMonster(CityMonsterType cityMonsterType, UInt32 monsterId, UInt64 monsterGuid);

	/**
	*@brief 移动
	*/
	void OnMoved();

	/**
	*@brief 改造装备
	*/
	void OnRemakeEquip(UInt32 id, UInt8 type);

	/**
	*@brief 计算装备分
	*/
	void OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief 镶嵌宝珠
	*/
	void OnAddPreciousBead();

	/**
	*@brief 强化装备 Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(UInt32 errorCode, UInt8 oldStrengthen, UInt8 newStrengthen);

	/**
	*@brief 分解装备 Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip();

	/**
	*@brief 穿戴装备 Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen);

	/**
	*@brief 穿戴装备 Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(ItemSubType subType, UInt8 color, UInt8 isTransparent);

	/**
	*@brief 捕获宠物
	*/
	void OnCatchPet(UInt32 id);

	/**
	*@brief 添加随从事件
	*/
	void OnAddRetinue(RetinueQuality quality);

	/**
	*@brief 添加随从洗练事件
	*/
	void OnRetinueChangeSkill(std::vector<RetinueSkill> newSkills);

	/**
	*@brief 随从升级事件
	*/
	void OnRetinueUpLevel(UInt32 retinueId, UInt8 retinueLevel);

	/**
	*@brief 随从升星事件
	*/
	void OnRetinueUpStar(UInt32 retinueId, UInt8 retinueStar);

	/**
	*@brief 副本通关
	*/
	void OnCopyScenePassed(UInt32 id);

	/**
	*@ 通关地下城 dungeonId - 地下城id，包含难度信息; beated - 通关时的被击次数; areas -通过的房间数; score - 关卡评分; usedTime - 使用时间, reviveCount - 复活次数
	*/
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/**
	*@ 通关活动地下城 dungeonId - 地下城id，包含难度信息; beated - 通关时的被击次数; areas -通过的房间数; score - 关卡评分; usedTime - 使用时间, reviveCount - 复活次数
	*/
	void OnClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/**
	*@brief 通天塔通关
	*/
	void OnBabelPassed(UInt32 id);

	/**
	*@brief 购买商城道具
	*/
	void OnBuyMallItem(UInt32 mallItemId, UInt32 mallItemNum);

	/**
	*@brief 进入部落
	*/
	void OnEnterTribe();

	/**
	*@brief 剧情结束
	*/
	void OnStoryEnd(UInt32 id);

	/**
	*@brief 任务成功
	*/
	virtual void OnTaskSucced(BaseTask* task);

	//added by aprilliu, 2016.04.18   成功提交任务
	void OnSubmitTaskSucceed(UInt32 taskId);

	//added by huchenhui
	void OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId);

	//added by huchenhui
	void OnSubmitCycleTaskSucceed(UInt32 taskId);

	/**
	*@brief 任务失败
	*/
	void OnTaskFailed(UInt32 id);

	/**
	*@brief 死亡
	*/
	void OnDied();

	/**
	*@brief 日常任务提交
	*/
	void OnDailyTaskSubmitted(UInt32 taskId, UInt8 type);

	/**
	*@brief 传统决斗
	*/
	void OnPVP(PkType type, PkRaceResult status);

	/**
	*@brief 转职
	*/
	virtual void OnChangeOccu();

	/**
	*@brief 附魔
	*/
	void OnAddMagic();

	/**
	*@brief 附魔卡合成
	*/
	void OnAddMagicCard();

	/**
	*@brief 开罐子
	*/
	void OnOpenMagicJar(JarType type, UInt32 combo);

	/**
	*@brief 好友赠送
	*/
	void OnFriendGive();

	/**
	*@brief 次元石升级
	*/
	void OnWarpStoneUpLevel(WarpStoneType type, UInt32 level);

	/**
	*@brief 次元石充能
	*/
	void OnWarpStoneAddEnergy(UInt32 energy);

	/**
	*@brief 商店刷新
	*/
	void OnRefreshShop();

	/**
	*@brief 商店购买物品
	*/
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num);

	/**
	*@brief 疲劳值消耗
	*/
	void OnRemoveFatigue(UInt16 usedFatigue);

	/**
	*@brief 开始深渊
	*/
	void OnBeginHell(UInt32 dungeonId);

	/**
	*@brief 通关深渊
	*/
	void OnClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score);

	/**
	*@brief 死亡之塔开始
	*/
	void OnDeathTowerBegin(UInt32 towerId);

	/**
	*@brief 死亡之塔扫荡开始
	*/
	void OnDeathTowerWipeoutBegin();

	/**
	*@brief 死亡之塔
	*/
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief 觉醒
	*/
	void OnAwaken();

	/**
	*@brief 复活币
	*/
	void OnReviveCoin(bool isSelf);

	/**
	*@brief 拍卖行 寄卖
	*/
	void OnAddAuction();

	/**
	*@brief 拍卖行 卖出
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief 拍卖行 购买
	*/
	void OnBuyAuction(UInt32 money);

	/**
	*@brief 提交任务物品
	*/
	void OnSetTaskItem(ItemQuality quality, UInt32 itemId, UInt32 num);

	/**
	*@brief 喂食宠物
	*/
	void OnFeedPet(PetFeedType type);

	/**
	*@brief 获得宠物
	*/
	void OnAddPet(PetType petType, PetQuality petQuality);

	/**
	*@brief 宠物升级
	*/
	void OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief 增加社会关系
	*/
	void OnAddRelation(RelationType type);

	/**
	*@brief 发喇叭
	*/
	void OnSendHorn();

	/**
	*@brief 加入工会
	*/
	void OnJoinGuild();

	/**
	*@brief 增加工会战积分
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief 占领工会战领地
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief 武道大会
	*/
	void OnWinWudaodahuiPkEnd(UInt32 winNum);

	/**
	*@brief 扩展包裹 Adder by huchenhui 2018.06.11
	*/
	void OnEnlargePackage(UInt32 size);

	/**
	*@brief 扩展仓库 Adder by huchenhui 2018.06.11
	*/
	void OnEnlargeStorage(UInt32 size);

	/**
	*@brief 好友度增加
	*/
	void OnIncIntimacy(ObjID_t targetId, UInt32 incIntimacy, UInt32 finalIntimacy);

	/**
	*@brief 组队通关地下城
	*/
	void OnTeamClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 组队通关活动地下城
	*/
	void OnTeamClearActivityDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 组队通关深渊地下城
	*/
	void OnTeamClearHell(UInt32 dungeonId, UInt32 usedTime, LevelScore score, std::vector<ObjID_t>& teammateIds);

	/**
	*@brief 玩家增加日常任务积分(活跃度)
	*/
	void OnIncDailyTaskScore(UInt32 totalScore);

	/*
	*@brief 收徒事件
	*/
	void OnApprentDisciple();

	/*
	*@brief 徒弟出师事件
	*/
	void OnDiscipleFinsch();

	/*
	*@brief 公会徽记升级
	*/
	void OnGuildEmblemUp(UInt32 lvl);

	/**
	*@brief 通关地下城 dungeonId - 地下城id，包含难度信息; score - 关卡评分; usedTime - 使用时间, reviveCount - 复活次数
	*/
	void OnClearDungeonCareScore(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount);

	/*
	 *@brief 团本通关
	 */
	void OnClearTeamCopy(UInt32 teamType, UInt32 grade, UInt32 costTime);

public:
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void SaveToDB();

protected:

	//任务map
	TaskMap		m_Tasks;

	//拥有者
	Player*		m_pOwner;
};

class GetTaskInfoVisitor : public BaseTaskVisitor
{
public:
	GetTaskInfoVisitor(std::vector<TaskBriefInfo>& taskList)
		:m_TaskList(taskList) {}

	bool operator()(BaseTask* task)
	{
		m_TaskList.resize(m_TaskList.size() + 1);
		task->GetBriefInfo(*(m_TaskList.end() - 1));
		return true;
	}
private:
	std::vector<TaskBriefInfo>& m_TaskList;
};

#endif