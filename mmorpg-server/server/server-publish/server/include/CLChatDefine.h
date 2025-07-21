#ifndef _CL_CHAT_DEFINE_H_
#define _CL_CHAT_DEFINE_H_

#include "CLDefine.h"
#include <AvalonNetStream.h>

//����������ݳ���
#define CL_MAX_CHAT_LEN (512)
//�����ʾ���
#define CL_MAX_SHOW_WIDTH (100)
//�����Ƶ���ƴ������ֵ
#define CHAT_HI_FRQ_MAX		5
//��Ƶ�������ʱ��
#define CHAT_HI_FRQ_TIME	1 * 60

/**
 *@brief ����ϵͳƵ������
 */
enum ChatChannelType
{
	CHAT_CHANNEL_AROUND = 1,	//����Ƶ��
	CHAT_CHANNEL_TEAM = 2,		//����Ƶ��
	CHAT_CHANNEL_WORLD = 3,		//����
	CHAT_CHANNEL_PRIVATE = 4,	//˽��
	CHAT_CHANNEL_TRIBE = 5,		//����
	CHAT_CHANNEL_TEAM_SPECIAL = 7,	//����Ȥζ����
	CHAT_CHANNEL_SYSTEM = 8,	//ϵͳ
	CHAT_CHANNEL_TEAM_INVITE = 9,	//�������Ƶ��
	CHAT_CHANNEL_HORN = 10,		// ����
	CHAT_CHANNEL_ROOM = 11,		//����

	CHAT_CHANNEL_TEAM_COPY_PREPARE = 12,	//�ű�׼��
	CHAT_CHANNEL_TEAM_COPY_TEAM = 13,		//�ű�����
	CHAT_CHANNEL_TEAM_COPY_SQUAD = 14,		//�ű�С��

	CHAT_CHANNEL_BATTLE = 15,			//ս��
	CHAT_CHANNEL_BAT_LOST_TEAM = 16,	//ս�����ζ���
	//TODO
	CHAT_CHANNEL_MAX,			
};



//����������
const static UInt32 CHAT_INTERVAL[] = { 0, 30000, 1000, 30000, 500, 1000, 10000, 100, 100, 30000, 0, 0, 30000, 5000, 5000 };
const static UInt32 NEWERTRIBE_CHAR_INTERVAL = 8000;

struct ChatDataInfo
{
	//Ƶ��
	UInt8		channel;
	//˵����id
	ObjID_t		objid;
	//�Ա�
	UInt8		sex;
	//ְҵ
	UInt8		occu;
	//�ȼ�
	UInt16		level;
	//vip�ȼ�
	UInt8		viplvl;
	//˵��������
	std::string	objname;
	//������id
	ObjID_t		receiverId;
	//���
	std::string	word;
	//�Ƿ�link
	UInt8		bLink;
	//�Ƿ�gm
	UInt8		isGm;
	//ʱ��
	UInt32		time;
	//����key
	std::string voiceKey;
	//��������
	UInt8		voiceDuration;
	// ͷ���
	UInt32		headFrame;
};

/**
 *@brief �����ʱ�� 
 */
class ChatTimer
{
public:
	ChatTimer(){ memset(m_Times,0,sizeof(m_Times)); }
	~ChatTimer(){}

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(m_Times, CHAT_CHANNEL_MAX);
	}

	/**
	 *@brief �ж�ʱ��
	 *@return ����жϳɹ�������0�����򷵻�ʣ��ʱ��
	 */
	inline UInt64 CheckChatTime(int channel, UInt64 now);
	inline void SetChatTime(int channel, UInt64 now);
	inline UInt64 GetChatTime(int channel);

private:
	//�ϴ�����ʱ��
	UInt64 m_Times[CHAT_CHANNEL_MAX];
};

UInt64 ChatTimer::CheckChatTime(int channel,UInt64 now)
{
	if(channel >= CHAT_CHANNEL_MAX) return 0;

	if(now < m_Times[channel] + CHAT_INTERVAL[channel]){
		return m_Times[channel] + CHAT_INTERVAL[channel] - now;
	}
	return 0;
}

void ChatTimer::SetChatTime(int channel, UInt64 now)
{
	if(channel >= CHAT_CHANNEL_MAX) return;

	m_Times[channel] = now;
}

UInt64 ChatTimer::GetChatTime(int channel)
{
	if(channel >= CHAT_CHANNEL_MAX) return 0;

	return m_Times[channel];
}

