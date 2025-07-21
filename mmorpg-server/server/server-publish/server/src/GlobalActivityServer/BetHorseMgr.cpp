#include "BetHorseMgr.h"

#include <CLBetHorseProtocol.h>
#include <CLRecordClient.h>
#include <CLErrorCode.h>
#include <CLBetHorseDataEntry.h>
#include <CLBetHorseDefine.h>
#include <udplog_typedef.h>

#include "GASPlayer.h"
#include "ActivityMgr.h"
#include "GASGameZoneMgr.h"
#include "GASItemTransaction.h"
#include "GASAnnouncementMgr.h"
#include "Utility.h"
#include "GASSelectInitDataCallBack.h"
#include "GASApplication.h"

// 赌马赛季信息查询回调
struct SelectBetHorseCourtCallBack : public SelectInitDataCallback
{
	SelectBetHorseCourtCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadCourtData(this);
	}
};

// 赌马地图数据查询回调
struct SelectBetHorseMapCallBack : public SelectInitDataCallback
{
	SelectBetHorseMapCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadMapData(this);
	}
};

// 赌马射手数据查询回调
struct SelectBetHorseShooterCallBack : public SelectInitDataCallback
{
	SelectBetHorseShooterCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadShooterData(this);
	}
};

// 赌马玩家押注信息查询回调
struct SelectBetHorseStakeCallBack : public SelectInitDataCallback
{
	SelectBetHorseStakeCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadStakeData(this);
	}
};

// 赌马射手历史战绩查询回调
struct SelectBetHorseShooterRecordCallBack : public SelectInitDataCallback
{
	SelectBetHorseShooterRecordCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadShooterRecordData(this);
	}
};

// 赌马比赛场次查询回调
struct SelectBetHorseBattleRecordCallBack : public SelectInitDataCallback
{
	SelectBetHorseBattleRecordCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadBattleRecordData(this);
	}
};

// 赌马玩家押注记录查询回调
struct SelectBetHorsePlayerStakeRecordCallBack : public SelectInitDataCallback
{
	SelectBetHorsePlayerStakeRecordCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadPlayerStakeRecordData(this);
	}
};

// 加载奖励未发放成功的玩家
struct SelectBetHorseNoSendRewardCallBack : public SelectInitDataCallback
{
	SelectBetHorseNoSendRewardCallBack() {}

	bool OnLoadFinished()
	{
		return BetHorseMgr::Instance()->OnLoadNoSendRewardData(this);
	}
};

/**
*@brief 押注扣除道具事务
*/
class BetHorseStakeRmItemTransaction : public GASItemTransaction
{
public:
	BetHorseStakeRmItemTransaction(GASPlayer *player, UInt32 _shooterId, UInt32 _stakeNum, UInt32 _totalStake)
		: GASItemTransaction(player), shooterId(_shooterId), stakeNum(_stakeNum), totalStake(_totalStake){}
	~BetHorseStakeRmItemTransaction() {}

	void OnCheckSucced(GASPlayer *player)
	{
		BetHorseMgr::Instance()->BulletItemTransactionCBHandler(player, shooterId, stakeNum, totalStake, this);
		UnLock();
	}

	void OnCheckFailed(GASPlayer* player)
	{
		ErrorLogStream << PLAYERINFO(player) << "Bet Horse Stake Bullet Check failed!" << LogStream::eos;
		CLProtocol::StakeRes res;
		res.ret = ErrorCode::ITEM_COPIES_NOT_ENOUGH;
		UnLock();
		player->SendProtocol(res);
	}

private:

	// 押注射手id
	UInt32 shooterId;
	// 当前押注数量
	UInt32 stakeNum;
	// 已经押注的总量
	UInt32 totalStake;
};

// -------------------------------------------------------------

BetHorseMgr::BetHorseMgr()
{
	m_seasonId = 0;
	m_courtId = 0;
	m_mysteryShooter = 0;
	m_lastTime = 0;
	m_weather = 0;
	m_phase = 0;
	m_config = NULL;
	m_initStartTime = 0;
	m_activityStatus = AS_END;
	m_champion = 0;
	m_maxReward = 0;
	m_maxRewardPlayer = NULL;
	m_recovCheck = false;
	m_repeatSendReward = Avalon::Time::CurrentTime();
}

BetHorseMgr::~BetHorseMgr()
{

}

bool BetHorseMgr::OnInit()
{
	// 获得赌马的配置
	m_config = BetHorseCfgEntryMgr::Instance()->FindEntry(1);
	if (NULL == m_config)
	{
		ErrorLogStream << "not find bet horse config !!!" << LogStream::eos;
		return false;
	}

	// 加载DB
	LoadDBData();

	return true;
}

void BetHorseMgr::OnTick(const Avalon::Time& now)
{
	// 活动已关闭
	if (m_activityStatus == AS_END)
		return;

	// 初始化当天活动开启的时间点
	if (!IsSameDay(now, m_initStartTime))
		_ResetDayTimePointMap(now);

	if (!m_recovCheck)
	{
		if (m_courtId == 0)	// 第一次开服
		{
			_StartTodayActivity(now, true);
		}
		else
		{
			// 恢复重启现场
			if (m_champion == 0)
			{
				// 上一局未结算
				UInt64 start_time = _GetStartTimePoint(now);
				if (start_time <= 1)
				{
					m_phase = PHASE_TYPE_RESULT;
					m_tickMap.insert(TickMap::value_type(m_lastTime, TICK_STAKE_SETTLEMENT));
				}
				else
				{
					if (m_lastTime < start_time)
					{
						// 上一局已经过了当前局的开始时间，直接结算
						m_phase = PHASE_TYPE_RESULT;
						m_tickMap.insert(TickMap::value_type(start_time, TICK_STAKE_SETTLEMENT));
					}
					else if (m_lastTime > start_time)
					{
						if (m_lastTime > start_time + m_config->stakeEndTime)
							m_phase = PHASE_TYPE_ADJUST;
						else
							m_phase = PHASE_TYPE_STAKE;

						// 上一次最后操作时间，大于当前局开启时间，直接跑完上一局没做的操作
						_InitTickMap(start_time, m_lastTime);
					}
					else
					{
						// 上一次最后操作时间，等于当前局开启时间，+1可以防止加载开局的tick
						m_phase = PHASE_TYPE_STAKE;
						_InitTickMap(start_time, m_lastTime + 1);
					}
				}
			}
			else	// 上一局已经结算，并且下一局未开始。初始化下一局的tickmap
			{
				_StartTodayActivity(now);
			}
		}

		m_recovCheck = true;
	}

	// 5分钟检查一次，上一场是否有奖励未发放成功
	if (m_repeatSendReward + 5 * 60 * 1000 < now)
	{
		m_repeatSendReward = now;
		_RepeatSendMailReward();
	}

	if (!_OnTickCurBattle(now))
	{
		ErrorLogStream << "bet horse _OnTickCurBattle error!!!" << LogStream::eos;
		return;
	}
}

void BetHorseMgr::Final()
{
	_ClearHorseMap();
	_ClearShooterMap();
	_ClearStakeMap();

	for (auto& iter : m_shooterRecordMap)
	{
		iter.second->historyVec.clear();
		delete iter.second;
		iter.second = NULL;
	}
	m_shooterRecordMap.clear();

	for (auto& iter : m_battleRecordMap)
	{
		delete iter.second;
		iter.second = NULL;
	}
	m_battleRecordMap.clear();

	for (auto& iter : m_playerStakeRecordMap)
	{
		iter.second.clear();
	}
	m_playerStakeRecordMap.clear();
}

