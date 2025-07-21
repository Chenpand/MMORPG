#ifndef __CL_REPLAY_DEFINE_H__
#define __CL_REPLAY_DEFINE_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLMatchDefine.h>
#include <CLRelayServerProtocol.h>
#include <memory>

enum ReplayListType
{
	REPLAY_LIST_TYPE_INVALID,
	// �Լ���ս��¼
	REPLAY_LIST_TYPE_SELF,
	// ���ֶԾ�
	REPLAY_LIST_TYPE_MASTER,
	// �ղ�
	REPLAY_LIST_TYPE_COLLECTION,
	// ���ֶԾ��ȴ��б�(��ʱ�䵽���ͻ�ӵ����ֶԾ���)
	REPLAY_LIST_TYPE_MASTER_WAIT,
	// �ͽ�������̭��
	REPLAY_LIST_TYPE_PREMIUM_LEAGUE_BATTLE,
	// ����
	REPLAY_LIST_NUM,
};

struct ReplayFighterInfo
{
	ReplayFighterInfo()
	{
		guid = 0;
		roleId = 0;
		occu = 0;
		level = 0;
		pos = 0;
		seasonLevel = 0;
		seasonStars = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & name & occu & level & pos & seasonLevel & seasonStars;
	}

	ObjID_t		guid;
	ObjID_t     roleId;
	std::string	name;
	UInt8       occu;
	UInt16		level;
	UInt8		pos;
	// ������λ
	UInt32		seasonLevel;
	// ��������
	UInt8		seasonStars;
};

// ¼������
enum ReplayEvaluation
{
	REPLAY_EVA_INVALID,
};

struct ReplayInfo
{
	AVALON_DEFINE_OBJECTPOOL(ReplayInfo, Avalon::NullMutex)
	ReplayInfo()
	{
		guid = 0;
		version = 0;
		raceId = 0;
		type = 0;
		evaluation = 0;
		recordTime = 0;
		result = 0;
		score = 0;
		viewNum = 0;
		dbRefNum = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & version & raceId & type & evaluation & recordTime & result & fighters & viewNum & score;
	}

	UInt64				guid;
	// �汾��
	UInt32				version;
	// ����ID
	ObjID_t				raceId;
	// ��������(��Ӧö��PkType)
	UInt8				type;
	// ����(��Ӧö��ReplayEvaluation)
	UInt8				evaluation;
	// ��¼ʱ��
	UInt32				recordTime;
	// ս���������һ����ҵĽ������Ӧö��PkRaceResult��
	UInt8				result;
	// ս��˫����Ϣ
	std::vector<ReplayFighterInfo>	fighters;
	// �ۿ�����
	UInt32				viewNum;
	// ¼�����
	UInt32				score;
	// �����ô���(�������ݿ��еı����ô���)
	UInt32				dbRefNum;
};
typedef std::shared_ptr<ReplayInfo> ReplayInfoPtr;

// ¼���ļ�ͷ
struct ReplayHeader
{
	ReplayHeader()
	{
		battleFlag = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & magicCode & version & startTime & sessionId & raceType & pkDungeonId & duration & players & battleFlag;
	}

	UInt32							magicCode;
	// �汾��
	UInt32							version;
	// ʱ��
	UInt32							startTime;
	// ����ID
	ObjID_t							sessionId;
	// �������ͣ���Ӧö��(RaceType)
	UInt8							raceType;
	// pk����
	UInt32							pkDungeonId;
	// ����ʱ��
	UInt32							duration;
	// �����Ϣ
	std::vector<RacePlayerInfo>		players;
	// ս�����
	UInt64							battleFlag;
};

// ¼���е�ս�����
struct ReplayRaceResult
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & pos & result;
	}

	// ��λ��
	UInt8		pos;
	// ս�����
	UInt8		result;
};

// ¼���ļ�
struct ReplayFile
{
	ReplayFile() : addedToDB(false) {}

	void Input(Avalon::NetInputStream& stream) 
	{
		stream & header;

		UInt32 num = 0;
		stream & num;

		CLProtocol::Frame frame;
		for (UInt32 i = 0; i < num; i++)
		{
			stream & frame;
			frames.push_back(frame);
		}

		stream & results;
	}

	void Output(Avalon::NetOutputStream& stream) 
	{
		stream & header;
		stream & (UInt32)frames.size();
		stream.SeriaArray((CLProtocol::Frame*)&frames[0], frames.size());
		stream & results;
	}

	// �Ƿ��Ѿ����뵽���ݿ�
	bool									addedToDB;
	// �ļ�ͷ
	ReplayHeader							header;
	// �в�����֡
	std::vector<CLProtocol::Frame>			frames;
	// ս�����
	std::vector<ReplayRaceResult>			results;
};

#endif