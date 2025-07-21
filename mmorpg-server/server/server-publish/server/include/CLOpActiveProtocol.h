#ifndef _CL_OP_ACTIVE_PROTOCOL_H_
#define _CL_OP_ACTIVE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	*@brief world->scene ͬ����Ӫ�����
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
	*@brief world->scene ͬ����Ӫ�״̬
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
		//�������
		ObjID_t		belongRole;
	};

	/**
	*@brief client->world ����cdk��֤
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
	*@brief scene->client cdk��֤����
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
	*@brief world->center ����cdk��֤
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
	*@brief center->scene cdk��֤����
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
	*@brief scene->client ͬ����Ӫ�data
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
	*@brief scene->client ͬ����Ӫ�����
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
	*@brief scene->client ͬ����Ӫ�����仯
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
	*@brief scene->client ͬ���ͻ�����Ӫ�״̬�仯
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
	*@brief client->scene ��ȡ��Ӫ�����
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
	*@brief scene->client ��ʼ��֪ͨ�б�
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
	*@brief scene->client ����֪ͨ�б�
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
	*@brief client->scene ɾ��֪ͨ�б�
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
	*@brief client->world ����������Ϣ
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
	*@brief world->client ���ػ������Ϣ
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
	*@brief client->scene �������Ϣ����
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
	*@brief world->center->gm ��gm������Ӫ�
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
	*@brief �ҳǩ��Ϣ
	*/
	struct ActivityTabInfo
	{
		ActivityTabInfo() :id(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & name & actIds;
		}

		// ҳǩid
		UInt32 id;
		// ҳǩ��
		std::string name;
		// ҳǩ�µĻid
		std::vector<UInt32> actIds;
	};

	/**
	*@brief world->client �ҳǩ��Ϣͬ��
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
	*@brief scene->client ��Ӫ�����ͬ��
	*/
	class SceneOpActivityVarSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_VAR_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & key & value;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ����key
		std::string key;
		// ����value
		std::string value;
	};

	/**
	*@brief client->scene ���������ۿ���Ϣ����
	*/
	class SceneArtifactJarDiscountInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_DISCOUNT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId;
		}

		// ��Ӫ�id
		UInt32 opActId;
	};

	/**
	*@brief client->scene ���������ۿ���Ϣͬ��
	*/
	class SceneArtifactJarDiscountInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_DISCOUNT_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & extractDiscountStatus & discountRate & discountEffectTimes;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ��ȡ�ۿ�״̬(ArtifactJarDiscountExtractStatus)
		UInt8 extractDiscountStatus;
		// �ۿ���
		UInt32 discountRate;
		// �ۿ���Ч����
		UInt32 discountEffectTimes;
	};

	/**
	*@brief client->scene ���������ۿ۳�ȡ����
	*/
	class SceneArtifactJarExtractDiscountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_EXTRACT_DISCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId;
		}

		// ��Ӫ�id
		UInt32 opActId;
	};

	/**
	*@brief scene->client ���������ۿ۳�ȡ����
	*/
	class SceneArtifactJarExtractDiscountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_EXTRACT_DISCOUNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & errorCode;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ������
		UInt32 errorCode;
	};


	/**
	*@brief world->scene �״λع�ͬ��
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
	*@brief scene->client ��ǩ�������·�
	*/
	class SceneWeekSignNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & signWeekSum & yetWeek;
		}

		// �����
		UInt32 opActType;
		// ��ǩ������
		UInt32 signWeekSum;
		// �Ѿ���ȡ��������
		std::vector<UInt32> yetWeek;
	};

	/**
	*@brief scene->client ��ǩ�����������·�
	*/
	class SceneWeekSignBoxNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_BOX_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & boxData;
		}

		// �����
		UInt32 opActType;
		// ��������
		std::vector<WeekSignBox> boxData;
	};

	/**
	*@brief client->scene ��ǩ��������ȡ����
	*/
	class SceneWeekSignRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEEK_SIGN_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & weekId;
		}

		// �����
		UInt32 opActType;
		// �ڼ���
		UInt32 weekId;
	};

	/**
	*@brief scene->client ��ǩ��������ȡ����
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
	*@brief client->globalActivity ��ǩ����齱��¼����
	*/
	class GASWeekSignRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType;
		}

		// �����
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
		// ����������
		std::string serverName;
		// �������
		std::string playerName;
		// ����Id
		UInt32 itemId;
		// ��������
		UInt32 itemNum;
		// ���id
		std::string playerId;
		// ����ʱ��
		UInt32 createTime;
	};

	/**
	*@brief globalActivity->client ��ǩ����齱��¼����
	*/
	class GASWeekSignRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActType & record;
		}

		// �����
		UInt32 opActType;
		// �齱��¼ 
		std::vector<WeekSignRecord> record;
	};

	/**
	*@brief scene->global ��ǩ����齱����
	*/
	class GASWeekSignLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & actType & actId & vipLvl;
		}

		// �����
		UInt32 actType;
		// �ID
		UInt32 actId;
		// vip
		UInt32 vipLvl;
	};
	
	/**
	*@brief global->scene ��ǩ����齱����
	*/
	class GASWeekSignLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_WEEK_SIGN_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & actType & actId & rewardVec;
		}

		// ���id
		UInt64 roleId;
		// �����
		UInt32 actType;
		// �ID
		UInt32 actId;
		// ����
		ItemRewardVec rewardVec;
	};

	/**
	*@brief scene->client ��Ӫ�����������
	*/
	class SceneOpActivityAcceptTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_ACCEPT_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & taskId;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ����id
		UInt32 taskId;
	};

	/**
	*@brief scene->client ��Ӫ������񷵻�
	*/
	class SceneOpActivityAcceptTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_ACCEPT_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & taskId & retCode;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ����id
		UInt32 taskId;
		// ����ֵ
		UInt32 retCode;
	};

	/**
	*@brief scene->client ��Ӫ���ȡ��������
	*/
	class SceneOpActivityGetCounterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_GET_COUNTER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & counterId;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ����id
		UInt32 counterId;
	};

	/**
	*@brief scene->client ��Ӫ���ȡ��������
	*/
	class SceneOpActivityGetCounterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OP_ACTIVITY_GET_COUNTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opActId & counterId & counterValue;
		}

		// ��Ӫ�id
		UInt32 opActId;
		// ����id
		UInt32 counterId;
		// ����ֵ
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

		// ��ս�߻���
		UInt32 score;
	};

}

#endif
