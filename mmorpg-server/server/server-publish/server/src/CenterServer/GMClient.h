#ifndef _GM_CLIENT_H_
#define _GM_CLIENT_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <set>

/**
 *@brief gm�ͻ���
 */
class GMClient
{
public:
	GMClient();
	~GMClient();

public:
	/**
	 *@brief ���û�ȡid
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

	/**
	 *@brief ���������ص����б�
	 */
	void SetChatMonitorServers(const std::vector<UInt32>& servers);
	
	/**
	 *@brief �ж��Ƿ�Ҫ����������
	 */
	bool IsChatMonitorServer(UInt32 serverid) const{ return m_ChatMonitorServers.find(serverid) != m_ChatMonitorServers.end(); }

public: //��Ϣ���
	/**
	 *@brief ������Ϣ
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//�����ؿͻ�������
	Avalon::NetConnectionPtr m_Conn;

	//��ע�����б�
	std::set<UInt32>	m_ChatMonitorServers;
};

typedef CLVisitor<GMClient>	GMClientVisitor;

#endif
