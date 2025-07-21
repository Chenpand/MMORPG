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
 *@brief 引用avalon基本类型定义
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
 *@brief 游戏类型定义
 */
typedef UInt64 ObjID_t;
#define ID_FORMAT AVALON_UINT64_FORMAT


//游戏区id
extern UInt32 ZONE_ID;
//中立区id
extern UInt32 NEUTRAL_ZONE_ID;
//服务器类型
extern UInt32 SERVER_TYPE;
//服务器id
extern UInt32 SERVER_ID;
//是否有脚本错误
extern UInt32 SCRIPT_ERROR;
//当前时间
extern Avalon::Time CURRENT_TIME;
//当前日期
extern Avalon::Date CURRENT_DATE;
//开服时间
extern UInt32	GAMESTART_TIME;
//世界等级
extern UInt32	WORLD_LEVEL;
//服务器版本号
extern std::string GAME_VERSION;
//平台
extern std::string PLATFORM;
//第一次开服时间
extern UInt32 GAME_FIRST_START_TIME;
//服务器名
extern std::string SERVER_NAME;
//合入的服务器(id串)
extern std::string MERGED_SERVERS;
//一次Tick最多处理的消息数
extern UInt32 MAX_PACKET_PERTICK;

//天转秒
#define DAY_TO_SEC	86400
//小时转秒
#define HOUR_TO_SEC	3600
//分钟转秒
#define MIN_TO_SEC	60
//second to micro second
#define SEC_TO_MSEC 1000
//天转毫秒
#define DAY_TO_MSEC 86400000

//每帧16ms
#define TIME_OF_FRAME_MSEC	16
#define PHYSICAL_FRAME_MSEC	64		//added by aprilliu,2016.06.01 物理帧频

// 重新加载配置
#define  SIG_RELOAD_CONFIG 10

#define U_INT16_MAX 65535


/**
 *@brief 判断是否同一天
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
 *@brief 获取今天开始的时间戳                                                                     
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
*@brief 获取今天开始的时间戳
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
*@brief 比较两个时间戳是否在同一逻辑天
*/
inline bool CheckInSameLogicDay(const UInt32& time1, const UInt32& time2)
{
	Avalon::Time t1((UInt64)time1 * Avalon::Time::MSECS_OF_SEC);
	Avalon::Time t2((UInt64)time2 * Avalon::Time::MSECS_OF_SEC);

	return GetDayBeginTime2(t1) == GetDayBeginTime2(t2);
}

/**
 *@brief 获取今天是开服第几天                                                                     
 */
inline UInt32 GetGameStartDays()
{
	UInt32 gamestartDay = UInt32(GetDayBeginTime(Avalon::Time(UInt64(GAMESTART_TIME) * 1000)) / 1000);
	UInt32 today = UInt32(GetDayBeginTime(CURRENT_TIME) / 1000);
	if(today <= gamestartDay) return 1;

	return (today - gamestartDay) / DAY_TO_SEC + 1;
}

/*
*	获得给定时间所在周的开始时间
* \param	time		时间
* \param	beginWDay	每周的启示时间从周几开始 0:周日 1:周一 .... 6:周六
* \return	UInt64		返回周开始的时间
* 以周日为起点,返回值是 周日的 0点0分0秒
* 以周一为起点,返回值是 周一的 0点0分0秒
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
*	获得给定时间所在周的开始时间
* \param	time		时间戳(毫秒)
* \param	beginWDay	每周的启示时间从周几开始 0:周日 1:周一 .... 6:周六
* \return	UInt64		返回周开始的时间
* 以周日为起点,返回值是 周日的 0点0分0秒
* 以周一为起点,返回值是 周一的 0点0分0秒
*/
inline UInt64 getWeekBeginTime(UInt64 time, int beginWDay = 1, int beginDayHour = 0)
{
	Avalon::Time curTime(time);
	return getWeekBeginTime(curTime, beginWDay, beginDayHour);
}

/**
*@brief 是否在同一周
*/
inline bool isSameWeek(const Avalon::Time& time1, const Avalon::Time& time2, int beginWDay = 1, int beginDayHour = 0)
{
	return getWeekBeginTime(time1, beginWDay) == getWeekBeginTime(time2, beginWDay);
}

/**
*@brief 是否在同一周
*/
inline bool isSameWeek(UInt64 time1, UInt64 time2, int beginWDay = 1, int beginDayHour = 0)
{
	return getWeekBeginTime(time1, beginWDay, beginDayHour) == getWeekBeginTime(time2, beginWDay, beginDayHour);
}

