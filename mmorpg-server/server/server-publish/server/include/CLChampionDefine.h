#ifndef __CL_CHAMPION_DEFINE_H__
#define __CL_CHAMPION_DEFINE_H__

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLSyncObject.h>
#include <CLSceneObject.h>

const UInt32 CHAMPION_ENROLL_GLOD = 3000;


const UInt32 CHAMPION_GAMBLE_COIN_ITEM_ID = 330000271;


const UInt32 CHAMPION_GAMBLE_OPTION_LIMIT = 1000;

const UInt32 CHAMPION_BATTLE_PACKET_LIMIT_ROLE = 6000;


enum ChampionStatus
{
	// ��״̬
	CS_NULL = 0,
	// ����
	CS_ENROLL = 1,

	CS_PREPARE_BEGIN = 10,
	// ��ѡ׼��
	CS_SEA_SELECT_PREPARE = 11,
	// ����׼��
	CS_RE_SEA_SELECT_PREPARE = 12,
	// 64ǿ׼��
	CS_64_SELECT_PREPARE = 13,
	// 16ǿ׼��
	CS_16_SELECT_PREPARE = 14,
	// 8ǿ׼��
	CS_8_SELECT_PREPARE = 15,
	// 4ǿ׼��
	CS_4_SELECT_PREPARE = 16,
	// �����׼��
	CS_2_SELECT_PREPARE = 17,
	// ����׼��
	CS_1_SELECT_PREPARE = 18,
	CS_PREPARE_END,

	CS_BATTLE_BEGIN = 50,
	// ��ѡս��
	CS_SEA_SELECT_BATTLE = 51,
	// ����ս��
	CS_RE_SEA_SELECT_BATTLE = 52,
	// 64ǿս��
	CS_64_SELECT_BATTLE = 53,
	// 16ǿս��
	CS_16_SELECT_BATTLE = 54,
	// 8ǿս��
	CS_8_SELECT_BATTLE = 55,
	// 4ǿս��
	CS_4_SELECT_BATTLE = 56,
	// �����ս��
	CS_2_SELECT_BATTLE = 57,
	// ����ս��
	CS_1_SELECT_BATTLE = 58,
	CS_BATTLE_END,

	//����׶��Ǹ��ͻ�����ʾ�õ�
	CS_PRE_BEGIN = 80,
	// ��ѡ����ǰ
	CS_SEA_SELECT_PRE = 81,
	// ��������ǰ
	CS_RE_SEA_SELECT_PRE = 82,
	// 64ǿ����ǰ
	CS_64_SELECT_PRE = 83,
	// 16ǿ����ǰ
	CS_16_SELECT_PRE = 84,
	// 8ǿ����ǰ
	CS_8_SELECT_PRE = 85,
	// 4ǿ����ǰ
	CS_4_SELECT_PRE = 86,
	// ���������ǰ
	CS_2_SELECT_PRE = 87,
	// ��������ǰ
	CS_1_SELECT_PRE = 88,
	CS_PRE_END,

	// ��������չʾ״̬
	CS_END_SHOW = 95,

	// ����
	CS_NUM
};


enum ChampionRaceAttribute
{
	CRA_GROUP_ID,
	CRA_RACE_ID,
	CRA_STATUS,
	CRA_ROLE_ID_1,
	CRA_ROLE_ID_2,
	CRA_IS_END,
	CRA_WINNER_ID,
	CRA_RESULT,
};


enum ChampionRelive
{
	CR_NO = 0,
	CR_CAN = 1,
	CR_ALREADY = 2,
};


enum ChampionAct
{
	CA_ENROLL = 0,
	CA_RELIVE = 1,
	CA_JOIN_RACE = 2,
	CA_RACE_END = 3,
	CA_OBSERVE = 4,
	CA_QUERY_SELF_ATTRIBUTES = 5,
	CA_CHECK_RELIVE = 6,
};

enum ChampionGroupBattleStatus
{
	CGBS_NOT_START = 0,
	CGBS_STARTED = 1,
	CGBS_END = 2,
};

struct ChampionStatusInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & status & startTime & endTime & preStartTime & prepareStartTime & battleStartTime & battleEndTime;
	}

	bool operator!=(const ChampionStatusInfo& rsh) const
	{
		if (status == rsh.status && startTime == rsh.startTime && endTime == rsh.endTime)
		{
			return false;
		}
		return true;
	}

	UInt32		status;
	UInt32		startTime;
	UInt32		endTime;
	UInt32		preStartTime;
	UInt32		prepareStartTime;
	UInt32		battleStartTime;
	UInt32		battleEndTime;
};


