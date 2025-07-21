#ifndef _CL_DATACENTER_PROTOCOL_H_
#define _CL_DATACENTER_PROTOCOL_H_

#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server ����udp��־����������
	*/
	class SceneDataCenterUdpLog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DATACENTER_UDPLOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & str1 & str2 & str3 & str4 & str5 & str6 & strType;
		}
	public:
		std::string	str1;
		std::string	str2;
		std::string	str3;
		std::string	str4;
		std::string	str5;
		std::string	str6;
		std::string	strType;
	};

	/**
	*@brief client->scene �ϱ��Զ�����־
	*/
	class SceneCustomLogReport : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CUSTOM_LOG_REPORT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & param;
		}
	public:
		UInt8 type;
		std::string param;
	};
}

#endif

