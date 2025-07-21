#ifndef __US_CHAMPION_RACE_H__
#define __US_CHAMPION_RACE_H__


#include "ChampionPlayer.h"
#include <CLGameSessionDefine.h>
#include "USPlayerMgr.h"
#include "USceneMgr.h"
#include "ChampionService.h"

/**
*@brief 冠军赛组信息管理
*/
class ChampionGroupInfoMgr : public Avalon::Singleton<ChampionGroupInfoMgr>
{
public:
	/**
	*@brief 初始化
	*/
	bool Init();
	/**
	*@brief 清理内部信息，会广播组信息
	*/
	void Clear();
	/**
	*@brief 压入一组信息并广播本组
	*/
	void Push(ChampionGroupStatus& groupInfo);
	/**
	*@brief 改变某组状态并广播本组
	*/
	void ChangeStatus(UInt32 groupID, UInt8 status);
	/**
	*@brief 组比分改变时，并广播本组信息
	*/
	void OnChanged(UInt32 groupID, ChampionPlayer* roleA, ChampionPlayer* roleB);
	/**
	*@brief 同步所有组的信息
	*/
	void SyncAllGroupInfo();
	/**
	*@brief 同步一组信息
	*/
	void SyncOneGroupInfo(UInt32 groupID);
	/**
	*@brief 获取一组信息
	*/
	ChampionGroupStatus& GetGroupInfo(UInt32 groupID);
private:
	//组id->组信息，发给客户端用
	std::map<UInt32, ChampionGroupStatus> m_GroupStatus;
};

class ChampionGroup
{
public:
	/**
	*@brief 获取设置组id
	*/
	inline const UInt32 GetID() const { return m_GroupID; }
	inline void SetID(UInt32 id) { m_GroupID = id; }
	/**
	*@brief 把该角色压入本组
	*/
	void Push(ChampionPlayer* player);
	/**
	*@brief 压入新的本组战斗记录，仅在内存里不保存
	*/
	void PushRecord(ChampionScoreBattleRecord& record);
	/**
	*@brief 查找本组某角色
	*/
	ChampionPlayer* Find(UInt64 roleId);
	/**
	*@brief 获取本组排名前top的角色
	*/
	std::vector<ChampionPlayer*> GetTopScorePlayer(UInt32 top);
	/**
	*@brief 修改本组所有角色，暂未实现
	*/
	void ModifyAllPlayer(std::function<void(ChampionPlayer*)> fun);
	/**
	*@brief 本组开始战斗
	*/
	void StartBattle(ChampionStatus status);
	/**
	*@brief 获取本组所有角色
	*/
	std::vector<ChampionPlayer*>& GetPlayerVec() { return m_PlayerVec; }
	/**
	*@brief 获取本组该角色的对手，暂未使用
	*/
	ChampionPlayer* GetEnemyPlayer(UInt64 roleId);
	/**
	*@brief 本组角色分数改变
	*/
	void OnPlayerScoreChanged(ChampionPlayer* player);
	/**
	*@brief 获取本组当前积分排行
	*/
	const std::list<ChampionPlayer*>& GetScoreRankList() const;
	/**
	*@brief 获取本组当前积分赛战斗记录
	*/
	const std::vector<ChampionScoreBattleRecord>& GetScoreRecord() const;
	/**
	*@brief 设置本组战斗为结束状态
	*/
	void SetEnd() { m_IsEnd = true; }
	/**
	*@brief 向本组角色广播消息
	*/
	void BroadCast(Avalon::Protocol& protocol);
private:
	// 组id
	UInt32 m_GroupID;
	// 在本组的玩家
	std::vector<ChampionPlayer*> m_PlayerVec;
	//本组第几场，目前只在8强赛及之后生效，用来区分一场比赛
	UInt32 m_Order = 0;
	//积分赛排行榜
	std::list<ChampionPlayer*> m_RankList;
	//积分赛战斗记录
	std::vector<ChampionScoreBattleRecord> m_ScoreRecords;
	// 本组战斗是否已结束
	bool m_IsEnd = false;
};

/**
*@brief 冠军赛状态对象
*/
class USChampionStatus
{
public:
	USChampionStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : m_Service(service),m_PlayerMgr(playerMgr), m_SceneMgr(sceneMgr) {}
	virtual ~USChampionStatus();
	/**
	*@brief Tick
	*/
	virtual void OnTick(const Avalon::Time& now);
	/**
	*@brief 当准备阶段开始
	*/
	virtual void OnPrepareStart();
	/**
	*@brief 当准备阶段结束
	*/
	virtual void OnPrepareEnd();
	/**
	*@brief 当战斗阶段开始
	*/
	virtual void OnBattleStart();
	/**
	*@brief 当战斗阶段结束
	*/
	virtual void OnBattleEnd();
	/**
	*@brief 当角色比赛结束
	*/
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr) = 0;
	/**
	*@brief 当角色进入本状态的场景
	*/
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
	/**
	*@brief 获取本状态的准备阶段
	*/
	virtual const ChampionStatus GetPrepareStatus() const = 0;
	/**
	*@brief 获取本状态的战斗阶段
	*/
	virtual const ChampionStatus GetBattleStatus() const = 0;
	/**
	*@brief 是否是积分状态
	*/
	bool IsScoreStatus() const;
	/**
	*@brief 当角色积分改变
	*/
	void OnPlayerScoreChanged(ChampionPlayer* player);
	/**
	*@brief 当某组比赛全部结束
	*/
	void OnGroupBattleEnd(UInt32 groupID);
	/**
	*@brief 当某组战斗改变
	*/
	void OnGroupBattleChanged(UInt32 groupID, ChampionPlayer* winner, ChampionPlayer* loser);
	/**
	*@brief 所有组战斗开始
	*/
	void OnAllGroupBattleStart();
	/**
	*@brief 玩家获取自己组的积分赛排行
	*/
	const std::list<ChampionPlayer*>& OnPlayerGetGroupScoreRank(ChampionPlayer* player);
	/**
	*@brief 玩家获取自己组的积分赛战斗记录
	*/
	const std::vector<ChampionScoreBattleRecord>& OnPlayerGetGroupScoreRecord(ChampionPlayer* player);
	//各种同步
	/**
	*@brief 同步战斗开始倒计时
	*/
	void SyncBattleCountdown(ChampionPlayer* player);
	/**
	*@brief 同步积分赛排行前几
	*/
	void SyncScoreRankTop(ChampionPlayer* player);
	/**
	*@brief 同步组id
	*/
	void SyncGroupID(ChampionPlayer* player);
	/**
	*@brief 同步bo5比分
	*/
	void SyncKnockoutScore(ChampionPlayer* player);