void BetHorseMgr::LoadDBData()
{
	// 加载赛季id，天气，神秘射手,开场时间等信息
	CLSelectCommand* cmd_season = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_court");
	CLRecordClient::Instance()->SendCommand(cmd_season, new SelectBetHorseCourtCallBack());

	// 加载地图数据
	CLSelectCommand* cmd_map = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_map");
	CLRecordClient::Instance()->SendCommand(cmd_map, new SelectBetHorseMapCallBack());

	// 加载射手数据
	CLSelectCommand* cmd_shooter = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_shooter");
	CLRecordClient::Instance()->SendCommand(cmd_shooter, new SelectBetHorseShooterCallBack());

	// 加载押注数据
	CLSelectCommand* cmd_stake = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_stake");
	CLRecordClient::Instance()->SendCommand(cmd_stake, new SelectBetHorseStakeCallBack());

	// 加载射手历史战绩
	CLSelectCommand* cmd_history = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_shooter_record");
	CLRecordClient::Instance()->SendCommand(cmd_history, new SelectBetHorseShooterRecordCallBack());

	// 加载场次比赛记录
	CLSelectCommand* cmd_battle = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_battle_record");
	CLRecordClient::Instance()->SendCommand(cmd_battle, new SelectBetHorseBattleRecordCallBack());

	// 加载玩家押注历史
	CLSelectCommand* cmd_player_stake = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_player_stake_record");
	CLRecordClient::Instance()->SendCommand(cmd_player_stake, new SelectBetHorsePlayerStakeRecordCallBack());

	// 加载上一轮未发奖的玩家
	if (GASApplication::Instance()->IsOpenBetHorseResend())
	{
		CLSelectCommand* cmd_no_send = CLRecordClient::Instance()->PrepareSelectCommand("t_bet_horse_reward");
		CLRecordClient::Instance()->SendCommand(cmd_no_send, new SelectBetHorseNoSendRewardCallBack());
	}
}

bool BetHorseMgr::OnLoadCourtData(CLRecordCallback* callback)
{
	if (!callback) return false;

	UInt32 record_size = 0;

	while (callback->NextRow())
	{
		m_seasonId = callback->GetData("season_id");
		m_courtId = callback->GetData("court_id");
		m_lastTime = callback->GetData("last_time");
		m_weather = callback->GetData("weather");
		m_mysteryShooter = callback->GetData("mystery_shooter");
		m_champion = callback->GetData("court_champion");

		record_size++;
	}

	// 如果记录的比赛场次大于1，出错
	if (record_size > 1)
	{
		ErrorLogStream << "database(t_bet_horse_court) record size than 1, error! " << LogStream::eos;
		return false;
	}

	return true;
}

bool BetHorseMgr::OnLoadMapData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		HorseYard* yard = new HorseYard();
		yard->SetDBData(callback);
		yard->SetID(UInt32(callback->GetKey()));
		m_horseYardMap[yard->GetMapID()] = yard;
	}

	return true;
}

bool BetHorseMgr::OnLoadShooterData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		ShooterData* shooter = new ShooterData();
		shooter->SetDBData(callback);
		shooter->SetID(UInt32(callback->GetKey()));
		const BetHorseShooterEntry* shooter_cfg = BetHorseShooterEntryMgr::Instance()->FindEntry(shooter->GetShooterID());
		if (NULL == shooter_cfg)
		{
			ErrorLogStream << "bet horse OnLoadShooterData not find shooter config error !!!" << LogStream::eos;
			return false;
		}

		shooter->SetCfg(shooter_cfg);

		m_shooterMap[shooter->GetShooterID()] = shooter;
	}

	// 计算赔率
	_StakeUpdateOdds();

	return true;
}

bool BetHorseMgr::OnLoadStakeData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		PlayerData data;
		data.uId = callback->GetData("role_id").GetString();
		data.name = callback->GetData("role_name").GetString();
		data.serverName = callback->GetData("server_name").GetString();
		data.adminNode = callback->GetData("admin_node");
		data.centerNode = callback->GetData("center_node");

		UInt32 shooter_id = callback->GetData("shooter_id");
		UInt32 stake_num = callback->GetData("stake_num");

		_AddStake(data, shooter_id, stake_num);
	}

	return true;
}

bool BetHorseMgr::OnLoadShooterRecordData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		UInt32 shooter_id = callback->GetData("shooter_id");
		UInt32 court_id = callback->GetData("court_id");
		UInt32 odds = callback->GetData("odds");
		UInt32 result = callback->GetData("result");

		_AddShooterHistoryRecord(shooter_id, court_id, odds, result);
	}

	// 计算射手的胜率和吃鸡数
	_RefreshAllShooterWinRateChampion();

	return true;
}

bool BetHorseMgr::OnLoadBattleRecordData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		BattleRecordData* record = new BattleRecordData();
		record->courtId = callback->GetData("court_id");
		record->champion = callback->GetData("champion");
		record->odds = callback->GetData("odds");
		record->maxProfit = callback->GetData("max_profit");
		record->endTime = callback->GetData("end_time");

		m_battleRecordMap[record->courtId] = record;
	}

	return true;
}

bool BetHorseMgr::OnLoadPlayerStakeRecordData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		std::string player_uid = callback->GetData("player_uid").GetString();
		UInt32 court_id = callback->GetData("court_id");
		UInt32 shooter_id = callback->GetData("shooter_id");
		UInt32 odds = callback->GetData("odds");
		UInt32 stake_num = callback->GetData("stake_num");
		Int32 profit = callback->GetData("profit");

		_AddPlayerStakeRecord(player_uid, court_id, shooter_id, odds, stake_num, profit);
	}

	return true;
}

bool BetHorseMgr::OnLoadNoSendRewardData(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		std::string player_uid = callback->GetData("player_uid").GetString();
		UInt32 admin_node = callback->GetData("admin_node");
		UInt32 center_node = callback->GetData("center_node");
		UInt32 court_id = callback->GetData("court_id");
		UInt32 stake_num = callback->GetData("stake_num");
		UInt32 reward_num = callback->GetData("reward_num");
		UInt32 is_success = callback->GetData("is_success");

		_AddBetHorseReward(player_uid, center_node, admin_node, court_id, stake_num, reward_num, is_success);
	}

	return true;
}

void BetHorseMgr::_AddStake(PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum, bool _save)
{
	auto iter = m_stakeMap.find(_playerData);
	if (iter == m_stakeMap.end())
	{
		std::map<UInt32, UInt32> stake_map;
		stake_map[_shooterId] = _stakeNum;
		m_stakeMap[_playerData] = stake_map;

		if (_save)
			_InsertDBStake(_playerData, _shooterId, _stakeNum);
	}
	else
	{
		auto iter1 = iter->second.find(_shooterId);
		if (iter1 != iter->second.end())
		{
			iter1->second += _stakeNum;
			if (_save)
				_UpdateDBStake(_playerData.uId, _shooterId, iter1->second);
		}
		else
		{
			iter->second[_shooterId] = _stakeNum;
			if (_save)
				_InsertDBStake(_playerData, _shooterId, _stakeNum);
		}
	}
}

