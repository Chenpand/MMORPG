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
	TICK_STAKE_START	= 1,	// ������ʼ
	TICK_REFRESH_ODDS	= 2,	// ˢ������
	TICK_BROADCAST_WARN = 3,	// �㲥����
	TICK_STAKE_STOP		= 4,	// ֹͣѺע
	TICK_STAKE_SETTLEMENT = 5,	// Ѻע����
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

	// �����������
	void HandlerBetHorseData(GASPlayer* player);
	// ��������ʵʱ����
	void HandlerRealTimeOdds(GASPlayer* player);
	// Ѻע
	void HandlerStake(GASPlayer* player, UInt32 shooterId, UInt32 stakeNum);
	// ��ȡ������ʷ��¼
	void HandlerShooterHistory(GASPlayer* player, UInt32 shooterId);
	// ͬ���ͻ���Ѻע�׶�
	void NotifyStakePhase();
	// �������Ѻע��ʷ
	void HandlerPlayerStakeRecord(GASPlayer* player);
	// ���������ʷ
	void HandlerBattleRecord(GASPlayer* player);
	// ������������
	void HandlerShooterRank(GASPlayer* player);
	// �ӵ��ݶ���ɹ��Ļص�����
	void BulletItemTransactionCBHandler(GASPlayer *player, UInt32 _shooterId, UInt32 _stakeNum, UInt32 _totalStake, GASItemTransaction* _tran);
	// ���û״̬
	void SetActivityStatus(UInt32 _seasonId, UInt32 _actStatus);
	// ���õ�ͼ
	bool SetHorseMap(HorseYard* yard);
	// �յ�world�����ʼ������ظ�
	void HandlerWorldBetHorseMailRewardRes(const std::string& _playerUid);

	// ��ʼ��
	bool OnInit();
	// DB������������
	void LoadDBData();
	// tick
	void OnTick(const Avalon::Time& now);
	// �ر�
	void Final();

	// �������ݣ����� ����id��
	bool OnLoadCourtData(CLRecordCallback* callback);
	// ��������ͼ��Ϣ
	bool OnLoadMapData(CLRecordCallback* callback);
	// �������ֵ���Ϣ
	bool OnLoadShooterData(CLRecordCallback* callback);
	// ����������ҵ���ע��Ϣ
	bool OnLoadStakeData(CLRecordCallback* callback);
	// �����������ֵ���ʷս��
	bool OnLoadShooterRecordData(CLRecordCallback* callback);
	// ���ر������μ�¼
	bool OnLoadBattleRecordData(CLRecordCallback* callback);
	// �������Ѻע��ʷ
	bool OnLoadPlayerStakeRecordData(CLRecordCallback* callback);
	// ����δ���Ž������
	bool OnLoadNoSendRewardData(CLRecordCallback* callback);
	// GM�޸Ľ׶β���
	void GMUpdatePhase(UInt32 _phase);
	// GM����ִ�г���
	void ExecuteCourtNum(UInt32 _num);

