#ifndef _CL_ENTERGAME_PROTOCOL_H_
#define _CL_ENTERGAME_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLErrorCode.h>
#include <CLGameDefine.h>
#include <CLDungeonDefine.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	struct RoleInfo  //noted by aprilliu ���ӽṹ��Ҫʵ���Լ������л�  ���� NetOutputStream& operator&(T& value) �Լ� NetInputStream& operator&(T& value)�ĵ���
	{
		RoleInfo() :roleId(0), sex(0), occu(0), level(0), offlineTime(0), deleteTime(0), isAppointmentOccu(0), isVeteranReturn(0),addPreferencesTime(0),delPreferencesTime(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & strRoleId & name & sex & occu & level & offlineTime & deleteTime & avatar & newBoot & preOccu & isAppointmentOccu & isVeteranReturn & playerLabelInfo & addPreferencesTime & delPreferencesTime;
		}

		//��ɫid
		ObjID_t	roleId;
		//�ַ�����ɫid
		std::string	strRoleId;
		//��ɫ��
		std::string name;
		//�Ա�
		UInt8 sex;
		//ְҵ
		UInt8 occu;
		//�ȼ�
		UInt16 level;
		//����ʱ��
		UInt32 offlineTime;
		//ɾ��ʣ��ʱ��
		UInt32 deleteTime; 
		//���
		PlayerAvatar avatar;
		//��������
		UInt32 newBoot;
		//Ԥתְ
		UInt8 preOccu;
		//�Ƿ�ԤԼ��ɫ
		UInt8 isAppointmentOccu;
		//�Ƿ��ϱ��ع�
		UInt8 isVeteranReturn;
		//��ұ�ǩ��Ϣ
		PlayerLabelInfo playerLabelInfo;
		//����ƫ��ʱ��
		UInt32 addPreferencesTime;
		//ȡ��ƫ��ʱ��
		UInt32 delPreferencesTime;
	};

	/**
	 *@brief gate->client ���ͽ�ɫ������Ϣ
	 */
	class GateSendRoleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_ROLEINFO)
	public:
		GateSendRoleInfo() : appointmentRoleNum(0), baseRoleField(0), extensibleRoleField(0), unlockedExtensibleRoleField(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roles & appointmentOccus & appointmentRoleNum & baseRoleField & extensibleRoleField 
				& unlockedExtensibleRoleField;
		}

		//��ɫ�б�
		std::vector<RoleInfo>	roles;
		//��ԤԼְҵ
		std::vector<UInt8>		appointmentOccus;
		//ԤԼ��ɫ����
		UInt32					appointmentRoleNum;
		//��ɫ������λ
		UInt32					baseRoleField;
		//����չ��ɫ��λ
		UInt32					extensibleRoleField;
		//����չ��ɫ������λ
		UInt32					unlockedExtensibleRoleField;
	};

	/**
	 *@brief client->gate ���󴴽���ɫ
	 */
	class GateCreateRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CREATEROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & sex & occu  & newer;
		}

		//����
		std::string name;
		//�Ա�
		UInt8 sex;
		//ְҵ
		UInt8 occu;
		//��ұ�ʶ��1 ����ң�0 �����, 2 ������
		UInt8 newer;
	};

	/**
	 *@brief gate->client ���ش������
	 */
	class GateCreateRoleRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CREATEROLE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//���
		UInt32 result;
	};

	/**
	 *@brief client->gate ����ɾ����ɫ
	 */
	class GateDeleteRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_DELETEROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & deviceId;
		}

		//��ɫid
		ObjID_t	roleId;
		//�豸Id
		std::string deviceId;
	};

	/**
	 *@brief client->gate ����ָ���ɫ
	 */
	class GateRecoverRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECOVERROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		//��ɫid
		ObjID_t	roleId;
	};

	/**
	 *@brief client->gate ���������Ϸ
	 */
	class GateEnterGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ENTERGAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & option & city & inviter;
		}

		//��ɫid
		ObjID_t roleId;
		//ѡ��
		UInt8	option;
		//����
		std::string	city;
		//������
		UInt32	inviter;
	};

	/**
	 *@brief gate->client ������Ϸ���
	 */
	class GateEnterGameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ENTERGAME_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//���
		UInt32 result;
	};

	/**
	*@brief client->gate ��������
	*/
	class GateReconnectGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECONNECT_GAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & roleId & sequence & session;
		}
		
		UInt32 accid;
		//��ɫid
		ObjID_t roleId;
		//��Ϣ���
		UInt32 sequence;
		//session
		std::vector<UInt8> session;
	};
	
	/**
	*@brief gate->client ��������
	*/
	class GateReconnectGameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECONNECT_GAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & lastRecvSequence;
		}

		UInt32 result;
		UInt32 lastRecvSequence;
	};

	/**
	*@brief client->gate �����������
	*/
	class GateFinishNewbeeGuide : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_FINISH_NEWBEE_GUIDE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & id;
		}

		ObjID_t roleId;
		UInt32 id;
	};

	/**
	*@brief gate->client �ָ���ɫ����
	*/
	class GateRecoverRoleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECOVERROLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & roleUpdateLimit;
		}

		ObjID_t		roleId;
		UInt32		result;
		std::string roleUpdateLimit;
	};

	/**
	*@brief gate->client ɾ����ɫ����
	*/
	class GateDeleteRoleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_DELETEROLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & roleUpdateLimit;
		}

		ObjID_t		roleId;
		UInt32		result;
		std::string roleUpdateLimit;
	};

	/**
	*@brief gate->client ɾ����ɫ����
	*/
	class GateNotifyLoginWaitInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_LOGIN_WAIT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & waitPlayerNum;
		}

		UInt32 waitPlayerNum;
	};

	/**
	*@brief gate->client ֪ͨ��ҿ��Ե�¼��
	*/
	class GateNotifyAllowLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_ALLOW_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->gate ����˳��Ŷ�
	*/
	class GateLeaveLoginQueue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_LEAVE_LOGIN_QUEUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client ͬ�����ϵͳ����
	*/
	class GateNotifySysSwitch : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_SYSTEM_SWITCH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cfgs;
		}

		std::vector<SysSwitchCfg> cfgs;
	};
	/**
	 *@brief Client->gate ���ý�ɫƫ��
	 */
	class GateSetRolePreferencesReq :public Avalon::Protocol {
		AVALON_DEFINE_PROTOCOLID(GATE_SET_ROLE_PREFERENCES_REQ)
	public:
		GateSetRolePreferencesReq() :roleId(0), preferencesFlag(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & preferencesFlag;
		}
		ObjID_t roleId;
		//ö��RolePreferences
		UInt8 preferencesFlag;
	};
	/**
	 * @brief Client->gate ���ý�ɫƫ�÷���
	 */
	class GateSetRolePreferencesRet :public Avalon::Protocol {
		AVALON_DEFINE_PROTOCOLID(GATE_SET_ROLE_PREFERENCES_RET)
	public:
		GateSetRolePreferencesRet() :roleId(0),result(0),addPreferencesTime(0),delPreferencesTime(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & addPreferencesTime & delPreferencesTime;
		}
		ObjID_t roleId;
		UInt32 result;
		UInt32 addPreferencesTime;
		UInt32 delPreferencesTime;

	};
	
	/** 
	 *@brief gate->client ͬ��������ʱ��
	 */
	class GateSyncServerTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_SERVER_TIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time;
		}

		UInt32	time;
	};

	/**
	 *@brief gate->world->scene ֪ͨ������ҽ�����Ϸ
	 */
	class SceneEnterGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERGAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & accid & name & srcZoneId & gmauthority & clientOption & openid & city
				& qqVipInfo & ip & source1 & source2 & inviter & pf & did & mapid & sceneid & pos 
				& gatenode & isPhoneBind & bornZoneId & antiAddictInfo & model & deviceVersion
				& roleReturnTime & roleReturnLevel & isAgainReturn;
		}

		//���guid
		ObjID_t			id;
		//�˺�id
		UInt32			accid;
		//�����
		std::string		name;
		//Դ��id
		UInt32			srcZoneId;
		//gmȨ��
		UInt8			gmauthority;
		//�ͻ���ѡ��
		UInt8			clientOption;
		//openid
		std::string		openid;
		//����
		std::string		city;
		//qq��Ȩ��Ϣ
		QQVipInfo		qqVipInfo;
		//ip��ַ
		std::string		ip;
		//��Դ1
		std::string		source1;
		//��Դ2
		std::string		source2;
		//������
		UInt32			inviter;
		//ƽ̨
		std::string		pf;
		//�豸id
		std::string		did;

		//�����ͼid
		UInt32			mapid;
		//���볡��id
		UInt32			sceneid;
		//����λ��
		CLPosition		pos;
		//��������
		UInt32			gatenode;
		//�Ƿ���ֻ�
		UInt8			isPhoneBind;

		//ԭ��ID
		UInt32			bornZoneId;
		//��������Ϣ
		AntiAddictInfo	antiAddictInfo;

		//�豸�ͺ�
		std::string		model;
		//�豸�汾
		std::string		deviceVersion;

		//��ɫ�ع�ʱ��
		UInt32			roleReturnTime;
		//��ɫ�ع�ȼ�
		UInt32			roleReturnLevel;
		// �Ƿ��ٴλع�
		UInt8			isAgainReturn;
	};

	/**
	 *@brief scene->gate ��ҽ�����Ϸ����
	 */
	class SceneEnterGameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERGAME_RET)
	public:
		SceneEnterGameRet():id(0),result(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & result;
		}

		//���guid
		ObjID_t			id;
		//���
		UInt32			result;
	};

	/**
	 *@brief scene->world ����֪ͨworldע�����
	 */
	class WorldRegisterPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & accid & name & zoneid & srcZoneId & occu & sex & headFrame & viplvl & qqVipInfo & level & power & point & createtime & offlinetime & localOfflineTime & gateid & openid & city & ip & pf & did & model & deviceVersion & auctionRefreshTime & auctionAdditionBooth & monthCardExpireTime & bornZoneId & roleReturnTime;

		}

		//id
		ObjID_t		id;
		//�˺�id
		UInt32		accid;
		//����
		std::string	name;
		//��id
		UInt16		zoneid;
		//ԭ��id
		UInt32		srcZoneId;
		//ְҵ
		UInt8		occu;
		//�Ա�
		UInt8		sex;
		// ͷ���
		UInt32		headFrame;
		//vip�ȼ�
		UInt8		viplvl;
		//qq��Ȩ��Ϣ
		QQVipInfo	qqVipInfo;
		//�ȼ�
		UInt16		level;
		//ս����
		UInt32		power;
		//�ㄻ
		UInt32		point;
		//����ʱ��
		UInt32		createtime;
		//�ϴ�����ʱ��
		UInt64		offlinetime;
		//�����ϴ�����ʱ��
		UInt64		localOfflineTime;
		//����id
		UInt32		gateid;
		//openid
		std::string openid;
		//����
		std::string	city;
		//ip
		std::string	ip;
		//ƽ̨
		std::string	pf;

		//�豸ID
		std::string did;
		//�豸�ͺ�
		std::string model;
		//�豸�汾
		std::string deviceVersion;

		//������ˢ��ʱ��
		UInt32	auctionRefreshTime;
		//���⹺�����������λ
		UInt32	auctionAdditionBooth;
		//�¿�����ʱ��
		UInt32		monthCardExpireTime;
		//������ID
		UInt32		bornZoneId;

		//��ɫ�ع�ʱ��
		UInt32 roleReturnTime;
	};

	/**
	 *@brief world->scene worldע����ҽ��
	 */
	class WorldRegisterPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & result;
		}

		//id
		ObjID_t		id;
		//���
		UInt32		result;
	};

	/**
	*@brief world->gate world֪ͨgate��ҿ��Ե�¼��
	*/
	class WorldAllowPlayerLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ALLOW_PLAYER_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//accid
		UInt32		accid;
	};

	/**
	*@brief world->gate world֪ͨgate��ҿ��Ե�¼��
	*/
	class WorldSyncPlayerLoginWaitInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_LOGIN_WAIT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & waitPlayerNum;
		}

		//accid
		UInt32		accid;
		//��Ҫ�ȴ��������
		UInt32		waitPlayerNum;
	};

	/**
	*@brief gate->world ֪ͨ����˳��Ŷ�
	*/
	class WorldPlayerLeaveLoginQueue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_LEAVE_LOGIN_QUEUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//accid
		UInt32		accid;
	};
	

	/**
	 *@brief gate->world ��֤Ψһ�Ե�¼
	 */
	class WorldVerifyUniqueReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_VERIFY_UNIQUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & checkWaitQueue;
		}

		//�˺�id
		UInt32	accid;		
		//�Ƿ��ж��Ŷ�
		UInt8	checkWaitQueue;
	};

	/**
	 *@brief world->gate ��֤Ψһ�Ե�¼����
	 */
	class WorldVerifyUniqueRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_VERIFY_UNIQUE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & needWait & waitPlayerNum & serverStartTime & infos;
		}

		//�˺�id
		UInt32	accid;
		//�Ƿ���Ҫ�ȴ�
		UInt8	needWait;
		//ǰ��������
		UInt32	waitPlayerNum;
		//����ʱ��
		UInt32  serverStartTime;
		//��¼������Ϣ
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief scene->world ��ѯ�˺���Ϣ����
	*/
	class SceneQueryAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//�˺�id
		UInt32	accid;
	};
	
	/**
	*@brief world->scene ��ѯ�˺���Ϣ����
	*/
	class WorldQueryAccountRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & accountGuid & accountId & point & creditPoint & vipLevel & vipExp
				& totalChargeNum & monthCardExpireTime & dungeonHardInfoes 
				& storageSize & moneyManageStatus & weaponLeaseTickets
				& adventureTeamLevel & adventureTeamGrade & accountAchievementScore & mallPoint & adventureCoin;
		}

		//���
		UInt32		result;
		//�˺ű�guid
		ObjID_t		accountGuid;
		//�˺�id
		UInt32		accountId;
		//�ㄻ
		UInt32		point;
		//���õㄻ
		UInt32		creditPoint;
		//vip�ȼ�
		UInt8		vipLevel;
		//vip����
		UInt32		vipExp;
		//�ܳ�ֵ���
		UInt32		totalChargeNum;
		//�¿�����ʱ��
		UInt32		monthCardExpireTime;
		// ���³��Ѷ���Ϣ
		std::vector<SceneDungeonHardInfo> dungeonHardInfoes;
		//�ֿ������
		UInt32		storageSize;

		//��ƹ���״̬
		UInt8		moneyManageStatus;
		// �������޺��˷�
		UInt32		weaponLeaseTickets;
		// ð�նӵȼ�
		UInt16		adventureTeamLevel;
		// ð�ն�����
		UInt8		adventureTeamGrade;
		// �˺ųɾ͵�
		UInt32		accountAchievementScore;
		// �̳ǻ���
		UInt32      mallPoint;
		//ð�ձ�
		UInt32      adventureCoin;
	};

	/**
	 *@brief client->server ͬ������������ϵ��
	 */
	class SceneSyncWallowFactor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_WALLOW_FACTOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & factor;
		}

		//ϵ��
		UInt32	factor;
	};

	/**
	*@brief client->server ��¼���°汾��
	*/
	class SceneUpdateBulletinRecord : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_BULLETIN_RECORD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & updateVersion;
		}

	public:
		//���°汾��
		UInt32 updateVersion;
	};

    
    /**
    *@brief world server->scene server ��������
    */
    class SceneGameReconnectReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_GAME_RECONN_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId & accid & gatenode;
        }

    public:
        //��ɫID
        ObjID_t         roleId;

        //�˺�ID        
        UInt32          accid;

        //��������
        UInt32			gatenode;
    };

    /**
    *@brief scene server->world server ��������
    */
    class SceneGameReconnectRet : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_GAME_RECONN_RET)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & roleId & result;
        }

    public:
        ObjID_t     roleId;
        UInt32      result;
	};

	/**
	*@brief client->gate �����ɫ�л�
	*/
	class RoleSwitchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ROLE_SWITCH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client �����ɫ�л�����
	*/
	class RoleSwitchRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ROLE_SWITCH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & hasrole & waitPlayerNum & serverStartTime;
		}

		//���
		UInt32	result;
		//��ľ�н�ɫ 1�� 0ľ��
		UInt8	hasrole;
		//��Ҫ�ȴ��������
		UInt32	waitPlayerNum;
		//����������ʱ��
		UInt32	serverStartTime;
	};

	/**
	*@brief gate->client ���͵�¼������Ϣ
	*/
	class GateSendLoginPushInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_LOGIN_PUSH_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}

		// ��¼������Ϣ
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief client->gate ����ת���˺���Ϣ
	*/
	class GateConvertAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CONVERT_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client ����ת���˺���Ϣ
	*/
	class GateConvertAccountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CONVERT_ACCOUNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & account & passwd & saveFile & screenShot;
		}

		std::string		account;
		std::string		passwd;
		// �Ƿ񱣴��ļ�
		UInt8			saveFile;
		// �Ƿ��ͼ
		UInt8			screenShot;
	};
	
	/**
	*@brief world->client ͬ����ҵ�½״̬
	*/
	class SyncPlayerLoginStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_LOGIN_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerLoginStatus;
		}

		// ��ҵ�¼״̬
		UInt8 playerLoginStatus;
	};

	/**
	*@brief gate->world ��ȡ��¼������Ϣ����
	*/
	class WorldGetLoginPushInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_LOGIN_PUSH_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid;
		}

		//��ɫid
		UInt64	roleid;
	};
	
	/**
	*@brief world->gate ��ȡ��¼������Ϣ����
	*/
	class WorldGetLoginPushInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_LOGIN_PUSH_INFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & infos;
		}

		//��ɫid
		UInt64	roleid;
		// ��¼������Ϣ
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief client->gate��������ѡ������
	*/
	class GateNoviceGuideChooseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOVICE_GUIDE_CHOOSE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & chooseFlag;
		}

	public:
		// ��ɫid
		ObjID_t roleid;
		// ѡ���־(NoviceGuideChooseFlag)
		UInt8 chooseFlag;
	};
}

#endif
