#ifndef _CL_DEFINE_H_
#define _CL_DEFINE_H_

#include <sstream>
#include <AvalonDefine.h>
#include <AvalonTime.h>
#include <AvalonDate.h>
#include <AvalonLogger.h>
#include <AvalonPos2D.h>
#include <AvalonSingleton.h>
#include <AvalonStringUtil.h>

/**
 *@brief ����avalon�������Ͷ���
 */
using Avalon::Int8;
using Avalon::UInt8;
using Avalon::Int16;
using Avalon::UInt16;
using Avalon::Int32;
using Avalon::UInt32;
using Avalon::Int64;
using Avalon::UInt64;
using Avalon::Real32;
using Avalon::Real64;

using Avalon::success;
using Avalon::fail;

/**
 *@brief ��Ϸ���Ͷ���
 */
typedef UInt64 ObjID_t;
#define ID_FORMAT AVALON_UINT64_FORMAT


//��Ϸ��id
extern UInt32 ZONE_ID;
//������id
extern UInt32 NEUTRAL_ZONE_ID;
//����������
extern UInt32 SERVER_TYPE;
//������id
extern UInt32 SERVER_ID;
//�Ƿ��нű�����
extern UInt32 SCRIPT_ERROR;
//��ǰʱ��
extern Avalon::Time CURRENT_TIME;
//��ǰ����
extern Avalon::Date CURRENT_DATE;
//����ʱ��
extern UInt32	GAMESTART_TIME;
//����ȼ�
extern UInt32	WORLD_LEVEL;
//�������汾��
extern std::string GAME_VERSION;
//ƽ̨
extern std::string PLATFORM;
//��һ�ο���ʱ��
extern UInt32 GAME_FIRST_START_TIME;
//��������
extern std::string SERVER_NAME;
//����ķ�����(id��)
extern std::string MERGED_SERVERS;
//һ��Tick��ദ�����Ϣ��
extern UInt32 MAX_PACKET_PERTICK;

//��ת��
#define DAY_TO_SEC	86400
//Сʱת��
#define HOUR_TO_SEC	3600
//����ת��
#define MIN_TO_SEC	60
//second to micro second
#define SEC_TO_MSEC 1000
//��ת����
#define DAY_TO_MSEC 86400000

//ÿ֡16ms
#define TIME_OF_FRAME_MSEC	16
#define PHYSICAL_FRAME_MSEC	64		//added by aprilliu,2016.06.01 ����֡Ƶ

// ���¼�������
#define  SIG_RELOAD_CONFIG 10

#define U_INT16_MAX 65535


/**
 *@brief �ж��Ƿ�ͬһ��
 */
inline bool IsSameDay(const Avalon::Time& time1, const Avalon::Time& time2)
{
	Avalon::Date date1(time1);
	Avalon::Date date2(time2);

	if(date1.Day() != date2.Day() || date1.Month() != date2.Month() || date1.Year() != date2.Year())
		return false;
	return true;
}

inline bool IsSameDay(UInt32 time1, UInt32 time2)
{
	return ::IsSameDay(Avalon::Time(UInt64(time1) * 1000), Avalon::Time(UInt64(time2) * 1000));
}

/**
 *@brief ��ȡ���쿪ʼ��ʱ���                                                                     
 */
inline UInt64 GetDayBeginTime(const Avalon::Time& time)
{
	Avalon::Date date(time);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);
	return date.ToTime().MSec();
}

/**
*@brief ��ȡ���쿪ʼ��ʱ���
*/
inline UInt64 GetDayBeginTime2(const Avalon::Time& time)
{
	Avalon::Date nowDate(time);
	Avalon::Date dayBeginDate = nowDate;
	dayBeginDate.Hour(6);
	dayBeginDate.Min(0);
	dayBeginDate.Sec(0);
	dayBeginDate.MSec(0);
	auto dayBeginTime = dayBeginDate.ToTime().MSec();

	if (nowDate.Hour() < 6)
	{
		dayBeginTime -= Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
	}

	return dayBeginTime;
}

/**
*@brief �Ƚ�����ʱ����Ƿ���ͬһ�߼���
*/
inline bool CheckInSameLogicDay(const UInt32& time1, const UInt32& time2)
{
	Avalon::Time t1((UInt64)time1 * Avalon::Time::MSECS_OF_SEC);
	Avalon::Time t2((UInt64)time2 * Avalon::Time::MSECS_OF_SEC);

	return GetDayBeginTime2(t1) == GetDayBeginTime2(t2);
}

/**
 *@brief ��ȡ�����ǿ����ڼ���                                                                     
 */
