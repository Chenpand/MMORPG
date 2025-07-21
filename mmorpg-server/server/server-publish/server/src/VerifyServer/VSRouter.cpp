#include "VSRouter.h"

#include <CLSysProtocol.h>
#include "VSInternalNetwork.h"

namespace Router
{
	//ʹ��ȫ�ֱ������浽�����������ӣ�����ÿ�η�����ʱ��Ҫͨ��id�����ҵ���Ӧserver������
	Avalon::NetConnectionPtr g_VerifyCenterConn = NULL;				//��verify center���ڲ�����

	void OnConnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (servType == ST_VERIFY_CENTER)
		{
			g_VerifyCenterConn = VSInternalNetwork::Instance()->FindConnection(id);
		}
	}

	void OnDisconnected(UInt32 id)
	{
		UInt32 servType = CL_GET_SERVERTYPE_FROM_NODEID(id);
		if (servType == ST_VERIFY_CENTER)
		{
			g_VerifyCenterConn = NULL;
		}
	}

	void SendToVerifyCenter(Avalon::Packet* packet)
	{
		if (g_VerifyCenterConn != NULL && packet != NULL)
		{
			if (!g_VerifyCenterConn->SendPacket(packet))
			{
				ErrorLogStream << "send packet failed! packet(id:" << packet->GetID() << ", size:" << packet->GetSize() << ")" << LogStream::eos;
			}
		}
	}

	void SendToVerifyCenter(Avalon::Protocol& protocol)
	{
		if (g_VerifyCenterConn != NULL)
		{
			CL_SEND_PROTOCOL(g_VerifyCenterConn, protocol);
		}
	}

};