private:

	// ����Ѻע
	void _AddStake(PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum, bool _save = false);
	// �������Ѻע����
	void _UpdateDBStake(const std::string& _playerUid, UInt32 _shooterId, UInt32 _stakeNum);
	// ����Ѻע����
	void _InsertDBStake(const PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum);
	// ����������ʷս��
	void _AddShooterHistoryRecord(UInt32 _shooterId, UInt32 _courtId, UInt32 _odds, UInt32 _result);
	// �������Ѻע��ʷ
	void _AddPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, UInt32 _shooterId, 
		UInt32 _odds, UInt32 _stakeNum, Int32 _profit, bool _save = false);
	// ����Ѻע��ʷ
	void _InsertDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
		UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum);
	// ����Ѻע��ʷ
	void _UpdateDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
		UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum, Int32 _profit);
	// ���ӷ�������
	void _AddBetHorseReward(const std::string& _playerUid, UInt32 _centerNode, UInt32 _adminNode, 
		UInt32 _courtId, UInt32 _stakeNum, UInt32 _rewardNum, UInt32 _isSuccess, bool _save = false);
	// ���·���״̬
	void _UpdateBetHorseReward(const std::string& _playerUid);
	// ��ʼ��tick�б�
	bool _InitTickMap(UInt64 _curStartTime, UInt64 _initTime);
	// ��ȡ������Ϣ
	ShooterData* _GetShooterData(UInt32 _shooterId);
	// ��ȡ���ֵ�ս���б�
	ShooterRecordData* _GetShooterHistory(UInt32 _shooterId);
	// ��ȡ��ҵ�Ѻע�б�
	PlayerStakeMap* _GetPlayerStakeMap(const std::string& _UId);
	// ��ȡ��ͼ
	HorseYard* _GetHorseYard(UInt32 _mapId);
	// ��ȡ��ҵ�Ѻע����
	UInt32 _GetPlayerStakeNum(const std::string& _UId);
	// ������������ʤ�ʺͳԼ�����
	void _RefreshAllShooterWinRateChampion();
	// ��ʼ����Ļ
	void _StartTodayActivity(const Avalon::Time& now, bool _isFirst = false);
	// ���õ����ʱ���map
	void _ResetDayTimePointMap(const Avalon::Time& now);
	// ��������Ŀ���ʱ��
	UInt64 _GetTomorrowStartTimePoint(const Avalon::Time& _time);
	// ��ȡ��ǰ���εĿ���ʱ���
	UInt64 _GetStartTimePoint(const Avalon::Time& _time);
	// ��ȡ��һ���Ŀ���ʱ���
	UInt64 _GetNextStartTimePoint(const Avalon::Time& _time);
	// tick��ǰ���ε�ʱ��
	bool _OnTickCurBattle(const Avalon::Time& now);
	// ִ��tick�Ĳ���
	bool _ExecuteTickMap(UInt32 _tickType, UInt64 _time);
	// Ѻע��ʼ
	bool _StakeStart();
	// ˢ������
	void _StakeUpdateOdds();
	// ����ͻ���Ѻע��������
	void _StakePublicEnd();
	// Ѻע����
	void _StakeEnd();
	// ��ʼ����
	bool _StakeSettlement();
	// ��ʼ����ͼ����
	bool _InitHorseMap(const std::vector<UInt32>& _shooterVec);
	// ��ʼ������
	bool _InitShooter(const std::vector<UInt32>& _shooterVec);
	// �����ͼ����
	void _ClearHorseMap();
	// �����������
	void _ClearShooterMap();
	// ���Ѻע��Ϣ
	void _ClearStakeMap();
	// ����ͼ�ִμ����ʤ��
	bool _CalculateWinShooterByMap(UInt32 _mapPhase);
	// ����Ѻע����
	bool _SendStakeReward();
	// ��¼��ʷ
	void _AddHistoryRecord();
	// ������ݿ�
	void _ClearDBTable(const std::string& _tableName);
	// �������һ�β�����ʱ��
	void _UpdateLastTickTime(UInt64 _time);
	// ���·���
	void _RepeatSendMailReward();

private:

	// ����id
	UInt32 m_seasonId;
	// ��ǰ����id
	UInt32	m_courtId;
	// ����(WeatherType)
	UInt32	m_weather;
	// ���һ�δ���ʱ��
	UInt64	m_lastTime;
	// ��������
	UInt32	m_mysteryShooter;
	// ���ֹھ�
	UInt32  m_champion;

	// ����ͼ
	HorseYardMap	m_horseYardMap;
	// Ѻע��Ϣ
	StakeMap	m_stakeMap;
	// ������Ϣ
	ShooterMap	m_shooterMap; 
	// ���ֵ���ʷս��
	ShooterRecordMap	m_shooterRecordMap;
	// ����������ʷ
	BattleRecordMap		m_battleRecordMap;
	// ���Ѻע��ʷ
	PlayerStakeRecordMap	m_playerStakeRecordMap;
	// δ���Ž��������
	PlayerNoSendRewardMap	m_noSendRewardMap;

	// ��ǰ��׶�
	UInt32		m_phase;
	// ������Ϣ
	BetHorseCfgEntry* m_config;
	// �״̬
	UInt32	m_activityStatus;

	// ���쿪ʼ���ʱ���
	std::set<UInt64> m_dayStartTimeSet;
	// ÿһ������tick�б�
	TickMap		m_tickMap;
	// ��ʼ��ʱ����ʱ��
	Avalon::Time m_initStartTime;
	// Ѻע���ѵ���ʱ
	std::set<UInt32> m_warnStake;
	// ����׶�ʱ���
	std::set<UInt64> m_phaseStampSet;

	// ��ʱ����
	// �����
	UInt32 m_maxReward;
	// �����󽱵����
	const PlayerData* m_maxRewardPlayer;
	// �����ָ��ֳ����
	bool m_recovCheck;

	// �����ط����ʱ��
	Avalon::Time	m_repeatSendReward;
};


#endif