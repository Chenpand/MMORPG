#ifndef _CL_GAMESESSION_DEFINE_H_
#define _CL_GAMESESSION_DEFINE_H_

#include <CLDefine.h>

enum _resultBitmask  //����ǻ���ģ�
{
	resultBitmask_win = 0x01,					//ʤ
	resultBitmask_lose = 0x02,					//��
	resultBitmask_draw = 0x04,					//ƽ
	resultBitmask_invalid = 0x08,				//��Ч��

};


typedef enum _gameSessionDismissType
{
	gamesessionDismissType_normal,					    //
	gamesessionDismissType_timeout,					    //��ս����ʱ�䳬ʱ
	gamesessionDismissType_wait_timeout,			    //�ȴ���ʼ��ʱ
	gamesessionDismissType_errro,					    //�쳣����
	gamesessionDismissType_system,					    //ϵͳ��ɢ
	gamesessionDismissType_end_timeout,			        //�ȴ�������ʱ
	gamesessionDismissType_offline,					    //���ڲ�ս������
	gamesessionDismissType_framechecksum_timeout,		//֡У�鳬ʱ
	gamesessionDismissType_framechecksum_different,		//֡У�鲻һ��
	gamesessionDismissType_teamcopy_destory_dungeon,	//�ű�����ݵ�

    // ������֮��ʹ��
    gamesessionDismissType_rs_shutdown = 30,            //relayserver����
    gamesessionDismissType_ms_terminate,                //ms֪ͨ��������
}EGameSessionDismissType;

class FightergResult
{
public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & resultFlag & pos & accId & roleId & remainHp & remainMp;
	}

	FightergResult()
	{
		resultFlag = 0;
		pos = 0xff;
		accId = 0;
		roleId = 0;
		remainHp = HP_MP_FULL_VALUE;
		remainMp = HP_MP_FULL_VALUE;
	}

	FightergResult& operator = (const FightergResult& origin)
	{
		if (this == &origin)
		{
			return *this;
		}
		resultFlag = origin.resultFlag;
		pos = origin.pos;
		accId = origin.accId;
		roleId = origin.roleId;
		remainHp = origin.remainHp;
		remainMp = origin.remainMp;

		return *this;
	}

	FightergResult(const FightergResult& origin)
	{
		resultFlag = origin.resultFlag;
		pos = origin.pos;
		accId = origin.accId;
		roleId = origin.roleId;
		remainHp = origin.remainHp;
		remainMp = origin.remainMp;
	}


	UInt8			resultFlag;					//ȡֵ��_resultBitmask
	UInt8			pos;						//ռλ0-9  ���֧��5V5 PK
	UInt32			accId;						//�ʺ�id
	ObjID_t			roleId;						//��ɫid
	UInt32			remainHp;					//ʣ��Ѫ��
	UInt32			remainMp;					//ʣ��ħ��


protected:
private:
};

enum FrameCommandType
{
	// ������ʼ
	FRAME_COMMAND_GAME_START,
	// �ƶ�
	FRAME_COMMAND_MOVE,
	// ֹͣ
	FRAME_COMMAND_STOP,
	// �ż���
	FRAME_COMMAND_SKILL,
	// ����뿪ս��
	FRAME_COMMAND_LEAVE,
	// ����
	FRAME_COMMAND_REBORN,
	// ������ʼ
	FRAME_COMMAND_RECONNECT_BEGIN,
	// ��������
	FRAME_COMMAND_RECONNECT_END,
	// ʹ����Ʒ
	FRAME_COMMAND_USE_ITEM,
	// �������
	FRAME_COMMAND_LEVEL_UP,
	// �Զ�ս��
	FRAME_COMMAND_AUTO_FIGHT,
	// ˫������
	FRAME_COMMAND_DOUBLE_PRESS_CONFIG,
	// ����˳�ս��
	FRAME_COMMAND_QUIT,
	// ս������
	FRAME_COMMAND_RACE_END,
	// ��������
	FRAME_COMMAND_NET_QUALITY,
	// ��ͣ֡��Ŀǰ������ʹ��
	FRAME_COMMAND_RACE_PAUSE,
	// �����л���֡����������֮������֤������
	FRAME_COMMAND_SCENE_CHANGE_AREA,
	// �жϼ���
	FRAME_COMMAND_STOP_SKILL,
	// ���ܲ�������
	FRAME_COMMAND_DO_ATTACK,
	// ƥ�����ͶƱ
	FRAME_COMMAND_MATCH_ROUND_VOTE,
	// ����������
	FRAME_COMMAND_PASS_DOOR,
	// �л�����
	FRAME_COMMAND_CHANGE_WEAPON,
	// ͬ����ͷ
	FRAME_COMMAND_SYNC_SIGHT,
	// boss�׶α仯
	FRAME_COMMAND_BOSS_PHASE_CHANGE,
	// ���³Ǳ�����
	FRAME_COMMAND_DESTORY_DUNGEON,
	// �ű���������
	FRAME_COMMAND_TEAMCOPY_RACE_END,
	// �ű�����Ī˹֮�Ľ���
	FRAME_COMMAND_TEAMCOPY_BIMS_PROGRESS,
	// �ű����³Ǹ������
	FRAME_COMMAND_TEAMCOPY_REVIVE_CNT,
};

