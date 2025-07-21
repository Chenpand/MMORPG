#ifndef __DUNGEON_RACE_H__
#define __DUNGEON_RACE_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <CLDungeonDataEntry.h>
#include <CLDungeonDefine.h>
#include <CLDungeonProtocol.h>
#include "DungeonArea.h"
#include "DungeonPlayer.h"
#include "DungeonChestTask.h"
#include <CLItemDefine.h>
#include <CLDungeonPrefixLimitDataEntry.h>
#include <CLSortListDefine.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <CLCheatDefine.h>
#include <CLGameSessionProtocol.h>

enum DungeonRaceStatus
{
    DRS_INIT,           // 初始状态
	DRS_CREATING,		// 创建中
    DRS_RACING,         // 比赛中
    DRS_RACE_END,       // 结算中
    DRS_WAIT_CLOSE,     // 等待关闭
};

enum DungeonRaceTransactionType
{
	DUNGEON_RACE_OPEN_PAY_CHEST,
	DUNGEON_RACE_MULTI_RACE_END_DROP,
	DUNGEON_RACE_REVIVE,
};

struct DungeonMonsterGenerateContext;


class DungeonRace
{
    AVALON_DEFINE_OBJECTPOOL(DungeonRace, Avalon::NullMutex)
public:
    DungeonRace();
    ~DungeonRace();

    void Tick(const Avalon::Time& now);

public:
	/**
	*@brief 各种Get/Set函数
	*/

	inline void SetID(UInt64 id) { m_id = id; }
	inline UInt64 GetID() const { return m_id; }

	inline void SetSessionId(ObjID_t session) { m_sessionId = session; }
	inline ObjID_t GetSessionId() { return m_sessionId; }

	inline void SetServiceId(UInt32 serviceId) { m_serviceId = serviceId; };
	inline UInt32 GetServiceId() { return m_serviceId; }

	inline void SetTeamID(UInt32 id) { m_teamId = id; }
	inline UInt32 GetTeamID() const { return m_teamId; }

	inline void SetStatus(DungeonRaceStatus status) { m_status = status; }
	inline DungeonRaceStatus GetStatus() const { return m_status; }

	inline void SetDungeonId(UInt32 dungeonRaceId) { m_dungeonId = dungeonRaceId; }
	inline UInt32 GetDungeonID() const { return m_dungeonId; }

	inline void SetDungeonData(DungeonDataEntry* entry) { m_dungeonData = entry; }
	inline DungeonDataEntry* GetDungeonData() const { return m_dungeonData; }

	inline void SetPassedAreaIndex(UInt32 index) { m_passedAreaIndex = index; }
	UInt32 GetPassedAreaIndex() { return m_passedAreaIndex; }

	inline UInt32 GetStartAreaID() const { return m_startAreaId; }

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	inline ObjID_t GetSession() const { return m_sessionId; }

	void SetRelayServerID(UInt32 id) { m_relayServerId = id; }
	inline UInt32 GetRelayServerID() const { return m_relayServerId; }

	void SetIsSettled(bool isSettled) { m_isSettled = isSettled; };
	bool IsSettled() { return m_isSettled; }

	void SetClearedDungeonIds(std::vector<UInt32>& clearedDungeonIds) { m_clearedDungeonIds = clearedDungeonIds; };
	std::vector<UInt32>& GetClearedDungeonIds() { return m_clearedDungeonIds; };

	void OnDestory();

	inline UInt32 GetLastFrame() const { return m_lastFrame; }

	void SetPlayerInfos(std::vector<DungeonRacePlayerInfo>& playerInfos) { m_playerInfoes = playerInfos; }
	std::vector<DungeonRacePlayerInfo>& GetDungeonRacePlayerInfo() { return m_playerInfoes; }

	void SetFighters(std::vector<CLProtocol::DungeonFighter>& fighters) { m_fighters = fighters; }
	const std::vector<CLProtocol::DungeonFighter>& GetFighters() const { return m_fighters; }
	bool GetFighter(ObjID_t roleId, CLProtocol::DungeonFighter& fighter);

	inline void SetAddTeamReviveCount(UInt32 cnt) { m_AddTeamReviveCount = cnt; }
	inline UInt32 GetAddTeamReviveCount() { return m_AddTeamReviveCount; }

	inline void IncTeamReviveCount() { m_teamReviveCount++; }
	inline UInt32 GetTeamReviveCount() { return m_teamReviveCount; }