/**
*@brief 是否在同一逻辑周，周一六点为分割线
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
*	获得给定时间所在月的开始时间
* \param	time		时间
* \return	UInt64		返回月开始的时间
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
*	获得给定时间所在月的开始时间
* \param	time		时间
* \return	UInt64		返回月开始的时间
*/
inline UInt64 getMonthBeginTime(UInt64 time)
{
	Avalon::Time curTime(time);
	return getMonthBeginTime(curTime);
}

/**
*@brief 是否在同一月
*/
inline bool isSameMonth(const Avalon::Time& time1, const Avalon::Time& time2)
{
	return getMonthBeginTime(time1) == getMonthBeginTime(time2);
}

/**
*@brief 是否在同一月
*/
inline bool isSameMonth(UInt64 time1, UInt64 time2)
{
	return getMonthBeginTime(time1) == getMonthBeginTime(time2);
}

/**
*@brief 获取年月日yyyymmdd
*/
inline UInt32 GetYearMonthDay(const Avalon::Time& time)
{
	Avalon::Date date(time);
	return date.Year() * 10000 + date.Month() * 100 + date.Day();
}

/**
*@brief 获取逻辑天，用时间减去6小时来计算，参数单位为毫秒
*/
inline Avalon::Date GetLogicDate(UInt64 time)
{
	Avalon::Time tmpTime(time - (6 * Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC));
	return Avalon::Date(tmpTime);
}

//最大游戏区id，这个值不能改变
#define MAX_ZONE_ID		1000

//根目录
#define ROOT_PATH		"../"
//配置目录
#define CONFIG_PATH		"../Config/"

/**
 *@brief 定义服务器类型
 */
enum ServerType
{
	ST_INVALID,		//无效类型
	ST_LOGIN,		//登录服务器
	ST_ADMIN,		//管理服务器
	ST_GATE,		//网关服务器
	ST_RECORD,		//档案服务器
	ST_SCENE,		//场景服务器
	ST_WORLD,		//世界服务器
	ST_GLOBAL_ACTIVITY, //全局活动服务器
	ST_CENTER,		//中心服务器
	ST_REPLAY,		//录像服务器
	ST_WSCENE,		//挑战场景服
	ST_TEAMCOPY,		//团本服务器
	ST_UNION,   //跨服服务器

	ST_RELAY,		//中继服务器 added by aprilliu, 2016.1.28 
	ST_UDPCONNECTOR,//udp接入服务器 added by aprilliu. 2016.2.18
	ST_MATCHSERVER,	//匹配服务器	 added by aprilliu. 2016.2.23
	ST_DIRSERVER,	//目录服务器
	ST_CROSS,	//跨服服务器			by huchenhui 2018.04.12
	ST_GLOBAL_RECORD,//全局档案服务器
	ST_VERIFY,		// 验证服务器
	ST_VERIFY_CENTER,// 验证中心服务器
	ST_SOCIAL,		//社交服务器
	ST_BATTLE,		//战场服务器
	ST_BSCENE,		//战场场景服

	ST_RECORD_SLAVER,//备用RecordServer,有一些sql可以转发到这个服务器去处理
	ST_SORTLIST,	//排行榜服务器
	ST_DUNGEON_SVR,		//地下城服务器
	ST_DBPROXY,		//数据库代理服务器
	ST_OBSERVE_RELAY, //观战Relay
	ST_OBSERVE,		//观战服
	ST_TRANSFER,	//中转服务器
	SERVER_TYPE_MAX,
};

enum UnionServerFunctionType
{
	USFT_NONE = 0,         //无功能类型，由协议本身的功能类型决定发往的UnionServer
	USFT_ALL = 1,          //广播类型，该类型协议会发往所有UnionServer
	USFT_CHAMPION = 2,     //冠军赛类型
	USFT_GOLD_CONSIGNMENT = 3,     //金币寄售
};

/**
 *@brief 判断是否管理服务器
 */
inline bool IsAdminServer(int type)
{
	return type == ST_ADMIN;
}

/**
 *@brief 判断是否是平台服务器
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
 *@brief　是否本地配置地址
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
 *@brief 断线处理类型
 */
enum DisconnectHandleType
{
	DHT_RECONNECT	= 0,	//重连接
	DHT_SHUTDOWN	= 1,	//停服
	DHT_NOTIFY		= 2,	//通知逻辑线程
};

/**
 *@brief 断线处理配置
 */
