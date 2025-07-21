#ifndef __RACE_DEFINE_H__
#define __RACE_DEFINE_H__

#include <CLDefine.h>
#include <AvalonSimpleTimer.h>
#include <functional>  

typedef enum _gamesessionEventId
{
	gamesessionEventId_dismiss = 1,				//��ս����
	gamesessionEventId_fighter_offline = 2,				//��ս������
	gamesessionEventId_start = 3,				//��ս��ʼ
	gamesessionEventId_fighter_login = 4,				//��ս�߽���
	gamesessionEventId_fighter_want_endgame = 5,				//��ս��Ҫ�����ս�����ϱ����Լ��Ķ�ս�����
	gamesessionEventId_fighter_input = 6,				//�ϱ���Ϸ����
	gamesessionEventId_fighter_framechecksum = 7,				//�ϱ�֡У������

}EGameSessionEventID;

typedef struct _gamesessionEvent
{
	UInt8		id;

	//other fields

}GameSessionEvent;

enum
{
    // ����ս���� 0~4 ��һ����5-9 ������һ��
    MAX_GAMER_COUNT                                 = 10,

    //֡У��ļ������λ��֡��
    FRAME_CHECKSUM_INTERVAL                         = 50,

	//���³������ȴ�ʱ��
	DUNGEON_RACE_RECONN_WAIT_TIME					= 5 * 60,

	//PVP�����ȴ�ʱ��
	PVP_RACE_RECONN_WAIT_TIME						= 90,	
};

// ������������
enum RaceInstanceType
{
    RACE_PVP,
    RACE_DUNGEON,
	RACE_PK_ROOM,
    RACE_NUM,
};

enum RaceStatus
{
    // ��ʼ״̬
    RACE_STATUS_INIT,
    // �ȴ�������ҵ�½
    RACE_STATUS_WAIT_LOGIN,
    // �ȴ�������Ҽ���
    RACE_STATUS_WAIT_LOAD,
    // ������
    RACE_STATUS_RACING,
    // �ȴ�������Ϣ
    RACE_STATUS_WAIT_RACE_END,
	// ����
	RACE_STATUS_RACE_END,
	// �ȴ��������������������ߺ�ᵽ���״̬��
	RACE_STATUS_WAIT_RECONNECT,
	// �ȴ����ݻ�
	RACE_STATUS_DESTORY,
    // ״̬����
    RACE_STATUS_NUM,
};

// ����뿪����ԭ��
enum GamerLeaveReason
{
    // ����
    GAMER_LEAVE_DISCONNECT,
    // ��¼��ʱ
    GAMER_LEAVE_LOGIN_TIMEOUT,
    // �����뿪
    GAMER_LEAVE_INITIATIVE,
    // ��ͬ��
    GAMER_LEAVE_UNSYNC,
	// ���س�ʱ
	GAMER_LEAVE_LOAD_TIMEOUT,
	// ��ʱ��û�յ������
	GAMER_LEAVE_NOT_RECV_CHECKSUM,
	// ������ʱ
	GAMER_LEAVE_HEARTBEAT_TIMEOUT,
};

struct RaceStatusTimer
{
    RaceStatusTimer()
    {
        active = false;
        waitTime = 0;
        //timeOutFunc = NULL;
    }
    // �Ƿ񼤻�
    bool                    active;

    // �ȴ�ʱ�䣨ms��
    UInt32                  waitTime;

    // ��ʱ������
    typedef std::function<void()> RaceStatusTimeOutFunc;
    RaceStatusTimeOutFunc   timeOutFunc;

    // ��ʱ��
    Avalon::SimpleTimer     timer;
};

struct RaceChecksumInfo
{
	RaceChecksumInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// ֡��
	UInt32 frame;
	// ������ҵ�У�飨����λ�ţ�
	UInt32 checksums[MAX_GAMER_COUNT];
};

#endif