void BetHorseMgr::_UpdateDBStake(const std::string& _playerUid, UInt32 _shooterId, UInt32 _stakeNum)
{
	std::ostringstream cond;
	cond << "role_id='" << _playerUid << "' and shooter_id=" << _shooterId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_stake", cond.str());
	cmd->PutData("stake_num", _stakeNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_InsertDBStake(const PlayerData& _playerData, UInt32 _shooterId, UInt32 _stakeNum)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_stake", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("role_id", _playerData.uId);
	cmd->PutData("role_name", _playerData.name);
	cmd->PutData("server_name", _playerData.serverName);
	cmd->PutData("admin_node", _playerData.adminNode);
	cmd->PutData("center_node", _playerData.centerNode);
	cmd->PutData("shooter_id", _shooterId);
	cmd->PutData("stake_num", _stakeNum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_AddShooterHistoryRecord(UInt32 _shooterId, UInt32 _courtId, UInt32 _odds, UInt32 _result)
{
	HistoryRecord history;
	history.id = _shooterId;
	history.courtId = _courtId;
	history.odds = _odds;
	history.result = _result;

	auto iter = m_shooterRecordMap.find(_shooterId);
	if (iter == m_shooterRecordMap.end())
	{
		ShooterRecordData* record = new ShooterRecordData();
		record->historyVec.push_back(history);
		m_shooterRecordMap[history.id] = record;
		return;
	}

	iter->second->historyVec.push_back(history);
}

void BetHorseMgr::_AddPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
	UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum, Int32 _profit, bool _save)
{
	PlayerStakeCourt stake_court;
	stake_court.courtId = _courtId;
	stake_court.shooterId = _shooterId;

	PlayerStakeRecord stake_record;
	stake_record.odds = _odds;
	stake_record.stakeNum = _stakeNum;
	stake_record.profit = _profit;

	auto iter = m_playerStakeRecordMap.find(_playerUid);
	if (iter == m_playerStakeRecordMap.end())
	{
		StakeRecordMap record_map;
		record_map[stake_court] = stake_record;
		m_playerStakeRecordMap[_playerUid] = record_map;

		if (_save)
			_InsertDBPlayerStakeRecord(_playerUid, _courtId, _shooterId, _odds, _stakeNum);
	}
	else
	{
		StakeRecordMap& record_map = iter->second;

		auto iter_stake = record_map.find(stake_court);
		if (iter_stake != record_map.end())
		{
			iter_stake->second.odds = _odds;
			iter_stake->second.stakeNum += _stakeNum;
			iter_stake->second.profit = _profit;

			if (_save)
				_UpdateDBPlayerStakeRecord(_playerUid, _courtId, _shooterId, _odds, iter_stake->second.stakeNum, _profit);
		}
		else
		{
			record_map[stake_court] = stake_record;
			if (_save)
				_InsertDBPlayerStakeRecord(_playerUid, _courtId, _shooterId, _odds, _stakeNum);
		}
	}
}

void BetHorseMgr::_InsertDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
	UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_player_stake_record", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("player_uid", _playerUid);
	cmd->PutData("court_id", _courtId);
	cmd->PutData("shooter_id", _shooterId);
	cmd->PutData("odds", _odds);
	cmd->PutData("stake_num", _stakeNum);
	cmd->PutData("profit", -1);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_UpdateDBPlayerStakeRecord(const std::string& _playerUid, UInt32 _courtId, 
	UInt32 _shooterId, UInt32 _odds, UInt32 _stakeNum, Int32 _profit)
{
	std::ostringstream cond;
	cond << "player_uid='" << _playerUid << "' and court_id=" << _courtId << " and shooter_id=" << _shooterId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_player_stake_record", cond.str());
	cmd->PutData("stake_num", _stakeNum);
	cmd->PutData("odds", _odds);
	cmd->PutData("profit", _profit);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_AddBetHorseReward(const std::string& _playerUid, UInt32 _centerNode, UInt32 _adminNode, 
	UInt32 _courtId, UInt32 _stakeNum, UInt32 _rewardNum, UInt32 _isSuccess, bool _save)
{
	BetHorseReward reward;
	reward.centerNode = _centerNode;
	reward.adminNode = _adminNode;
	reward.courtId = _courtId;
	reward.stakeNum = _stakeNum;
	reward.rewardNum = _rewardNum;
	reward.isSuccess = _isSuccess;

	auto iter = m_noSendRewardMap.find(_playerUid);
	if (iter != m_noSendRewardMap.end())
		return;

	if (GASApplication::Instance()->IsOpenBetHorseResend())
	{
		m_noSendRewardMap[_playerUid] = reward;
	}

	if (_save)
	{
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_reward", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("player_uid", _playerUid);
		cmd->PutData("center_node", _centerNode);
		cmd->PutData("admin_node", _adminNode);
		cmd->PutData("court_id", _courtId);
		cmd->PutData("stake_num", _stakeNum);
		cmd->PutData("reward_num", _rewardNum);
		cmd->PutData("is_success", _isSuccess);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void BetHorseMgr::_UpdateBetHorseReward(const std::string& _playerUid)
{
	InfoLogStream << "send bet horse reword to player(" << _playerUid << ") success." << LogStream::eos;

	auto iter = m_noSendRewardMap.find(_playerUid);
	if (iter == m_noSendRewardMap.end())
		return;

	m_noSendRewardMap.erase(_playerUid);

	std::ostringstream cond;
	cond << "player_uid='" << _playerUid<<"'";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_bet_horse_reward", cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool BetHorseMgr::_InitTickMap(UInt64 _curStartTime, UInt64 _initTime)
{
	if (m_config->refreshOddsInterval == 0)
		return false;

	if (_initTime == _curStartTime)
	{
		m_tickMap.insert(TickMap::value_type(_curStartTime, TICK_STAKE_START));
		m_phaseStampSet.insert(_curStartTime);
	}

	UInt32 refresh_cnt = m_config->stakeEndTime / m_config->refreshOddsInterval;
	UInt64 time_point = _curStartTime;
	while (--refresh_cnt)
	{
		time_point += m_config->refreshOddsInterval;
		if (_initTime < time_point)
			m_tickMap.insert(TickMap::value_type(time_point, TICK_REFRESH_ODDS));
	}

	for (auto& iter : m_config->endWarnTimeSet)
	{
		time_point = _curStartTime + iter;
		if (_initTime < time_point)
		{
			auto warn_iter = m_tickMap.find(time_point);
			if (warn_iter != m_tickMap.end())
			{
				//如果时间点有重复，往后移动3秒
				m_tickMap.insert(TickMap::value_type(time_point + 3, TICK_BROADCAST_WARN));
			}
			else
				m_tickMap.insert(TickMap::value_type(time_point, TICK_BROADCAST_WARN));

			m_warnStake.insert((m_config->stakeEndTime - iter) / 60);
		}
	}

	time_point = _curStartTime + m_config->stakeEndTime;
	if (_initTime < time_point)
	{
		m_tickMap.insert(TickMap::value_type(time_point, TICK_STAKE_STOP));
		m_phaseStampSet.insert(time_point);
	}

	time_point = _curStartTime + m_config->stakeEndTime + 60 * 2;
	if (_initTime < time_point)
		m_tickMap.insert(TickMap::value_type(time_point, TICK_REFRESH_ODDS));

	time_point = _curStartTime + m_config->adjustEndTime;
	if (_initTime < time_point)
	{
		m_tickMap.insert(TickMap::value_type(time_point, TICK_STAKE_SETTLEMENT));
		m_phaseStampSet.insert(time_point);
	}
	
	return true;
}

ShooterData* BetHorseMgr::_GetShooterData(UInt32 _shooterId)
{
	auto iter = m_shooterMap.find(_shooterId); 
	return iter != m_shooterMap.end() ? iter->second : NULL;
}

ShooterRecordData* BetHorseMgr::_GetShooterHistory(UInt32 _shooterId)
{
	auto iter = m_shooterRecordMap.find(_shooterId);
	return iter != m_shooterRecordMap.end() ? iter->second : NULL;
}

PlayerStakeMap* BetHorseMgr::_GetPlayerStakeMap(const std::string& _UId)
{
	// 有玩家唯一id就可查询
	PlayerData data;
	data.uId = _UId;

	auto iter = m_stakeMap.find(data);
	return iter != m_stakeMap.end() ? &(iter->second) : NULL;
}

HorseYard* BetHorseMgr::_GetHorseYard(UInt32 _mapId)
{
	auto iter = m_horseYardMap.find(_mapId);
	return iter != m_horseYardMap.end() ? iter->second : NULL;
}

UInt32 BetHorseMgr::_GetPlayerStakeNum(const std::string& _UId)
{
	PlayerStakeMap* stake_map = _GetPlayerStakeMap(_UId);
	if (NULL == stake_map)
		return 0;

	UInt32 stake_num = 0;
	for (auto& iter : *stake_map)
	{
		stake_num += iter.second;
	}

	return stake_num;
}

void BetHorseMgr::HandlerBetHorseData(GASPlayer* player)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player) return;
	CLProtocol::BetHorseRes res;
	res.weather = m_weather;
	res.mysteryShooter = m_mysteryShooter;
	res.phase = m_phase;

	auto it = m_phaseStampSet.upper_bound(Avalon::Time::CurrentTime().Sec());
	if (it != m_phaseStampSet.end())
		res.stamp = UInt32(*it);

	// 地图信息
	for (auto& iter : m_horseYardMap)
	{
		CLProtocol::BetHorseMapInfo map_info;
		HorseYard* yard = iter.second;
		if (NULL == yard)
			continue;

		map_info.id = yard->GetMapID();
		map_info.terrain = yard->GetTerrain();
		map_info.winShooterId = yard->GetWinShooter();
		if (yard->GetShooter_1() != 0)
			map_info.shooterList.push_back(yard->GetShooter_1());

		if (yard->GetShooter_2() != 0)
			map_info.shooterList.push_back(yard->GetShooter_2());

		res.mapList.push_back(map_info);
	}

	// 射手信息
	for (auto& shooter : m_shooterMap)
	{
		CLProtocol::ShooterInfo info;
		info.id = shooter.second->GetShooterID();
		info.dataId = shooter.second->GetShooterID();
		info.status = shooter.second->GetStatus();
		info.odds = shooter.second->GetOdds();

		ShooterRecordData* history = _GetShooterHistory(shooter.first);
		if (NULL != history)
		{
			info.winRate = history->winRate;
			info.champion = history->champion;
		}

		res.shooterList.push_back(info);
	}

	player->SendProtocol(res);
}

void BetHorseMgr::HandlerRealTimeOdds(GASPlayer* player)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player) return;
	CLProtocol::ShooterOddsRes res;

	for (auto& iter : m_shooterMap)
	{
		CLProtocol::OddsRateInfo odds_info;

		odds_info.shooterId = iter.second->GetShooterID();
		odds_info.shooterOdds = iter.second->GetOdds();

		res.oddsList.push_back(odds_info);
	}

	player->SendProtocol(res);
}

void BetHorseMgr::HandlerStake(GASPlayer* player, UInt32 shooterId, UInt32 stakeNum)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player || 0 == stakeNum || 0 == shooterId || m_activityStatus != AS_IN) return;

	CLProtocol::StakeRes res;
	// 是否在押注阶段
	if (m_phase != PHASE_TYPE_STAKE)
	{
		res.ret = ErrorCode::BET_HORSE_NOT_STAKE_PHASE;
		player->SendProtocol(res);
		return;
	}

	// 获取玩家的押注数量
	UInt32 stake_num = _GetPlayerStakeNum(player->GetUId());
	if (stake_num + stakeNum > m_config->stakeMax)
	{
		res.ret = ErrorCode::BET_HORSE_STAKE_MAX;
		player->SendProtocol(res);
		return;
	}

	ShooterData* shooter_data = _GetShooterData(shooterId);
	if (NULL == shooter_data)
	{
		ErrorLogStream << "BulletItemTransactionCBHandler not find shooter end! id:" << shooterId << LogStream::eos;
		res.ret = ErrorCode::BET_HORSE_SHOOTER_NOT_JOIN;
		player->SendProtocol(res);
		return;
	}

	// 开始事务检查子弹够不够
	auto trans = new BetHorseStakeRmItemTransaction(player, shooterId, stakeNum, stake_num);
	trans->SetItem(m_config->bulletID ,stakeNum);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << "bet horse stake remove item failed, begin transaction failed." << LogStream::eos;
		res.ret = ErrorCode::BET_HORSE_STAKE_FAILED;
		player->SendProtocol(res);
	}
}

