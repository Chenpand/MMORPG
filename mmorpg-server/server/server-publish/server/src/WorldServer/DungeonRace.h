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

#include "DungeonRoll.h"

enum DungeonRaceStatus
{
    DRS_INIT,           // 初始状态
    DRS_WAIT,           // 等待比赛开始
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

enum BattleFlagType
{
	Eff_Devilddom_hidden_room = 7,
	// 是否是单人
	Eff_Single_race = 32
};

class WSPlayer;
struct DungeonMonsterGenerateContext;

// 掉落统计key
struct DropKey
{
	DropKey(UInt32 itemId = 0, UInt8 equipType_ = 0) : itemTypeId(itemId), equipType(equipType_) {}

	bool operator<(const DropKey& other) const
	{
		if (itemTypeId != other.itemTypeId)
		{
			return itemTypeId < other.itemTypeId;
		}

		return equipType < other.equipType;
	}
	// 道具ID
	UInt32	itemTypeId;
	// 装备类型
	UInt8	equipType;
};

struct DropCount
{
	UInt64 times;
	UInt64 num;
};

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

	inline void SetTeamID(UInt32 id) { m_teamId = id; }
	inline UInt32 GetTeamID() const { return m_teamId; }

	inline void SetStatus(DungeonRaceStatus status) { m_status = status; }
	inline DungeonRaceStatus GetStatus() const { return m_status; }

	inline void SetDungeonId(UInt32 dungeonRaceId) { m_dungeonId = dungeonRaceId; }
	inline UInt32 GetDungeonID() const { return m_dungeonId; }

	inline void SetHellDungeonEntryID(UInt32 id) { m_hellDungeonEntryId = id; }

	inline void SetDungeonData(DungeonDataEntry* entry) { m_dungeonData = entry; }
	inline DungeonDataEntry* GetDungeonData() const { return m_dungeonData; }

	inline void SetPassedAreaIndex(UInt32 index) { m_passedAreaIndex = index; }

	inline bool IsHell() const { return m_isHellMode; }
	inline void SetHell() { m_isHellMode = true; }

	inline DungeonHellMode GetHellMode() const { return m_hellHardMode; }

	inline UInt32 GetStartAreaID() const { return m_startAreaId; }

	inline void SetDestoryTime(Avalon::Time closeTime) { m_destoryTime = closeTime; }
	inline ObjID_t GetSession() const { return m_sessionId; }

	inline UInt32 GetRelayServerID() const { return m_relayServerId; }

	inline UInt32 GetLastFrame() const { return m_lastFrame; }

	inline std::map<DropKey, DropCount>& GetDropSta() { return m_DropCount; };

	inline const SceneNpc& GetCityMonster() const { return m_CityMonster; }
	void SetCityMonster(const SceneNpc& cityMonster) { m_CityMonster = cityMonster; }

	void SetZjslDungeonInfo(const CLProtocol::ZjslDungeonInfo& info) { m_ZjslDungeonInfo = info; }

	/**
	*@brief 玩家相关
	*/
	inline const MapDungeonPlayer& GetPlayerList() const { return m_players; }
	void AddPlayer(const DungeonRacePlayerInfo& info);
	DungeonPlayer* FindDungeonPlayer(UInt64 roleId);

	/**
	*@brief 是否组队模式（大于等级2个玩家）
	*/
	bool IsTeamMode() const { return m_players.size() > 1; }

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
	UInt32 SetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief 重置地下城房间索引
	*/
	static UInt32 ResetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId);

	/**
	*@brief 是否设置地下城房间索引
	*/
	static bool IsSetDungeonAreaIndex(UInt64 roleId, UInt32 dungeonId, UInt32 areaIndex);

	/**
	*@brief 结束公会地下城
	*/
	void EndGuildDungeon();

	/**
	*@brief 结束公会地下城
	*/
	void NotifyGuildDungeonBossOddBlood(UInt64 oddBlood, UInt64 totalBlood);

