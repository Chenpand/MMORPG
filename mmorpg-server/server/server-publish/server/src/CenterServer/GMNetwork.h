#ifndef _GM_NETWORK_H_
#define _GM_NETWORK_H_

#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <AvalonIDGenerator.h>
#include <CLExternalNetwork.h>

/**
 *@brief ��������
 */
class GMConnection : public CLExternalConnection
{
public:
	GMConnection(CLNetwork* network);
	~GMConnection();

public:
	bool PreHandlePacket(Avalon::Packet* packet);

	virtual void OnTick(const Avalon::Time& now);

private:
};

/**
 *@brief ����
 */
class GMNetwork : public CLExternalNetwork, public Avalon::Singleton<GMNetwork>
{
public:
	GMNetwork();
	~GMNetwork();

public:
	/**
	 *@brief ����
	 */
	bool Startup(Avalon::PropertySet* properties,CLNetAddress* addresses = NULL);

	/**
	 *@brief ��ȡ����
	 */
	const std::string& GetPasswd() const{ return m_Passwd; }

	/**
	 *@brief ����һ��id
	 */
	UInt32 GenID(){ return m_IDGen.Generate(); }

	void RegisterProtocols();

	/**
	 *@brief ����һ������
	 */
	Avalon::NetConnection* CreateConnection();

public://�¼�
	/**
	 *@brief һ�����ӽ���
	 */
	virtual void OnConnected(UInt32 id);

	/**
	 *@brief һ�����ӶϿ�
	 */
	virtual void OnDisconnected(UInt32 id);

private:
	/**
	 *@brief ��½����
	 */
	std::string m_Passwd;
	/**
	 *@brief id������
	 */
	Avalon::IDGenerator m_IDGen;
};

#endif
