#ifndef _CL_ROUTINE_PROTOCOL_H_
#define _CL_ROUTINE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	 *箱子领取标志 1-4为活跃度箱子 11为签到箱子 12为vip签到箱子 14为黄钻推荐位礼包
	 */

	/**
	 *@brief client->server 请求活跃度
	 */
	class SceneActiveDegreeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTIVE_DEGREE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief server->client 返回活跃度
	 */
	class SceneActiveDegreeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTIVE_DEGREE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeEvents & boxGotFlag & activeDegree;
		}

		//活跃度项目完成次数
		std::map<UInt8,UInt8>	activeEvents;
		//箱子领取标志 1-4为活跃度箱子
		UInt32	boxGotFlag;
		//活跃度
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client 通知活跃项完成
	 */
	class SceneNotifyActiveEventFinished : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_ACTIVEEVENT_FINISHED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & times & activeDegree;
		}

		//事件编号
		UInt8	id;
		//完成次数
		UInt8	times;
		//当前活跃度
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client 上线同步日常基本信息
	 */
	class SceneRoutineSyncBaseInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_SYNC_BASEINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bGotQQVipGiftbag & onlineTime;
		}

		//是否已领取黄钻推荐礼包
		UInt8	bGotQQVipGiftbag;
		//在线时间 秒
		UInt32	onlineTime;
	};

	/**
	 *@brief client->server 请求领取箱子
	 */
	class SceneRoutineGetBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_GETBOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & index;
		}

		//箱子  1-4为活跃度箱子  11为签到奖励 12为vip签到礼包 14为黄钻推荐位礼包
		UInt8	index;
	};

	/**
	 *@brief server->client 领取箱子返回
	 */
	class SceneRoutineGetBoxRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_GETBOX_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & index & boxGotFlag & activeDegree;
		}

		//箱子索引
		UInt8	index;
		//箱子领取标志
		UInt32	boxGotFlag;
		//当前活跃度
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client 通知重置活跃度
	 */
	class SceneNotifyResetActiveDegree : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_RESET_ACTIVEDEGREE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & signinDays & signinBoxId & vipSigninBoxId;
		}

		//签到天数
		UInt8	signinDays;
		//签到箱子id
		UInt32	signinBoxId;
		//vip签到箱子id
		UInt32	vipSigninBoxId;
	};

	/**
	 *@brief client->scene 请求今日登陆奖励
	 */
	class SceneRoutineTodaySigninAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_TODAY_SIGNINAWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 返回今日登陆奖励
	 */
	class SceneRoutineTodaySigninAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_TODAY_SIGNINAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & signinDays & signinBoxId & vipSigninBoxId & boxGotFlag;
		}

		//签到天数
		UInt8	signinDays;
		//签到箱子奖励
		UInt32	signinBoxId;
		//vip签到箱子奖励
		UInt32	vipSigninBoxId;
		//箱子领取标志
		UInt32	boxGotFlag;
	};

	/**
	 *@brief world->client 通知客户端活动开始或结束
	 */
	class WorldNotifyClientActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_CLIENT_ACTIVITY)
	public:
		WorldNotifyClientActivity() :type(0), id(0), level(0), preTime(0), startTime(0), dueTime(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & name & level  & preTime & startTime & dueTime;
		}

		//1为开始 0为结束 2为准备
		UInt8		type;
		//活动id
		UInt32		id;
		//活动名
		std::string	name;
		//等级
		UInt16		level;
		//准备时间
		UInt32		preTime;
		//开始时间
		UInt32		startTime;
		//截止时间
		UInt32		dueTime;
	};

	/**
	 *@brief world->scene 通知场景活动开始或结束
	 */
	class WorldNotifySceneActivity : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_SCENE_ACTIVITY)
	public:
		WorldNotifySceneActivity() :type(0), id(0), level(0), preTime(0), startTime(0), dueTime(0), tribeId(0), belongRole(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & name & level & preTime & startTime & dueTime & tribeId & belongRole;
		}

		//1为开始 0为结束
		UInt8		type;
		//活动id
		UInt32		id;
		//活动名
		std::string name;
		//等级
		UInt16		level;
		//准备时间
		UInt32		preTime;
		//开始时间
		UInt32		startTime;
		//截止时间
		UInt32		dueTime;
		//活动发起部落，如果不为0表示部落活动
		ObjID_t		tribeId;
		//归属玩家
		ObjID_t		belongRole;
	};

	/**
	 *@brief world->client 同步活动列表到客户端
	 */
	class WorldSyncClientActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_CLIENT_ACTIVITIES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities;
		}

		//活动列表
		std::vector<ActivityInfo>		activities;
	};


	// 请求活动详情
	class WorldActivityDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_DETAIL_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeName;
		}

		//活动id
		std::string	activeName;
	};

	// 请求活动详情返回
	class WorldActivityDetailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_DETAIL_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & id & name & level & preTime & startTime & dueTime;
		}
		//错误码
		UInt32      code;
		//活动id
		UInt32		id;
		//活动名字
		std::string name;
		//等级
		UInt16		level;
		//准备时间
		UInt32		preTime;
		//开始时间
		UInt32		startTime;
		//截止时间
		UInt32		dueTime;
	};
	
	// 请求活动领取奖励
	class WorldActivityTakeRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TAKE_REWARD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//活动名字
		std::string	name;
	};
	
	// 领取活动奖励返回
	class WorldActivityTakeRewardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TAKE_REWARD_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//返回值
		UInt32 code;
	};

	/**
	 *@brief noted by aprilliu, 2016.04.27 world --> scene  world server同步活动信息给scene server
	 */
	class WorldSyncSceneActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_SCENE_ACTIVITIES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities & tribeactivities;
		}

		//活动列表
		std::vector<ActivityInfo>	activities;
		//部落活动列表
		std::map<ObjID_t, std::vector<ActivityInfo> >	tribeactivities;
	};


	/**
	*@brief server->client 每日首次登录通知
	*/
	class SceneFirstLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FIRST_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->world->client 同步活动计数
	*/
	class WorldSyncActivesCounter : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ACTIVITITY_COUNTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & value;
		}
	public:
		ObjID_t playerId;
		//0表示烟花璀璨值 1代表浪漫度
		UInt8 type;
		UInt32 value;
	};

	/**
	*@brief client->wolrd 通知计数窗口状态信息
	*/
	class WorldNotifyCounterWindow : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_COUNTER_WINDOW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & flag;
		}
	public:
		//计数窗口类型 0璀璨度 1浪漫度
		UInt8 type;
		//窗口标记 0关闭 1打开
		UInt8 flag;
	};

	/**
	*@brief scene->client 全服播放特效通知
	*/
	class SceneBroadcastEffect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BROADCAST_EFFECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
	public:
		//特效类型，0璀璨度 1浪漫度
		UInt8 type;
	};
}

#endif
