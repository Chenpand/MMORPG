#ifndef _CL_BET_HORSE_PROTOCOL_H_
#define _CL_BET_HORSE_PROTOCOL_H_

#include "CLProtocolDefine.h"


namespace CLProtocol
{
	/**
	*@brief 射手信息
	*/
	struct ShooterInfo
	{
		ShooterInfo()
		{
			id = 0;
			dataId = 0;
			status = 0;
			odds = 0;
			winRate = 0;
			champion = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & dataId & status & odds & winRate & champion;
		}

		// 射手唯一id
		UInt32 id;
		// 射手配置id
		UInt32 dataId;
		// 状态
		UInt32 status;
		// 赔率
		UInt32 odds;
		// 胜率
		UInt32 winRate;
		// 吃鸡数(夺冠次数)
		UInt32 champion;
	};

	/**
	*@brief 地图信息
	*/
	struct BetHorseMapInfo
	{
		BetHorseMapInfo()
		{
			id = 0;
			terrain = 0;
			winShooterId = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & terrain & winShooterId & shooterList;
		}

		// 地图id
		UInt32 id;
		// 地形
		UInt32 terrain;
		// 胜利的射手
		UInt32 winShooterId;
		// 地图上的射手
		std::vector<UInt32> shooterList;
	};

	/**
	*@brief client->server 赌马界面请求
	*/
	class BetHorseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client 赌马界面返回
	*/
	class BetHorseRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_INFO_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & weather & mysteryShooter & phase & stamp & shooterList & mapList;
		}

		BetHorseRes()
		{
			weather = 0;
			mysteryShooter = 0;
			phase = 0;
			stamp = 0;
		}

		// 当前天气
		UInt32 weather;
		// 神秘射手id
		UInt32 mysteryShooter;
		// 赌马阶段
		UInt32 phase;
		// 时间戳
		UInt32 stamp;
		// 射手列表
		std::vector<ShooterInfo> shooterList;
		// 地图列表
		std::vector<BetHorseMapInfo> mapList;
	};

	/**
	*@brief client->server 射手赔率请求
	*/
	class ShooterOddsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_ODDS_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief 射手赔率信息
	*/
	struct OddsRateInfo
	{
		OddsRateInfo()
		{
			shooterId = 0;
			shooterOdds = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterId & shooterOdds;
		}

		// 射手id
		UInt32 shooterId;
		// 赔率
		UInt32 shooterOdds;
	};

	/**
	*@brief server->client 射手赔率返回
	*/
	class ShooterOddsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_ODDS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oddsList;
		}

		// 赔率列表
		std::vector<OddsRateInfo> oddsList;
	};

	/**
	*@brief client->server 射手历史战绩请求
	*/
	class ShooterHistoryRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_HISTORY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterId;
		}

		// 射手id
		UInt32 shooterId;
	};

	/**
	*@brief 射手历史信息
	*/
	struct ShooterRecord
	{
		ShooterRecord()
		{
			courtId = 0;
			odds = 0;
			result = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & courtId & odds & result;
		}

		// 场次
		UInt32 courtId;
		// 自己的赔率
		UInt32 odds;
		// 胜负
		UInt32 result;
	};

	/**
	*@brief server->client 射手历史战绩返回
	*/
	class ShooterHistoryRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_HISTORY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & recordList;
		}

		// 射手id
		UInt32 id;
		// 历史战绩列表
		std::vector<ShooterRecord> recordList;
	};

	/**
	*@brief client->server 玩家押注请求
	*/
	class StakeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

		// 押注的射手
		UInt32 id;
		// 押注子弹数量
		UInt32 num;
	};

	/**
	*@brief server->client 玩家押注返回
	*/
	class StakeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & id & num;
		}

		// 押注结果
		UInt32 ret;
		// 押注的射手
		UInt32 id;
		// 押注子弹数量
		UInt32 num;
	};

	/**
	*@brief server->client 赌马阶段状态同步
	*/
	class BetHorsePhaseSycn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_PHASE_SYCN)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & phaseSycn & stamp;
		}

		BetHorsePhaseSycn()
		{
			phaseSycn = 0;
			stamp = 0;
		}

		// 赌马阶段同步
		UInt32 phaseSycn;
		// 时间戳
		UInt32 stamp;
	};

	/**
	*@brief client->server 玩家押注历史请求
	*/
	class StakeRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_RECORD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct StakeRecord
	{
		StakeRecord()
		{
			courtId = 0;
			stakeShooter = 0;
			odds = 0;
			stakeNum = 0;
			profit = -1;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & courtId & stakeShooter & odds & stakeNum & profit;
		}

		// 场次id
		UInt32 courtId;
		// 押注射手
		UInt32 stakeShooter;
		// 赔率
		UInt32 odds;
		// 支援数量
		UInt32 stakeNum;
		// 盈利
		Int32 profit;
	};

	/**
	*@brief server->client 玩家押注历史返回
	*/
	class StakeRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_RECORD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & StakeRecordList;
		}

		// 押注记录列表
		std::vector<StakeRecord> StakeRecordList;
	};

	//---------------------------------------------------------

	/**
	*@brief client->server 比赛历史请求
	*/
	class BattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_BATTLE_RECORD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct BattleRecord
	{
		BattleRecord()
		{
			courtId = 0;
			champion = 0;
			odds = 0;
			maxProfit = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & courtId & champion & odds & maxProfit;
		}

		// 场次id
		UInt32 courtId;
		// 冠军射手
		UInt32 champion;
		// 赔率
		UInt32 odds;
		// 最大盈利
		UInt32 maxProfit;
	};

	/**
	*@brief server->client 比赛历史返回
	*/
	class BattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_BATTLE_RECORD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & BattleRecordList;
		}

		// 押注记录列表
		std::vector<BattleRecord> BattleRecordList;
	};

	//---------------------------------------------------------

	/**
	*@brief client->server 选手排名请求
	*/
	class ShooterRankReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_RANK_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct ShooterRankInfo
	{
		ShooterRankInfo()
		{
			shooterId = 0;
			battleNum = 0;
			winRate = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterId & battleNum & winRate;
		}

		// 射手id
		UInt32 shooterId;
		// 参赛次数
		UInt32 battleNum;
		// 胜率
		UInt32 winRate;
	};

	/**
	*@brief server->client 选手排名返回
	*/
	class ShooterRankRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_RANK_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterRankList;
		}

		// 押注记录列表
		std::vector<ShooterRankInfo> shooterRankList;
	};
}

#endif