inline UInt32 GetGameStartDays()
{
	UInt32 gamestartDay = UInt32(GetDayBeginTime(Avalon::Time(UInt64(GAMESTART_TIME) * 1000)) / 1000);
	UInt32 today = UInt32(GetDayBeginTime(CURRENT_TIME) / 1000);
	if(today <= gamestartDay) return 1;

	return (today - gamestartDay) / DAY_TO_SEC + 1;
}

/*
*	��ø���ʱ�������ܵĿ�ʼʱ��
* \param	time		ʱ��
* \param	beginWDay	ÿ�ܵ���ʾʱ����ܼ���ʼ 0:���� 1:��һ .... 6:����
* \return	UInt64		�����ܿ�ʼ��ʱ��
* ������Ϊ���,����ֵ�� ���յ� 0��0��0��
* ����һΪ���,����ֵ�� ��һ�� 0��0��0��
*/
inline UInt64 getWeekBeginTime(const Avalon::Time& time, int beginWDay = 1, int beginDayHour = 0)
{
	Avalon::Date date(time);
	date.Hour(beginDayHour);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);

	UInt64 t_time = date.ToTime().Sec();

	if (date.WDay() < beginWDay)
		t_time -= (7 - beginWDay + date.WDay()) * 24 * 60 * 60;
	else if (date.WDay() > beginWDay)
		t_time -= (date.WDay() - beginWDay) * 24 * 60 * 60;
	else
	{
		if (time.Sec() >= t_time)
		{
			return t_time;
		}
		else
		{
			t_time -= (7 - beginWDay + date.WDay()) * 24 * 60 * 60;
		}
	}
	return t_time;
}

/*
*	��ø���ʱ�������ܵĿ�ʼʱ��
* \param	time		ʱ���(����)
* \param	beginWDay	ÿ�ܵ���ʾʱ����ܼ���ʼ 0:���� 1:��һ .... 6:����
* \return	UInt64		�����ܿ�ʼ��ʱ��
* ������Ϊ���,����ֵ�� ���յ� 0��0��0��
* ����һΪ���,����ֵ�� ��һ�� 0��0��0��
*/
inline UInt64 getWeekBeginTime(UInt64 time, int beginWDay = 1, int beginDayHour = 0)
{
	Avalon::Time curTime(time);
	return getWeekBeginTime(curTime, beginWDay, beginDayHour);
}

/**
*@brief �Ƿ���ͬһ��
*/
inline bool isSameWeek(const Avalon::Time& time1, const Avalon::Time& time2, int beginWDay = 1, int beginDayHour = 0)
{
	return getWeekBeginTime(time1, beginWDay) == getWeekBeginTime(time2, beginWDay);
}

/**
*@brief �Ƿ���ͬһ��
*/
inline bool isSameWeek(UInt64 time1, UInt64 time2, int beginWDay = 1, int beginDayHour = 0)
{
	return getWeekBeginTime(time1, beginWDay, beginDayHour) == getWeekBeginTime(time2, beginWDay, beginDayHour);
}

/**
*@brief �Ƿ���ͬһ�߼��ܣ���һ����Ϊ�ָ���
*/
inline bool isSameLogicWeek(UInt32 time1, UInt32 time2)
{
	return isSameWeek(time1*1000, time2*1000, 1, 6);
}

inline bool isSameLogicWeek(const Avalon::Time& time1, const Avalon::Time& time2)
{
	return isSameWeek(time1, time2, 1, 6);
}

/*
*	��ø���ʱ�������µĿ�ʼʱ��
* \param	time		ʱ��
* \return	UInt64		�����¿�ʼ��ʱ��
*/
inline UInt64 getMonthBeginTime(const Avalon::Time& time)
{
	Avalon::Date date(time);
	date.Day(1);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);

	return date.ToTime().Sec();
}

inline UInt64 getMonthLogicBeginTime(const Avalon::Time& time)
{
	Avalon::Date date(time);
	date.Day(1);
	date.Hour(6);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);

	return date.ToTime().Sec();
}

inline UInt64 getMonthLogicBeginTime(UInt64 time)
{
	Avalon::Time curTime(time);
	return getMonthLogicBeginTime(curTime);
}

inline UInt64 getNextMonthLogicBeginTime(UInt64 curTime)
{
	Avalon::Date date(curTime);
	date.Day(1);
	date.Hour(6);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);
	if (date.Month() == 12)
	{
		date.Year(date.Year() + 1);
		date.Month(1);
	}
	else
	{
		date.Month(date.Month() + 1);
	}
	return date.ToTime().Sec();
}

inline UInt64 getLastMonthLogicBeginTime(UInt64 curTime)
{
	Avalon::Date date(curTime);
	date.Day(1);
	date.Hour(6);
	date.Min(0);
	date.Sec(0);
	date.MSec(0);
	if (date.Month() == 1)
	{
		date.Year(date.Year() - 1);
		date.Month(12);
	}
	else
	{
		date.Month(date.Month() - 1);
	}
	return date.ToTime().Sec();
}
/*
*	��ø���ʱ�������µĿ�ʼʱ��
* \param	time		ʱ��
* \return	UInt64		�����¿�ʼ��ʱ��
*/
inline UInt64 getMonthBeginTime(UInt64 time)
{
	Avalon::Time curTime(time);
	return getMonthBeginTime(curTime);
}