void BetHorseMgr::HandlerShooterHistory(GASPlayer* player, UInt32 shooterId)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player)return;
	CLProtocol::ShooterHistoryRecordRes res;
	res.id = shooterId;

	do
	{
		ShooterRecordData* shooter_his = _GetShooterHistory(shooterId);
		if (shooter_his == NULL)
			break;

		for (auto& iter : shooter_his->historyVec)
		{
			CLProtocol::ShooterRecord shooter_record;
			shooter_record.courtId = iter.courtId;
			shooter_record.odds = iter.odds;
			shooter_record.result = iter.result;

			res.recordList.push_back(shooter_record);
		}

	} while (0);

	player->SendProtocol(res);
}

void BetHorseMgr::NotifyStakePhase()
{
	CLProtocol::BetHorsePhaseSycn phase_sycn;
	phase_sycn.phaseSycn = m_phase;

	auto it = m_phaseStampSet.upper_bound(Avalon::Time::CurrentTime().Sec());
	if (it != m_phaseStampSet.end())
		phase_sycn.stamp = UInt32(*(m_phaseStampSet.begin()));

	GASGameZoneMgr::Instance()->BroadcastToGate(phase_sycn);
}

void BetHorseMgr::HandlerPlayerStakeRecord(GASPlayer* player)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player) return;

	CLProtocol::StakeRecordRes res;
	auto iter = m_playerStakeRecordMap.find(player->GetUId());
	if(iter != m_playerStakeRecordMap.end())
	{
		for (auto& it : iter->second)
		{
			CLProtocol::StakeRecord record;
			record.courtId = it.first.courtId;
			record.stakeShooter = it.first.shooterId;

			if (m_courtId == record.courtId)
			{
				ShooterData* shooter_data = _GetShooterData(record.stakeShooter);
				if (NULL == shooter_data)
					continue; 

				record.odds = shooter_data->GetOdds();
			}
			else
				record.odds = it.second.odds;

			record.stakeNum = it.second.stakeNum;
			record.profit = it.second.profit;
			res.StakeRecordList.push_back(record);
		}
	}

	player->SendProtocol(res);
}

void BetHorseMgr::HandlerBattleRecord(GASPlayer* player)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player) return;

	CLProtocol::BattleRecordRes res;
	for (auto& iter : m_battleRecordMap)
	{
		BattleRecordData* data = iter.second;
		if (NULL == data)
			continue;

		CLProtocol::BattleRecord battle_record;
		battle_record.courtId = data->courtId;
		battle_record.champion = data->champion;
		battle_record.odds = data->odds;
		battle_record.maxProfit = data->maxProfit;

		res.BattleRecordList.push_back(battle_record);
	}

	player->SendProtocol(res);
}

void BetHorseMgr::HandlerShooterRank(GASPlayer* player)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (NULL == player) return;

	CLProtocol::ShooterRankRes res;

	PlayerData data;
	data.uId = player->GetUId();
	data.name = player->GetPlayerName();
	data.serverName = player->GetServerName();
	data.adminNode = player->GetSourceNode(ST_ADMIN);
	data.centerNode = player->GetSourceNode(ST_CENTER);

	for (auto& iter : m_shooterRecordMap)
	{
		ShooterRecordData* data = iter.second;
		if (NULL == data)
			continue;

		CLProtocol::ShooterRankInfo shooter_rank;
		shooter_rank.shooterId = iter.first;
		shooter_rank.battleNum = data->historyVec.size();
		shooter_rank.winRate = data->winRate;

		res.shooterRankList.push_back(shooter_rank);
	}
	
	player->SendProtocol(res);
}

void BetHorseMgr::BulletItemTransactionCBHandler(GASPlayer *player, UInt32 _shooterId, UInt32 _stakeNum, UInt32 _totalStake, GASItemTransaction* _tran)
{
	if (NULL == player)
	{
		ErrorLogStream << "BulletItemTransactionCBHandler failed!" << LogStream::eos;
		return;
	}

	if (NULL == _tran)
	{
		ErrorLogStream << "BulletItemTransactionCBHandler failed! playerUid:"<<player->GetUId()<<
			" stake shooterId:"<<_shooterId <<" stakeNum:"<<_stakeNum<<" last total stake:"<<_totalStake<< LogStream::eos;
		return;
	}

	CLProtocol::StakeRes res;
	// 是否在押注阶段
	if (m_phase != PHASE_TYPE_STAKE)
	{
		ErrorLogStream << "BulletItemTransactionCBHandler stake end!" << LogStream::eos;
		res.ret = ErrorCode::BET_HORSE_NOT_STAKE_PHASE;
		player->SendProtocol(res);
		return;
	}

	ShooterData* shooter_data = _GetShooterData(_shooterId);
	if (NULL == shooter_data)
	{
		ErrorLogStream << "BulletItemTransactionCBHandler not find shooter end! playerUid:" << player->GetUId() <<
			" stake shooterId:" << _shooterId << " stakeNum:" << _stakeNum << " last total stake:" << _totalStake << LogStream::eos;
		res.ret = ErrorCode::BET_HORSE_NOT_STAKE_PHASE;
		player->SendProtocol(res);
		return;
	}

	PlayerData data;
	data.uId = player->GetUId();
	data.name = player->GetPlayerName();
	data.serverName = player->GetServerName();
	data.adminNode = player->GetSourceNode(ST_ADMIN);
	data.centerNode = player->GetSourceNode(ST_CENTER);

	// 修改内存，并存库
	_AddStake(data, _shooterId, _stakeNum, true);
	
	// 射手押注数量存库更新
	shooter_data->AddStakeNum(_stakeNum);
	shooter_data->UpdateToDB();

	// 修改押注历史,-1代表未开奖
	_AddPlayerStakeRecord(player->GetUId(), m_courtId, _shooterId, shooter_data->GetOdds(), _stakeNum, -1, true);

	// 扣除射手子弹
	_tran->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_BET_HORSE_STAKE, m_courtId, _shooterId));

	//InfoLogStream << "player stake success playerUid:" << player->GetUId() <<" stake shooterId:" 
	//	<< _shooterId << " stakeNum:" << _stakeNum << " total stake:" << _totalStake + _stakeNum << LogStream::eos;

	res.ret = ErrorCode::SUCCESS;
	res.id = _shooterId;
	res.num = _totalStake + _stakeNum;
	player->SendProtocol(res);
}