// ����¼�
enum SceneServerSyncPlayerEvent
{
	// ����
	SYNC_PLAYER_EVENT_LEVELUP,
	// ����
	SYNC_PLAYER_EVENT_REVIVE,
	// ���³Ǳ�����
	SYNC_DESTORY_DUNGEON,
	// ��������
	SYNC_END_RACE,
	// �ű���ϣĦ˹֮�������������
	SYNC_TEAMCOPY_BIMS_ENERY_PROGRESS,
	// ʹ�õ���
	SYNC_USE_ITEM,
	// �ű����Ӹ������
	SYNC_ADD_REVIVE_CNT,
};

struct PkPlayerRaceEndInfo
{
	PkPlayerRaceEndInfo() : roleId(0), resultFlag(0),remainHp(0),remainMp(0),damagePercent(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & pos & resultFlag & remainHp & remainMp & damagePercent;
	}

	bool operator ==(const PkPlayerRaceEndInfo& info) const
	{
		return roleId == info.roleId && resultFlag == info.resultFlag;
	}

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "roleid:" << roleId << ", pos:" << pos << ", resultFlag:" << resultFlag << ", remainHp:" << remainHp << ", remainMp:" << remainMp << ", damage:" << damagePercent;
		return ss.str();
	}

	ObjID_t			roleId;
	UInt8			pos;
	UInt8			resultFlag;
	UInt32			remainHp;
	UInt32			remainMp;
	// �˺��ٷֱȣ���10000��
	UInt32			damagePercent;
};

struct Pk3V3PlayerRaceEndInfo
{
	Pk3V3PlayerRaceEndInfo() {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roomId & info & seasonLevel & seasonStar & seasonExp & matchScore & matchScoreChange & scoreWarBaseScore & scoreWarContriScore;
	}

	// ����ID
	UInt32			roomId;
	// ������Ϣ
	PkPlayerRaceEndInfo info;
	// ��λ
	UInt32			seasonLevel;
	// ��������
	UInt32			seasonStar;
	// ��������
	UInt32			seasonExp;
	// ����
	UInt32			matchScore;
	// ���ֱ仯
	Int32			matchScoreChange;
	// 3v3������������
	UInt32			scoreWarBaseScore;
	// 3v3���������׷�
	UInt32			scoreWarContriScore;
};

struct PkRaceEndInfo
{
	PkRaceEndInfo() : gamesessionId(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & gamesessionId & results & replayScore;
	}

	bool operator ==(const PkRaceEndInfo& other) const
	{
		return gamesessionId == other.gamesessionId && results == other.results && replayScore == other.replayScore;
	}

	bool operator !=(const PkRaceEndInfo& other) const
	{
		return !(*this == other);
	}

	bool empty() const { return gamesessionId == 0 || results.empty(); }

	std::string ToString()
	{
		std::stringstream ss;
		ss << "session:" << gamesessionId << ", replayScore:" << replayScore << ", infos:";
		for (UInt32 i = 0; i < results.size(); i++)
		{
			auto& info = results[i];
			if (i != 0)
			{
				ss << "|";
			}
			ss << info.ToString();
		}

		return ss.str();
	}

	ObjID_t                                 gamesessionId;
	std::vector<PkPlayerRaceEndInfo>		results;
	UInt32									replayScore;
};

#endif