/**
*@brief �Ƿ���ͬһ��
*/
inline bool isSameMonth(const Avalon::Time& time1, const Avalon::Time& time2)
{
	return getMonthBeginTime(time1) == getMonthBeginTime(time2);
}

/**
*@brief �Ƿ���ͬһ��
*/
inline bool isSameMonth(UInt64 time1, UInt64 time2)
{
	return getMonthBeginTime(time1) == getMonthBeginTime(time2);
}

/**
*@brief ��ȡ������yyyymmdd
*/
inline UInt32 GetYearMonthDay(const Avalon::Time& time)
{
	Avalon::Date date(time);
	return date.Year() * 10000 + date.Month() * 100 + date.Day();
}

/**
*@brief ��ȡ�߼��죬��ʱ���ȥ6Сʱ�����㣬������λΪ����
*/
inline Avalon::Date GetLogicDate(UInt64 time)
{
	Avalon::Time tmpTime(time - (6 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC));
	return Avalon::Date(tmpTime);
}

//�����Ϸ��id�����ֵ���ܸı�
#define MAX_ZONE_ID		1000

//��Ŀ¼
#define ROOT_PATH		"../"
//����Ŀ¼
#define CONFIG_PATH		"../Config/"

/**
 *@brief �������������
 */
enum ServerType
{
	ST_INVALID,		//��Ч����
	ST_LOGIN,		//��¼������
	ST_ADMIN,		//���������
	ST_GATE,		//���ط�����
	ST_RECORD,		//����������
	ST_SCENE,		//����������
	ST_WORLD,		//���������
	ST_GLOBAL_ACTIVITY, //ȫ�ֻ������
	ST_CENTER,		//���ķ�����
	ST_REPLAY,		//¼�������
	ST_WSCENE,		//��ս������
	ST_TEAMCOPY,		//�ű�������
	ST_UNION,   //���������

	ST_RELAY,		//�м̷����� added by aprilliu, 2016.1.28 
	ST_UDPCONNECTOR,//udp��������� added by aprilliu. 2016.2.18
	ST_MATCHSERVER,	//ƥ�������	 added by aprilliu. 2016.2.23
	ST_DIRSERVER,	//Ŀ¼������
	ST_CROSS,	//���������			by huchenhui 2018.04.12
	ST_GLOBAL_RECORD,//ȫ�ֵ���������
	ST_VERIFY,		// ��֤������
	ST_VERIFY_CENTER,// ��֤���ķ�����
	ST_SOCIAL,		//�罻������
	ST_BATTLE,		//ս��������
	ST_BSCENE,		//ս��������

	ST_RECORD_SLAVER,//����RecordServer,��һЩsql����ת�������������ȥ����
	ST_SORTLIST,	//���а������
	ST_DUNGEON_SVR,		//���³Ƿ�����
	ST_DBPROXY,		//���ݿ���������
	ST_OBSERVE_RELAY, //��սRelay
	ST_OBSERVE,		//��ս��
	ST_TRANSFER,	//��ת������
	SERVER_TYPE_MAX,
};

enum UnionServerFunctionType
{
	USFT_NONE = 0,         //�޹������ͣ���Э�鱾��Ĺ������;���������UnionServer
	USFT_ALL = 1,          //�㲥���ͣ�������Э��ᷢ������UnionServer
	USFT_CHAMPION = 2,     //�ھ�������
	USFT_GOLD_CONSIGNMENT = 3,     //��Ҽ���
};

/**
 *@brief �ж��Ƿ���������
 */
inline bool IsAdminServer(int type)
{
	return type == ST_ADMIN;
}

/**
 *@brief �ж��Ƿ���ƽ̨������
 */
inline bool IsPlatformServer(int type)
{
	switch(type)
	{
	case ST_LOGIN:
	case ST_SOCIAL:
	case ST_CENTER:
	case ST_CROSS:
	case ST_WSCENE:
	case ST_SORTLIST:
	case ST_DBPROXY:
	case ST_RELAY:						//added by aprilliu
	case ST_MATCHSERVER:				//added by aprilliu
	case ST_REPLAY:
	case ST_GLOBAL_RECORD:
	case ST_VERIFY:
	case ST_VERIFY_CENTER:
	case ST_GLOBAL_ACTIVITY:
	case ST_BATTLE:
	case ST_BSCENE:
	case ST_TEAMCOPY:
	case ST_DUNGEON_SVR:
	case ST_UNION:
	case ST_OBSERVE:
	case ST_OBSERVE_RELAY:
	case ST_TRANSFER:
		return true;
	}
	return false;
}