/** 
 *@brief ����������������
 */
enum ChatLinkDataType
{
	CHAT_LINKDATA_INVALID = 0,
	CHAT_LINKDATA_ITEM = 1,
	CHAT_LINKDATA_PET = 2,
};


/**
 *@brief ��������
 */
enum RequestType
{
	REQUEST_TRADE = 0,				//��������
	REQUEST_INVITETEAM = 1,			//�������
	REQUEST_JOINTEAM = 2,			//�������(���ݶ����ԱID)
	REQUEST_FRIEND = 3,				//�����Ϊ����
	REQUEST_MASTER = 4,			//�����ʦ
	REQUEST_DISCIPLE = 5,		//������ͽ
	REQUEST_RECOMMENDTRIBE = 6,		//�Ƽ�����
	REQUEST_JOINCOPYTEAM = 7,		//������븱������
	REQUEST_CANCELENTER = 8,		//ȡ�����벿������
	REQUEST_TRIBECALL = 9,			//�����ٻ�
	REQUEST_BLESSLEVELUP = 10,		//ף������
	REQUEST_QUICKFRIEND = 11,		//һ������
	REQUEST_FLOWER = 14,			 //�����ʻ���ʾ
	REQUEST_TRIBE_UNION = 15,		//��������
	REQUEST_TRIBE_CANCEL_UNION = 16,	//ȡ����������
	REQUEST_TRIBEWAR_INVITE = 17,	//����ս����
	REQUEST_TRIBEWAR_CL_INVIATE = 18,	//ȡ������ս����
	REQUEST_MARRY = 19,				//���
	REQUEST_MARRY_SUCC = 20,		//���ɹ�
	REQUEST_JOINTEAM_TEAMID = 21,	//�������(���ݶ���ID)

	REQUEST_MARRY_ASK = 22,			//��Ҫ����
	REQUEST_MARRY_GIVEGIFT1 = 23,	//����С����
	REQUEST_MARRY_GIVEGIFT2 = 24,	//�����к���
	REQUEST_MARRY_GIVEGIFT3 = 25,	//���ʹ����
	REQUEST_MARRY_RITE = 26,		//�����
	REQUEST_DIVORCE = 27,			//���

	REQUEST_ROLL = 28,				//roll��

	REQUEST_FRIEND_BY_NAME = 29,				//ͨ�����������Ϊ����

    REQUEST_CHALLENGE_PK = 30,      // ��ս
	REQUEST_INVITEGUILD = 31,		// ���빫��
	REQUEST_EQUAL_PK    = 32,       //��ƽ����������
	//TODO
	REQUEST_MAX,
};

//ÿ�������������
#define	MAX_REQUEST_NUM		100
//���������Чʱ��
#define REQUEST_VALID_TIME	600

//�����������ȼ�
#define MIN_CHAT_LEVEL	10

struct HornInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roldId & name & occu & level & viplvl & content & minTime & maxTime & combo & num & headFrame;
	}

	// �����ߵĻ�����Ϣ
	//��ɫid
	ObjID_t			roldId;
	//����
	std::string		name;
	//ְҵ
	UInt8			occu;
	//�ȼ�
	UInt16			level;
	//vip�ȼ�
	UInt8			viplvl;
	
	// ���Ⱦ�����Ϣ
	// ����
	std::string		content;
	// ����ʱ��
	UInt8			minTime;
	// ����ʱ��
	UInt8			maxTime;
	// combo��
	UInt16			combo;
	// ��������
	UInt8			num;
	// ͷ���
	UInt32			headFrame;
};

// �����Ϣ
enum ChatMask
{
	CHAT_MASK_RED_PACKET = 1 << 0,
	CHAT_MASK_ADD_FRIEND = 1 << 1,		//��Ӻ���
	CHAT_MASK_NOT_HEADPOINT = 1 << 2,	//��Ҫ�����ʾ
	CHAT_MASK_SYS = 1 << 3, //ϵͳ��ʾ
};

// �Զ�����־�ϱ�����
enum CustomLogReportType
{
	CLRT_INVALID,
	// ������������
	CLRT_JOIN_VOICE_ROOM,
	// �˳���������
	CLRT_QUIT_VOICE_ROOM,
	// ����¼��
	CLRT_SEND_RECORD_VOICE,
	// ����¼��
	CLRT_LOAD_RECORD_VOICE,
};

#endif