struct DisconnectHandleCfg
{
	//服务器类型
	ServerType	servType;
	//断线处理类型
	DisconnectHandleType  handleType;
	//断线重连间隔(毫秒)
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
 *@brief 服务器网络节点id相关定义
 *		由三部分组成，区id、服务器类型、服务器id
 */
//区id在网络节点id中的偏移
#define CL_ZONEID_OFFSET_IN_NODEID		100000
//服务器类型在网络节点idz中的偏移
#define CL_SERVTYPE_OFFSET_IN_NODEID	1000

//根据区id、服务器类型和服务器id构造节点id
#define CL_MAKE_NODEID(Zone,Type,Id)\
	(Zone * CL_ZONEID_OFFSET_IN_NODEID + Type * CL_SERVTYPE_OFFSET_IN_NODEID + Id)
#define NODE_ID CL_MAKE_NODEID(ZONE_ID,SERVER_TYPE,SERVER_ID)

//从节点id中获取区id
#define CL_GET_ZONEID_FROM_NODEID(NodeID)\
	(NodeID / CL_ZONEID_OFFSET_IN_NODEID)
//从节点id中获取服务器类型
#define CL_GET_SERVERTYPE_FROM_NODEID(NodeID)\
	(NodeID % CL_ZONEID_OFFSET_IN_NODEID / CL_SERVTYPE_OFFSET_IN_NODEID)
//从节点id中获取服务器id
#define CL_GET_SERVERID_FROM_NODEID(NodeID)\
	(NodeID % CL_SERVTYPE_OFFSET_IN_NODEID)

//获取social服务器节点id
inline UInt32 GetSocialNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_SOCIAL, id);
}

//获取中心服务器节点id
inline UInt32 GetCenterNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_CENTER, id);
}

//获取挑战服务器节点id
inline UInt32 GetCrossNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_CROSS, id);
}

//获取战场服务器节点id
inline UInt32 GetBattleNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_BATTLE, id);
}

//获取排行服务器节点id
inline UInt32 GetSortListNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_SORTLIST, id);
}

//获取record服务器节点id
inline UInt32 GetRecordNodeID(UInt32 zoneId)
{
	return CL_MAKE_NODEID(zoneId, ST_RECORD, 0);
}

//获取全局活动服务器节点id
inline UInt32 GetGlobalActivityNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_GLOBAL_ACTIVITY, id);
}

//获取团本服务器节点id
//获取团本服务器节点id
inline UInt32 GetTeamCopyNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_TEAMCOPY, id);
}

//获取Union服务器节点id
inline UInt32 GetUnionNodeID(UInt32 id)
{
	return CL_MAKE_NODEID(0, ST_UNION, id);
}

/**
 *@brief 安全删除指针
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
 *@brief 一些常量定义
 */
#define CL_PI 3.141592653589793f
// hp,mp满值
#define HP_MP_FULL_VALUE 1000

/**
 *@brief 场景相关
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
	// 朝向
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
 *@brief 系统日志  目录位于: ./Log/
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
 *@brief 游戏日志  noted by aprilliu, 游戏具体系统的日志， 目录： ./GameLog/
 */
#define GameInfoLogStream(Name) Avalon::Logger::getInstance(Name)->GetStream(Avalon::LOG_INFO)
#define GameErrorLogStream(Name) Avalon::Logger::getInstance(Name)->GetStream(Avalon::LOG_ERROR)

/**
 *@brief 数据日志
 */
#define DataLogStream(Name) Avalon::Logger::getInstance("datalog_"##Name)

/**
 *@brief qq平台特权标识位                                                                     
 */
enum QQVipFlag
{
	QQVIP_ISVIP =		1 << 0,	//是否黄蓝钻
	QQVIP_YEARVIP =		1 << 1,	//是否年费黄蓝钻
	QQVIP_SUPERBLUE =	1 << 2,	//是否超级蓝钻
	QQVIP_ISBLUE =		1 << 3,	//是否蓝钻
	QQVIP_SUPERYELLOW = 1 << 4,	//是否超级黄钻
};
/**
 *@brief qq平台特权信息                                                                     
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

	//标识位
	UInt8	flag;
	//黄蓝钻等级
	UInt8	level;
	//3366等级
	UInt8	lv3366;
};

//used by UdpConnServer and Relay Server
enum enmChannelParam
{
	MAX_CS_CHANNEL_SIZE = 0x2000000,		//32MB
	max_relaysvr_cs_packet_size = 8192,		//relay client到service 的cs协议最大包长度
	max_relaysvr_sc_packet_size = 64000,	//64k  service到client的最大包长度
	cs_message_id_offset = 2,				//Byte 协议id的偏移（在编码后的buffer中）
	cs_message_min_size = 6,				//cs协议的最小长度(2Byte包长字段 + 4Byte 协议ID)
};

/**
*@brief 玩家标签信息
*/
struct PlayerLabelInfo
{
	PlayerLabelInfo() : awakenStatus(0), returnStatus(0), noviceGuideChooseFlag(0), headFrame(0), guildId(0), returnYearTitle(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & awakenStatus & returnStatus & noviceGuideChooseFlag & headFrame & guildId & returnYearTitle;
	}