/**
 *@brief���Ƿ񱾵����õ�ַ
 */
inline bool IsSelfConfigAddress(int type)
{
	if(IsPlatformServer(type)) return true;

	switch(type)
	{
	case ST_ADMIN:
	case ST_RELAY:					//added by aprilliu
	case ST_MATCHSERVER:
		return true;
	}
	return false;
}


/**
 *@brief ���ߴ�������
 */
enum DisconnectHandleType
{
	DHT_RECONNECT	= 0,	//������
	DHT_SHUTDOWN	= 1,	//ͣ��
	DHT_NOTIFY		= 2,	//֪ͨ�߼��߳�
};

/**
 *@brief ���ߴ�������
 */
struct DisconnectHandleCfg
{
	//����������
	ServerType	servType;
	//���ߴ�������
	DisconnectHandleType  handleType;
	//�����������(����)
	UInt32		retry_inter;
};

const static DisconnectHandleCfg DISCONNECT_HANDLE_CFG[] = 
{ 
	{ ST_LOGIN,		DHT_RECONNECT,	5000	},
	{ ST_ADMIN,		DHT_SHUTDOWN,	0		},
	{ ST_GATE,		DHT_NOTIFY,		0		},
	{ ST_RECORD,	DHT_SHUTDOWN,	0		},
	{ ST_RECORD_SLAVER, DHT_RECONNECT, 0 },
	{ ST_SCENE,		DHT_RECONNECT,	5000	},
	{ ST_WORLD,		DHT_SHUTDOWN,	0		},
	{ ST_SOCIAL,	DHT_RECONNECT,	5000	},
	{ ST_CENTER,	DHT_RECONNECT,	5000	},
	{ ST_CROSS,		DHT_RECONNECT, 5000		},
	{ ST_WSCENE,	DHT_RECONNECT,  5000	},
	{ ST_SORTLIST,	DHT_RECONNECT,  5000	},
	{ ST_DBPROXY,	DHT_RECONNECT,	5000	},
	{ ST_MATCHSERVER, DHT_RECONNECT, 5000	},					//added by aprilliu
	{ ST_REPLAY,	DHT_RECONNECT,	5000	},					//added by aprilliu
	{ ST_VERIFY,	DHT_RECONNECT,	5000	},		
	{ ST_VERIFY_CENTER, DHT_RECONNECT, 5000 },
	{ ST_GLOBAL_ACTIVITY, DHT_RECONNECT, 5000 },
	{ ST_BATTLE, DHT_RECONNECT, 5000 },
	{ ST_BSCENE, DHT_RECONNECT, 5000 },
	{ ST_TEAMCOPY, DHT_RECONNECT, 5000 },
	{ ST_DUNGEON_SVR, DHT_RECONNECT, 5000 },
	{ ST_UNION, DHT_RECONNECT, 5000 },
	{ ST_OBSERVE, DHT_RECONNECT, 5000 },
	{ ST_TRANSFER, DHT_RECONNECT, 5000 },
};

inline const DisconnectHandleCfg* GetDisconnectHandleCfg(ServerType type)
{
	for(size_t i = 0; i < sizeof(DISCONNECT_HANDLE_CFG) / sizeof(DISCONNECT_HANDLE_CFG[0]); ++i)
	{
		if(DISCONNECT_HANDLE_CFG[i].servType == type)
		{
			return DISCONNECT_HANDLE_CFG + i;
		}
	}
	return NULL;
}


/**
 *@brief ����������ڵ�id��ض���
 *		����������ɣ���id�����������͡�������id
 */
//��id������ڵ�id�е�ƫ��
#define CL_ZONEID_OFFSET_IN_NODEID		100000
//����������������ڵ�idz�е�ƫ��
#define CL_SERVTYPE_OFFSET_IN_NODEID	1000

//������id�����������ͺͷ�����id����ڵ�id
#define CL_MAKE_NODEID(Zone,Type,Id)\
	(Zone * CL_ZONEID_OFFSET_IN_NODEID + Type * CL_SERVTYPE_OFFSET_IN_NODEID + Id)
#define NODE_ID CL_MAKE_NODEID(ZONE_ID,SERVER_TYPE,SERVER_ID)

//�ӽڵ�id�л�ȡ��id
#define CL_GET_ZONEID_FROM_NODEID(NodeID)\
	(NodeID / CL_ZONEID_OFFSET_IN_NODEID)
//�ӽڵ�id�л�ȡ����������
#define CL_GET_SERVERTYPE_FROM_NODEID(NodeID)\
	(NodeID % CL_ZONEID_OFFSET_IN_NODEID / CL_SERVTYPE_OFFSET_IN_NODEID)
