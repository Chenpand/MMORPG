#ifndef _BET_HORSE_MGR_H_
#define _BET_HORSE_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include "BetHorseInfo.h"

class GASPlayer;
struct BetHorseCfgEntry;
class GASItemTransaction;
namespace CLProtocol{
	struct BattleResult;
}

enum TickType
{
	TICK_END		= 0,
	TICK_STAKE_START	= 1,	// 比赛开始
	TICK_REFRESH_ODDS	= 2,	// 刷新赔率
	TICK_BROADCAST_WARN = 3,	// 广播提醒
	TICK_STAKE_STOP		= 4,	// 停止押注
	TICK_STAKE_SETTLEMENT = 5,	// 押注结算
};

typedef std::map<UInt32, HorseYard*>	HorseYardMap;
typedef std::map<UInt32, ShooterData*>	ShooterMap;
typedef std::map<UInt32, ShooterRecordData*> ShooterRecordMap;
typedef std::map<UInt32, UInt32> PlayerStakeMap;
typedef std::map<PlayerData, PlayerStakeMap> StakeMap;
typedef std::map<UInt64, UInt32>		TickMap;
typedef std::map<UInt32, BattleRecordData*>		BattleRecordMap;
typedef std::map<PlayerStakeCourt, PlayerStakeRecord>	StakeRecordMap;
typedef std::map<std::string, StakeRecordMap> PlayerStakeRecordMap;
typedef std::map<std::string, BetHorseReward> PlayerNoSendRewardMap;

class BetHorseMgr : public Avalon::Singleton<BetHorseMgr>
{

public:
	BetHorseMgr();
	~BetHorseMgr();

	// 请求赌马数据
	void HandlerBetHorseData(GASPlayer* player);
	// 请求射手实时赔率
	void HandlerRealTimeOdds(GASPlayer* player);
	// 押注
	void HandlerStake(GASPlayer* player, UInt32 shooterId, UInt32 stakeNum);
	// 获取射手历史记录
	void HandlerShooterHistory(GASPlayer* player, UInt32 shooterId);
	// 同步客户端押注阶段
	void NotifyStakePhase();
	// 请求玩家押注历史
	void HandlerPlayerStakeRecord(GASPlayer* player);
	// 请求比赛历史
	void HandlerBattleRecord(GASPlayer* player);
	// 请求射手排行
	void HandlerShooterRank(GASPlayer* player);
	// 子弹份额检查成功的回调处理
	void BulletItemTransactionCBHandler(GASPlayer *player, UInt32 _shooterId, UInt32 _stakeNum, UInt32 _totalStake, GASItemTransaction* _tran);
	// 设置活动状态
	void SetActivityStatus(UInt32 _seasonId, UInt32 _actStatus);
	// 设置地图
	bool SetHorseMap(HorseYard* yard);
	// 收到world赌马邮件奖励回复
	void HandlerWorldBetHorseMailRewardRes(const std::string& _playerUid);

	// 初始化
	bool OnInit();
	// DB加载所有数据
	void LoadDBData();
	// tick
	void OnTick(const Avalon::Time& now);
	// 关闭
	void Final();

	// 加载数据，天气 场次id等
	bool OnLoadCourtData(CLRecordCallback* callback);
	// 加载马场地图信息
	bool OnLoadMapData(CLRecordCallback* callback);
	// 加载射手的信息
	bool OnLoadShooterData(CLRecordCallback* callback);
	// 加载所有玩家的下注信息
	bool OnLoadStakeData(CLRecordCallback* callback);
	// 加载所有射手的历史战绩
	bool OnLoadShooterRecordData(CLRecordCallback* callback);
	// 加载比赛场次记录
	bool OnLoadBattleRecordData(CLRecordCallback* callback);
	// 加载玩家押注历史
	bool OnLoadPlayerStakeRecordData(CLRecordCallback* callback);
	// 加载未发放奖励玩家
	bool OnLoadNoSendRewardData(CLRecordCallback* callback);
	// GM修改阶段操作
	void GMUpdatePhase(UInt32 _phase);
	// GM操作执行场次
	void ExecuteCourtNum(UInt32 _num);

private:

