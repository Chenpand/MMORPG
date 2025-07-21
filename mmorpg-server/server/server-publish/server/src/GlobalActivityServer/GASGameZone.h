#ifndef _GAS_GAME_ZONE_H_
#define _GAS_GAME_ZONE_H_

#include <CLObject.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>

/**
 *@brief 游戏区
 */
class GASGameZone
{
public:
	GASGameZone();
	~GASGameZone();

public:
	/**
	 *@brief 设置获取id
	 */
	bool Init(UInt32 id);
	UInt32 GetID() const{ return m_Id; }

public: //消息相关
	/**
	 *@brief 发送消息
	 */
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief 转发玩家消息
	*/
	void TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Packet* packet);
	void TransmitPlayerMsg(const std::string uid, UInt32 adminNode, Avalon::Protocol& protocol);

	/**
	*@brief 转送消息到admin
	*/
	void TransmitToAdmin(UInt32 adminNode, Avalon::Protocol& protocol);

	/**
	*@brief 转送消息到gate
	*/
	void BroadcastToGate(UInt32 adminNode, Avalon::Protocol& protocol);
	void BroadcastToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 转发消息到word
	 */
	void BroadcastToWorld(Avalon::Protocol& protocol);
	void TransmitToWorld(UInt32 adminNode, Avalon::Protocol& protocol);

private:
	//id
	UInt32	m_Id;
	//游戏区管理服务器连接
	Avalon::NetConnectionPtr m_Conn;
};

typedef CLVisitor<GASGameZone>	GASGameZoneVisitor;

#endif
