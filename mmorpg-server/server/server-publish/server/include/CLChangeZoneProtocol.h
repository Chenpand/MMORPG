#ifndef _CL_CHANGEZONE_PROTOCOL_H_
#define _CL_CHANGEZONE_PROTOCOL_H_

#include <AvalonSocket.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	 *@brief zone->gate ����һ����������ͬ��������Ϣ                                                                     
	 */
	class GateChangeZoneCheckReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_CHECK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcZoneId & accId & playerId & name & mapId & srcGateId & bReturn;
			stream & gmAuthority  & openid & qqvipinfo & source1 & source2;
			stream & pf & level;
		}

	public:
		//ԭ��
		UInt32		srcZoneId;
		//�˺�id
		UInt32		accId;
		//���id
		ObjID_t		playerId;
		//�����
		std::string	name;
		//Ŀ���ͼ
		UInt32		mapId;
		//Դgateid
		UInt32		srcGateId;
		
		//�Ƿ��Ƿ���
		UInt8		bReturn;
		//gmȨ��
		UInt8		gmAuthority;
		//openid
		std::string	openid;
		//qq��Ȩ��Ϣ
		QQVipInfo	qqvipinfo;
		//��Դ1
		std::string	source1;
		//��Դ2
		std::string	source2;
		//pf
		std::string	pf;
		//�ȼ�
		UInt16		level;
	};

	/**
	 *@brief gate->zone                                                                     
	 */
	class GateChangeZoneCheckRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_CHECK_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & zoneId & mapId & gateAddr & srcGateId & result;
			stream.SeriaArray(key, 20);
		}

		//���id
		ObjID_t playerId;
		//Ŀ����id
		UInt32	zoneId;
		//Ŀ���ͼid
		UInt32	mapId;
		//Ŀ����gate��ַ
		Avalon::SockAddr gateAddr;
		//ԭgateid
		UInt32	srcGateId;
		//���
		UInt32	result;
		//����
		UInt8	key[20];
	};

	/**
	 *@brief scene->gate ֪ͨ���ؿ�ʼ���                                                                     
	 */
	class GateChangeZoneNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CHANGEZONE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & dstMapId;
		}

		//���id
		ObjID_t	playerId;
		//Ŀ����id
		UInt32	dstZoneId;
		//Ŀ������ͼ
		UInt32	dstMapId;
	};

	/**
	 *@brief scene->world ֪ͨworld���                                                                     
	 */
	class WorldChangeZoneNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGEZONE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & attr;
			stream.SeriaArray(key, 20);
		}

		ObjID_t		playerId;
		UInt32		dstZoneId;
		Avalon::SockAddr	attr;
		UInt8		key[20];
	};

	/**
	 *@brief world->client ֪ͨ�ͻ��˿��                                                                     
	 */
	class WorldChangeZoneNotifyClient : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHANGEZONE_NOTIFY_CLIENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & dstZoneId & attr;
			stream.SeriaArray(key, 20);
		}

		ObjID_t	playerId;
		UInt32	dstZoneId;
		Avalon::SockAddr	attr;
		UInt8	key[20];
	};

	/**
	*@brief client->server ���󷵻�ԭ��
	*/
	class SceneChangeZoneReturn : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGEZONE_RETURN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
}

#endif
