#ifndef _GS_SCENE_H_
#define _GS_SCENE_H_

#include <AvalonProtocol.h>
#include <CLScene.h>

#include "GSPlayer.h"

/**
 *@brief 网关场景
 */
class GSScene : public CLScene
{
public:
	GSScene();
	~GSScene();

public:
	bool Init(UInt32 id, const std::string& name, UInt32 length,
		UInt32 width, UInt32 syncRange, UInt32 nodeid,
		UInt32 sceneDataId, UInt8 sceneType);

	/**
	 *@brief 添加一个玩家
	 */
	bool AddPlayer(UInt32 gridIndex,GSPlayer* player);

	/**
	 *@brief 移除一个玩家
	 */
	void RemovePlayer(GSPlayer* player);

	/**
	 *@brief 获取所在节点id
	 */
	UInt32 GetNodeID() const;

	void SetSceneDataId(UInt32 sceneDataId);
	UInt32 GetSceneDataId();

	void SetSceneType(UInt8 sceneType);
	UInt8 GetSceneType();
public://visit接口
	/**
	 *@brief 访问所有玩家
	 */
	void VisitPlayers(GSPlayerVisitor& visitor);

	/**
	 *@brief 访问周围所有玩家
	 */
	void VisitAround(UInt32 grid,GSPlayerVisitor& visitor);

	/**
	 *@brief 访问新同步域玩家
	 */
	void VisitExcept(UInt32 grid1,UInt32 grid2,GSPlayerVisitor& visitor);

public://网络消息
	/**
	 *@brief 广播协议到周围
	 */
	void BroadcastAround(UInt32 grid,Avalon::Protocol& protocol);

	/**
	 *@brief 广播消息到周围
	 */
	void BroadcastAround(UInt32 grid,Avalon::Packet* packet,ObjID_t except = 0);

	/**
	 *@brief 广播消息到新同步域
	 */
	void BroadcastExcept(UInt32 grid1, UInt32 grid2,Avalon::Packet* packet,ObjID_t except);

	/**
	 *@brief 发送消息到场景
	 */
	void SendToSceneServer(Avalon::Protocol& protocol);
private:
	//scene连接
	Avalon::NetConnectionPtr m_Conn;
	//场景表id
	UInt32 m_sceneDataId;
	//场景类型
	UInt8 m_sceneType;
};

typedef CLVisitor<GSScene> GSSceneVisitor;

#endif
