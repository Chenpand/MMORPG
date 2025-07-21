#ifndef _PLAYER_MGR_H_
#define _PLAYER_MGR_H_

#include <AvalonPropertySet.h>
#include <AvalonSingleton.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include "Player.h"

/**
 *@brief 玩家管理器
 */
class PlayerMgr : public Avalon::Singleton<PlayerMgr>
{
public:
	PlayerMgr();
	~PlayerMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief 添加一个玩家
	 */
	bool AddPlayer(Player* player);

	/**
	*@brief 删除一个玩家
	*/
	void DelPlayer(Player* player);

	/**
	 *@brief 查找一个玩家
	 */
	Player* FindPlayer(ObjID_t id);
	Player* FindNormalPlayer(ObjID_t id);
	Player* FindPlayer(const std::string &strName);
	Player* FindNormalPlayer(const std::string &strName);
	Player* FindLoadingFinishPlayer(ObjID_t id);
	// 离线加载玩家回掉处理逻辑
	void LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, PlayerLoadingCallback* cb);

	/**
	 *@brief 移除一个玩家
	 */
	void RemovePlayer(Player* player, bool removeFromMap = false);

public:
	/**
	 *@brief 执行一个消息包
	 */
	void ExecutePacket(ObjID_t id,Avalon::Packet* packet);

	/**
	*@brief 展示一帧内收到的包
	*/
	void ShowPacketInOneTick();

public:
	/**
	 *@brief 访问所有玩家
	 */
	void VisitPlayers(PlayerVisitor& visitor);
	void VisitNormalPlayers(PlayerVisitor& visitor);

public:
	/**
	 *@brief 网关断开连接
	 */
	void OnGateDisconnected(UInt32 id);

	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 账号永久性删除
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId);

	/**
	*@brief 获得角色初始值配置
	*/
	Avalon::PropertySet* GetPlayerAttr() { return &m_InitAttrs; }

	/**
	*@brief 获取玩家初始场景
	*/
	UInt32 GetInitSceneID() { return GetPlayerAttr()->GetProperty("PlayerInitInfo", "scene"); }

private:
	/**
	 *@brief 注册玩家协议
	 */
	void RegisterProtocols();

	/**
	*@brief 判断缓存的玩家数量，如果超过一定值，就尝试删除部分玩家
	*/
	void TryRelaseCachePlayerData();

private:
	//id->player索引
	CLIDObjIndex<Player>	m_Players;
	//玩家消息包执行器
	CLParamPacketExecutor<Player*>	m_Executor;

	//分定时器
	Avalon::SimpleTimer		m_MinTimer;

	//角色初始属性
	Avalon::PropertySet		m_InitAttrs;

	// 一个tick内各个消息的数量
	std::map<UInt32, UInt32>	m_PacketNumInOneTick;
};


#endif