public:
	/* 玩家相关操作 */
	
	/**
	*@brief 开始战斗（计算区域，怪物，返回客户端...）
	*/
	UInt32 StartRace(ObjID_t sessionId, const Avalon::SockAddr& addr, UInt32 relayServerId);

	/**
	*@brief 玩家进入游戏
	*/
	bool OnPlayerEnterRace(WSPlayer* player);

	/**
	*@brief 结算
	*/
	bool OnPlayerRaceEnd(WSPlayer* player, LevelScore score, UInt16 beHitCount, UInt32 usedTime, UInt8 md5[16], UInt32 lastFrame = 0, 
							UInt32 maxDamage = 0, UInt32 skillId = 0, UInt32 param = 0, UInt64 totalDamage = 0, UInt64 bossDamage = 0, 
							UInt32 playerRemainHp = 0, UInt32 playerRemainMp = 0, UInt32 boss1ID = 0, UInt32 boss2ID = 0, UInt32 boss1RemainHp = 0, 
							UInt32 boss2RemainHp = 0, UInt32 lastChecksum = 0);

	/**
	*@brief 玩家杀死怪物
	*/
	void OnPlayerKilledMonster(WSPlayer* player, std::vector<UInt32>& monsterIds);

	/**
	*@brief 玩家杀完当前房间的所有怪物
	*/
	void OnPlayerClearAreaMonsters(WSPlayer* player, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp, UInt8 md5[16], UInt32 lastFrame);

	/**
	*@brief 玩家进入新的区域
	*/
	UInt32 OnPlayerEnterNextArea(WSPlayer* player, UInt32 areaId, UInt32 lastFrame, const DungeonStaticValue& staticVal);

	/**
	*@brief 玩家拾取掉落
	*/
	void OnPlayerPickUpDropItems(WSPlayer* player, UInt32 lastFrame, std::vector<UInt32>& dropItemIds, UInt8 md5[16]);

	/**
	*@brief 玩家请求开宝箱
	*/
	bool OnPlayerOpenChest(WSPlayer* player, UInt8 pos);

	/**
	*@brief 玩家请求结算掉落
	*/
	bool OnPlayerRequestRaceEndDrop(WSPlayer* player, UInt8 multiple);

	/**
	*@brief 玩家请求复活玩家（可以复活别人）
	*/
	UInt32 OnPlayerReviveRequest(WSPlayer* player, ObjID_t targetId, UInt32 reviveId, UInt32 reviveCoinNum);

public:
	/**
	*@brief 玩家上报帧和随机数校验
	*/
	UInt32 OnPlayerReportFrame(WSPlayer* player, const std::vector<CLProtocol::Frame>& frames, const std::vector<CLProtocol::FrameChecksum>& checksums);

public:
	/* 事务相关 */

	/**
	*@brief 满足打开付费宝箱的条件
	*/
	bool OnCheckOpenPayChestRet(WSPlayer* player, UInt8 pos);

	/**
	*@brief 满足加倍结算掉落的条件
	*/
	bool OnCheckMultiRaceEndDropRet(WSPlayer* player, UInt8 multi);

	/**
	*@brief 满足加倍结算掉落的条件
	*/
	UInt32 OnCheckReviveRet(WSPlayer* player, UInt32 reviveId, ObjID_t targetId);

public:
	/* 统计掉落数据相关 */

	/**
	*@brief 统计掉落
	*/
	void GenDropStatistic(UInt32 times);

	/**
	*@brief 设置掉落统计状态（开启或关闭统计）
	*/
	void SetDropCountState(bool state) { m_DropCountState = state; }

	/**
	*@brief 掉落数据写入文件
	*/
	void RecordDropCountToFile(WSPlayer* player, UInt32 dungenId, UInt8 isHell, UInt32 times);

public:
	/* 验证服务器相关 */

	/**
	*@brief 创建验证任务返回
	*/
	void OnCreateVerifyReturn(UInt32 nodeid);

	/**
	*@brief 验证成功
	*/
	void OnVerifySuccess(ObjID_t roleId);

