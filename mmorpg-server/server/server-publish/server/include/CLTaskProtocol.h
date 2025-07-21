#ifndef _CL_TASK_PROTOCOL_H_
#define _CL_TASK_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLTaskDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server 访问任务对话
	 */
	class SceneVisitTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_VISIT_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & npcId & taskId;
		}

	public:
		//npc id
		ObjID_t npcId;
		//访问任务id
		UInt32	taskId;
	};

	/**
	 *@brief server->client 通知显示任务对话框
	 */
	class SceneShowTaskDialog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOW_TASK_DIALOG)
	public:
		SceneShowTaskDialog(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & publishType & publishId & npcId & taskId & taskType & quality & taskName & status & words & replies & bShowReward;
			stream & rewards;
		}

	public:
		//发布类型 TaskPublishType
		UInt8   publishType; 
		//发布id  为npc或道具的id
		ObjID_t	publishId;
		//关联npc数据id
		UInt32	npcId;
		//任务
		UInt32	taskId;
		//任务类型
		UInt8	taskType;
		//品质
		UInt8	quality;
		//任务名
		std::string	taskName;
		//任务状态
		UInt8	status;
		//内容
		std::vector<std::string> words;
		//回复
		std::vector<std::string> replies;
		//是否显示奖励
		UInt8	bShowReward;
		//奖励
		std::vector<ItemReward> rewards;
	};

	/**
	 *@brief client->server 接一个任务
	 */
	class SceneAcceptTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACCEPT_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & publishType & publishId & taskId;
		}

	public:
		//发布类型 TaskPublishType
		UInt8   publishType; 
		//发布id  为npc或道具的id  modified by aprilliu, 发布npc的资源id
		UInt32	publishId;
		//任务
		UInt32  taskId;
	};

	/**
	 *@brief client->server 提交一个任务
	 */
	class SceneSubmitTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUBMIT_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream  & submitType & npcId & taskId;
		}

	public:
		//
		UInt8	submitType;   //提交类型，取值：TaskSubmitType
		//npc id
		UInt32	npcId;
		//任务
		UInt32 taskId;
	};

	/**
	 *@brief client->server 放弃一个任务
	 */
	class SceneAbandonTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ABANDON_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

	public:
		//任务
		UInt32 taskId;
	};

	/**
	 *@brief server->client 任务列表（包括已接和可接）
	 */
	class SceneTaskList : public Avalon::Protocol
	{
		//modified by aprilliu, 2016.04.21   去掉协议ID最高位的压缩标识
		/*
		AVALON_DEFINE_COMPRESS_PROTOCOLID(SCENE_TASK_LIST)
		*/
		AVALON_DEFINE_PROTOCOLID(SCENE_TASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;  //modifie by aprilliu. 2016.04.09
	};

	/**
	 *@brief server->client 通知新的任务
	 */
	class SceneNotifyNewTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_NEWTASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & task;
		}

	public:
		//任务
		TaskBriefInfo task;
	};

	/**
	 *@brief server->client 通知删除一个任务
	 */
	class SceneNotifyDeleteTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_DELETETASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & reason;
		}

	public:
		//任务id
		UInt32	taskId;
		//取值: DeleteTaskReason
		UInt8	reason;
	};

	/**
	 *@brief server->client 任务状态变更
	 */
	class SceneNotifyTaskStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_TASK_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & status & finTime;
		}

	public:
		//任务
		UInt32 taskId;
		//状态
		UInt8  status;
		//完成时间
		UInt32 finTime;
		//当前状态目标 , deleted by aprilliu 2016.04.09
		//std::string	target;
	};

	/**
	*@brief server->client 同步活动
	*/
	class SceneSyncClientActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACTIVITIS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities;
		}

		//活动列表
		std::vector<ActivityInfo>		activities;
	};
	

	/**
	*@brief server->client 活动任务状态变更
	*/
	class SceneNotifyActiveTaskStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_ACTIVETASK_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & status;
		}

	public:
		//任务
		UInt32 taskId;
		//状态
		UInt8  status;
	};

	/**
	 *@brief server->client任务变量更新
	 */
	class SceneNotifyTaskVar : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_TASK_VAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & key & value;
		}

	public:
		//任务id
		UInt32 taskId;
		//键
		std::string key;
		//值
		std::string value;
	};

	/**
	*@brief server->client活动任务变量更新
	*/
	class SceneNotifyActiveTaskVar : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_ACTIVETASK_VAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & key & value;
		}

	public:
		//任务id
		UInt32 taskId;
		//键
		std::string key;
		//值
		std::string value;
	};

	/**
	*@brief server->client同步进度礼包
	*/
	class SceneSyncPhaseGift : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PHASE_GIFT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftId & canTake;
		}

	public:
		//礼包id
		UInt32	giftId;
		//状态1.可领取，0.未完成
		UInt8	canTake;
	};


	/**
	*@brief client->server领取进度礼包
	*/
	class SceneTakePhaseGiftReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TAKE_PHASE_GIFT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftId;
		}

	public:
		//礼包id
		UInt32	giftId;
	};


	/**
	 *@brief server->client 上线发送日常任务次数
	 */
	class SceneSyncDailyTaskTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_DAILYTASK_TIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskTimes;
		}

		//次数
		std::map<UInt8, DailyTaskTimes>	taskTimes;
	};

	/**
	 *@brief server->client 更新某个日常任务次数
	 */
	class SceneUpdateDailyTaskTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_DAILYTASK_TIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & times & maxtimes;
		}

		//任务类型
		UInt8	type;
		//次数
		UInt16	times;
		//最大次数
		UInt16	maxtimes;
	};

	/**
	*@brief client->server 请求传奇之路列表
	*/
	class SceneLegendTaskListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LEGEND_TASK_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

	/**
	*@brief server->client 响应传奇之路列表
	*/
	class SceneLegendTaskListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_LEGEND_TASK_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server 提交传奇之路
	*/
	class SceneSubmitLegendTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUBMIT_LEGEND_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}
	public:
		//任务id
		UInt32	taskId;
	};

	/**
	*@brief client->server 重置任务
	*/
	class SceneResetTaskSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RESET_TASK_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}
	public:
		// 重置任务信息
		TaskBriefInfo info;
	};

	/**
	 *@brief client->server 快速完成
	 */
	class SceneTaskQuickSubmit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TASK_QUICK_SUBMIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

	public:
		//任务id
		UInt32	taskId;
	};

	/**
	 *@brief server->client 转换阵营时同步新的描述
	 */
	class SceneTaskSyncDescription : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TASK_SYNC_DESCRIPTION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & target & description;
		}

	public:
		//任务id
		UInt32 taskId;
		//目标
		std::string	target;
		//描述
		std::string	description;
	};

	/**
	*@brief server->client 日常任务列表
	*/
	class SceneDailyTaskList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DAILY_TASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief server->client 同步活动任务列表
	*/
	class SceneSyncActiveTaskList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACTIVETASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server 提交一个活动任务
	*/
	class SceneActiveTaskSubmit : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTIVETASK_SUBMIT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & param1;
		}

	public:
		//任务
		UInt32 taskId;
		UInt32 param1;
	};
	
	/**
	*@brief client->server 补签签到
	*/
	class SceneSignInRp : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SIGNIN_REPAIR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskIds;
		}

	public:
		//任务
		std::vector<UInt32> taskIds;
	};


	/**
	*@brief client->server 新自然月签到
	*/
	class SceneNewSignIn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & day & isAll;
		}

	public:
		//哪一天,0代表今天，非0代表补签那一天
		UInt8 day;
		//代表是否全部补签
		UInt8 isAll;
	};

	class SceneNewSignRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

	public:
		UInt32 errorCode;
	};

	/**
	*@brief client->server 查询新月签到情况
	*/
	class SceneNewSignInQuery : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN_QUERY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	public:

	};

	class SceneNewSignInQueryRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN_QUERY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & signFlag & collectFlag & noFree & free & activite & activiteCount;
		}

	public:
		//签到记录按位存储
		UInt32 signFlag;
		//已领取累计奖励的累计天数按位存储
		UInt32 collectFlag;
		//剩余收费补签次数
		UInt8 noFree;
		//剩余免费补签次数
		UInt8 free;
		//剩余活跃度补签次数
		UInt8 activite;
		//活跃度已补签次数
		UInt8 activiteCount;
	};

	class SceneNewSignInCollect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN_COLLECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & day;
		}

	public:
		//领取累计几天的奖励
		UInt8 day;
	};

	class SceneNewSignInCollectRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NEW_SIGNIN_COLLECT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

	public:
		UInt32 errorCode;
	};

	

	/**
	*@brief sever->client 请求七日剩余时间
	*/
	class SceneRestTimeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REST_TIME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	public:
	};

	/**
	*@brief sever->client 请求七日剩余时间返回
	*/
	class SceneRestTimeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REST_TIME_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time1 & time2 & time3;
		}

	public:
		
		UInt32 time1;
		UInt32 time2;
		UInt32 time3;
	};


	/**
	*@brief client->server 提交一个日常任务
	*/
	class SceneSubmitDailyTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUBMIT_DAILY_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

	public:
		//任务
		UInt32 taskId;
	};

	/**
	*@brief client->server VIP消耗点卷, 完成所有日常任务
	*/
	class SceneSubmitAllDailyTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUBMIT_ALL_DAILY_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	public:
	};

		

	/**
	*@brief server->client 成就列表
	*/
	class SceneAchievementTaskList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACHIEVEMENT_TASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server 提交一个成就
	*/
	class SceneSubmitAchievementTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SUBMIT_ACHIEVEMENT_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

	public:
		//任务
		UInt32 taskId;
	};

	/**
	*@brief client->server 请求提交任务物品
	*/
	class SceneSetTaskItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_TASK_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & itemIds;
		}

	public:
		UInt32 taskId;
		std::vector<UInt64> itemIds;
	};

	/**
	*@brief server->client 响应提交任务物品
	*/
	class SceneSetTaskItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_TASK_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 刷新循环任务
	*/
	class SceneRefreshCycleTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REFRESH_CYCLE_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	

	/**
	*@brief client->server 请求领取每日积分奖励
	*/
	class SceneDailyScoreRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DAILY_SCORE_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & boxId;
		}

	public:
		UInt8 boxId;
	};

	/**
	*@brief server->client 响应领取每日积分奖励
	*/
	class SceneDailyScoreRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DAILY_SCORE_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief client->server 请求领取成就积分奖励
	*/
	class SceneAchievementScoreRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACHIEVEMENT_SCORE_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rewardId;
		}

	public:
		UInt32 rewardId;
	};

	/**
	*@brief server->client 响应领取成就积分奖励
	*/
	class SceneAchievementScoreRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACHIEVEMENT_SCORE_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32 result;
	};

	/**
	*@brief world->scene 活动任务完成后发送奖励
	*/
	class WorldActivityTaskSendRewards : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TASK_SEND_REWARDS)
	public:
		AVALON_DEFINE_NETSERIALIZATION() 
		{
			stream & roleId & taskId & rewards & notify;
		}

	public:
		UInt64 roleId;
		UInt32 taskId;
		std::vector<ItemReward> rewards;
		UInt8 notify;
	};

	/**
	*@brief server->client 同步师门日常任务列表
	*/
	class SceneSyncMasterDailyTaskList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTER_DAILY_TASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};


	/**
	*@brief server->client 同步师门学业成长任务列表
	*/
	class SceneSyncMasterAcademicTaskList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTER_ACADEMIC_TASK_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

	public:
		//任务列表
		std::vector<TaskBriefInfo> tasks;
	};


	/**
	*@brief client->world  发布师门任务请求
	*/
	class WorldPublishMasterDialyTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUBLISH_MASTER_DAILY_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}

		ObjID_t	discipleId;
	};

	/**
	*@brief world->client 发布师门任务返回
	*/
	class WorldPublishMasterDialyTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUBLISH_MASTER_DAILY_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & discipleId;
		}

		UInt32	code;		//结果
		ObjID_t	discipleId; //徒弟id
	};

	/**
	*@brief world->scene  通知scene发布师门任务
	*/
	class WorldNotifyPublishMasterTaskToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_PUBLISH_MASTERTASK_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & discipleId;
		}

	public:
		UInt64 masterId;
		UInt64 discipleId;
	};

	/**
	*@brief scene->world  Scene通知World徒弟接取任务结果
	*/
	class SceneNotifyPublishMasterTaskResToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_PUBLISH_MASTERTASKRES_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & masterId & discipleId;
		}

		UInt32 code;
		UInt64 masterId;
		UInt64 discipleId;
	};

	/**
	*@brief client->world  获取徒弟师门任务数据请求 
	*/
	class WorldGetDiscipleMasterTasksReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}

		UInt64 discipleId;   //徒弟id
	};

	/**
	*@brief world->client  获取徒弟师门任务数据返回
	*/
	class WorldGetDiscipleMasterTasksRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & discipleId & masterTaskShareData;
		}
		UInt32   code;							//结果
		ObjID_t  discipleId;					//徒弟id
		MasterTaskShareData masterTaskShareData; //徒弟离线师门数据
	};

	/**
	*@brief world->scene 获取在线徒弟师们任务数据
	*/
	class WorldGetDiscipleMasterTasksToSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_TOSCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & discipleId;
		}

		ObjID_t masterId;	 //师傅id
		ObjID_t discipleId;  //徒弟id
	};

	/**
	*@brief scene->world 获取在线徒弟师们任务数据返回
	*/

	class WorldGetDiscipleMasterTasksToSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_TOSCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & masterId & discipleId & masterTaskShareData;
		}

		UInt32  code;		 //结果
		ObjID_t masterId;	 //师傅id
		ObjID_t discipleId;  //徒弟id
		MasterTaskShareData masterTaskShareData; //徒弟离线师门数据
	};

	/**
	*@brief scene->world 同步师门离线数据
	*/
	class SceneSyncMasterTaskShareDataToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTERTASKSHAREDATA_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & masterTaskShareData;
		}

		ObjID_t roleId;							 //角色id
		MasterTaskShareData masterTaskShareData; //徒弟离线师门数据
	};

	/**
	*@brief client->world 徒弟汇报师门任务完成请求
	*/
	class WorldReportMasterDailyTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REPORT_MASTER_DAILY_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId;
		}
		ObjID_t masterId;							 //师傅角色id
	};
	
	/**
	*@brief world->client 徒弟汇报师门任务完成返回
	*/
	class  WorldReportMasterDailyTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REPORT_MASTER_DAILY_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //结果
	};


	/**
	*@brief world->scene 检查徒弟师门任务是否完成
	*/
	class WorldCheckMasterDailyTaskReqToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTERDAILYTASK_REQ_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}
		ObjID_t  discipleId;		 //徒弟id
	};

	/**
	*@brief scene->world 查看师徒师门任务完成返回
	*/
	class SceneCheckMasterDailyTaskResToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_MASTERDAILYTASK_REQ_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId & completed;
		}
		ObjID_t  discipleId;		 //徒弟id
		UInt8   completed;		 //0:未完成 1:完成
	};

	/**
	*@brief client->world 领取师门日常任务完成奖励
	*/
	class WorldReceiveMasterDailyTaskRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERDAILYTASK_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & discipeleId;
		}
		UInt8	 type;			//1:师傅，2:徒弟
		ObjID_t  discipeleId;   //师傅领取的时候 要填徒弟id
	};

	/**
	*@brief world->client  领取师门日常任务完成奖励返回
	*/
	class WorldReceiveMasterDailyTaskRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERDAILYTASK_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //结果

	};

	/**
	*@brief client->world 徒弟领取师门成长奖励请求
	*/
	class WorldReceiveMasterAcademicTaskRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERACADEMIC_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftDataId;
		}
		UInt32 giftDataId;	//masterSysGiftTable表ID
	};

	/**
	*@brief world->client 徒弟领取师门成长奖励返回
	*/
	class WorldReceiveMasterAcademicTaskRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERACADEMIC_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //结果
	};

	/**
	*@brief  world->scene 发放师门礼包
	*/
	class WorldNotifyGiveMasterSysGiftReqToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GIVE_MASTERGIFT_REQ_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & discipleId & type & giftDataId;
		}
		ObjID_t     masterId;				//师傅id
		ObjID_t		discipleId;				//徒弟id 
		UInt8		type;					//类型
		UInt32		giftDataId;				//师门礼包表id
		

	};

	/**
	*@brief	 scene->world 发师门礼包结果返回
	*/
	class SceneNotifyGiveMasterSysGiftResToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_GIVE_MASTERGIFT_RES_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & masterId & discipleId & type & giftDataId;
		}
		UInt32		code;		 //结果
		ObjID_t     masterId;	 //师傅id
		ObjID_t		discipleId;	 //徒弟id 
		UInt8		type;		 //类型
		UInt32		giftDataId;	 //师门礼包表id
		
	};

	/*
	*@brief scene->world 同步师门系统相关值
	*/
	class SceneSyncMasterSysDataToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTERSYS_DATA_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & m_roleId & m_AcademicTotalGrowth & m_GoodTeacherValue & m_MasterGiveEquipGrowth;
		}
		ObjID_t		m_roleId;
		UInt32		m_AcademicTotalGrowth;
		UInt32		m_GoodTeacherValue;
		UInt32		m_MasterGiveEquipGrowth;
	};

	/**
	*@brief  world->scene 账号任务检查背包容量充足请求
	*/
	class WorldAccountTaskCheckPakcageEnoughReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_TASK_CHECK_PACKAGE_ENOUGH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & taskId & rewards;
		}

	public:
		// 角色id
		ObjID_t roleid;
		// 任务id
		UInt32 taskId;
		// 奖励组
		ItemRewardVec rewards;
	};

	/**
	*@brief	 scene->world 账号任务检查背包容量充足返回
	*/
	class WorldAccountTaskCheckPakcageEnoughRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_TASK_CHECK_PACKAGE_ENOUGH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & roleid & taskId & rewards;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 角色id
		ObjID_t roleid;
		// 任务id
		UInt32 taskId;
		// 奖励组
		ItemRewardVec rewards;
	};

	/**
	*@brief client->world 提交一个账号任务
	*/
	class WorldSubmitAccountTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SUBMIT_ACCOUNT_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

	public:
		// 任务id
		UInt32 taskId;
	};

	/**
	*@brief client->world 请求提交任务物品
	*/
	class WorldSetTaskItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_TASK_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & items;
		}

	public:
		UInt32 taskId;
		std::vector<WorldSetTaskItemStruct> items;
	};

	/**
	*@brief server->client 响应提交任务物品
	*/
	class WorldSetTaskItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_TASK_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};
}

#endif