void BetHorseMgr::_RefreshAllShooterWinRateChampion()
{
	for (auto& iter : m_shooterRecordMap)
	{
		ShooterRecordData* history = iter.second;
		if (NULL == history)
		{
			ErrorLogStream << "_RefreshAllShooterWinRateChampion failed! shooterID: "<< iter.first<< LogStream::eos;
			continue;
		}
		
		UInt32 champion_cnt = 0;
		UInt32 total_cnt = 0;

		for (auto& record : history->historyVec)
		{
			if (record.result != 0)
				champion_cnt++;

			total_cnt++;
		}

		history->champion = champion_cnt;
		history->winRate = champion_cnt * RATE_EXPAND_NUM / total_cnt;
	}
}

void BetHorseMgr::SetActivityStatus(UInt32 _seasonId, UInt32 _actStatus)
{
	if (_seasonId != m_seasonId && _actStatus == AS_IN)
	{
		// 新赛季开始，清除上赛季所有数据
		m_champion = 0;
		m_courtId = 0;
		m_seasonId = 0;
		m_lastTime = 0;
		Final();
		_ClearDBTable("t_bet_horse_battle_record");
		_ClearDBTable("t_bet_horse_court");
		_ClearDBTable("t_bet_horse_map");
		_ClearDBTable("t_bet_horse_player_stake_record");
		_ClearDBTable("t_bet_horse_shooter");
		_ClearDBTable("t_bet_horse_shooter_record");
		_ClearDBTable("t_bet_horse_stake");
	}

	m_seasonId = _seasonId;
	m_activityStatus = _actStatus;
}

bool BetHorseMgr::SetHorseMap(HorseYard* yard)
{
	if (NULL == yard)
	{
		ErrorLogStream << "bet horse SetHorseMap error !!!" << LogStream::eos;
		return false;
	}

	m_horseYardMap[yard->GetMapID()] = yard;

	return true;
}

void BetHorseMgr::HandlerWorldBetHorseMailRewardRes(const std::string& _playerUid)
{
	_UpdateBetHorseReward(_playerUid);
}

void BetHorseMgr::_StartTodayActivity(const Avalon::Time& now, bool _isFirst)
{
	// 上一局已经结算，并且下一局未开始。初始化下一局的tickmap
	UInt64 start_time = _GetStartTimePoint(now);
	if (start_time == 0)	// 当天还未开局,初始化当天的第一局tickmap
	{
		if (m_courtId == 0) //  赛季第一场
			m_phase = PHASE_TYPE_READY;
		else
			m_phase = PHASE_TYPE_DAY_END;

		_InitTickMap(*++m_dayStartTimeSet.begin()++, *++m_dayStartTimeSet.begin()++);
	}
	else if (start_time == 1)
	{
		// 今天活动结束了，初始化明天的第一局tickmap
		m_phase = PHASE_TYPE_DAY_END;
		UInt64 tomorrow = _GetTomorrowStartTimePoint(now);
		_InitTickMap(tomorrow, tomorrow);
	}
	else 
	{
		if (_isFirst)	// 首次开服正常开始当前局
		{
			_InitTickMap(start_time, start_time);
		}
		else       // 非首次开服，初始化下一局
		{
			if (m_lastTime < start_time)
			{
				_InitTickMap(start_time, start_time);
			}
			else
			{
				m_phase = PHASE_TYPE_RESULT;
				UInt64 next_time = _GetNextStartTimePoint(now);
				_InitTickMap(next_time, next_time);
			}
		}
	}

}

void BetHorseMgr::_ResetDayTimePointMap(const Avalon::Time& now)
{
	m_initStartTime = now;
	m_dayStartTimeSet.clear();

	UInt64 today_begin = GetDayBeginTime(now) / 1000;
	m_dayStartTimeSet.insert(0);
	for (auto& iter : m_config->startTimeSet)
	{
		m_dayStartTimeSet.insert(today_begin + iter);
	}

	UInt32 last_time = *(--m_config->startTimeSet.end());
	m_dayStartTimeSet.insert(today_begin + last_time + m_config->stakeEndTime);
}

UInt64 BetHorseMgr::_GetTomorrowStartTimePoint(const Avalon::Time& _time)
{
	UInt64 tomorrow_begin = GetDayBeginTime(_time + 24 * 3600 * 1000) / 1000;
	return tomorrow_begin + *(m_config->startTimeSet.begin());
}

UInt64 BetHorseMgr::_GetStartTimePoint(const Avalon::Time& _time)
{
	std::set<UInt64>::iterator iter = m_dayStartTimeSet.lower_bound(_time.Sec());
	// 今天比赛结束了
	if (iter == m_dayStartTimeSet.end())
		return 1;

	return *(--iter);
}

UInt64 BetHorseMgr::_GetNextStartTimePoint(const Avalon::Time& _time)
{
	std::set<UInt64>::iterator iter = m_dayStartTimeSet.upper_bound(_time.Sec());
	if (iter == m_dayStartTimeSet.end())
		return 0;

	auto iter1 = iter;
	if (++iter1 == m_dayStartTimeSet.end())
		return 0;

	return *(iter);
}

bool BetHorseMgr::_OnTickCurBattle(const Avalon::Time& now)
{
	// 获取当前阶段
	UInt32 cur_tick = TICK_END;
	if (!m_tickMap.empty())
	{
		auto iter = m_tickMap.begin();
		if (now.Sec() >= iter->first)
		{
			m_lastTime = iter->first;
			cur_tick = iter->second;
			m_tickMap.erase(iter);
		}
	}

	return _ExecuteTickMap(cur_tick, m_lastTime);
}

bool BetHorseMgr::_ExecuteTickMap(UInt32 _tickType, UInt64 _exeTime)
{
	switch (_tickType)
	{
	case TICK_END: return true;
	case TICK_STAKE_START:
	{
		if (!_StakeStart())
			return false;
	}
	break;
	case TICK_REFRESH_ODDS: _StakeUpdateOdds(); break;
	case TICK_BROADCAST_WARN: _StakePublicEnd(); break;
	case TICK_STAKE_STOP:_StakeEnd(); break;
	case TICK_STAKE_SETTLEMENT:
	{
		if (!_StakeSettlement())
			return false;
	}
	break;
	default:
		return false;
	}

	_UpdateLastTickTime(_exeTime);

	return true;
}