	// 增加押注
	void _AddStake(PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum, bool _save = false);
	// 更新玩家押注数量
	void _UpdateDBStake(const std::string& _playerUid, UInt32 _shooterId, UInt32 _stakeNum);
	// 插入押注数据
	void _InsertDBStake(const PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum);
	// 增加射手历史战绩
	void _AddShooterHistoryRecord(UInt32 _shooterId, UInt32 _courtId, UInt32 _odds, UInt32 _result);
	// 增加玩家押注历史
	void _AddPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, UInt32 _shooterId, 
		UInt32 _odds, UInt32 _stakeNum, Int32 _profit, bool _save = false);
	// 插入押注历史
	void _InsertDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
		UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum);
	// 更新押注历史
	void _UpdateDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
		UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum, Int32 _profit);
	// 增加发奖数据
	void _AddBetHorseReward(const std::string& _playerUid, UInt32 _centerNode, UInt32 _adminNode, 
		UInt32 _courtId, UInt32 _stakeNum, UInt32 _rewardNum, UInt32 _isSuccess, bool _save = false);
	// 更新发奖状态
	void _UpdateBetHorseReward(const std::string& _playerUid);
	// 初始化tick列表
	bool _InitTickMap(UInt64 _curStartTime, UInt64 _initTime);
	// 获取射手信息
	ShooterData* _GetShooterData(UInt32 _shooterId);
	// 获取射手的战绩列表
	ShooterRecordData* _GetShooterHistory(UInt32 _shooterId);
	// 获取玩家的押注列表
	PlayerStakeMap* _GetPlayerStakeMap(const std::string& _UId);
	// 获取地图
	HorseYard* _GetHorseYard(UInt32 _mapId);
	// 获取玩家的押注数量
	UInt32 _GetPlayerStakeNum(const std::string& _UId);
	// 更新所有射手胜率和吃鸡次数
	void _RefreshAllShooterWinRateChampion();
	// 开始当天的活动
	void _StartTodayActivity(const Avalon::Time& now, bool _isFirst = false);
	// 重置当天的时间点map
	void _ResetDayTimePointMap(const Avalon::Time& now);
	// 获得明天活动的开启时间
	UInt64 _GetTomorrowStartTimePoint(const Avalon::Time& _time);
	// 获取当前场次的开场时间点
	UInt64 _GetStartTimePoint(const Avalon::Time& _time);
	// 获取下一场的开场时间点
	UInt64 _GetNextStartTimePoint(const Avalon::Time& _time);
	// tick当前场次的时间
	bool _OnTickCurBattle(const Avalon::Time& now);
	// 执行tick的操作
	bool _ExecuteTickMap(UInt32 _tickType, UInt64 _time);
	// 押注开始
	bool _StakeStart();
	// 刷新赔率
	void _StakeUpdateOdds();
	// 公告客户端押注即将结束
	void _StakePublicEnd();
	// 押注结束
	void _StakeEnd();
	// 开始结算
	bool _StakeSettlement();
	// 初始化地图数据
	bool _InitHorseMap(const std::vector<UInt32>& _shooterVec);
	// 初始化射手
	bool _InitShooter(const std::vector<UInt32>& _shooterVec);
	// 清除地图数据
	void _ClearHorseMap();
	// 清除射手数据
	void _ClearShooterMap();
	// 清除押注信息
	void _ClearStakeMap();
	// 按地图轮次计算获胜者
	bool _CalculateWinShooterByMap(UInt32 _mapPhase);
	// 发放押注奖励
	bool _SendStakeReward();
	// 记录历史
	void _AddHistoryRecord();
	// 清除数据库
	void _ClearDBTable(const std::string& _tableName);
	// 更新最后一次操作的时间
	void _UpdateLastTickTime(UInt64 _time);
	// 重新发奖
	void _RepeatSendMailReward();

private:

	// 赛季id
	UInt32 m_seasonId;
	// 当前场次id
	UInt32	m_courtId;
	// 天气(WeatherType)
	UInt32	m_weather;
	// 最后一次处理时间
	UInt64	m_lastTime;
	// 神秘射手
	UInt32	m_mysteryShooter;
	// 本轮冠军
	UInt32  m_champion;

	// 马场地图
	HorseYardMap	m_horseYardMap;
	// 押注信息
	StakeMap	m_stakeMap;
	// 射手信息
	ShooterMap	m_shooterMap; 
	// 射手的历史战绩
	ShooterRecordMap	m_shooterRecordMap;
	// 比赛场次历史
	BattleRecordMap		m_battleRecordMap;
	// 玩家押注历史
	PlayerStakeRecordMap	m_playerStakeRecordMap;
	// 未发放奖励的玩家
	PlayerNoSendRewardMap	m_noSendRewardMap;

	// 当前活动阶段
	UInt32		m_phase;
	// 配置信息
	BetHorseCfgEntry* m_config;
	// 活动状态
	UInt32	m_activityStatus;

	// 当天开始活动的时间点
	std::set<UInt64> m_dayStartTimeSet;
	// 每一场比赛tick列表
	TickMap		m_tickMap;
	// 初始化时间点的时间
	Avalon::Time m_initStartTime;
	// 押注提醒倒计时
	std::set<UInt32> m_warnStake;
	// 赌马阶段时间戳
	std::set<UInt64> m_phaseStampSet;

	// 临时数据
	// 最大奖励
	UInt32 m_maxReward;
	// 获得最大奖的玩家
	const PlayerData* m_maxRewardPlayer;
	// 重启恢复现场完毕
	bool m_recovCheck;

	// 奖励重发检查时间
	Avalon::Time	m_repeatSendReward;
};


#endif