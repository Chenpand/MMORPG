#ifndef _WS_PLAYER_MGR_H_
#define _WS_PLAYER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonSimpleTimer.h>
#include <CLObjectIndex.h>
#include <CLPacketExecutor.h>
#include <map>
#include "WSPlayer.h"

class WSScene;

/**
 *@brief 玩家管理器
 */
class WSPlayerMgr : public Avalon::Singleton<WSPlayerMgr>
{
	typedef std::map<UInt32, WSPlayer*>	AccidPlayerMap;

	//存档超时时间
	const static UInt32	DESTROY_TIMEOUT = 60;
	//上报social服间隔
	const static UInt32	REPORT_SOCIAL_INTERVAL = 60;

public:
	WSPlayerMgr();
	~WSPlayerMgr();

public:
	bool Init();
	void Final();

	/**
	 *@brief 添加、删除、查找玩家
	 */
	bool AddPlayer(WSPlayer* player);
	void RemovePlayer(WSPlayer* player);

	//离线查询玩家缓存
	bool AddOfflinePlayerCache(WSPlayer* player);

	/**
	 *@brief 根据账号id添加、删除、查找玩家
	 */
	bool AddPlayerByAccId(WSPlayer* player);
	void RemovePlayerByAccId(WSPlayer* player);
	
	WSPlayer* FindPlayer(ObjID_t id);
	WSPlayer* FindPlayerByAccid(UInt32 accid);
	WSPlayer* FindNormalPlayer(ObjID_t id);
	WSPlayer* FindNormalPlayerByName(const std::string& name);

	/**
	 *@brief 遍历所有玩家
	 */
	void VisitPlayers(WSPlayerVisitor& visitor);
	void VisitNormalPlayers(WSPlayerVisitor& visitor);

	/**
	 *@brief 获得角色初始值配置
	 */
	Avalon::PropertySet* GetPlayerAttr() { return &m_InitAttrs; }

	/**
	 *@brief 获取在线人数
	 */
	UInt32	GetOnlineNum() const{ return m_OnlineNum; }

	/**
	*@brief 获取玩家所在的SceneServer
	*/
	UInt32 GetPlayerSceneServerID(ObjID_t roleId);

	/**
	*@brief 系统开放的可扩展角色数量
	*/
	UInt16 GetSystemOpenedExtensibleRoleNum() const { return m_SystemOpenedExtensibleRoleNum; }

public://数据中心上报相关
	//void SendUdpLog(const char* str1, const char* str2, const char* str3 , const char* str4, const char* str5, const char* str6, const char* strType, Int32 nCount = 1);

public:
	/**
	 *@brief 执行一个玩家消息
	 */
	void ExecutePacket(ObjID_t id,Avalon::Packet* packet);

public://事件
	/**
	 *@brief 场景连接断开
	 */
	void OnSceneDisconnected(UInt32 nodeid);

	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);
	
	/**
	 *@brief 跨天
	 */
	void OnDayChanged();

	/**
	 *@brief 改名了
	 */
	void OnNameChanged(ObjID_t playerId, const std::string& newname);

	/**
	 *@brief 改性别了                                                                     
	 */
	void OnSexChanged(ObjID_t playerId, UInt8 sex);

	/**
	*@brief 玩家在Scene上加载完成
	*/
	void OnPlayerLoadFinishOnSceneServer(ObjID_t roleId, UInt32 sceneNodeId);

	/**
	*@brief 玩家在Scene上加载完成
	*/
	void OnPlayerLeaveSceneServer(ObjID_t roleId);

	/**
	*@brief 账号永久性删除
	*/
	void OnPlayerForeverDeleted(ObjID_t roleId, UInt8 type);

	/**
	*@brief 查询这个时间段内永久性删除的角色
	*/
	void QueryForeverDeletedPlayer();

public:
	// 排队相关

	/**
	*@brief 是否需要排队
	*/
	bool IsNeedWaitOhters();

	/**
	*@brief 有玩家退出游戏或者其他原因，尝试让排第一个的玩家登录
	*/
	bool TryLetFirstPlayerLogin();

	/**
	*@brief 添加到队伍中，返回排在前面的玩家数
	*/
	UInt32 AddPlayerToWaitList(UInt32 accid);

	/**
	*@brief 从队伍中删除
	*/
	void DelFromWaitList(UInt32 accid);

	/**
	*@brief 获取玩家在队伍中的位置
	*/
	int GetIndexInWaitList(UInt32 accid);

	/**
	*@brief 同步排队中的玩家的排队信息
	*/
	void SyncWaitLoginPlayerInfo();

	/**
	*@brief 加载中的玩家
	*/
	void AddLoadingPlayer(ObjID_t roleid) { m_loadingPlayer.insert(roleid); }
	void DelLoadingPlayer(ObjID_t roleid) { m_loadingPlayer.erase(roleid); }
	UInt32 GetLoadingPlayerNum() const { return m_loadingPlayer.size() + m_loadingPlayerNum; }
	
	// 更新渠道人数
	void ClearPfPlayerNum();
	void AddPfPlayerNum(std::string pf);
	UInt32 GetPfPlayerNum(std::string pf);

private:
	//注册协议
	void RegisterProtocols();

	bool _RemovePlayerInAccountMap(WSPlayer* player);


private:
	//id玩家索引
	CLIDObjIndex<WSPlayer>	m_Players;
	//name玩家所有
	CLNameObjectIndex<WSPlayer>	m_NamePlayers;
	//账号id玩家索引
	AccidPlayerMap	m_AccidPlayers;
	//玩家消息包执行器
	CLParamPacketExecutor<WSPlayer*>	m_Executor;
	//玩家所在SceneServerID映射表
	HashMap<ObjID_t, UInt32>	m_player2SceneServer;

	Avalon::SimpleTimer			m_CheckDeletePlayerTimer;

	//在线人数
	UInt32				m_OnlineNum;

	//角色初始属性
	Avalon::PropertySet	m_InitAttrs;

	//上次记录在线人数分钟
	UInt8				m_LogOnlineMin;
	//上次上报在线人数分钟
	UInt8				m_ReportOnlineMin;
	//下次上报在线时间
	UInt32				m_ReportSocialTime;
	//今日最多人数
	UInt32				m_TodayMaxOnlineNum;
	//今日最多人数时刻
	Avalon::Time		m_TodayMaxOnlineTime;
	//今日最少人数
	UInt32				m_TodayMinOnlineNum;
	//今日最少人数时刻
	Avalon::Time		m_TodayMinOnlineTime;

	// 排队相关数据
	// 排队同步定时器
	Avalon::SimpleTimer			m_waitQueueTimer;
	// 正在排队的玩家
	std::list<UInt32>			m_waitPlayers;
	// 是否需要同步排队信息
	bool						m_needSyncWaitQueue;
	// 正在加载中的玩家
	std::set<ObjID_t>			m_loadingPlayer;
	// 正在加载中的玩家数量
	UInt32						m_loadingPlayerNum;
	// 渠道在线人数
	std::map<std::string, UInt32>	m_Pf2PlayerNum;

	//系统开放的可扩展角色数量
	UInt32		m_SystemOpenedExtensibleRoleNum;
};

#endif