	inline const SceneNpc& GetMonster() const { return m_monster; }
	void SetMonster(const SceneNpc& monster) { m_monster = monster; }

	inline UInt64 GetBattleTempRaceId() const { return m_battleTempRaceId; }
	void SetBattleTempRaceId(UInt64 tempRaceId) { m_battleTempRaceId = tempRaceId; }
	
	void SetSourceSceneNodeId(UInt32 nodeId) { m_sourceSceneNodeId = nodeId; };
	UInt32	GetSourceSceneNodeId() { return m_sourceSceneNodeId; }
	/**
	*@brief 玩家相关
	*/
	inline const MapDungeonPlayer& GetPlayerList() const { return m_players; }
	void AddPlayer(const DungeonRacePlayerInfo& info);
	DungeonPlayer* FindDungeonPlayer(UInt64 roleId);

	/**
	*@brief 是否组队模式（大于等级2个玩家）
	*/
	bool IsTeamMode() const { return m_players.size() > 0; }

	/**
	*@brief 玩家离开比赛
	*/
	void OnPlayerLeave(UInt64 roleId, bool isDisconnect);

	/**
	*@brief 是不是所有玩家都离开比赛了
	*/
	bool IsAllPlayerLeave();

	/**
	*@brief 发送日志
	*/
	void SendDungeonLogWhenAllPlayerLeave();

	/**
	*@brief 是否有玩家断线
	*/
	bool HasPlayerDisconnect();

	/**
	*@brief 获取Area信息
	*/
	void GetAreaInfo(UInt64 roleId, std::vector<CLProtocol::SceneDungeonArea>& areas);

	/**
	*@brief 获取该区域掉落信息
	*/
	void GetAreaDropInfo(UInt64 roleId, UInt32 areaId, CLProtocol::SceneDungeonEnterNextAreaRes& res);

	/**
	*@brief 获取深渊信息
	*/
	void GetHellInfo(UInt64 roleId, CLProtocol::DungeonHellInfo& info);

	/**
	*@brief 是否需要被摧毁
	*/
	bool IsTimeToDestory(const Avalon::Time& now);

	/**
	*@brief 玩家是否已经结算过
	*/
	bool IsPlayerRaceEnd(ObjID_t id);

	/**
	*@brief 获取地下城房间索引
	*/
	static UInt32 GetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief 设置地下城房间索引，按位存储房间索引，起始为1
	*/
	static UInt32 SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief 重置地下城房间索引
	*/
	static UInt32 ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief 是否设置地下城房间索引
	*/
	static bool IsSetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

public:
	/* 玩家相关操作 */
	
	/**
	*@brief 开始战斗（计算区域，怪物，返回客户端...）
	*/
	UInt32 StartRace(ObjID_t sessionId, const Avalon::SockAddr& addr, UInt32 relayServerId);

	/*
	*@brief 通知玩家开始战斗返回
	*/
	void NotifyPlayerStartDunegonRes();

	/**
	*@brief 玩家进入游戏
	*/
	bool OnPlayerEnterRace(DungeonPlayer* dungeonPlayer);

	/**
	*@brief 结算
	*/
	bool OnPlayerRaceEnd(DungeonPlayer* player, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame = 0, UInt32 maxDamage = 0, UInt32 skillId = 0, UInt32 param = 0, UInt64 totalDamage = 0, UInt64 bossDamage = 0);

	/**
	*@brief 结算比赛
	*/
	bool OnRaceEnd(UInt8 reason, DungeonRaceEndInfo& raceEndInfo);

