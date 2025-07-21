#ifndef _AS_NETWORK_H_
#define _AS_NETWORK_H_

#include <AvalonSingleton.h>
#include "CLInternalNetwork.h"
#include <CLProtocolDefine.h>

/**
 *@brief ����
 */
class ASInternalNetwork : public CLInternalNetwork, public Avalon::Singleton<ASInternalNetwork>
{
public:
	ASInternalNetwork();
	~ASInternalNetwork();

public:
	bool Startup(Avalon::PropertySet *properties, CLNetAddress *addresses);

public:
	void RegisterProtocols();

	/**
	 *@brief ִ��һ����Ϣ��
	 */
	bool ExecutePacket(Avalon::Packet* packet);

	bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);


	/**
	 *@brief ��ȡһ��ָ�����������ͺ͹��ܵ�����
	 */
	Avalon::NetConnectionPtr FindUnionConnectionByFun(UnionServerFunctionType funType);

public://�¼�
	/**
	 *@brief һ�����ӽ���
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief һ�����ӶϿ�
	 */
	virtual void OnDisconnected(UInt32 id);

	/**
	 *@brief ֹͣ����
	 */
	virtual void OnShutdownService();

	/**
	 *@brief �ӵ�UnionServer֪ͨ���ѿ����Ĺ���
	 */
	void OnUnionServerNotifyOpenFunction(UInt32 connID, std::vector<UInt32> funVec);

private:
	//�Ƿ�����center
	bool m_bCenterConnected;
	//���ܶ�Ӧ��UnionServer����id
	std::map<UnionServerFunctionType, UInt32> m_UnionFunctionConnID;
	UInt64 m_addID = 0;
};

#endif
