#ifndef __CL_PREMIUM_LEAGUE_PROTOCOL_H__
#define __CL_PREMIUM_LEAGUE_PROTOCOL_H__

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLPremiumLeagueDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->client ͬ���ͽ�����״̬
	*/
	class WorldPremiumLeagueSyncStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_SYNC_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		PremiumLeagueStatusInfo		info;
	};

	/**
	*@brief client->world �����ͽ������������Ϣ
	*/
	class WorldPremiumLeagueRewardPoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_REWARD_POOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client �����ͽ���������
	*/
	class WorldPremiumLeagueRewardPoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_REWARD_POOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & enrollPlayerNum & money;
			stream.SeriaArray(rewards, PL_REWARD_NUM);
		}

	public:
		// ��������
		UInt32			enrollPlayerNum;
		// �ܽ����
		UInt32			money;
		// ��������õĽ���
		UInt32			rewards[PL_REWARD_NUM];
	};

	/**
	*@brief client->world �ͽ�������������
	*/
	class WorldPremiumLeagueEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client �ͽ�������������
	*/
	class WorldPremiumLeagueEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32			result;
	};

	/**
	*@brief world->client ��ʼ����̭������б�
	*/
	class WorldPremiumLeagueBattleGamerInit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_GAMER_INIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gamers;
		}

	public:
		std::vector<PremiumLeagueBattleGamer>			gamers;
	};

	/**
	*@brief world->client ������̭�������Ϣ
	*/
	class WorldPremiumLeagueBattleGamerUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_GAMER_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & winNum & isLose;
		}

	public:
		ObjID_t			roleId;
		UInt32			winNum;
		UInt8			isLose;
	};

	/**
	*@brief world->client ����Լ����ͽ�������Ϣ
	*/
	class WorldPremiumLeagueSelfInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_SELF_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		PremiumLeagueSelfInfo	info;
	};

	/**
	*@brief client->world �����ͽ�����ս����¼
	*/
	class WorldPremiumLeagueBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isSelf & startIndex & count;
		}

	public:
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief world->client �����ͽ�����ս����¼
	*/
	class WorldPremiumLeagueBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & totalCount & records;
		}

	public:
		UInt32 totalCount;
		std::vector<PremiumLeagueRecordEntry> records;
	};

	/**
	*@brief world->client ͬ���ͽ�����ս����¼
	*/
	class WorldPremiumLeagueBattleRecordSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BTTTLE_RECORD_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & record;
		}

	public:
		PremiumLeagueRecordEntry record;
	};

	/**
	*@brief world->client �ͽ���������
	*/
	class WorldPremiumLeagueRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_RACE_END)
	public:
		WorldPremiumLeagueRaceEnd() : isPreliminay(0), result(0), oldScore(0), newScore(0), preliminayRewardNum(0), getHonor(0){}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isPreliminay & result & oldScore & newScore & preliminayRewardNum & getHonor;
		}

	public:
		// �ǲ���Ԥѡ��
		UInt8	isPreliminay;
		// ս�����
		UInt8	result;
		UInt32	oldScore;
		UInt32	newScore;
		// ��������
		UInt32  preliminayRewardNum;
		//�������
		UInt32  getHonor;
	};

	/**
	*@brief world->client ��ʼ����̭����ս�б�
	*/
	class WorldPremiumLeagueBattleInfoInit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_INFO_INIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battles;
		}

	public:
		std::vector<CLPremiumLeagueBattle>	battles;
	};

	/**
	*@brief world->client ������̭����ս��Ϣ
	*/
	class WorldPremiumLeagueBattleInfoUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_BATTLE_INFO_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battle;
		}

	public:
		CLPremiumLeagueBattle	battle;
	};

	/**
	*@brief world->scene �ͽ��������㵽scene
	*/
	class WorldPremiumLeagueEndToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PREMIUM_LEAGUE_END_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & pvpType & honor & reason;
		}

		UInt64 roleId;
		UInt32 pvpType;
		UInt32 honor;
		std::string reason;
	};
};


#endif