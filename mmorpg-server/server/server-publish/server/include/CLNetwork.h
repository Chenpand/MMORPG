#ifndef _CL_NETWORK_H_
#define _CL_NETWORK_H_

#include <AvalonNetService.h>
#include <AvalonPropertySet.h>
#include <AvalonProtocol.h>
#include "CLDefine.h"
#include "CLPacketExecutor.h"

/**
 *@brief ��������
 */
enum NetworkType
{
	NT_INTERVAL,	//����
	NT_EXTERVAL,	//����
	NT_CONSOLE,		//����̨
	NT_BILLING,		//֧��
};

/**
 *@brief ��ȡ������
 */
inline const char* GetNetworkNameByType(int type)
{
	switch(type)
	{
	case NT_INTERVAL:	return "InternalNetwork";
	case NT_EXTERVAL:	return "ExternalNetwork";
	case NT_CONSOLE:	return "ConsoleNetwork";
	case NT_BILLING:	return "BillingNetwork";
	}
	return "";
}


class CLNetwork;

extern bool CL_SEND_PROTOCOL(Avalon::NetConnectionPtr Conn, Avalon::Protocol& Req);
/**
 *@brief ��������
 */
class CLNetConnection : public Avalon::NetConnection
{
public:
	explicit CLNetConnection(CLNetwork* network);
	~CLNetConnection();

	void OnDataReached();  //�� NetConnection::HandleInput�б����ã� reactor ��⵽�ɶ��¼�

	void OnTick(const Avalon::Time& now);

	CLNetwork* GetNetwork(){ return m_pNetwork; }

	bool SendProtocol(Avalon::Protocol& protocol);

private:
	//����
	CLNetwork *m_pNetwork;
};


/**
 *@brief ����
 */
class CLNetwork : public Avalon::NetService
{
public:
	CLNetwork();
	virtual ~CLNetwork();

public:
	/**
	 *@brief �������������
	 */
	bool Startup(Avalon::PropertySet* properties);

	/**
	 *@brief �ر��������
	 */
	void Shutdown(UInt32 inputTimeout);

	/**
	 *@brief ������������
	 */
	virtual NetworkType GetType() const = 0;

	/**
	 *@brief ��ȡ������
	 */
	virtual const std::string GetName() const;

	/**
	 *@brief ��ȡһ��ָ�����͵�����
	 */
	Avalon::NetConnectionPtr FindConnectionByType(ServerType type);

	/**
	 *@brief ��ȡһ��ָ�����͵������б�
	 */
	void FindConnectionByType(ServerType type, std::vector<Avalon::NetConnectionPtr>& connVec);

	/**
	 *@brief Ԥ������Ϣ
	 */
	virtual bool PreHandlePacket(CLNetConnection* conn,Avalon::Packet* packet);

	/**
	 *@brief �ַ���Ϣ
	 */
	virtual void DispatchPacket(Avalon::Packet* packet);

	/**
	 *@brief ��ȡ��Ϣ������                                                                     
	 */
	CLPacketExecutor& GetPacketExecutor() { return m_Executor; }

public://�¼�
	/**
	 *@brief ����һ����������
	 */
	Avalon::NetConnection* CreateConnection();

	/**
	 *@brief һ�����ӽ���(����֤)
	 */
	void OnAsynConnected(UInt32 id, Avalon::NetConnectionType type);

	/**
	 *@brief һ�����ӶϿ�
	 */
	void OnAsynDisconnected(UInt32 id, Avalon::NetConnectionType type);

	/**
	 *@brief һ�����ӽ���  �����֪ͨ�߼���  �ѽ��������� ����ķ��������������Ҫʵ�ָýӿ�
	 */
	virtual void OnConnected(UInt32 id)  = 0;

	/**
	 *@brief һ�����ӶϿ�
	 */
	virtual void OnDisconnected(UInt32 id) = 0;

	/**
	 *@brief tick�¼�
	 */
	virtual void OnTick(const Avalon::Time& now);

protected:
	/**
	 *@brief ע��Э��
	 */
	virtual void RegisterProtocols(){}

	/**
	 *@brief ��һ��Э��  Ҳ���ǰ�Э��id�ͶԸ�Э��Ĵ�������һ��
	 */
	void RegisterProtocol(UInt32 id,CLPacketFunction* func){ m_Executor.Bind(id,func); }

	/**
	 *@brief ִ��һ����Ϣ��
	 */
	virtual bool ExecutePacket(Avalon::Packet* packet);

private:
	//Ĭ�ϵ���Ϣ����  �ô���Ϣ����ʵ����һ����Ϣ�ɷ����ƣ��첽������OnTickʱ����ʵ�ʵ���Ϣ����
	Avalon::PacketQueue m_Queue;
	//��Ϣ��ִ����
	CLPacketExecutor	m_Executor;
};

#endif