struct ChampionTop16Player
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & pos & name & occu  & avatar & server & zoneID;
	}

	UInt64			roleID;
	UInt32			pos;
	std::string		name;
	UInt8			occu;
	PlayerAvatar	avatar;
	std::string		server;
	UInt32			zoneID;
};

struct ChampionGambleOption
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & option & num & odds;
	}

	UInt64 option;
	UInt64 num;
	UInt32 odds;
};

struct ChampionGambleInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & optionVec  & startTime & endTime & param;
	}

	UInt32 id;
	UInt8 type;
	std::vector<ChampionGambleOption> optionVec;
	UInt32 startTime;
	UInt32 endTime;
	UInt64 param;
};

struct ChampionGambleRecord
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & option & result  & reward & time & num;
	}

	UInt32 id;
	UInt64 option;
	UInt64 result;
	UInt64 reward;
	UInt32 time;
	UInt64 num;
};


struct RankRole
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & name & score & server;
	}
	UInt64 roleID;
	std::string name;
	UInt32 score = 0;
	std::string server;
};

//������¼
struct ChampionBattleRecord 
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & order & raceID & winner & isEnd;
	}
	//�ڼ�������
	UInt32 order;
	//����ID
	UInt64 raceID;
	//ʤ��id
	UInt64 winner;
	//�Ƿ���� 0��δ���� 1���ѽ���
	UInt8 isEnd;
};

//��̭��ս������
struct ChampionKnockoutPlayer {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleID & name & occu & server & score;
	}
	UInt64 roleID;
	std::string name;
	UInt8 occu;
	std::string server;
	UInt32 score;
};

//������ս����¼
struct ChampionScoreBattleRecord {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleIDA & nameA  & roleIDB & nameB & result;
	}
	UInt64 roleIDA;
	UInt64 roleIDB;
	std::string nameA;
	std::string nameB;
	UInt8 result;
};

struct ChampionGroupStatus {
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & groupID & roleA & roleB & scoreA & scoreB & status;
	}
	bool IsChanged(const ChampionGroupStatus& other)
	{
		if (roleA == other.roleA)
		{
			if (scoreA != other.scoreA || scoreB != other.scoreB || status != other.status)
			{
				return true;
			}
		}
		else if (roleA == other.roleB)
		{
			if (scoreA != other.scoreB || scoreB != other.scoreA || status != other.status)
			{
				return true;
			}
		}
		return false;
	}
	UInt32 groupID;
	UInt64 roleA;
	UInt64 roleB;
	UInt32 scoreA;
	UInt32 scoreB;
	//0 δ��ʼ 1�ѿ�ʼ 2�ѽ���
	UInt8 status;
};

struct ChampionRaceData : public CLSyncObject
{
	ObjUInt32 order;
	ObjUInt32 groupID;
	ObjUInt64 raceID;
	ObjUInt8 championStatus;
	ObjUInt64 roleID1;
	ObjUInt64 roleID2;
	ObjUInt8 isEnd;
	ObjUInt64 winnerID;
	ObjUInt32 result;
	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("order", order)
		CL_DBPROPERTY("group_id", groupID)
		CL_DBPROPERTY("race_id", raceID)
		CL_DBPROPERTY("status", championStatus)
		CL_DBPROPERTY("roleid_1", roleID1)
		CL_DBPROPERTY("roleid_2", roleID2)
		CL_DBPROPERTY("is_end", isEnd)
		CL_DBPROPERTY("winner_id", winnerID)
		CL_DBPROPERTY("result", result)
	CL_DBPROPERTY_END()

	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)
		CL_SYNC_PROPERTY(championStatus, CRA_STATUS, SST_TEST)
		CL_SYNC_PROPERTY(roleID1, CRA_ROLE_ID_1, SST_TEST)
		CL_SYNC_PROPERTY(roleID2, CRA_ROLE_ID_2, SST_TEST)
		CL_SYNC_PROPERTY(winnerID, CRA_WINNER_ID, SST_TEST)
		CL_SYNC_PROPERTY(isEnd, CRA_IS_END, SST_TEST)
		CL_SYNC_PROPERTY(result, CRA_RESULT, SST_TEST)
	CL_SYNC_PROPERTY_END()
};

#endif