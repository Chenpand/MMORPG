#ifndef __CL_PREMIUM_LEAGUE_DEFINE_H__
#define __CL_PREMIUM_LEAGUE_DEFINE_H__

#include <AvalonNetStream.h>
#include <CLDefine.h>
#include <AvalonSocket.h>

enum PremiumLeagueStatus
{
	// ��ʼ״̬
	PLS_INIT,
	// ����
	PLS_ENROLL,
	// Ԥ��
	PLS_PRELIMINAY,
	// ��ǿ׼��
	PLS_FINAL_EIGHT_PREPARE,
	// ��ǿ��
	PLS_FINAL_EIGHT,
	// ��ǿ��
	PLS_FINAL_FOUR,
	// ����
	PLS_FINAL,
	// ���������ȴ�����״̬
	PLS_FINAL_WAIT_CLEAR,
	// ����
	PLS_NUM
};

struct PremiumLeagueStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & status & startTime & endTime;
	}

	UInt8		status;
	UInt32		startTime;
	UInt32		endTime;
};

// ��̭�������Ϣ
struct PremiumLeagueBattleGamer
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & name & occu & ranking & winNum & isLose;
	}

	ObjID_t			roleId;
	std::string		name;
	UInt8			occu;
	UInt32			ranking;
	UInt32			winNum;
	UInt8			isLose;
};

// �Լ����ͽ�������Ϣ
struct PremiumLeagueSelfInfo
{
	PremiumLeagueSelfInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & winNum & score & ranking & enrollCount & loseNum & preliminayRewardNum;
	}

	UInt32		winNum;
	UInt32		score;
	UInt32		ranking;
	UInt32		enrollCount;
	UInt32		loseNum;
	UInt32		preliminayRewardNum;
};

enum PremiumLeagueRewardType
{
	// ��1���Ľ���
	PL_REWARD_NO_1,
	// ��2���Ľ���
	PL_REWARD_NO_2,
	// 3-4���Ľ���
	PL_REWARD_NO_3_4,
	// 5-8���Ľ���
	PL_REWARD_NO_5_8,
	// 9-20���Ľ���
	PL_REWARD_NO_9_20,

	PL_REWARD_NUM
};

class BattleRecordEntry : public Avalon::INetSerializable
{
public:
	BattleRecordEntry() {}
	virtual ~BattleRecordEntry() {}

public:
	UInt32 GetIndex() const { return m_Index; }
	void SetIndex(UInt32 index) { m_Index = index; }

	UInt32 GetTime() const { return m_Time; }
	void SetTime(UInt32 time) { m_Time = time; }

public:
	// ��Ҫ�̳еĽӿ�

	/**
	*@brief �ж�������¼�ǲ�������������
	*/
	virtual bool IsOwner(ObjID_t roleId) = 0;

protected:
	UInt32	m_Index;
	UInt32	m_Time;
};

struct PremiumLeagueRecordFighter
{
	PremiumLeagueRecordFighter() :id(0), winStreak(0), gotScore(0), totalScore(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & winStreak & gotScore & totalScore;
	}

	// ID
	ObjID_t			id;
	// ����
	std::string		name;
	// ��:ս��ǰ����ʤ�� Ӯ:ս������ʤ
	UInt8			winStreak;
	// ��û���
	UInt16			gotScore;
	//�ܻ���
	UInt16			totalScore;
};

class PremiumLeagueRecordEntry : public BattleRecordEntry
{
public:
	PremiumLeagueRecordFighter& GetWinner() { return m_Winner; }
	void SetWinner(PremiumLeagueRecordFighter& fighter) { m_Winner = fighter; }

	PremiumLeagueRecordFighter& GetLoser() { return m_Loser; }
	void SetLoser(PremiumLeagueRecordFighter& fighter) { m_Loser = fighter; }

	/**
	*@brief �ж�������¼�ǲ�������������
	*/
	virtual bool IsOwner(ObjID_t roleId)
	{
		if (m_Winner.id == roleId || m_Loser.id == roleId)
		{
			return true;
		}
		return  false;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Index & m_Time & m_Winner & m_Loser;
	}

protected:
	// ʤ����
	PremiumLeagueRecordFighter	m_Winner;
	// ʧ����
	PremiumLeagueRecordFighter	m_Loser;

};

struct PremiumLeagueBattleFighter
{
	PremiumLeagueBattleFighter()
	{
		id = 0;
		occu = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu;
	}

	ObjID_t		id;
	std::string name;
	UInt8		occu;
};

struct CLPremiumLeagueBattle
{
	CLPremiumLeagueBattle()
	{
		guid = 0;
		raceId = 0;
		type = 0;
		isEnd = 0;
		winnerId = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & raceId & type & fighter1 & fighter2 & isEnd & winnerId & relayAddr;
	}

	ObjID_t							guid;
	ObjID_t							raceId;
	UInt8							type;
	PremiumLeagueBattleFighter		fighter1;
	PremiumLeagueBattleFighter		fighter2;
	UInt8							isEnd;
	ObjID_t							winnerId;
	Avalon::SockAddr				relayAddr;
};

#endif