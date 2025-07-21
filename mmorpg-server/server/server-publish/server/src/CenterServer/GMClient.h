#ifndef _GM_CLIENT_H_
#define _GM_CLIENT_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <set>

/**
 *@brief gm客户端
 */
class GMClient
{
public:
	GMClient();
	~GMClient();

public:
	/**
	 *@brief 设置获取id
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

	/**
	 *@brief 设置聊天监控的区列表
	 */
	void SetChatMonitorServers(const std::vector<UInt32>& servers);
	
	/**
	 *@brief 判断是否要监控聊天的区
	 */
	bool IsChatMonitorServer(UInt32 serverid) const{ return m_ChatMonitorServers.find(serverid) != m_ChatMonitorServers.end(); }

public: //消息相关
	/**
	 *@brief 发送消息
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//聊天监控客户端连接
	Avalon::NetConnectionPtr m_Conn;

	//关注的区列表
	std::set<UInt32>	m_ChatMonitorServers;
};

typedef CLVisitor<GMClient>	GMClientVisitor;

#endif