	/**
	*@brief 玩家杀死怪物
	*/
	void OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<UInt32>& monsterIds);
	void OnPlayerKilledMonster(DungeonPlayer* dungeonPlayer, std::vector<DungeonKilledMonsterInfo>& monsters);

	/**
	*@brief 玩家杀完当前房间的所有怪物
	*/
	void OnPlayerClearAreaMonsters(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame);

	/**
	*@brief 玩家进入新的区域
	*/
	UInt32 OnPlayerEnterNextArea(DungeonPlayer* dungeonPlayer, UInt32 areaId);

	/**
	*@brief 玩家拾取掉落
	*/
	void OnPlayerPickUpDropItems(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief 玩家请求结算掉落
	*/
	bool OnPlayerRequestRaceEndDrop(DungeonPlayer* dungeonPlayer, UInt8 multiple);

	/**
	*@brief 玩家请求复活玩家（可以复活别人）
	*/
	UInt32 OnPlayerReviveRequest(DungeonPlayer* dungeonPlayer, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum,
		UInt32  reviveItem, UInt32 reviveItemNum, UInt32 reviveHp, UInt32 reviveMp);

public:
	/**
	*@brief 玩家上报帧和随机数校验
	*/
	UInt32 OnPlayerReportFrame(DungeonPlayer* dungeonPlayer, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums);

public:
	/* 事务相关 */

	/**
	*@brief 复活
	*/
	UInt32 OnCheckReviveRet(DungeonPlayer* player, UInt32 reviveId, ObjID_t targetId, UInt32 hp, UInt32 mp);

public:
	/* 验证服务器相关 */

	/**
	*@brief 创建验证任务返回
	*/
	void OnCreateVerifyReturn(UInt32 nodeid);

private:
	/**
	*@brief 是否正在验证
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief 开始验证
	*/
	void _StartVerify();

	/**
	*@brief 发送消息到验证服务器
	*/
	void _SendToVerifyServer(Avalon::Protocol& protocol);

private:
	/* 关卡数据生成相关 */

	/**
	*@brief 计算所有区域（怪物、掉落等）
	*/
	bool _GenAreas();

	/**
	*@brief 计算起始区域
	*/
	bool _GenStartArea();

	/**
	*@brief 计算单局怪物
	*/
	void _GenMonsters(DungeonMonsterGenerateContext& context, DungeonArea& area);

	/**
	*@brief 计算BOSS掉落
	*/
	bool _GenBossDropItems();

	/**
	*@brief 计算怪物词缀信息
	*/
	void _GenMonsterPrefix();

	/**
	*@brief 计算掉落物
	*/
	void _GenDropItems(DungeonMonster& monster);

	/**
	*@brief 计算掉落物
	*/
	std::vector<PlayerDropItem> _GenDropItemsByDropSet(DungeonPlayer* player, std::vector<UInt32> dropSetIds, UInt32 monsterId, UInt32 monsterTypeId);

	/**
	*@brief 计算通用掉落方案ID
	*/
	UInt32 _GetCommonDropSetID(MonsterType type);

	/**
	*@brief 生成掉落ID
	*/
	UInt32 _GenDropItemID();

	/**
	*@brief 组队奖励
	*/
	ItemReward _GenTeamReward(DungeonPlayer* player, LevelScore score);

private:
	/* 内部使用的一些函数 */

	/**
	*@brief 获取战斗开始的数据
	*/
	void _GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res);

	/**
	*@brief 杀怪
	*/
	bool _OnPlayerKillMonster(DungeonPlayer* dungeonPlayer, DungeonMonster* monster);

	/**
	*@brief 查找掉落物
	*/
	PlayerDropItem* _FindDropItem(UInt32 itemId, DungeonPlayer* player = NULL, bool mustEnteredArea = 0);

	/**
	*@brief 随机物品的强化等级
	*/
	UInt8 _RandomItemStrenth(UInt32 itemId);

	/**
	*@brief 把物品放到ItemReward数组中，尝试与之前的物品叠加
	*/
	void _PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt16 num, UInt8 strenth = 0, UInt8 equipType = 0);

	/**
	*@brief 获取结算经验奖励
	*/
	UInt32 _GetRaceEndExpReward(DungeonPlayer* player, LevelScore score, DungeonAdditionInfo& addition);

	/**
	*@brief 获取地下城的结算经验
	*/
	UInt32 _GetDungeonRaceEndExp(UInt32 dungeonId, UInt16 level);

	/**
	*@brief 计算适配等级（0代表不需要适配）
	*/
	UInt16 _GetAdapterLevel();

	/**
	*@brief 根据areaId获取area
	*/
	DungeonArea* _FindArea(UInt32 areaId);

	/**
	*@brief 查找怪物
	*/
	DungeonMonster* _FindMonster(UInt32 id);

	/**
	*@brief 计算关卡的词缀怪数量信息
	*/
	DungeonPrefixLimitDataEntry* _GetPrefixMonsterLimit(DungeonHardType type, UInt32 level);

	/**
	*@brief 计算单个区域的词缀怪最大数量
	*/
	UInt8 _GetPerAreaPrefixMonsterMaxNum(UInt32 totalNum);

	/**
	*@brief 结算掉落加倍消耗，如果返回UINT32_MAX代表错误的倍率
	*/
	UInt32 _MultiRaceEndDropCost(UInt8 multi);

	/**
	*@brief 获取基础倍数
	*/
	UInt8 _GetRaceEndDropBaseMulti(DungeonPlayer* dungeonPlayer);

	/**
	*@brief 广播消息
	*/
	void _BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	*@brief 获取地下城房间进度
	*/
	UInt32 _GetIndexByAreaID(UInt32 areaId);

	/**
	*@brief 通过地下城ID获取普通难度的地下城ID
	*/
	static UInt32 _GetNormalHardDungeonID(UInt32 dungeonId);

	/**
	*@brief 将玩家杀了怪，但是没有发拾取的道具发给玩家
	*/
	void _GiveOnPickDropItem(DungeonPlayer* dungeonPlayer);