protected:
	/**
	*@brief 获得该阶段的玩家
	*/
	USPlayerVec GetStatusPlayer(ChampionStatus status);
	/**
	*@brief 将该阶段玩家分这么多组
	*/
	void GroupPlayer(ChampionStatus status, UInt32 groupNum);
	/**
	*@brief 根据积分将该阶段玩家分这么多组
	*/
	void GroupPlayerByScore(ChampionStatus status, UInt32 groupNum);
	/**
	*@brief 根据上一次分组分组，8强赛及之后会用
	*/
	void GroupPlayerByLastGroup(ChampionStatus status);
	/**
	*@brief 根据排名将该阶段玩家分组
	*/
	void GroupPlayerByRank(ChampionStatus status);
	/**
	*@brief 将该阶段角色归入自己组内
	*/
	void GroupPlayerByStatus(ChampionStatus status);
	/**
	*@brief 角色归入该组
	*/
	void PlayerBelongToGroup(ChampionPlayer* player, UInt32 groupId);
	/**
	*@brief 生成场景
	*/
	void GenScene(UInt32 num);
	/**
	*@brief 将该状态玩家压入对应场景
	*/
	void PushAllGroupPlayerToScene(ChampionStatus status);
	/**
	*@brief 将该状态玩家压入一个场景
	*/
	void PushAllGroupPlayerToOneScene(ChampionStatus status);
	/**
	*@brief 积分赛结束战斗
	*/
	UInt32 OnPlayerBattleEndScore(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result);
	/**
	*@brief 淘汰赛结束战斗
	*/
	UInt32 OnPlayerBattleEndCount(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result);
	/**
	*@brief BO5结束战斗
	*/
	UInt32 OnPlayerBattleEnd3Win(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info);
	/**
	*@brief 锁定所有在场景中的玩家
	*/
	void LockOnlinePlayerForBattle();
	/**
	*@brief 清除状态内所有组内玩家胜负战斗场数
	*/
	void ClearPlayerWinCount();
	/**
	*@brief 清除所有组
	*/
	void ClearGroup();
protected:
	//被持有的服务
	ChampionService& m_Service;
	//服务持有的PlayerMgr
	USPlayerMgr& m_PlayerMgr;
	//服务持有的Scene
	USceneMgr& m_SceneMgr;
	//组id-》组
	std::map<UInt32, ChampionGroup> m_GroupMap;

private:
	
	// 匹配定时器每6min一次战斗
	Avalon::SimpleTimer							m_BattleTimer;
	// 积分前几排行同步定时器
	Avalon::SimpleTimer							m_ScoreTopSyncTimer;
};


class SeaStatus : public USChampionStatus
{
public:
	SeaStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_SEA_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_SEA_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
private:
	UInt32 m_CurrSceneId = 1;
};

class ReSeaStatus : public USChampionStatus
{
public:
	ReSeaStatus(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_RE_SEA_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_RE_SEA_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
private:

};

class Select64Status : public USChampionStatus
{
public:
	Select64Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_64_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_64_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select16Status : public USChampionStatus
{
public:
	Select16Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_16_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_16_SELECT_BATTLE; }
	virtual void OnPrepareStart();
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select8Status : public USChampionStatus
{
	
public:
	Select8Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_8_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_8_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select4Status : public USChampionStatus
{
public:
	Select4Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_4_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_4_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select2Status : public USChampionStatus
{
public:
	Select2Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_2_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_2_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};

class Select1Status : public USChampionStatus
{
public:
	Select1Status(ChampionService& service, USPlayerMgr& playerMgr, USceneMgr& sceneMgr) : USChampionStatus(service, playerMgr, sceneMgr) {}
	virtual const ChampionStatus GetPrepareStatus() const { return CS_1_SELECT_PREPARE; }
	virtual const ChampionStatus GetBattleStatus() const { return CS_1_SELECT_BATTLE; }
	virtual UInt32 OnPlayerRaceEnd(ChampionPlayer* winner, ChampionPlayer* loser, _resultBitmask result, BattleResultActInfo* info = nullptr);
	virtual void OnPrepareStart();
	virtual void OnBattleStart();
	virtual void OnBattleEnd();
	virtual UInt32 OnPlayerJoinRace(ChampionPlayer* player);
};





#endif