bool BetHorseMgr::_StakeStart()
{
	if (PHASE_TYPE_STAKE == m_phase)
		return false;

	// 当前阶段 押注
	m_phase = PHASE_TYPE_STAKE;
	m_champion = 0;
	m_maxReward = 0;
	m_maxRewardPlayer = NULL;
	m_courtId++;

	InfoLogStream << "bet horse _StakeStart start!!! court:" << m_courtId << LogStream::eos;

	// 随机天气
	m_weather = BetHorseRandEntryMgr::Instance()->GetRandWeather();

	// 随机神秘射手
	m_mysteryShooter = BetHorseShooterEntryMgr::Instance()->GetRandMysteryShooter(m_config->mysteryRate);

	// 随机射手
	std::vector<UInt32> shooter_vec;
	if (m_mysteryShooter != 0)
		shooter_vec.push_back(m_mysteryShooter);

	BetHorseShooterEntryMgr::Instance()->RandShooterVec(m_config->needShooterNum, shooter_vec);

	// 随机地形,地图分配射手
	_ClearHorseMap();
	_ClearDBTable("t_bet_horse_map");
	if (!_InitHorseMap(shooter_vec))
	{
		ErrorLogStream << "bet horse _InitHorseMap error !!!" << LogStream::eos;
		return false;
	}

	// 加入射手列表，随机射手状态，初始赔率
	_ClearShooterMap();
	_ClearDBTable("t_bet_horse_shooter");
	if (!_InitShooter(shooter_vec))
	{
		ErrorLogStream << "bet horse _InitShooter error !!!" << LogStream::eos;
		return false;
	}

	// 清除数据库
	_ClearDBTable("t_bet_horse_court");
	// 记录本轮信息
	CLInsertCommand* insert = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_court", CLRecordClient::Instance()->GenGuid());
	insert->PutData("season_id", m_seasonId);
	insert->PutData("court_id", m_courtId);
	insert->PutData("last_time", m_lastTime);
	insert->PutData("weather", m_weather);
	insert->PutData("mystery_shooter", m_mysteryShooter);
	CLRecordClient::Instance()->SendCommand(insert);

	// 清除上一轮的押注信息
	_ClearStakeMap();
	// 清除数据库
	_ClearDBTable("t_bet_horse_stake");

	// 公告客户端即将开始比赛
	if (m_config->startStakePublic != 0)
		GASAnnouncementMgr::Instance()->PublicAnnouncement(false, m_config->startStakePublic, UInt32(CURRENT_TIME.Sec()), m_courtId);

	NotifyStakePhase();

	InfoLogStream << "bet horse _StakeStart finish!!! court:" << m_courtId << LogStream::eos;

	return true;
}

bool BetHorseMgr::_InitHorseMap(const std::vector<UInt32>& _shooterVec)
{
	class BetHorseMapVisitor : public Avalon::DataEntryVisitor<BetHorseMapEntry>
	{
	public:
		BetHorseMapVisitor() {}

		bool operator()(BetHorseMapEntry* dataEntry)
		{
			if (NULL == dataEntry) return false;

			HorseYard* yard = new HorseYard();
			yard->SetMapID(dataEntry->mapId);
			yard->SetTerrain(BetHorseRandEntryMgr::Instance()->GetRandTerrain());

			// 首轮地图分配射手,每个地图分配两名射手
			if (dataEntry->mapPhase == MAP_PHASE_FIRST)
			{
				UInt32 id = RandGetShooter();
				if (0 == id)
				{
					ErrorLogStream << "bet horse _InitHorseMap  RandGetShooter_1 error !!!" << LogStream::eos;
					return false;
				}
				yard->SetShooter_1(id);

				id = RandGetShooter();
				if (0 == id)
				{
					ErrorLogStream << "bet horse _InitHorseMap  RandGetShooter_2 error !!!" << LogStream::eos;
					return false;
				}
				yard->SetShooter_2(id);
			}

			yard->InsertToDB();

			if (!BetHorseMgr::Instance()->SetHorseMap(yard))
				return false;

			return true;
		}

		void SetShooterVec(const std::vector<UInt32>& _shooterVec)
		{
			shooterVec = _shooterVec;
		}

		UInt32 RandGetShooter()
		{
			if (shooterVec.empty())
				return 0;

			UInt32 offset = Avalon::Random::RandBetween(0, shooterVec.size() - 1);
			std::vector<UInt32>::iterator iter = shooterVec.begin();
			std::advance(iter, offset);
			UInt32 id = *iter;
			shooterVec.erase(iter);
			return id;
		}

	private:
		std::vector<UInt32> shooterVec;
	};

	InfoLogStream << "bet horse _InitHorseMap start!!! court:"<<m_courtId << LogStream::eos;

	BetHorseMapVisitor visitor;
	visitor.SetShooterVec(std::vector<UInt32>(_shooterVec));
	BetHorseMapEntryMgr::Instance()->Visit(visitor);

	if (m_horseYardMap.size() != 7)
	{
		ErrorLogStream << "bet horse _InitHorseMap map num error! map size:" << UInt32(m_horseYardMap.size()) << LogStream::eos;
		return false;
	}

	InfoLogStream << "bet horse _InitHorseMap finish!!! court:" << m_courtId << LogStream::eos;

	return true;
}

bool BetHorseMgr::_InitShooter(const std::vector<UInt32>& _shooterVec)
{
	InfoLogStream << "bet horse _InitShooter start!!! court:" << m_courtId << LogStream::eos;

	for (auto& iter : _shooterVec)
	{
		ShooterData* data = new ShooterData();
		data->SetShooterID(iter);
		data->SetStatus(BetHorseRandEntryMgr::Instance()->GetRandStatus());
		const BetHorseShooterEntry* shooter_cfg = BetHorseShooterEntryMgr::Instance()->FindEntry(iter);
		if (NULL == shooter_cfg)
		{
			ErrorLogStream << "bet horse _InitShooter not find shooter config error !!!" << LogStream::eos;
			return false;
		}

		data->SetCfg(shooter_cfg);

		m_shooterMap[iter] = data;

		// 存库
		data->InsertToDB();

		// 记录每场的射手日志
		SysUtility::SendUdpLog("bet_horse_court_shooter", LOG_BET_HORSE_SHOOTER, m_courtId, iter, UInt32(CURRENT_TIME.Sec()));
	}

	InfoLogStream << "bet horse _InitShooter finish!!! court:" << m_courtId << LogStream::eos;

	return true;
}

void BetHorseMgr::_ClearHorseMap()
{
	for (auto& iter : m_horseYardMap)
	{
		delete iter.second;
		iter.second = NULL;
	}

	m_horseYardMap.clear();
}

void BetHorseMgr::_ClearShooterMap()
{
	for (auto& iter : m_shooterMap)
	{
		delete iter.second;
		iter.second = NULL;
	}

	m_shooterMap.clear();
}

void BetHorseMgr::_ClearStakeMap()
{
	m_stakeMap.clear();
}

void BetHorseMgr::_StakeUpdateOdds()
{
	// 所有射手的总的押注数量
	UInt32 total_stake = 0;
	for (auto& iter : m_shooterMap)
		total_stake += iter.second->GetStakeNum();

	// 更新射手赔率
	for (auto& iter : m_shooterMap)
		iter.second->UpdateOdds(total_stake);
}

void BetHorseMgr::_StakePublicEnd()
{
	InfoLogStream << "bet horse _StakePublicEnd finish!!! court:" << m_courtId << LogStream::eos;

	// 公告客户端进入押注阶段
	if (m_config->endStakePublic != 0 && !m_warnStake.empty())
	{
		auto iter = --m_warnStake.end();
		UInt32 warn_min = iter != m_warnStake.end() ? *iter : 0;
		m_warnStake.erase(iter);
		GASAnnouncementMgr::Instance()->PublicAnnouncement(false, m_config->endStakePublic, UInt32(CURRENT_TIME.Sec()), warn_min);
	}
}

void BetHorseMgr::_StakeEnd()
{
	InfoLogStream << "bet horse _StakeEnd finish!!! court:" << m_courtId << LogStream::eos;

	// 押注结束，进入调整阶段
	m_phase = PHASE_TYPE_ADJUST;
	NotifyStakePhase();
}