	// 觉醒状态
	UInt8 awakenStatus;
	// 回归状态
	UInt8 returnStatus;
	// 新手引导选择标志
	UInt8 noviceGuideChooseFlag;
	// 头像框
	UInt32 headFrame;
	// 公会ID
	UInt64 guildId;
	// 回归周年称号
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
	// 名字
	std::string	name;
	// 职业
	UInt8		occu;
	// 等级
	UInt16		level;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

enum SysSwitchType
{
	SST_NONE = 0,
};

//系统开关配置
struct SysSwitchCfg
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & sysType & switchValue;
	}

	UInt32	sysType;			//SysSwitchType
	UInt8	switchValue;		//0-关, 1-开
};

#define  MAX_INT32		0x7FFFFFFF

enum UnionEntryType
{
	// 无效
	UNION_ENTRY_INVALID,
	// 固定数值
	UNION_ENTRY_FIX,
	// 随等级自动计算（差值）
	UNION_ENTRY_GROW,
	// 各等级数值
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
		// 对应的值就是 a + k * (level - 1)
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
		// 格式说明：
		// 固定模式：只有数字
		// 成长模式：a;k
		// 数组模式：a,b,c,d,e...

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

	//角色回归时间
	UInt32	roleReturnTime;
	//账号回归时间
	UInt32	accountReturnTime;
};

/**
*@brief 冒险队(佣兵团)信息
*/
struct AdventureTeamInfo
{
	AdventureTeamInfo() :adventureTeamLevel(0), adventureTeamExp(0) {}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & adventureTeamName & adventureTeamLevel & adventureTeamExp;
	}

	// 冒险队队名
	std::string	adventureTeamName;
	// 冒险队等级
	UInt16	adventureTeamLevel;
	// 冒险队经验
	UInt64	adventureTeamExp;
};

/**
*@brief 账号计数类型
*/
enum AccountCounterType
{
	ACC_COUNTER_INVALID = 0,
	// 赐福水晶(佣兵勋章)
	ACC_COUNTER_BLESS_CRYSTAL,
	// 赐福经验(佣兵勋章经验)
	ACC_COUNTER_BLESS_CRYSTAL_EXP,
	// 传承祝福(成长药剂)
	ACC_COUNTER_INHERIT_BLESS,
	// 传承经验(成长药剂经验)
	ACC_COUNTER_INHERIT_BLESS_EXP,
	// 精力货币
	ACC_COUNTER_ENERGY_COIN,
	// 赏金
	ACC_COUNTER_BOUNTY,
	// 公会红包日领取上限
	ACC_GUILD_REDPACKET_DAILY_MAX,
	// 冒险通信证邮件发送
	ACC_ADVENTURE_PASS_MAIL_SEND,
	// 新服礼包打折标记
	ACC_NEW_SERVER_GIFT_DISCOUNT,
	// 招募硬币
	ACC_COUNTER_HIRE_COIN,
	// 招募推送
	ACC_COUNTER_HIRE_PUSH,

	/************* 账号刷新相关 ************/

	// 账号逻辑周刷新
	ACC_COUNTER_LOGIC_WEEK_REFRESH = 100,

	ACC_COUNTER_TYPE_MAX,
};

enum AccountDataType
{
	OFFLINE_TIME = 1,	// 离线时间
	MAX_ACC_ROLE_LEVEL = 2, // 最大角色等级
};

/**
*@brief 新手引导选择标志
*/
enum NoviceGuideChooseFlag
{
	// 初态
	NGCF_INIT = 0,
	// 弹出选择
	NGCF_POPUP = 1,
	// 选择跳过引导
	NGCF_PASS = 2,
	// 选择不跳过
	NGCF_NOT_PASS = 3,

	NGCF_MAX,
};

/**
*@brief 消息转发包头
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

	// 目的区服
	UInt32 destZone;
	// 目的服务
	UInt32 destServer;
	// 目的玩家
	UInt64 destObj;
	// 原区服
	UInt32 srcZone;
	// 原服务
	UInt32 srcServer;
	// 原玩家
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
*@brief 吃鸡玩家状态
*/
enum BattlePlayerState
{
	//存活
	BPS_ALIVE = 0,
	//匹配
	BPS_MATCH = 1,
	//打架
	BPS_PK = 2,
	//出局
	BPS_OUT = 3,
	//免战
	BPS_NO_WAR = 4,
};

/**
 *@brief 偏好设置 
 */
enum RolePreferences {
	RP_SELECT = 0,
	RP_CANCLE =1
};
#endif