//�ӽڵ�id�л�ȡ������id
#define CL_GET_SERVERID_FROM_NODEID(NodeID)\
	(NodeID % CL_SERVTYPE_OFFSET_IN_NODEID)

//��ȡsocial�������ڵ�id
inline UInt32 GetSocialNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_SOCIAL, id);
}

//��ȡ���ķ������ڵ�id
inline UInt32 GetCenterNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_CENTER, id);
}

//��ȡ��ս�������ڵ�id
inline UInt32 GetCrossNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_CROSS, id);
}

//��ȡս���������ڵ�id
inline UInt32 GetBattleNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_BATTLE, id);
}

//��ȡ���з������ڵ�id
inline UInt32 GetSortListNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_SORTLIST, id);
}

//��ȡrecord�������ڵ�id
inline UInt32 GetRecordNodeID(UInt32 zoneId)
{
	return CL_MAKE_NODEID(zoneId, ST_RECORD, 0);
}

//��ȡȫ�ֻ�������ڵ�id
inline UInt32 GetGlobalActivityNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_GLOBAL_ACTIVITY, id);
}

//��ȡ�ű��������ڵ�id
//��ȡ�ű��������ڵ�id
inline UInt32 GetTeamCopyNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_TEAMCOPY, id);
}

//��ȡUnion�������ڵ�id
inline UInt32 GetUnionNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_UNION, id);
}

/**
 *@brief ��ȫɾ��ָ��
 */
#define CL_SAFE_DELETE(Ptr) \
	do{\
		delete Ptr;\
		Ptr = NULL;\
	}while(false)

#define CL_SAFE_DELETE_ARRAY(Ptr)\
	do{\
		delete[] Ptr;\
		Ptr = NULL;\
	}while(false)

/**
 *@brief һЩ��������
 */
#define CL_PI 3.141592653589793f
// hp,mp��ֵ
#define HP_MP_FULL_VALUE 1000

/**
 *@brief �������
 */
typedef Avalon::Dir2D CLDirection;
typedef Avalon::Pos2D CLPosition;

class ScenePos
{
public:
    ScenePos(UInt32 x_ = 0, UInt32 y_ = 0) : x(x_), y(y_) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & x & y;
    }

    bool operator!=(const ScenePos& other)
    {
        return x != other.x || y != other.y;
    }

	bool operator<(const ScenePos& other) const
	{
		if (x != other.x)
		{
			return x < other.x;
		}
		if (y != other.y)
		{
			return y < other.y;
		}

		return false;
	}

    bool operator=(const ScenePos& other)
    {
        x = other.x;
        y = other.y;
        return true;
    }

    inline UInt32 GetX() const { return x; }
    inline void SetX(UInt32 x_) { x = x_; }

    inline UInt32 GetY() const { return y; }
    inline void SetY(UInt32 y_) { y = y_; }

    inline CLPosition GetGridePos(float width, float height) const
    {
        CLPosition pos;
        pos.x = (UInt16)(x / SCALE_TO_FLOAT / width);
        pos.y = (UInt16)(y / SCALE_TO_FLOAT / height);

        return pos;
    }

    static ScenePos GetScenePosByGridPos(CLPosition pos, float width, float height)
    {
        ScenePos scenePos;
        scenePos.x = (UInt32)(pos.x * SCALE_TO_FLOAT * width);
        scenePos.y = (UInt32)(pos.y * SCALE_TO_FLOAT * height);
        return scenePos;
    }

	UInt32 GetDistance(const ScenePos& other) const
	{
		UInt32 x = 0;
		UInt32 y = 0;

		if (GetX() > other.GetX())
			x = GetX() - other.GetX();
		else
			x = other.GetX() - GetX();

		if (GetY() > other.GetY())
			y = GetY() - other.GetY();
		else
			y = other.GetY() - GetY();

		return UInt32(sqrt(UInt64(x)*UInt64(x) + UInt64(y)*UInt64(y)));
	}

	const static int SCALE_TO_FLOAT = 10000;

private:
    UInt32 x;
    UInt32 y;
};

class SceneDir
{
public:
    const static int SCALE_TO_FLOAT = 10000;

    SceneDir(Int16 x_ = 0, Int16 y_ = 0, UInt8 facing_ = 0) : x(x_), y(y_), facing(facing_) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & x & y & facing;
    }

    bool operator!=(const SceneDir& other)
    {
        return x != other.x || y != other.y || facing != other.facing;
    }

    bool operator=(const SceneDir& other)
    {
        x = other.x;
        y = other.y;
		facing = other.facing;
        return true;
    }

    bool IsValid()
    {
        if (x < -SCALE_TO_FLOAT || x > SCALE_TO_FLOAT ||
            y < -SCALE_TO_FLOAT || y > SCALE_TO_FLOAT) 
        {
            return false;
        }

        return true;
    }

    inline Int16 GetX() const { return x; }
    inline void SetX(Int16 x_) { x = x_; }

    inline Int16 GetY() const { return y; }
    inline void SetY(Int16 y_) { y = y_; }