private:
	/**
	*@brief 是否正在验证
	*/
	bool _IsVerifying() const { return m_VSId > 0; };

	/**
	*@brief 是否需要验证
	*/
	bool _IsNeedVerify();

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
	*@brief 计算深渊怪
	*/
	bool _GenHellMonsters(DungeonMonsterGenerateContext& context);

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
	*@brief 生成结算宝箱
	*/
	void _GenChest(WSPlayer* player);

	/**
	*@brief 生成深渊柱子房间ID
	*/
	UInt32 _GenHellAreaID();

	/**
	*@brief 组队奖励
	*/
	ItemReward _GenTeamReward(WSPlayer* player, LevelScore score);

	/**
	*@brief 老兵回归掉落奖励
	*/
	ItemReward _GenVeteranReturnDropReward(WSPlayer* player, LevelScore score);

	/**
	*@brief 计算好友经验加成系数
	*/
	void _GenFriendExpRate();

	/**
	*@brief 计算是否触发隐藏房间
	*/
	void _GenTriggerHiddenRoom();
private:
	/* 内部使用的一些函数 */

	/**
	*@brief 获取战斗开始的数据
	*/
	void _GetStartInfo(DungeonPlayer* dungeonPlayer, std::vector<RacePlayerInfo>& racePlayerInfoes, const Avalon::SockAddr& addr, CLProtocol::SceneDungeonStartRes& res);

	/**
	*@brief 杀怪
	*/
	bool _OnPlayerKillMonster(WSPlayer* player, DungeonPlayer* dungeonPlayer, DungeonMonster* monster);

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
	void _PushItemToReward(ItemRewardVec& reward, UInt32 itemId, UInt32 num, UInt8 strenth = 0, UInt8 equipType = 0);

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
	*@brief 地下城不同难度在深渊模式下神器掉落加成
	*/
	UInt16 _HellModeDropAddition();

	/**
	*@brief 结算掉落加倍消耗，如果返回UINT32_MAX代表错误的倍率
	*/
	UInt32 _MultiRaceEndDropCost(UInt8 multi);

	/**
	*@brief 获取基础倍数
	*/
	UInt8 _GetRaceEndDropBaseMulti(DungeonPlayer* dungeonPlayer);

	/**
	*@brief 获取深渊掉落
	*/
	const std::vector<UInt32>&  _GetHellDropID(DungeonHellMode mode);

	/**
	*@brief 广播消息
	*/
	void _BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	*@brief 获取死亡之塔层数
	*/
	UInt32 _GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId);

	/**
	*@brief 获取地下城房间进度
	*/
	UInt32 _GetIndexByAreaID(UInt32 areaId);

	/**
	*@brief 通过地下城ID获取普通难度的地下城ID
	*/
	static UInt32 _GetNormalHardDungeonID(UInt32 dungeonId);

	/**
	*@brief 通知relay比赛事件
	*/
	void _NotifyRelayPlayerEvent(ObjID_t roleId, UInt8 type, ObjID_t param1, ObjID_t param2);

private:
	// 作弊判断

	/**
	*@brief 检测地下城使用时间
	*/
	bool _CheckDungeonUsedTime(DungeonPlayer* dungeonPlayer, UInt32 floor, UInt32 usedTime);

	/**
	*@brief 玩家作弊的处理
	*/
	void _OnPlayerCheat(WSPlayer* player, PlayerCheatReason cheatReason);

	/**
	*@brief 玩家用了加速挂
	*/
	void _OnPlayerSpeedUpCheat(WSPlayer* player);

