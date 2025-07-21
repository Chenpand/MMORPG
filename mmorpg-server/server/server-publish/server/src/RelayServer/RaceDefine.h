#ifndef __RACE_DEFINE_H__
#define __RACE_DEFINE_H__

#include <CLDefine.h>
#include <AvalonSimpleTimer.h>
#include <functional>  

typedef enum _gamesessionEventId
{
	gamesessionEventId_dismiss = 1,				//对战结束
	gamesessionEventId_fighter_offline = 2,				//参战者离线
	gamesessionEventId_start = 3,				//对战开始
	gamesessionEventId_fighter_login = 4,				//参战者进入
	gamesessionEventId_fighter_want_endgame = 5,				//参战者要求结束战斗（上报了自己的对战结果）
	gamesessionEventId_fighter_input = 6,				//上报游戏输入
	gamesessionEventId_fighter_framechecksum = 7,				//上报帧校验数据

}EGameSessionEventID;

typedef struct _gamesessionEvent
{
	UInt8		id;

	//other fields

}GameSessionEvent;

enum
{
    // 最多参战人数 0~4 是一方，5-9 是另外一方
    MAX_GAMER_COUNT                                 = 10,

    //帧校验的间隔（单位：帧）
    FRAME_CHECKSUM_INTERVAL                         = 50,

	//地下城重连等待时间
	DUNGEON_RACE_RECONN_WAIT_TIME					= 5 * 60,

	//PVP重连等待时间
	PVP_RACE_RECONN_WAIT_TIME						= 90,	
};

// 比赛对象类型
enum RaceInstanceType
{
    RACE_PVP,
    RACE_DUNGEON,
	RACE_PK_ROOM,
    RACE_NUM,
};

enum RaceStatus
{
    // 初始状态
    RACE_STATUS_INIT,
    // 等待所有玩家登陆
    RACE_STATUS_WAIT_LOGIN,
    // 等待所有玩家加载
    RACE_STATUS_WAIT_LOAD,
    // 比赛中
    RACE_STATUS_RACING,
    // 等待结算消息
    RACE_STATUS_WAIT_RACE_END,
	// 结算
	RACE_STATUS_RACE_END,
	// 等待玩家重连（所有玩家离线后会到这个状态）
	RACE_STATUS_WAIT_RECONNECT,
	// 等待被摧毁
	RACE_STATUS_DESTORY,
    // 状态数量
    RACE_STATUS_NUM,
};

// 玩家离开比赛原因
enum GamerLeaveReason
{
    // 断线
    GAMER_LEAVE_DISCONNECT,
    // 登录超时
    GAMER_LEAVE_LOGIN_TIMEOUT,
    // 主动离开
    GAMER_LEAVE_INITIATIVE,
    // 不同步
    GAMER_LEAVE_UNSYNC,
	// 加载超时
	GAMER_LEAVE_LOAD_TIMEOUT,
	// 长时间没收到随机数
	GAMER_LEAVE_NOT_RECV_CHECKSUM,
	// 心跳超时
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
    // 是否激活
    bool                    active;

    // 等待时间（ms）
    UInt32                  waitTime;

    // 超时处理函数
    typedef std::function<void()> RaceStatusTimeOutFunc;
    RaceStatusTimeOutFunc   timeOutFunc;

    // 定时器
    Avalon::SimpleTimer     timer;
};

struct RaceChecksumInfo
{
	RaceChecksumInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// 帧数
	UInt32 frame;
	// 所有玩家的校验（按座位号）
	UInt32 checksums[MAX_GAMER_COUNT];
};

#endif