private:
    Int16 x;
    Int16 y;
	// ����
	UInt8 facing;
};

struct CLZone
{
	CLZone():mapid(0){}
	UInt32 mapid;
	CLPosition pos1;
	CLPosition pos2;
};

/**
 *@brief ϵͳ��־  Ŀ¼λ��: ./Log/
 */
extern Avalon::Logger* gSysLogger;
using Avalon::LogStream;

class NoUseStream : public Avalon::Singleton<NoUseStream>
{
public:
	template<class T>
	NoUseStream& operator<<(const T& info) { return *this; }
};

#define LOG_FILE_INFO "[" << __FILE__ << "] [" << __FUNCTION__ << "] [" << __LINE__ << "] "

#ifndef _PUBLISH
#define DevLogStream	gSysLogger->GetStream(Avalon::LOG_DEV) << LOG_FILE_INFO
#else
#define DevLogStream	*NoUseStream::Instance() << LOG_FILE_INFO
#endif
#define ErrorLogStream	gSysLogger->GetStream(Avalon::LOG_ERROR) << LOG_FILE_INFO
#define InfoLogStream	gSysLogger->GetStream(Avalon::LOG_INFO) << LOG_FILE_INFO
#define DebugLogStream	gSysLogger->GetStream(Avalon::LOG_DEBUG) << LOG_FILE_INFO
#define WarnLogStream	gSysLogger->GetStream(Avalon::LOG_WARN) << LOG_FILE_INFO
#define TraceLogStream	gSysLogger->GetStream(Avalon::LOG_TRACE) << LOG_FILE_INFO
#define FatalLogStream	gSysLogger->GetStream(Avalon::LOG_FATAL) << LOG_FILE_INFO

