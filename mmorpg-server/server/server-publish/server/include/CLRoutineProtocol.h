#ifndef _CL_ROUTINE_PROTOCOL_H_
#define _CL_ROUTINE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"
#include "CLActivityDefine.h"

namespace CLProtocol
{
	/**
	 *������ȡ��־ 1-4Ϊ��Ծ������ 11Ϊǩ������ 12Ϊvipǩ������ 14Ϊ�����Ƽ�λ���
	 */

	/**
	 *@brief client->server �����Ծ��
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
	 *@brief server->client ���ػ�Ծ��
	 */
	class SceneActiveDegreeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTIVE_DEGREE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeEvents & boxGotFlag & activeDegree;
		}

		//��Ծ����Ŀ��ɴ���
		std::map<UInt8,UInt8>	activeEvents;
		//������ȡ��־ 1-4Ϊ��Ծ������
		UInt32	boxGotFlag;
		//��Ծ��
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client ֪ͨ��Ծ�����
	 */
	class SceneNotifyActiveEventFinished : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_ACTIVEEVENT_FINISHED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & times & activeDegree;
		}

		//�¼����
		UInt8	id;
		//��ɴ���
		UInt8	times;
		//��ǰ��Ծ��
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client ����ͬ���ճ�������Ϣ
	 */
	class SceneRoutineSyncBaseInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_SYNC_BASEINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bGotQQVipGiftbag & onlineTime;
		}

		//�Ƿ�����ȡ�����Ƽ����
		UInt8	bGotQQVipGiftbag;
		//����ʱ�� ��
		UInt32	onlineTime;
	};

	/**
	 *@brief client->server ������ȡ����
	 */
	class SceneRoutineGetBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_GETBOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & index;
		}

		//����  1-4Ϊ��Ծ������  11Ϊǩ������ 12Ϊvipǩ����� 14Ϊ�����Ƽ�λ���
		UInt8	index;
	};

	/**
	 *@brief server->client ��ȡ���ӷ���
	 */
	class SceneRoutineGetBoxRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_GETBOX_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & index & boxGotFlag & activeDegree;
		}

		//��������
		UInt8	index;
		//������ȡ��־
		UInt32	boxGotFlag;
		//��ǰ��Ծ��
		UInt32	activeDegree;
	};

	/**
	 *@brief server->client ֪ͨ���û�Ծ��
	 */
	class SceneNotifyResetActiveDegree : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_RESET_ACTIVEDEGREE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & signinDays & signinBoxId & vipSigninBoxId;
		}

		//ǩ������
		UInt8	signinDays;
		//ǩ������id
		UInt32	signinBoxId;
		//vipǩ������id
		UInt32	vipSigninBoxId;
	};

	/**
	 *@brief client->scene ������յ�½����
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
	 *@brief scene->client ���ؽ��յ�½����
	 */
	class SceneRoutineTodaySigninAwardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ROUTINE_TODAY_SIGNINAWARD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & signinDays & signinBoxId & vipSigninBoxId & boxGotFlag;
		}

		//ǩ������
		UInt8	signinDays;
		//ǩ�����ӽ���
		UInt32	signinBoxId;
		//vipǩ�����ӽ���
		UInt32	vipSigninBoxId;
		//������ȡ��־
		UInt32	boxGotFlag;
	};

	/**
	 *@brief world->client ֪ͨ�ͻ��˻��ʼ�����
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

		//1Ϊ��ʼ 0Ϊ���� 2Ϊ׼��
		UInt8		type;
		//�id
		UInt32		id;
		//���
		std::string	name;
		//�ȼ�
		UInt16		level;
		//׼��ʱ��
		UInt32		preTime;
		//��ʼʱ��
		UInt32		startTime;
		//��ֹʱ��
		UInt32		dueTime;
	};

	/**
	 *@brief world->scene ֪ͨ�������ʼ�����
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

		//1Ϊ��ʼ 0Ϊ����
		UInt8		type;
		//�id
		UInt32		id;
		//���
		std::string name;
		//�ȼ�
		UInt16		level;
		//׼��ʱ��
		UInt32		preTime;
		//��ʼʱ��
		UInt32		startTime;
		//��ֹʱ��
		UInt32		dueTime;
		//������䣬�����Ϊ0��ʾ����
		ObjID_t		tribeId;
		//�������
		ObjID_t		belongRole;
	};

	/**
	 *@brief world->client ͬ����б��ͻ���
	 */
	class WorldSyncClientActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_CLIENT_ACTIVITIES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities;
		}

		//��б�
		std::vector<ActivityInfo>		activities;
	};


	// ��������
	class WorldActivityDetailReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_DETAIL_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeName;
		}

		//�id
		std::string	activeName;
	};

	// �������鷵��
	class WorldActivityDetailRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_DETAIL_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & id & name & level & preTime & startTime & dueTime;
		}
		//������
		UInt32      code;
		//�id
		UInt32		id;
		//�����
		std::string name;
		//�ȼ�
		UInt16		level;
		//׼��ʱ��
		UInt32		preTime;
		//��ʼʱ��
		UInt32		startTime;
		//��ֹʱ��
		UInt32		dueTime;
	};
	
	// ������ȡ����
	class WorldActivityTakeRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TAKE_REWARD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//�����
		std::string	name;
	};
	
	// ��ȡ���������
	class WorldActivityTakeRewardRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACTIVITY_TAKE_REWARD_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//����ֵ
		UInt32 code;
	};

	/**
	 *@brief noted by aprilliu, 2016.04.27 world --> scene  world serverͬ�����Ϣ��scene server
	 */
	class WorldSyncSceneActivities : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_SCENE_ACTIVITIES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activities & tribeactivities;
		}

		//��б�
		std::vector<ActivityInfo>	activities;
		//�����б�
		std::map<ObjID_t, std::vector<ActivityInfo> >	tribeactivities;
	};


	/**
	*@brief server->client ÿ���״ε�¼֪ͨ
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
	*@brief scene->world->client ͬ�������
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
		//0��ʾ�̻���ֵ 1����������
		UInt8 type;
		UInt32 value;
	};

	/**
	*@brief client->wolrd ֪ͨ��������״̬��Ϣ
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
		//������������ 0�貶� 1������
		UInt8 type;
		//���ڱ�� 0�ر� 1��
		UInt8 flag;
	};

	/**
	*@brief scene->client ȫ��������Ч֪ͨ
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
		//��Ч���ͣ�0�貶� 1������
		UInt8 type;
	};
}

#endif
