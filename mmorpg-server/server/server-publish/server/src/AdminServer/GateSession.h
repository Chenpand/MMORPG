#ifndef _GATESESSION_H_
#define _GATESESSION_H_

#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonTime.h>
#include <CLDefine.h>

/**
 *@brief 网关会话
 */
class GateSession
{
public:
	GateSession();
	~GateSession();

public:
	/**
	 *@brief 初始化
	 */
	bool Init(UInt32 connid);
	/**
	 *@brief 获取节点id
	 */
	UInt32 GetID() const { return m_Id; }

	/**
	 *@brief 设置获取在线玩家数
	 */
	void SetPlayerNum(UInt32 num){ m_PlayerNum = num; }
	UInt32 GetPlayerNum() const{ return m_PlayerNum; } 

	/**
	 *@brief 设置获取最大在线玩家数                                                                     
	 */
	void SetMaxPlayerNum(UInt32 num){ m_MaxPlayerNum = num; }
	UInt32 GetMaxPlayerNum() const{ return m_MaxPlayerNum; }

	UInt32 GetOnlinePercent() const;

public:
	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief 发送协议
	 */
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

private:
	//节点id
	UInt32	m_Id;
	//连接
	Avalon::NetConnectionPtr m_Conn;

	//玩家数
	UInt32	m_PlayerNum;
	//最大玩家数
	UInt32	m_MaxPlayerNum;
};

#endif
