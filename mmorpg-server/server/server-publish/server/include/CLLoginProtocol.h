#ifndef _CL_LOGIN_PROTOCOL_H_
#define _CL_LOGIN_PROTOCOL_H_

#include <AvalonSocket.h>
#include <CLProtocolDefine.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	/*
		noted by aprilliu, ����Э�鶼Ҫ�̳���Avalon::Protocol���������Լ���Э��id���Լ�ʵ���Լ���serialize������encode�Լ�decode, 
		NetInputStream�Լ�NetOutputStream���ص�&�����������decode�Լ�encode������
	*/
	/**
	 *@brief client->admin ������֤�˺�
	 */
	class AdminLoginVerifyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_VERIFY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & version & source1 & append1 & source2 & append2;
			stream.SeriaArray(tableMd5, 16);
			stream & svnVersion;
			stream.SeriaArray(append3, 12);
			stream & params;
			stream.SeriaArray(hashValue,20);
		}
	
		// �汾��
		UInt32      version;
		//��Դ1
		std::string	source1;
		// ��Ч�ֶ�
		UInt32		append1;
		//��Դ2
		std::string	source2;
		// ��Ч�ֶ�
		UInt8		append2;
		//���md5
		UInt8		tableMd5[16];
		//svn�汾��
		UInt32		svnVersion;
		// ��Ч�ֶ�
		UInt8		append3[12];
		//��½��Ϣ
		std::string	params;
		//hash
		UInt8 hashValue[20];
	};

	/**
	 *@brief admin->client ������֤�˺Ž��
	 */
	class AdminLoginVerifyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_VERIFY_RET)
	public:
		AdminLoginVerifyRet()
		{
			phoneBindRoleId = 0;
			accid = 0;
			result = 0;
			encryptProtocol = 0;
			openBugly = 0;
			voiceFlag = 0;
			openNewReconnectAlgo = 0;
			openNewReportFrameAlgo = 0;
			zoneId = ZONE_ID;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & phoneBindRoleId & errMsg & accid & dirSig & addr & result & replayAgentAddr & encryptProtocol & 
				openBugly & voiceFlag & activityYearSortListUrl & openNewReconnectAlgo & openNewReportFrameAlgo & webActivityUrl
				& commentServerAddr & zoneId & redPacketRankUrl & convertUrl & reportUrl & battleUseTcp & writeQuestionnaireUrl;
		}

		
		//�ֻ���Ľ�ɫID
		ObjID_t				phoneBindRoleId;
		//�����ַ���
		std::string			errMsg;
		//�˺�id
		UInt32				accid;
		//Ŀ¼������У��ǩ��
		std::string			dirSig;
		//�����ַ
		Avalon::SockAddr	addr;
		//���
		UInt32				result;
		//¼���������ַ
		std::string			replayAgentAddr;
		//�Ƿ�Э�����
		UInt8				encryptProtocol;
		//�Ƿ���bugly
		UInt8				openBugly;
		//������ʶ
		UInt32				voiceFlag;
		// ����а�URL
		std::string			activityYearSortListUrl;
		// �Ƿ����µ���������
		UInt8				openNewReconnectAlgo;
		// �Ƿ����µķ���֡�㷨
		UInt8				openNewReportFrameAlgo;
		// ��ҳ�����
		std::string			webActivityUrl;
		// װ��ͼ�����۷���ַ
		std::string			commentServerAddr;
		// ��id
		UInt32				zoneId;
		// �����������
		std::string			redPacketRankUrl;
		// ת����Ϣ���ӵ�ַ
		std::string			convertUrl;
		// �ٱ�������
		std::string			reportUrl;
		// ս���Ƿ�ʹ��tcp
		UInt8				battleUseTcp;
		// ��д�ʾ�
		std::string			writeQuestionnaireUrl;
	};

	/**
	 *@brief admin->admin �����߳�֪ͨ�߼��߳��µ���֤����
	 */
	class AdminLoginNewPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_NEWPLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
			stream & gmauthority;
			stream.SeriaArray(hash, 20);
			stream & openid & qqVipInfo & source1 & source2 & pf & did & phoneBindRoleId & antiAddictInfo & model & deviceVersion;
		}

	public:
		//�˺�id
		UInt32	accid;
		//gmȨ��
		UInt8	gmauthority;
		//hashValue
		UInt8	hash[20];
		//openid
		std::string	openid;
		//qq��Ȩ��Ϣ
		QQVipInfo	qqVipInfo;
		//��Դ1
		std::string	source1;
		//��Դ2
		std::string	source2;
		//ƽ̨
		std::string	pf;
		//�豸id
		std::string did;
		//�ֻ��󶨵Ľ�ɫ
		ObjID_t		phoneBindRoleId;
		//��������Ϣ
		AntiAddictInfo antiAddictInfo;
		//�豸�ͺ�
		std::string model;
		//�豸�汾
		std::string deviceVersion;
	};

	/**
	*@brief admin->world ��ѯ�Ѿ��������ҵ�gateserver
	*/
	class AdminPlayerGateInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_PLAYER_GATE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		// ��¼��Ϣ
		AdminLoginNewPlayer info;
	};

	/**
	*@brief world->admin �����Ѿ��������ҵ�gateserver
	*/
	class AdminPlayerGateInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_LOGIN_PLAYER_GATE_INFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & errMsg & gateId & info;
		}

		// ���
		UInt32 result;
		// �ַ���
		std::string errMsg;
		//�ѷ����gateserver
		UInt32	gateId;
		// ��¼��Ϣ
		AdminLoginNewPlayer info;
	};
	

	/**
	 *@brief admin->gate ����½�����͵����ʵ�����
	 */
	class GateSendAccountInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_ACCOUNT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & gmauthority;
			stream.SeriaArray(hash, 20);
			stream & openid & qqVipInfo & source1 & source2 & pf & did & phoneBindRoleId & antiAddictInfo & model & deviceVersion;
		}

		//�˺�id
		UInt32	accid;
		//gmȨ��
		UInt8	gmauthority;
		//hashValue
		UInt8	hash[20];
		//openid
		std::string	openid;
		//qq��Ȩ��Ϣ
		QQVipInfo	qqVipInfo;
		//��Դ1
		std::string	source1;
		//��Դ2
		std::string	source2;
		//ƽ̨
		std::string	pf;
		//�豸id
		std::string did;
		//�ֻ��󶨽�ɫID
		ObjID_t		phoneBindRoleId;
		//��������Ϣ
		AntiAddictInfo antiAddictInfo;
		//�豸�ͺ�
		std::string model;
		//�豸�汾
		std::string deviceVersion;
	};

	/**
	 *@brief gate->admin ���ط��ص�¼��ַ
	 */
	class GateNetAddressRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NETADDRESS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & errMsg & accid & openid;
			if(result == 0)
			{
				stream & addr & phoneBindRoleId & encryptProtocol & pf;
			}
		}

		//���
		UInt32				result;
		// ������
		std::string			errMsg;
		//�˺�id
		UInt32				accid;
		//openid
		std::string			openid;
		//�����ַ
		Avalon::SockAddr	addr;
		//�ֻ���Ľ�ɫID
		ObjID_t				phoneBindRoleId;
		//�Ƿ�Э�����
		UInt8				encryptProtocol;
		//ƽ̨
		std::string			pf;
	};

	/**
	 *@brief client->gate �������ӵ�����
	 */
	class GateClientLoginReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CLIENTLOGIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId;
			stream.SeriaArray(hashValue, 20);
		}

		//��½��Ϣ
		UInt32	accId;
		//hash
		UInt8 hashValue[20];
	};

	/**
	 *@brief gate->client ���ط������ӽ��
	 */
	class GateClientLoginRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CLIENTLOGIN_RET)
	public:
		GateClientLoginRet() : notifyVeteranReturn(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & hasrole & waitPlayerNum & serverStartTime & notifyVeteranReturn;
		}

		//���
		UInt32	result;
		//��ľ�н�ɫ 1�� 0ľ��
		UInt8	hasrole;
		//��Ҫ�ȴ��������
		UInt32	waitPlayerNum;
		//����������ʱ��
		UInt32	serverStartTime;
		//֪ͨ�ϱ��ع�
		UInt8   notifyVeteranReturn;
	};

	/**
	 *@brief gate->admin ������adminͬ����½����
	 */
	class AdminSyncGatePlayerNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_GATEPLAYER_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & maxPlayerNum;
		}

		//����
		UInt32	playerNum;
		//�������
		UInt32	maxPlayerNum;
	};

	/**
	*@brief world->admin world��adminͬ����½����
	*/
	class AdminSyncWorldPlayerNum : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_WORLDPLAYER_NUM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerNum & maxPlayerNum;
		}

		//����
		UInt32	playerNum;
		//�������
		UInt32	maxPlayerNum;
	};

	/**
	*@brief gate->admin gate��adminͬ���Ŷ���Ϣ
	*/
	class AdminSyncLoginQueueInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_SYNC_LOGIN_QUEUE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inQueuePlayerNum;
		}

		//�Ŷ��е�����
		UInt32	inQueuePlayerNum;
	};
}

#endif
