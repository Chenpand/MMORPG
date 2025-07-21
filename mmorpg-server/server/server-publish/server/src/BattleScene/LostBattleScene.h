#ifndef _LOST_BATTLE_SCENE_H_
#define _LOST_BATTLE_SCENE_H_

#include "Scene.h"
#include <functional>
#include "DungeonEntrySceneDynamic.h"

/**
*@brief 迷失地牢战场场景
*/
class LostBattleScene : public Scene
{
	AVALON_DEFINE_OBJECTPOOL(LostBattleScene, Avalon::NullMutex)

public:
	LostBattleScene();
	~LostBattleScene();

public:
	bool Load(UInt32 mapid, const std::string& config);
	UInt8 GetType() const { return SCENE_TYPE_LOST_DUNGEON; }

	//战场id
	inline void SetBattleId(UInt32 battleId) { m_battleId = battleId; }
	inline UInt32	GetBattleId() { return m_battleId; }
	inline void SetBattleDataId(UInt32 battleDataId) { m_battleDataId = battleDataId; }
	UInt32	GetBattleDataId() { return m_battleDataId; }
	inline void SetFloor(UInt32 floor) { m_floor = floor; }
	inline UInt32 GetFloor() { return m_floor; }

	void Init();

	void Destroy(UInt8 reason);

	virtual void KickPlayersOut();
	virtual void KickPlayerOut(Player* player);
	virtual void OnKickPlayerTimeout();

	UInt64 GenResourceId();

	void GenMonsters();

	void GenResources();

	void GenDugeonEntrySceneDynamics();

	void DelAllDungeonEntrySceneDynamics();

	bool VistResourceItems(const std::function<bool(SceneItem*)>& ff);

	SceneItem* FindResourceItem(UInt64 guid);

	void RemoveResourceItem(UInt64 guid);

	UInt32 OnPlayerPickUpItem(Player* player, UInt64 guid);

	const ScenePos* RandPosition();

	virtual void OnPlayerEnter(Player* player);

	virtual void OnTick(const Avalon::Time& now);

	virtual void  OnMonsterKilled(CityMonster* monster);

	/**
	 *@brief 场景退出
	 */
	virtual void OnSceneQuit();

	void BroadcastToDungeonEntrys(Avalon::Protocol& protocol);
public:	// 爬塔任务相关

	// 任务领取数量
	void AddTaskAcceptNum(UInt32 taskId);
	UInt32 GetTaskAcceptNum(UInt32 taskId);
	void RemoveTaskAcceptNum(UInt32 taskId);

	// 刷新击杀指定人数的任务
	void RefreshFixNumTask();

	// 获取爬塔战场人数
	virtual UInt32 BattlePlayerNum() { return m_PlayerSet.size(); }
	// 随机获得一个玩家
	UInt64 RandGetPlayer(UInt64 exceptId);
	// 随机获取多个玩家
	std::set<UInt64>& RandGetPlayerList();

	void OnPlayerBeforeEnterBattle(Player* player);
	//玩家进入战场
	void OnPlayerEnterBattle(Player* player);
	//玩家离开战场
	void OnPlayerLeaveBattle(Player* player, UInt8 result);
	//是否战场出口
	bool IsBattleExitDoor(UInt32 doorId);
	//玩家战斗内复活
	void OnPlayerRevive(ObjID_t playerId, ObjID_t targetId, UInt32 reviveId, UInt32 reviveItem, UInt32 reviveItemNum);
	//设置延迟关闭
	void SetDelayClose(bool delayClose) { m_IsDelayClosing = delayClose; }
private:
	//战场id(battleServer生成)
	UInt32	m_battleId;
	//战场表id
	UInt32  m_battleDataId;

	UInt32	m_floor;

	//传送点
	std::map<UInt32, std::vector<ScenePos>>		m_TransferNodes;
	
	//任务领取数量
	std::map<UInt32, UInt32>	m_TaskAcceptMap;

	//玩家
	std::set<UInt64>	m_PlayerSet;

	//资源点坐标
	std::map<UInt32, std::vector<ScenePos>>		m_ResourceNodes;
	std::vector<ScenePos>						m_ResourceVec;

	std::vector<SceneItem*>						m_AllItems;

	UInt64 m_GuidGen;

	// 活动结束战场有人延后结束定时器
	Avalon::SimpleTimer m_DestroyDelayTimer;
	// 活动结束战场有人保底结束定时器
	Avalon::SimpleTimer m_DestroyDelayGuardTimer;

	//是否延迟关闭
	bool  m_IsDelayClosing;
	bool  m_IsGuardDelayClosing;

	// 是否重选杀指定人数的任务
	bool m_IsRefreshKillFixNumTask;
	// 击杀指定人数的任务
	Avalon::SimpleTimer m_KillFixNumTimer;

	//场景内创建的动态地下城入口场景
	std::set<DungeonEntrySceneDynamicKey> m_DungeonEntrySceneDynamics;

};

#endif