private:
	// 作弊判断

	/**
	*@brief 检查杀怪的伤害信息是否合法
	*/
	bool _CheckMonsterDamage(DungeonPlayer* dungeonPlayer, DungeonMonster* monster, const DungeonKilledMonsterInfo& info);


	/**
	*@brief 检测地下城使用时间
	*/
	bool _CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime);

	/**
	*@brief 玩家作弊的处理
	*/
	void _OnPlayerCheat(DungeonPlayer* dungeonPlayer, PlayerCheatReason cheatReason);

private:
	/**
	*@brief 检测拾取掉落请求的md5
	*/
	bool _CheckDropRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 lastFrame, const std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief 检测清图请求的md5
	*/
	bool _CheckClearAreaRequestMd5(DungeonPlayer* dungeonPlayer, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16]);

	/**
	*@brief 检测结算请求的md5
	*/
	bool _CheckRaceEndRequestMd5(DungeonPlayer* dungeonPlayer, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16]);

	/**
	*@brief 生成一个加密的密钥
	*/
	void _GenEncryptKey(CLProtocol::SceneDungeonStartRes& res);

public:
	bool SendToService(Avalon::Protocol& protocol);

	bool SendToRelay(Avalon::Protocol& protocol);
private:
	// ID
	UInt64                  m_id;

	// 队伍ID
	UInt32					m_teamId;

	// 开始时间
	UInt64					m_startTime;

	// RelayServerID
	UInt32					m_relayServerId;

	// Session
	ObjID_t					m_sessionId;

	// 状态
	DungeonRaceStatus       m_status;

	// 地下城区域
	VDungeonArea            m_areas;

	// 所有玩家
	MapDungeonPlayer        m_players;

	// 目标地下城ID
	UInt32                  m_dungeonId;

	// 目标地下城数据
	DungeonDataEntry*       m_dungeonData;

	// 起始区域
	UInt32                  m_startAreaId;

	// 这个area之前的area就不需要生成了（包括这个area）
	UInt32					m_passedAreaIndex;

	// 掉落物ID种子
	UInt32					m_dropItemSeed;

	// 比赛被摧毁时间
	Avalon::Time            m_destoryTime;

	// 已经执行过的复活任务
	std::set<UInt32>		m_reviveTasks;

	// 加密密钥
	UInt32					m_encryptKey;

	// 收到的最后一帧
	UInt32					m_lastFrame;

	// 所有帧
	std::vector<CLProtocol::Frame>	m_frames;

	// 所有随机数
	std::vector<CLProtocol::FrameChecksum>	m_checksums;

	// 验证服务器节点
	UInt32					m_VSId;

	// 是否已经发过战斗结束消息
	bool						m_HasSendRaceOver;
	
	// 是否已经记录通关次数(按队伍计次数)
	bool						m_flagGuildDungeon;

	// 玩法服务器连接id
	UInt32						m_serviceId;

	// 玩家信息
	std::vector<DungeonRacePlayerInfo>	m_playerInfoes;

	// 是否结算过
	bool						m_isSettled;

	// 已经通过地下城id
	std::vector<UInt32>         m_clearedDungeonIds;

	// fighter数据
	std::vector<CLProtocol::DungeonFighter>	m_fighters;

	// 增加的可复活次数
	UInt32					m_AddTeamReviveCount;

	// 复活次数
	UInt32					m_teamReviveCount;	

	// 战场小怪
	SceneNpc					m_monster;

	// 战场生成的临时raceId
	UInt64				m_battleTempRaceId;

	// 源scene node id
	UInt64				m_sourceSceneNodeId;
};

#endif