bool  BetHorseMgr::_StakeSettlement()
{
	// 比赛开始公告
	if (m_config->battleStartPublic != 0)
		GASAnnouncementMgr::Instance()->PublicAnnouncement(false, m_config->battleStartPublic, UInt32(CURRENT_TIME.Sec()), m_courtId);

	// 进入结算阶段
	m_phase = PHASE_TYPE_RESULT;

	// 计算首轮获胜者
	if (!_CalculateWinShooterByMap(MAP_PHASE_FIRST)) return false;
	
	// 计算第二轮获胜者,
	if (!_CalculateWinShooterByMap(MAP_PHASE_SECOND)) return false;

	// 决赛，计算冠军
	if (!_CalculateWinShooterByMap(MAP_PHASE_LAST)) return false;

	// 发奖
	if (!_SendStakeReward()) return false;

	// 记录历史
	_AddHistoryRecord();
	
	// 通知客户端结算了
	NotifyStakePhase();

	// 初始化下一局的tickmap
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 next_time = _GetNextStartTimePoint(now);
	if (next_time == 0)	// 当天已结束,初始化第二天
	{
		m_phase = PHASE_TYPE_DAY_END;
		NotifyStakePhase();
		UInt64 tomorrow = _GetTomorrowStartTimePoint(now);
		_InitTickMap(tomorrow, tomorrow);
	}
	else // 初始化下一局
	{
		_InitTickMap(next_time, next_time);
	}

	return true;
}

bool BetHorseMgr::_SendStakeReward()
{
	if (0 == m_champion || m_phase != PHASE_TYPE_RESULT)
		return false;

	InfoLogStream << "bet horse _SendStakeReward start!!! court:" << m_courtId << LogStream::eos;

	ShooterData* champion_shooter = _GetShooterData(m_champion);
	if (NULL == champion_shooter)
	{
		ErrorLogStream << "bet horse _SendStakeReward not find champion_shooter error !!! champion: "<<m_champion << LogStream::eos;
		return false;
	}

	// 冠军赔率
	float odds_rate = float(champion_shooter->GetOdds()) / RATE_EXPAND_NUM;
	InfoLogStream << "court: " << m_courtId << " champion: "<<m_champion<< " odds: " << odds_rate << LogStream::eos;
	UInt32 decimal = champion_shooter->GetOdds() % RATE_EXPAND_NUM / 1000;

	// 中奖人数
	UInt32 win_player_num = 0;

	for (auto& iter : m_stakeMap)
	{
		UInt32 reward_num = 0;
		const PlayerData& player_data = iter.first;
		PlayerStakeMap& stake_it = iter.second;
		auto shooter = stake_it.find(m_champion);
		if (shooter != stake_it.end())
		{
			win_player_num++;

			// 中奖数量，小数与整数分开计算
			reward_num = UInt32(ceil(double(decimal * shooter->second) / 10)) + champion_shooter->GetOdds() / RATE_EXPAND_NUM * shooter->second;

			// 记录最大奖励
			if (reward_num > m_maxReward)
			{
				m_maxReward = reward_num;
				m_maxRewardPlayer = &(iter.first);
			}

			// 邮件发放奖励
			ItemReward reward;
			reward.id = m_config->bulletID;
			reward.num = reward_num;
			std::vector<ItemReward> rewards = { reward };

			std::string sender = SysNotifyMgr::Instance()->MakeString(9909);
			std::string title = SysNotifyMgr::Instance()->MakeString(9910);
			std::string content = SysNotifyMgr::Instance()->MakeString(9911, champion_shooter->GetCfg()->shooterName, shooter->second, odds_rate, reward_num);
			std::string reason = GetReason(SOURCE_TYPE_BET_HORSE_REWARD, m_courtId, m_champion);
			SysUtility::SendMail(player_data.centerNode, player_data.adminNode, player_data.uId, sender,title,content,reason,rewards);

			_AddBetHorseReward(player_data.uId, player_data.centerNode, player_data.adminNode,m_courtId,shooter->second,reward_num, 0, true);

			InfoLogStream << "send bet horse reward to player(" << player_data.uId << ") reward(" << reward_num << ")." << LogStream::eos;
		}

		// 记录玩家押注历史
		for (auto& stake : stake_it)
		{
			ShooterData* shooter_data = _GetShooterData(stake.first);
			if (NULL == shooter_data)
			{
				ErrorLogStream << "bet horse _SendStakeReward not find shooter_data error !!! id:" << stake.first << LogStream::eos;
				continue;
			}

			// 玩家押注记录
			Int32 profit = stake.first == m_champion ? reward_num:0;
			_AddPlayerStakeRecord(player_data.uId, m_courtId, stake.first, shooter_data->GetOdds(), 0, profit, true);
		}
	}

	InfoLogStream << "bet horse _SendStakeReward finish!!! court:" << m_courtId << LogStream::eos;

	m_repeatSendReward = CURRENT_TIME;

	// 公布中奖人数,广播最大奖励
	if (m_maxRewardPlayer != NULL && m_config->openRewardPublic != 0)
	{
		GASAnnouncementMgr::Instance()->PublicAnnouncement(false, m_config->openRewardPublic,
			UInt32(CURRENT_TIME.Sec()), m_courtId, m_maxRewardPlayer->serverName, m_maxRewardPlayer->name, m_maxReward, win_player_num);
	}
	
	return true;
}

bool BetHorseMgr::_CalculateWinShooterByMap(UInt32 _mapPhase)
{
	InfoLogStream << "bet horse _CalculateWinShooterByMap start!!! court:" << m_courtId << " phase:" << _mapPhase << LogStream::eos;

	for (auto& iter : m_horseYardMap)
	{
		BetHorseMapEntry* map_entry = BetHorseMapEntryMgr::Instance()->FindEntry(iter.first);
		if (NULL == map_entry)
		{
			ErrorLogStream << "bet horse _CalculateWinByMapPhase find map entry error !!!" << LogStream::eos;
			return false;
		}

		// 按地图的轮次计算当前胜利者
		if (_mapPhase != map_entry->mapPhase)
			continue;

		// 地图出错,地图上两个射手也出错
		HorseYard* yard = iter.second;
		if (NULL == yard || yard->GetShooter_1() == 0 || yard->GetShooter_2() == 0)
		{
			ErrorLogStream << "bet horse _CalculateWinByMapPhase horse yard NULL error !!!" << LogStream::eos;
			return false;
		}

		UInt32 shooter_1 = yard->GetShooter_1();
		ShooterData* data_1 = _GetShooterData(shooter_1);
		if (NULL == data_1)
		{
			ErrorLogStream << "bet horse _CalculateWinByMapPhase shooter data_1 NULL error !!!" << LogStream::eos;
			return false;
		}

		UInt32 shooter_2 = yard->GetShooter_2();
		ShooterData* data_2 = _GetShooterData(shooter_2);
		if (NULL == data_2)
		{
			ErrorLogStream << "bet horse _CalculateWinByMapPhase shooter data_2 NULL error !!!" << LogStream::eos;
			return false;
		}

		// 胜率总和
		UInt32 total_rate = m_config->baseWinRate * 2;
		total_rate += BetHorseShooterStatusEntryMgr::Instance()->GetShooterStatusWinRate(data_1->GetStatus(), data_2->GetStatus());
		total_rate += BetHorseOccuSuppressEntryMgr::Instance()->GetOccuSuppressWinRate(data_1->GetCfg()->occupation, data_2->GetCfg()->occupation);
		total_rate += BetHorseRateEntryMgr::Instance()->GetWeatherOccpationWinRate(m_weather, data_1->GetCfg()->occupation, data_2->GetCfg()->occupation);
		total_rate += BetHorseRateEntryMgr::Instance()->GetTerrainOccpationWinRate(yard->GetTerrain(), data_1->GetCfg()->occupation, data_2->GetCfg()->occupation);
	
		// 分别计算两个射手的获胜概率
		std::map<UInt32, UInt32> win_rate_map;
		win_rate_map[m_config->baseWinRate] = shooter_2;
		win_rate_map[total_rate] = shooter_1;

		UInt32 rand_num = Avalon::Random::RandBetween(0, total_rate - 1);
		auto rand_iter = win_rate_map.upper_bound(rand_num);
		if (rand_iter == win_rate_map.end())
		{
			ErrorLogStream << "bet horse _CalculateWinByMapPhase shooter win rate rand error !!!" << LogStream::eos;
			return false;
		}

		// 计算出胜利的射手
		yard->SetWinShooter(rand_iter->second);

		// 胜利射手加入下一轮比赛的地图
		HorseYard* next_yard = _GetHorseYard(map_entry->nextMap);
		if (NULL != next_yard)
		{
			if (next_yard->GetShooter_1() == 0)
				next_yard->SetShooter_1(yard->GetWinShooter());
			else
				next_yard->SetShooter_2(yard->GetWinShooter());

			next_yard->UpdateToDB();
		}

		// 最后一轮，设置冠军
		if (MAP_PHASE_LAST == _mapPhase)
		{
			m_champion = yard->GetWinShooter();

			std::ostringstream cond;
			cond << "court_id=" << m_courtId;
			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_court", cond.str());
			cmd->PutData("court_champion", m_champion);
			CLRecordClient::Instance()->SendCommand(cmd);
		}

		// 更新地图
		yard->UpdateToDB();
	}

	InfoLogStream << "bet horse _CalculateWinShooterByMap finish!!! court:" << m_courtId << " phase:" << _mapPhase << LogStream::eos;

	return true;
}

