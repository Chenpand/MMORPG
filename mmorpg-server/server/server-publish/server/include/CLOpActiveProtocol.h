#ifndef _CL_OP_ACTIVE_PROTOCOL_H_
#define _CL_OP_ACTIVE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->scene 同步运营活动配置
	*/
	class WSSyncOpActivityData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_SYNC_OP_ACTIVITY_DATA)
	public:
		WSSyncOpActivityData(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & datas;
		}

		OpActRegMap datas;
	};

	/**
	*@brief world->scene 同步运营活动状态
	*/
	class WSOpActivityChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_OP_ACTIVITY_CHANGE)
	public:
		WSOpActivityChange() : belongRole(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActRegInfo & belongRole;
		}

		OpActivityRegInfo opActRegInfo;
		//归属玩家
		ObjID_t		belongRole;
	};

	/**
	*@brief client->world 请求cdk验证
	*/
	class CWCDKVerifyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CW_CDK_VERIFY_REQ)
	public:
		CWCDKVerifyReq(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cdk;
		}

		std::string cdk;
	};

	/**
	*@brief scene->client cdk验证返回
	*/
// 	class SCCDKVerifyRes : public Avalon::Protocol
// 	{
// 		AVALON_DEFINE_PROTOCOLID(CS_CDK_VERIFY_RES)
// 	public:
// 		SCCDKVerifyRes(){}
// 
// 		AVALON_DEFINE_NETSERIALIZATION()
// 		{
// 			stream & code;
// 		}
// 
// 		UInt32 code;
// 	};

	/**
	*@brief world->center 请求cdk验证
	*/
	class WCCDKVerifyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WC_CDK_VERIFY_REQ)
	public:
		WCCDKVerifyReq(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & playerUid & serverId & cdk & platform & connId & reason & queryTime;
		}

		UInt32	accId;
		ObjID_t	playerUid;
		UInt32	serverId;
		std::string cdk;
		std::string	platform;
		UInt32 connId;
		UInt32 reason;
		Int64  queryTime;
	};

	/**
	*@brief center->scene cdk验证返回
	*/
	class CSCDKVerifyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SC_CDK_VERIFY_RES)
	public:
		CSCDKVerifyRes(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid & result & errorCode & errorMsg & itemRewards & connId & reason & queryTime;
		}

		ObjID_t	playerUid;
		Int32 result;
		UInt32 errorCode;
		std::string errorMsg;
		std::vector<ItemReward>	itemRewards;
		UInt32 connId;
		UInt32 reason;
		Int64 queryTime;
	};

	/**
	*@brief scene->client 同步运营活动data
	*/
	class SCSyncOpActData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SC_SYNC_OPACTIVITY_DATA)
	public:
		SCSyncOpActData(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & datas;
		}

		std::vector<OpActivityRegInfo> datas;
	};

	/**
	*@brief scene->client 同步运营活动任务
	*/
	class SCSyncOpActTasks : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SC_SYNC_OPACTIVITY_TASKS)
	public:
		SCSyncOpActTasks(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

		std::vector<OpActTask> tasks;
	};

	/**
	*@brief scene->client 同步运营活动任务变化
	*/
	class SCSyncOpActTaskChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SC_SYNC_OPACTIVITY_TASK_CHANGE)
	public:
		SCSyncOpActTaskChange(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tasks;
		}

		std::vector<OpActTask> tasks;
	};

	
	/**
	*@brief scene->client 同步客户端运营活动状态变化
	*/
	class SCSyncOpActStateChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SC_OPACTIVITY_STATE_CHANGE)
	public:
		SCSyncOpActStateChange(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		OpActivityRegInfo info;
	};
	

	/**
	*@brief client->scene 领取运营活动奖励
	*/
	class CSOpActTakeRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CS_OPACTIVITY_TAKE_REWARD_REQ)
	public:
		CSOpActTakeRewardReq(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activityDataId & dataId & param;
		}

		UInt32	activityDataId;
		UInt32	dataId;
		UInt64  param;
	};


	/**
	*@brief scene->client 初始化通知列表
	*/
	class SCInitNotifyList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INIT_NOTIFY_LIST)
	public:
		SCInitNotifyList(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & notifList;
		}

		std::vector<NotifyInfo> notifList;

	};

	/**
	*@brief scene->client 更新通知列表
	*/
	class SCUpdateNotifyList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_NOTIFY_LIST)
	public:
		SCUpdateNotifyList(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & notify;
		}

		NotifyInfo notify;

	};

	/**
	*@brief client->scene 删除通知列表
	*/
	class CSDeleteNotifyList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DELETE_NOTIFY_REQ)
	public:
		CSDeleteNotifyList(){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & notify;
		}

		NotifyInfo notify;

	};
	
	/**
	*@brief client->world 请求活动怪物信息
	*/
	class WorldActivityMonsterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_MONSTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activityId & ids;
		}

		UInt32	activityId;
		std::vector<UInt32> ids;
	};

	/**
	*@brief world->client 返回活动怪物信息
	*/
	class WorldActivityMonsterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_MONSTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activityId & monsters;
		}

		UInt32	activityId;
		VActivityMonsterInfo monsters;
	};

	/**
	*@brief client->scene 活动任务信息请求
	*/
	class SceneOpActivityTaskInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_TASK_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId;
		}

		UInt32 opActId;
	};

	/**
	*@brief world->center->gm 向gm请求运营活动
	*/
	class WorldGmOpActivtyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GM_OP_ACTIVITY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mergeServers;
		}

		std::string mergeServers;
	};

	/**
	*@brief 活动页签信息
	*/
	struct ActivityTabInfo
	{
		ActivityTabInfo() :id(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & name & actIds;
		}

		// 页签id
		UInt32 id;
		// 页签名
		std::string name;
		// 页签下的活动id
		std::vector<UInt32> actIds;
	};

	/**
	*@brief world->client 活动页签信息同步
	*/
	class WorldActivityTabsInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TABS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & tabsInfo;
		}

		std::vector<ActivityTabInfo> tabsInfo;
	};

	/**
	*@brief scene->client 运营活动变量同步
	*/
	class SceneOpActivityVarSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_VAR_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & key & value;
		}

		// 运营活动id
		UInt32 opActId;
		// 变量key
		std::string key;
		// 变量value
		std::string value;
	};

	/**
	*@brief client->scene 神器罐子折扣信息请求
	*/
	class SceneArtifactJarDiscountInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_DISCOUNT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId;
		}

		// 运营活动id
		UInt32 opActId;
	};

	/**
	*@brief client->scene 神器罐子折扣信息同步
	*/
	class SceneArtifactJarDiscountInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_DISCOUNT_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & extractDiscountStatus & discountRate & discountEffectTimes;
		}

		// 运营活动id
		UInt32 opActId;
		// 抽取折扣状态(ArtifactJarDiscountExtractStatus)
		UInt8 extractDiscountStatus;
		// 折扣率
		UInt32 discountRate;
		// 折扣生效次数
		UInt32 discountEffectTimes;
	};

	/**
	*@brief client->scene 神器罐子折扣抽取请求
	*/
	class SceneArtifactJarExtractDiscountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_EXTRACT_DISCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId;
		}

		// 运营活动id
		UInt32 opActId;
	};

	/**
	*@brief scene->client 神器罐子折扣抽取返回
	*/
	class SceneArtifactJarExtractDiscountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_EXTRACT_DISCOUNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & errorCode;
		}

		// 运营活动id
		UInt32 opActId;
		// 错误码
		UInt32 errorCode;
	};


	/**
	*@brief world->scene 首次回归同步
	*/
	class WorldFirstReturnSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_FIRST_RETURN_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		UInt64 roleId;
	};

	/**
	*@brief scene->client 周签到数据下发
	*/
	class SceneWeekSignNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & signWeekSum & yetWeek;
		}

		// 活动类型
		UInt32 opActType;
		// 已签到周数
		UInt32 signWeekSum;
		// 已经领取奖励的周
		std::vector<UInt32> yetWeek;
	};

	/**
	*@brief scene->client 周签到宝箱数据下发
	*/
	class SceneWeekSignBoxNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_BOX_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & boxData;
		}

		// 活动类型
		UInt32 opActType;
		// 宝箱数据
		std::vector<WeekSignBox> boxData;
	};

	/**
	*@brief client->scene 周签到奖励领取请求
	*/
	class SceneWeekSignRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & weekId;
		}

		// 活动类型
		UInt32 opActType;
		// 第几周
		UInt32 weekId;
	};

	/**
	*@brief scene->client 周签到奖励领取返回
	*/
	class SceneWeekSignRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->globalActivity 周签到活动抽奖记录请求
	*/
	class GASWeekSignRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType;
		}

		// 活动类型
		UInt32 opActType;
	};

	struct WeekSignRecord
	{
		WeekSignRecord() : guid(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & serverName & playerName & itemId & itemNum & createTime;
		}

		UInt64 guid;
		// 服务器名字
		std::string serverName;
		// 玩家名字
		std::string playerName;
		// 道具Id
		UInt32 itemId;
		// 道具数量
		UInt32 itemNum;
		// 玩家id
		std::string playerId;
		// 创建时间
		UInt32 createTime;
	};

	/**
	*@brief globalActivity->client 周签到活动抽奖记录返回
	*/
	class GASWeekSignRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & record;
		}

		// 活动类型
		UInt32 opActType;
		// 抽奖记录 
		std::vector<WeekSignRecord> record;
	};

	/**
	*@brief scene->global 周签到活动抽奖请求
	*/
	class GASWeekSignLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actType & actId & vipLvl;
		}

		// 活动类型
		UInt32 actType;
		// 活动ID
		UInt32 actId;
		// vip
		UInt32 vipLvl;
	};
	
	/**
	*@brief global->scene 周签到活动抽奖返回
	*/
	class GASWeekSignLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & actType & actId & rewardVec;
		}

		// 玩家id
		UInt64 roleId;
		// 活动类型
		UInt32 actType;
		// 活动ID
		UInt32 actId;
		// 奖励
		ItemRewardVec rewardVec;
	};

	/**
	*@brief scene->client 运营活动接任务请求
	*/
	class SceneOpActivityAcceptTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_ACCEPT_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & taskId;
		}

		// 运营活动id
		UInt32 opActId;
		// 任务id
		UInt32 taskId;
	};

	/**
	*@brief scene->client 运营活动接任务返回
	*/
	class SceneOpActivityAcceptTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_ACCEPT_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & taskId & retCode;
		}

		// 运营活动id
		UInt32 opActId;
		// 任务id
		UInt32 taskId;
		// 返回值
		UInt32 retCode;
	};

	/**
	*@brief scene->client 运营活动拉取计数请求
	*/
	class SceneOpActivityGetCounterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_GET_COUNTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & counterId;
		}

		// 运营活动id
		UInt32 opActId;
		// 计数id
		UInt32 counterId;
	};

	/**
	*@brief scene->client 运营活动拉取计数返回
	*/
	class SceneOpActivityGetCounterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_GET_COUNTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & counterId & counterValue;
		}

		// 运营活动id
		UInt32 opActId;
		// 计数id
		UInt32 counterId;
		// 计数值
		UInt32 counterValue;
	};

	class SceneChallengeScoreReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHALLENGE_SCORE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};

	class SceneChallengeScoreRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHALLENGE_SCORE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & score;
		}

		// 挑战者积分
		UInt32 score;
	};

}

#endif