#define SYSLOG_ERROR(Format, ...) \
	do{ \
		if(gSysLogger != NULL) gSysLogger->Error(Format, ##__VA_ARGS__); \
	}while(false);

#define SYSLOG_INFO(Format, ...) \
	do{ \
		if(gSysLogger != NULL) gSysLogger->Info(Format, ##__VA_ARGS__); \
	}while(false);

#define SYSLOG_DEBUG(Format, ...) \
	do{ \
		if(gSysLogger != NULL) gSysLogger->Debug(Format, ##__VA_ARGS__); \
	}while(false);

#define SYSLOG_TRACE(Format, ...) \
	do{ \
		if(gSysLogger != NULL) gSysLogger->Trace(Format, ##__VA_ARGS__); \
	}while(false);

#define SYSLOG_FATAL(Format, ...) \
	do{ \
		if(gSysLogger != NULL) gSysLogger->Fatal(Format, ##__VA_ARGS__); \
	}while(false);

/**
 *@brief ��Ϸ��־  noted by aprilliu, ��Ϸ����ϵͳ����־�� Ŀ¼�� ./GameLog/
 */
#define GameInfoLogStream(Name) Avalon::Logger::getInstance(Name)->GetStream(Avalon::LOG_INFO)
#define GameErrorLogStream(Name) Avalon::Logger::getInstance(Name)->GetStream(Avalon::LOG_ERROR)

/**
 *@brief ������־
 */
#define DataLogStream(Name) Avalon::Logger::getInstance("datalog_"##Name)

/**
 *@brief qqƽ̨��Ȩ��ʶλ                                                                     
 */
enum QQVipFlag
{
	QQVIP_ISVIP =		1 << 0,	//�Ƿ������
	QQVIP_YEARVIP =		1 << 1,	//�Ƿ���ѻ�����
	QQVIP_SUPERBLUE =	1 << 2,	//�Ƿ񳬼�����
	QQVIP_ISBLUE =		1 << 3,	//�Ƿ�����
	QQVIP_SUPERYELLOW = 1 << 4,	//�Ƿ񳬼�����
};
/**
 *@brief qqƽ̨��Ȩ��Ϣ                                                                     
 */
struct QQVipInfo
{
	QQVipInfo():flag(0), level(0), lv3366(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & flag & level & lv3366;
	}

	void Encode(Avalon::NetOutputStream& stream,UInt32 type,bool bDirty){ stream & flag & level & lv3366; }
	void Decode(Avalon::NetInputStream& stream){ stream & flag & level & lv3366; }

	bool IsDirty(UInt32 flag) const { return false; }
	void ClearDirty(UInt32 flag) { }
	bool IsDefaultValue() const{ return false; }

	void SetIsVip() { flag |= QQVIP_ISVIP; }
	bool IsVip() const{ return (flag & QQVIP_ISVIP) != 0; }

	void SetYearVip() { flag |= QQVIP_YEARVIP; }
	bool IsYearVip() const{ return (flag & QQVIP_YEARVIP) != 0; }

	void SetSuperBlue(){ flag |= QQVIP_SUPERBLUE; }
	bool IsSuperBlue() const{ return (flag & QQVIP_SUPERBLUE) != 0; }

	void SetIsBlue(){flag |= QQVIP_ISBLUE;}
	bool IsBlue() const{ return (flag & QQVIP_ISBLUE) != 0; }

	void SetSuperYellow(){ flag |= QQVIP_SUPERYELLOW; }
	bool IsSuperYellow() const{ return (flag & QQVIP_SUPERYELLOW) != 0; }

	//��ʶλ
	UInt8	flag;
	//������ȼ�
	UInt8	level;
	//3366�ȼ�
	UInt8	lv3366;
};

//used by UdpConnServer and Relay Server
enum enmChannelParam
{
	MAX_CS_CHANNEL_SIZE = 0x2000000,		//32MB
	max_relaysvr_cs_packet_size = 8192,		//relay client��service ��csЭ����������
	max_relaysvr_sc_packet_size = 64000,	//64k  service��client����������
	cs_message_id_offset = 2,				//Byte Э��id��ƫ�ƣ��ڱ�����buffer�У�
	cs_message_min_size = 6,				//csЭ�����С����(2Byte�����ֶ� + 4Byte Э��ID)
};

/**
*@brief ��ұ�ǩ��Ϣ
*/
struct PlayerLabelInfo
{
	PlayerLabelInfo() : awakenStatus(0), returnStatus(0), noviceGuideChooseFlag(0), headFrame(0), guildId(0), returnYearTitle(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & awakenStatus & returnStatus & noviceGuideChooseFlag & headFrame & guildId & returnYearTitle;
	}

	// ����״̬
	UInt8 awakenStatus;
	// �ع�״̬
	UInt8 returnStatus;
	// ��������ѡ���־
	UInt8 noviceGuideChooseFlag;
	// ͷ���
	UInt32 headFrame;
	// ����ID
	UInt64 guildId;
	// �ع�����ƺ�
	UInt32 returnYearTitle;
};

struct PlayerIcon
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu & level & playerLabelInfo;
	}

	// ID
	ObjID_t		id;
	// ����
	std::string	name;
	// ְҵ
	UInt8		occu;
	// �ȼ�
	UInt16		level;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

enum SysSwitchType
{
	SST_NONE = 0,
};

//ϵͳ��������
struct SysSwitchCfg
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sysType & switchValue;
	}

	UInt32	sysType;			//SysSwitchType
	UInt8	switchValue;		//0-��, 1-��
};

#define  MAX_INT32		0x7FFFFFFF

enum UnionEntryType
{
	// ��Ч
	UNION_ENTRY_INVALID,
	// �̶���ֵ
	UNION_ENTRY_FIX,
	// ��ȼ��Զ����㣨��ֵ��
	UNION_ENTRY_GROW,
	// ���ȼ���ֵ
	UNION_ENTRY_ARRAY,
};

struct UnionEntry
{
	UnionEntry()
	{
		type = UNION_ENTRY_INVALID;
		fixValue = 0;
		growValue.a = 0;
		growValue.k = 0;
	}

	UnionEntryType	type;
	UInt32		fixValue;
	struct {
		// ��Ӧ��ֵ���� a + k * (level - 1)
		UInt32	a;
		UInt32	k;
	}			growValue;
	std::vector<UInt32> arrayValue;

	UInt32 GetValue(UInt8 level = 1)
	{
		if (type == UNION_ENTRY_FIX)
		{
			return fixValue;
		}
		else if (type == UNION_ENTRY_GROW)
		{
			return growValue.a + (level - 1) * growValue.k;
		}
		else if (type == UNION_ENTRY_ARRAY)
		{
			if (level >= arrayValue.size())
			{
				return 0;
			}
			return arrayValue[level - 1];
		}

		return 0;
	}

	bool DecodeFromString(std::string str)
	{
		// ��ʽ˵����
		// �̶�ģʽ��ֻ������
		// �ɳ�ģʽ��a;k
		// ����ģʽ��a,b,c,d,e...

		if (str.find(';') != std::string::npos)
		{
			type = UNION_ENTRY_GROW;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ";");
			if (params.size() != 2)
			{
				return false;
			}
			growValue.a = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
			growValue.k = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
		}
		else if (str.find(',') != std::string::npos)
		{
			type = UNION_ENTRY_ARRAY;
			std::vector<std::string> params;
			Avalon::StringUtil::Split(str, params, ",");
			for (auto& param : params)
			{
				arrayValue.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(param));
			}
		}
		else
		{
			type = UNION_ENTRY_FIX;
			fixValue = Avalon::StringUtil::ConvertToValue<UInt32>(str);
		}

		return true;
	}
};