void BetHorseMgr::_AddHistoryRecord()
{
	InfoLogStream << "bet horse _AddHistoryRecord start!!! court:" << m_courtId << LogStream::eos;

	UInt32 champion_odds = 0;

	// 增加选手战绩
	for (auto& iter : m_shooterMap)
	{
		// 是否是冠军
		UInt32 result = (m_champion == iter.first ? 1 : 0);

		_AddShooterHistoryRecord(iter.first, m_courtId, iter.second->GetOdds(), result);

		// 记录到数据库
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_shooter_record", CLRecordClient::Instance()->GenGuid());
		cmd->PutData("shooter_id", iter.first);
		cmd->PutData("court_id", m_courtId);
		cmd->PutData("odds", iter.second->GetOdds());
		cmd->PutData("result", result);
		CLRecordClient::Instance()->SendCommand(cmd);
		
		// 获得冠军赔率
		if (iter.first == m_champion)
			champion_odds = iter.second->GetOdds();
	}

	// 刷新内存吃鸡次数和胜率
	_RefreshAllShooterWinRateChampion();

	UInt64 cur_time = Avalon::Time::CurrentTime().Sec();

	// 增加比赛历史
	BattleRecordData* record = new BattleRecordData();
	record->courtId = m_courtId;
	record->champion = m_champion;
	record->odds = champion_odds;
	record->maxProfit = m_maxReward;
	record->endTime = cur_time;
	m_battleRecordMap[record->courtId] = record;

	// 保存比赛历史到数据库
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_bet_horse_battle_record", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("court_id", m_courtId);
	cmd->PutData("champion", m_champion);
	cmd->PutData("odds", champion_odds);
	cmd->PutData("max_profit", m_maxReward);
	cmd->PutData("end_time", cur_time);
	CLRecordClient::Instance()->SendCommand(cmd);

	InfoLogStream << "bet horse _AddHistoryRecord finish!!! court:" << m_courtId << LogStream::eos;
}

void BetHorseMgr::_ClearDBTable(const std::string& _tableName)
{
	std::ostringstream cond;
	cond << "1 = 1";
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(_tableName, cond.str());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_UpdateLastTickTime(UInt64 _time)
{
	std::ostringstream cond;
	cond << "court_id=" << m_courtId << " and season_id=" << m_seasonId;
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_bet_horse_court", cond.str());
	cmd->PutData("last_time", _time);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void BetHorseMgr::_RepeatSendMailReward()
{
	if (!GASApplication::Instance()->IsOpenBetHorseResend())
	{
		return;
	}

	InfoLogStream << "start to resend bet horse reward." << LogStream::eos;

	if (m_noSendRewardMap.empty())
		return;

	for (auto& iter : m_noSendRewardMap)
	{
		if (iter.second.isSuccess != 0)
			continue;

		const BetHorseReward& bet_reward = iter.second;


		auto champion_iter = m_battleRecordMap.find(bet_reward.courtId);
		if (champion_iter == m_battleRecordMap.end())
		{
			ErrorLogStream << "_RepeatSendMailReward not find court:"<< bet_reward.courtId << LogStream::eos;
			continue;
		}

		BetHorseShooterEntry* shooter_entry = BetHorseShooterEntryMgr::Instance()->FindEntry(champion_iter->second->champion);
		if (NULL == shooter_entry)
		{
			ErrorLogStream << "_RepeatSendMailReward not find shooter entry:" << champion_iter->second->champion << LogStream::eos;
			continue;
		}

		float champion_odds = float(champion_iter->second->odds) / RATE_EXPAND_NUM;

		ItemReward reward;
		reward.id = m_config->bulletID;
		reward.num = bet_reward.rewardNum;
		std::vector<ItemReward> rewards = { reward };

		std::string sender = SysNotifyMgr::Instance()->MakeString(9909);
		std::string title = SysNotifyMgr::Instance()->MakeString(9910);
		std::string content = SysNotifyMgr::Instance()->MakeString(9911, shooter_entry->shooterName, bet_reward.stakeNum, champion_odds, bet_reward.rewardNum);
		std::string reason = GetReason(SOURCE_TYPE_BET_HORSE_REWARD, bet_reward.courtId, shooter_entry->shooterId);
		SysUtility::SendMail(bet_reward.centerNode, bet_reward.adminNode, iter.first, sender, title, content, reason, rewards);

		InfoLogStream << "resend bet horse reward to player(" << iter.first << ") reward(" << reward.num << ")." << LogStream::eos;
	}
}

void BetHorseMgr::GMUpdatePhase(UInt32 _phase)
{
	if (GASApplication::Instance()->IsOpenBetHorse())
		return;

	if (_phase < PHASE_TYPE_STAKE || _phase > PHASE_TYPE_DAY_END)
		return;

	InfoLogStream << "bet horse GMUpdatePhase phase:" << _phase << LogStream::eos;

	Avalon::Time now = Avalon::Time::CurrentTime();
	switch (_phase)
	{
		case PHASE_TYPE_STAKE:
		{
			if (m_champion == 0)
			{
				_StakeSettlement();
			}

			m_tickMap.clear();
			m_phaseStampSet.clear();
			_InitTickMap(now.Sec(), now.Sec());

			m_phase = PHASE_TYPE_READY;
			NotifyStakePhase();
		}
		break;
		case PHASE_TYPE_ADJUST:
		{
			for (TickMap::iterator iter = m_tickMap.begin();iter != m_tickMap.end();)
			{
				if (iter->second <= TICK_STAKE_STOP)
				{
					_ExecuteTickMap(iter->second, iter->first);
					if (iter->second == TICK_STAKE_STOP)
						return;

					iter = m_tickMap.erase(iter);
				}
				else
					iter++;
			}
		}
			break;
		case PHASE_TYPE_RESULT:
		{
			for (TickMap::iterator iter = m_tickMap.begin(); iter != m_tickMap.end();)
			{
				if (iter->second <= TICK_STAKE_SETTLEMENT)
				{
					_ExecuteTickMap(iter->second, iter->first);
					if (iter->second == TICK_STAKE_SETTLEMENT)
						return;

					iter = m_tickMap.erase(iter);
				}
				else
					iter++;
			}
		}
		break;
		case PHASE_TYPE_DAY_END:
		{
			m_phase = _phase;
			NotifyStakePhase();
		}
		break;
		default:break;
	}
}

void BetHorseMgr::ExecuteCourtNum(UInt32 _num)
{
	// 每赛季最多10000场
	if (m_courtId > 10000)
		return;

	// 每次最多执行500场
	if (_num > 500)
		_num = 500;

	if (m_champion == 0)
	{
		_StakeSettlement();
	}

	InfoLogStream << "bet horse ExecuteCourtNum num:" << _num << LogStream::eos;

	Avalon::Time now = Avalon::Time::CurrentTime();

	while (_num--)
	{
		m_tickMap.clear();
		m_phaseStampSet.clear();
		_InitTickMap(now.Sec(), now.Sec());
		m_phase = PHASE_TYPE_READY;
		_StakeStart();
		_StakeSettlement();
		NotifyStakePhase();

		_InitTickMap(now.Sec(), now.Sec());
	}
}