private:
	/* 结算宝箱相关 */

	/**
	*@brief 查找结算宝箱信息
	*/
	DungeonChestTask* _FindDungeonChestTask(ObjID_t roleId);

	/**
	*@brief 计算一个没有打开的宝箱位置
	*/
	UInt8 _GenUnopenedChestPos();

	/**
	*@brief 根据位置获取宝箱类型
	*/
	DungeonChestType _GetChestTypeByPos(UInt8 pos);

	/**
	*@brief 打开宝箱
	*/
	bool _OpenChest(WSPlayer* player, DungeonChestTask* task, DungeonChestType type, UInt8 pos);

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

	/**
	*@brief 随机神秘商人
	*/
	void _RandMysticalMerchant();

	/**
	*@brief 检查和添加增好友亲密度
	*/
	void  checkAndAddIntimacy(UInt32 areaId);
	
	/**
	*@brief 发送师徒互动log
	*/
	void SendMasterDiscipleInteractionLog();

	/**
	*@brief 清楚隐藏房间标记
	*/
	void _ClearPlayerHiddenRoomFlag();

	/**
	*@brief 生成roll掉落
	*/
	void _GenRollDrops();

	/**
	*@brief roll生成掉落个数(消耗精力玩家个数)
	*/
	UInt32 _GenRollDropNum();

	/**
	*@brief 是否精英地下城
	*/
	bool  _IsEliteDungeon();

	/**
	*@brief 查找rollitem
	*/
	DungeonRollItem* _FindRollItem(UInt8 index);

	/**
	*@brief 结算roll掉落
	*/
	void _SettleRollDrops();

	/**
	*@brief 结算一个rool掉落
	*/
	void _SettleOneRollDrop(DungeonRollItem& rollItem);

	/**
	*@brief 获取唯一非助战玩家
	*/
	bool _GetOnlyNotAssistPlayer(ObjID_t& playerId);

	/**
	*@brief 单人模式结算roll掉落
	*/
	void _SettleRollSingleMode();

	/**
	*@brief 检查是否可以结算roll
	*/
	bool _CheckCanSettleRollDrop();
public:
	/*
	*@brief 处理roll请求
	*/
	UInt32 HandleOneRollReq(WSPlayer* player, UInt8  dropIndex, UInt8 opType, UInt32& rollPoint);

private:
	friend class DungeonRaceMgr;
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

	// 深渊入口ID
	UInt32					m_hellDungeonEntryId;

	// 目标地下城ID
	UInt32                  m_dungeonId;

	// 目标地下城数据
	DungeonDataEntry*       m_dungeonData;

	// 起始区域
	UInt32                  m_startAreaId;

	// 这个area之前的area就不需要生成了（包括这个area）
	UInt32					m_passedAreaIndex;

	// 是否是深渊模式
	bool                    m_isHellMode;

	// 深渊模式具体难度
	DungeonHellMode         m_hellHardMode;

	// 掉落物ID种子
	UInt32					m_dropItemSeed;

	// 掉落数据统计状态
	bool					m_DropCountState;

	// 掉落数据统计
	std::map<DropKey, DropCount> m_DropCount;

	// 比赛被摧毁时间
	Avalon::Time            m_destoryTime;

	// 已经打开了的宝箱掩码（哪一位置代表第几个宝箱已经打开了）
	UInt32                  m_openedChestMask;

	// 宝箱信息
	VDungeonChestTask       m_chestTasks;

	// 已经执行过的复活任务
	std::set<UInt32>		m_reviveTasks;

	// 深渊柱子所在的房间ID
	UInt32					m_hellAreaId;

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

	// 所有的掉落组ID
	std::vector<UInt32>			m_allDropSet;

	// 开始计算掉落的时候计算下现在开放的活动列表，之后就不重新计算了（为了效率）
	std::set<UInt32>			m_OpenedActivityList;

	// 神秘商人id
	UInt32						m_MysticalMerchantId;

	// 城镇怪物ID
	SceneNpc					m_CityMonster;

	// 是否已经发过战斗结束消息
	bool						m_HasSendRaceOver;

	// 是否发送过师傅组队log
	bool						m_bSendMasterDiscipleInteractionLog;
	
	// 是否已经记录通关次数(按队伍计次数)
	bool						m_flagGuildDungeon;
	// 是否触发异界隐藏房间
	bool						m_TriggerHiddenRoom;

	// 终极试炼信息
	CLProtocol::ZjslDungeonInfo	m_ZjslDungeonInfo;

	// 是否生成公共掉落
	bool						m_isGeneratePublishDrops;

	// 公共掉落
	std::map<UInt8, DungeonRollItem>	m_rollItems;

	// 公共掉落是否单人模式
	bool		m_isSingleRoll;

	// 公共掉落是否已经结算
	bool		m_isRollSettled;

	// 公共掉落结算时间
	UInt32           m_rollEndTime;
};

#endif
