#ifndef _CL_SKILL_PROTOCOL_H_
#define _CL_SKILL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSkillDefine.h"

namespace CLProtocol
{

	class SceneChangeSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_SKILLS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & changeSkills;
		}

	public:

		// ������������
		UInt8 configType;

		//�仯�ļ����б�
		std::vector<ChangeSkill> changeSkills;
	};

	class SceneChangeSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_SKILLS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};

	/**
	 *@brief ʩ��һ������
	 */
	class SceneUseSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillid & pos & target & timestamp;
		}
		
	public:
		//����id
		UInt16		skillid;
		//λ��
		CLPosition	pos;
		//objid
		ObjID_t		target;
		//ʱ���
		UInt32		timestamp;
	};

	/**
	 *@brief �ͷż��ܷ���                                                                     
	 */
	class SceneUseSkillRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_SKILL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId & timestamp & result;
		}
		//����id
		UInt32	skillId;
		//ʱ���
		UInt32	timestamp;
		//���  1�ɹ� 0ʧ��
		UInt8	result;
	};

	//���buff
	class SceneAddBuff : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_BUFF)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId;
		}
		//buffid
		UInt32	buffId;
	};

	//֪ͨɾ��buff
	class SceneNotifyRemoveBuff : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMOVE_BUFF_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId;
		}
		//buffid
		UInt32	buffId;
	};

	/**
	 *@brief client->scene ����
	 */
	class SceneReviveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REVIVE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & index;
		}

	public:
		//�������� 0Ϊԭ�ظ���  ...
		UInt32	type;
		//�����������Ŀǰ����ս�����ã�0 ��Ӫ����㣬1 �ľݵ�
		UInt8	index;
	};

	/**
	 *@brief server->client ֪ͨ����
	 */
	class SceneNotifyDeath : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_DEATH)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reviveCost;
		}

	public:
		//ԭ�ظ�������������������
		UInt32	reviveCost;
	};

	/**
	 *@brief client->server �л�pkģʽ
	 */
	class SceneSwitchPkMode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SWITCH_PKMODE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pkmode;
		}

	public:
		//pkģʽ
		UInt8	pkmode;
	};

	/** 
	 *@brief server->client ֪ͨѪ���ͷ����仯
	 */
	class SceneNotifyHpMpChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_HPMP_CHANGE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type & value;
		}

	public:
		//obj id
		ObjID_t	id;
		//���� 0ΪѪ�� 1Ϊ����
		UInt8	type;
		//ֵ�仯 ��Ϊ�� 1Ϊ��
		Int32	value;
	};

	/**
	*@brief client->server �����ʼ������
	*/
	class SceneInitSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INIT_SKILLS_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType;
		}
		// �������÷���
		UInt32 configType;
	};

	/**
	 *@brief server->client �����ʼ�����ܷ���
	 */
	class SceneInitSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INIT_SKILLS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// ����ֵ
		UInt32 result;
	};

	/**
	*@brief client->server �����Ƽ���������
	*/
	class SceneRecommendSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RECOMMEND_SKILLS_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType;
		}

	public:
		// �������÷���
		UInt32 configType;
	};

	/**
	*@brief server->client �����Ƽ��������÷���
	*/
	class SceneRecommendSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RECOMMEND_SKILLS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// ����ֵ
		UInt32 result;
	};

	/**
	*@brief server->client ���ܲ۽���
	*/
	class SceneSkillSlotUnlockNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SKILL_SLOT_UNLOCK_NOTIFY)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & slot;
		}

	public:
		// ������λ
		UInt32 slot;
	};

	/**
	*@brief client->server ���ü���ҳ
	*/
	class SceneSetSkillPageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SKILL_PAGE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page;
		}

	public:
		// �������÷���
		UInt32 configType;
		//���õļ���ҳ
		UInt8 page;
	};

	/**
	*@brief server->client ���ü���ҳ����
	*/
	class SceneSetSkillPageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SKILL_PAGE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page & result;
		}

	public:
		// �������÷���
		UInt32 configType;
		//���õļ���ҳ
		UInt8 page;
		// ����ֵ
		UInt32 result;
	};

	/**
	*@brief client->server ������ҳ
	*/
	class SceneBuySkillPageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_SKILL_PAGE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page;
		}

	public:
		UInt32 configType;
		//����ļ���ҳ
		UInt8 page;
	};

	/**
	*@brief server->client ������ҳ����
	*/
	class SceneBuySkillPageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_SKILL_PAGE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// ����ֵ
		UInt32 result;
	};

	
}

#endif
