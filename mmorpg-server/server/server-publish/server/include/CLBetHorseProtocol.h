#ifndef _CL_BET_HORSE_PROTOCOL_H_
#define _CL_BET_HORSE_PROTOCOL_H_

#include "CLProtocolDefine.h"


namespace CLProtocol
{
	/**
	*@brief ������Ϣ
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

		// ����Ψһid
		UInt32 id;
		// ��������id
		UInt32 dataId;
		// ״̬
		UInt32 status;
		// ����
		UInt32 odds;
		// ʤ��
		UInt32 winRate;
		// �Լ���(��ڴ���)
		UInt32 champion;
	};

	/**
	*@brief ��ͼ��Ϣ
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

		// ��ͼid
		UInt32 id;
		// ����
		UInt32 terrain;
		// ʤ��������
		UInt32 winShooterId;
		// ��ͼ�ϵ�����
		std::vector<UInt32> shooterList;
	};

	/**
	*@brief client->server �����������
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
	*@brief server->client ������淵��
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

		// ��ǰ����
		UInt32 weather;
		// ��������id
		UInt32 mysteryShooter;
		// ����׶�
		UInt32 phase;
		// ʱ���
		UInt32 stamp;
		// �����б�
		std::vector<ShooterInfo> shooterList;
		// ��ͼ�б�
		std::vector<BetHorseMapInfo> mapList;
	};

	/**
	*@brief client->server ������������
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
	*@brief ����������Ϣ
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

		// ����id
		UInt32 shooterId;
		// ����
		UInt32 shooterOdds;
	};

	/**
	*@brief server->client �������ʷ���
	*/
	class ShooterOddsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_ODDS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oddsList;
		}

		// �����б�
		std::vector<OddsRateInfo> oddsList;
	};

	/**
	*@brief client->server ������ʷս������
	*/
	class ShooterHistoryRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_HISTORY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterId;
		}

		// ����id
		UInt32 shooterId;
	};

	/**
	*@brief ������ʷ��Ϣ
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

		// ����
		UInt32 courtId;
		// �Լ�������
		UInt32 odds;
		// ʤ��
		UInt32 result;
	};

	/**
	*@brief server->client ������ʷս������
	*/
	class ShooterHistoryRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_HISTORY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & recordList;
		}

		// ����id
		UInt32 id;
		// ��ʷս���б�
		std::vector<ShooterRecord> recordList;
	};

	/**
	*@brief client->server ���Ѻע����
	*/
	class StakeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & num;
		}

		// Ѻע������
		UInt32 id;
		// Ѻע�ӵ�����
		UInt32 num;
	};

	/**
	*@brief server->client ���Ѻע����
	*/
	class StakeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & id & num;
		}

		// Ѻע���
		UInt32 ret;
		// Ѻע������
		UInt32 id;
		// Ѻע�ӵ�����
		UInt32 num;
	};

	/**
	*@brief server->client ����׶�״̬ͬ��
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

		// ����׶�ͬ��
		UInt32 phaseSycn;
		// ʱ���
		UInt32 stamp;
	};

	/**
	*@brief client->server ���Ѻע��ʷ����
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

		// ����id
		UInt32 courtId;
		// Ѻע����
		UInt32 stakeShooter;
		// ����
		UInt32 odds;
		// ֧Ԯ����
		UInt32 stakeNum;
		// ӯ��
		Int32 profit;
	};

	/**
	*@brief server->client ���Ѻע��ʷ����
	*/
	class StakeRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_STAKE_RECORD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & StakeRecordList;
		}

		// Ѻע��¼�б�
		std::vector<StakeRecord> StakeRecordList;
	};

	//---------------------------------------------------------

	/**
	*@brief client->server ������ʷ����
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

		// ����id
		UInt32 courtId;
		// �ھ�����
		UInt32 champion;
		// ����
		UInt32 odds;
		// ���ӯ��
		UInt32 maxProfit;
	};

	/**
	*@brief server->client ������ʷ����
	*/
	class BattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_BATTLE_RECORD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & BattleRecordList;
		}

		// Ѻע��¼�б�
		std::vector<BattleRecord> BattleRecordList;
	};

	//---------------------------------------------------------

	/**
	*@brief client->server ѡ����������
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

		// ����id
		UInt32 shooterId;
		// ��������
		UInt32 battleNum;
		// ʤ��
		UInt32 winRate;
	};

	/**
	*@brief server->client ѡ����������
	*/
	class ShooterRankRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_BETHORSE_SHOOTER_RANK_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shooterRankList;
		}

		// Ѻע��¼�б�
		std::vector<ShooterRankInfo> shooterRankList;
	};
}

#endif