struct VeteranReturnInfo
{
	VeteranReturnInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleReturnTime & accountReturnTime;
	}

	//��ɫ�ع�ʱ��
	UInt32	roleReturnTime;
	//�˺Żع�ʱ��
	UInt32	accountReturnTime;
};

/**
*@brief ð�ն�(Ӷ����)��Ϣ
*/
struct AdventureTeamInfo
{
	AdventureTeamInfo() :adventureTeamLevel(0), adventureTeamExp(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & adventureTeamName & adventureTeamLevel & adventureTeamExp;
	}

	// ð�նӶ���
	std::string	adventureTeamName;
	// ð�նӵȼ�
	UInt16	adventureTeamLevel;
	// ð�նӾ���
	UInt64	adventureTeamExp;
};

/**
*@brief �˺ż�������
*/
enum AccountCounterType
{
	ACC_COUNTER_INVALID = 0,
	// �͸�ˮ��(Ӷ��ѫ��)
	ACC_COUNTER_BLESS_CRYSTAL,
	// �͸�����(Ӷ��ѫ�¾���)
	ACC_COUNTER_BLESS_CRYSTAL_EXP,
	// ����ף��(�ɳ�ҩ��)
	ACC_COUNTER_INHERIT_BLESS,
	// ���о���(�ɳ�ҩ������)
	ACC_COUNTER_INHERIT_BLESS_EXP,
	// ��������
	ACC_COUNTER_ENERGY_COIN,
	// �ͽ�
	ACC_COUNTER_BOUNTY,
	// ����������ȡ����
	ACC_GUILD_REDPACKET_DAILY_MAX,
	// ð��ͨ��֤�ʼ�����
	ACC_ADVENTURE_PASS_MAIL_SEND,
	// �·�������۱��
	ACC_NEW_SERVER_GIFT_DISCOUNT,
	// ��ļӲ��
	ACC_COUNTER_HIRE_COIN,
	// ��ļ����
	ACC_COUNTER_HIRE_PUSH,

	/************* �˺�ˢ����� ************/

	// �˺��߼���ˢ��
	ACC_COUNTER_LOGIC_WEEK_REFRESH = 100,

	ACC_COUNTER_TYPE_MAX,
};

enum AccountDataType
{
	OFFLINE_TIME = 1,	// ����ʱ��
	MAX_ACC_ROLE_LEVEL = 2, // ����ɫ�ȼ�
};

/**
*@brief ��������ѡ���־
*/
enum NoviceGuideChooseFlag
{
	// ��̬
	NGCF_INIT = 0,
	// ����ѡ��
	NGCF_POPUP = 1,
	// ѡ����������
	NGCF_PASS = 2,
	// ѡ������
	NGCF_NOT_PASS = 3,

	NGCF_MAX,
};

/**
*@brief ��Ϣת����ͷ
*/
struct TransferHeader
{
	TransferHeader() : destZone(0), destServer(0), destObj(0), srcZone(ZONE_ID), srcServer(SERVER_TYPE), srcObj(0) {}
	TransferHeader(UInt32 _dz, UInt32 _ds, UInt64 _do = 0, UInt64 _so = 0) : destZone(_dz), destServer(_ds), destObj(_do), 
		srcZone(ZONE_ID), srcServer(SERVER_TYPE), srcObj(_so) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & destZone & destServer & destObj & srcZone & srcServer & srcObj;
	}

	// Ŀ������
	UInt32 destZone;
	// Ŀ�ķ���
	UInt32 destServer;
	// Ŀ�����
	UInt64 destObj;
	// ԭ����
	UInt32 srcZone;
	// ԭ����
	UInt32 srcServer;
	// ԭ���
	UInt64 srcObj;

	std::string LogStr() 
	{
		std::stringstream ss;
		ss << " (dz:" << destZone;
		ss << ", ds:" << destServer;
		ss << ", do:" << destObj;
		ss << ", sz:" << srcZone;
		ss << ", ss:" << srcServer;
		ss << ", so:" << srcObj;
		ss << ")";
		return ss.str();
	}

	void SwapHeader()
	{
		std::swap(destZone, srcZone);
		std::swap(destServer, srcServer);
		std::swap(srcObj, destObj);
	}
};

/**
*@brief �Լ����״̬
*/
enum BattlePlayerState
{
	//���
	BPS_ALIVE = 0,
	//ƥ��
	BPS_MATCH = 1,
	//���
	BPS_PK = 2,
	//����
	BPS_OUT = 3,
	//��ս
	BPS_NO_WAR = 4,
};

/**
 *@brief ƫ������ 
 */
enum RolePreferences {
	RP_SELECT = 0,
	RP_CANCLE =1
};
#endif
