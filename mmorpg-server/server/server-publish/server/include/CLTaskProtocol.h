#ifndef _CL_TASK_PROTOCOL_H_
#define _CL_TASK_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLTaskDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server ��������Ի�
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
		//��������id
		UInt32	taskId;
	};

	/**
	 *@brief server->client ֪ͨ��ʾ����Ի���
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
		//�������� TaskPublishType
		UInt8   publishType; 
		//����id  Ϊnpc����ߵ�id
		ObjID_t	publishId;
		//����npc����id
		UInt32	npcId;
		//����
		UInt32	taskId;
		//��������
		UInt8	taskType;
		//Ʒ��
		UInt8	quality;
		//������
		std::string	taskName;
		//����״̬
		UInt8	status;
		//����
		std::vector<std::string> words;
		//�ظ�
		std::vector<std::string> replies;
		//�Ƿ���ʾ����
		UInt8	bShowReward;
		//����
		std::vector<ItemReward> rewards;
	};

	/**
	 *@brief client->server ��һ������
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
		//�������� TaskPublishType
		UInt8   publishType; 
		//����id  Ϊnpc����ߵ�id  modified by aprilliu, ����npc����Դid
		UInt32	publishId;
		//����
		UInt32  taskId;
	};

	/**
	 *@brief client->server �ύһ������
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
		UInt8	submitType;   //�ύ���ͣ�ȡֵ��TaskSubmitType
		//npc id
		UInt32	npcId;
		//����
		UInt32 taskId;
	};

	/**
	 *@brief client->server ����һ������
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
		//����
		UInt32 taskId;
	};

	/**
	 *@brief server->client �����б������ѽӺͿɽӣ�
	 */
	class SceneTaskList : public Avalon::Protocol
	{
		//modified by aprilliu, 2016.04.21   ȥ��Э��ID���λ��ѹ����ʶ
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;  //modifie by aprilliu. 2016.04.09
	};

	/**
	 *@brief server->client ֪ͨ�µ�����
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
		//����
		TaskBriefInfo task;
	};

	/**
	 *@brief server->client ֪ͨɾ��һ������
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
		//����id
		UInt32	taskId;
		//ȡֵ: DeleteTaskReason
		UInt8	reason;
	};

	/**
	 *@brief server->client ����״̬���
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
		//����
		UInt32 taskId;
		//״̬
		UInt8  status;
		//���ʱ��
		UInt32 finTime;
		//��ǰ״̬Ŀ�� , deleted by aprilliu 2016.04.09
		//std::string	target;
	};

	/**
	*@brief server->client ͬ���
	*/
	class SceneSyncClientActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACTIVITIS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities;
		}

		//��б�
		std::vector<ActivityInfo>		activities;
	};
	

	/**
	*@brief server->client �����״̬���
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
		//����
		UInt32 taskId;
		//״̬
		UInt8  status;
	};

	/**
	 *@brief server->client�����������
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
		//����id
		UInt32 taskId;
		//��
		std::string key;
		//ֵ
		std::string value;
	};

	/**
	*@brief server->client������������
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
		//����id
		UInt32 taskId;
		//��
		std::string key;
		//ֵ
		std::string value;
	};

	/**
	*@brief server->clientͬ���������
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
		//���id
		UInt32	giftId;
		//״̬1.����ȡ��0.δ���
		UInt8	canTake;
	};


	/**
	*@brief client->server��ȡ�������
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
		//���id
		UInt32	giftId;
	};


	/**
	 *@brief server->client ���߷����ճ��������
	 */
	class SceneSyncDailyTaskTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_DAILYTASK_TIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskTimes;
		}

		//����
		std::map<UInt8, DailyTaskTimes>	taskTimes;
	};

	/**
	 *@brief server->client ����ĳ���ճ��������
	 */
	class SceneUpdateDailyTaskTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_DAILYTASK_TIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & times & maxtimes;
		}

		//��������
		UInt8	type;
		//����
		UInt16	times;
		//������
		UInt16	maxtimes;
	};

	/**
	*@brief client->server ������֮·�б�
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
	*@brief server->client ��Ӧ����֮·�б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server �ύ����֮·
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
		//����id
		UInt32	taskId;
	};

	/**
	*@brief client->server ��������
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
		// ����������Ϣ
		TaskBriefInfo info;
	};

	/**
	 *@brief client->server �������
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
		//����id
		UInt32	taskId;
	};

	/**
	 *@brief server->client ת����Ӫʱͬ���µ�����
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
		//����id
		UInt32 taskId;
		//Ŀ��
		std::string	target;
		//����
		std::string	description;
	};

	/**
	*@brief server->client �ճ������б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief server->client ͬ��������б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server �ύһ�������
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
		//����
		UInt32 taskId;
		UInt32 param1;
	};
	
	/**
	*@brief client->server ��ǩǩ��
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
		//����
		std::vector<UInt32> taskIds;
	};


	/**
	*@brief client->server ����Ȼ��ǩ��
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
		//��һ��,0������죬��0����ǩ��һ��
		UInt8 day;
		//�����Ƿ�ȫ����ǩ
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
	*@brief client->server ��ѯ����ǩ�����
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
		//ǩ����¼��λ�洢
		UInt32 signFlag;
		//����ȡ�ۼƽ������ۼ�������λ�洢
		UInt32 collectFlag;
		//ʣ���շѲ�ǩ����
		UInt8 noFree;
		//ʣ����Ѳ�ǩ����
		UInt8 free;
		//ʣ���Ծ�Ȳ�ǩ����
		UInt8 activite;
		//��Ծ���Ѳ�ǩ����
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
		//��ȡ�ۼƼ���Ľ���
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
	*@brief sever->client ��������ʣ��ʱ��
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
	*@brief sever->client ��������ʣ��ʱ�䷵��
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
	*@brief client->server �ύһ���ճ�����
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
		//����
		UInt32 taskId;
	};

	/**
	*@brief client->server VIP���ĵ��, ��������ճ�����
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
	*@brief server->client �ɾ��б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};

	/**
	*@brief client->server �ύһ���ɾ�
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
		//����
		UInt32 taskId;
	};

	/**
	*@brief client->server �����ύ������Ʒ
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
	*@brief server->client ��Ӧ�ύ������Ʒ
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
	*@brief client->server ˢ��ѭ������
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
	*@brief client->server ������ȡÿ�ջ��ֽ���
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
	*@brief server->client ��Ӧ��ȡÿ�ջ��ֽ���
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
	*@brief client->server ������ȡ�ɾͻ��ֽ���
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
	*@brief server->client ��Ӧ��ȡ�ɾͻ��ֽ���
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
	*@brief world->scene �������ɺ��ͽ���
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
	*@brief server->client ͬ��ʦ���ճ������б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};


	/**
	*@brief server->client ͬ��ʦ��ѧҵ�ɳ������б�
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
		//�����б�
		std::vector<TaskBriefInfo> tasks;
	};


	/**
	*@brief client->world  ����ʦ����������
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
	*@brief world->client ����ʦ�����񷵻�
	*/
	class WorldPublishMasterDialyTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PUBLISH_MASTER_DAILY_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & discipleId;
		}

		UInt32	code;		//���
		ObjID_t	discipleId; //ͽ��id
	};

	/**
	*@brief world->scene  ֪ͨscene����ʦ������
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
	*@brief scene->world  Scene֪ͨWorldͽ�ܽ�ȡ������
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
	*@brief client->world  ��ȡͽ��ʦ�������������� 
	*/
	class WorldGetDiscipleMasterTasksReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}

		UInt64 discipleId;   //ͽ��id
	};

	/**
	*@brief world->client  ��ȡͽ��ʦ���������ݷ���
	*/
	class WorldGetDiscipleMasterTasksRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & discipleId & masterTaskShareData;
		}
		UInt32   code;							//���
		ObjID_t  discipleId;					//ͽ��id
		MasterTaskShareData masterTaskShareData; //ͽ������ʦ������
	};

	/**
	*@brief world->scene ��ȡ����ͽ��ʦ����������
	*/
	class WorldGetDiscipleMasterTasksToSceneReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_TOSCENE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & discipleId;
		}

		ObjID_t masterId;	 //ʦ��id
		ObjID_t discipleId;  //ͽ��id
	};

	/**
	*@brief scene->world ��ȡ����ͽ��ʦ���������ݷ���
	*/

	class WorldGetDiscipleMasterTasksToSceneRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_DISCIPLE_MASTERTASKS_TOSCENE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & masterId & discipleId & masterTaskShareData;
		}

		UInt32  code;		 //���
		ObjID_t masterId;	 //ʦ��id
		ObjID_t discipleId;  //ͽ��id
		MasterTaskShareData masterTaskShareData; //ͽ������ʦ������
	};

	/**
	*@brief scene->world ͬ��ʦ����������
	*/
	class SceneSyncMasterTaskShareDataToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTERTASKSHAREDATA_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & masterTaskShareData;
		}

		ObjID_t roleId;							 //��ɫid
		MasterTaskShareData masterTaskShareData; //ͽ������ʦ������
	};

	/**
	*@brief client->world ͽ�ܻ㱨ʦ�������������
	*/
	class WorldReportMasterDailyTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REPORT_MASTER_DAILY_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId;
		}
		ObjID_t masterId;							 //ʦ����ɫid
	};
	
	/**
	*@brief world->client ͽ�ܻ㱨ʦ��������ɷ���
	*/
	class  WorldReportMasterDailyTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REPORT_MASTER_DAILY_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //���
	};


	/**
	*@brief world->scene ���ͽ��ʦ�������Ƿ����
	*/
	class WorldCheckMasterDailyTaskReqToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTERDAILYTASK_REQ_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}
		ObjID_t  discipleId;		 //ͽ��id
	};

	/**
	*@brief scene->world �鿴ʦͽʦ��������ɷ���
	*/
	class SceneCheckMasterDailyTaskResToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_MASTERDAILYTASK_REQ_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId & completed;
		}
		ObjID_t  discipleId;		 //ͽ��id
		UInt8   completed;		 //0:δ��� 1:���
	};

	/**
	*@brief client->world ��ȡʦ���ճ�������ɽ���
	*/
	class WorldReceiveMasterDailyTaskRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERDAILYTASK_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & discipeleId;
		}
		UInt8	 type;			//1:ʦ����2:ͽ��
		ObjID_t  discipeleId;   //ʦ����ȡ��ʱ�� Ҫ��ͽ��id
	};

	/**
	*@brief world->client  ��ȡʦ���ճ�������ɽ�������
	*/
	class WorldReceiveMasterDailyTaskRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERDAILYTASK_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //���

	};

	/**
	*@brief client->world ͽ����ȡʦ�ųɳ���������
	*/
	class WorldReceiveMasterAcademicTaskRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERACADEMIC_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftDataId;
		}
		UInt32 giftDataId;	//masterSysGiftTable��ID
	};

	/**
	*@brief world->client ͽ����ȡʦ�ųɳ���������
	*/
	class WorldReceiveMasterAcademicTaskRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RECEIVE_MASTERACADEMIC_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32  code;		 //���
	};

	/**
	*@brief  world->scene ����ʦ�����
	*/
	class WorldNotifyGiveMasterSysGiftReqToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_GIVE_MASTERGIFT_REQ_TOSCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & discipleId & type & giftDataId;
		}
		ObjID_t     masterId;				//ʦ��id
		ObjID_t		discipleId;				//ͽ��id 
		UInt8		type;					//����
		UInt32		giftDataId;				//ʦ�������id
		

	};

	/**
	*@brief	 scene->world ��ʦ������������
	*/
	class SceneNotifyGiveMasterSysGiftResToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_GIVE_MASTERGIFT_RES_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & masterId & discipleId & type & giftDataId;
		}
		UInt32		code;		 //���
		ObjID_t     masterId;	 //ʦ��id
		ObjID_t		discipleId;	 //ͽ��id 
		UInt8		type;		 //����
		UInt32		giftDataId;	 //ʦ�������id
		
	};

	/*
	*@brief scene->world ͬ��ʦ��ϵͳ���ֵ
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
	*@brief  world->scene �˺������鱳��������������
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
		// ��ɫid
		ObjID_t roleid;
		// ����id
		UInt32 taskId;
		// ������
		ItemRewardVec rewards;
	};

	/**
	*@brief	 scene->world �˺������鱳���������㷵��
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
		// ������
		UInt32 resCode;
		// ��ɫid
		ObjID_t roleid;
		// ����id
		UInt32 taskId;
		// ������
		ItemRewardVec rewards;
	};

	/**
	*@brief client->world �ύһ���˺�����
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
		// ����id
		UInt32 taskId;
	};

	/**
	*@brief client->world �����ύ������Ʒ
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
	*@brief server->client ��Ӧ�ύ������Ʒ
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
