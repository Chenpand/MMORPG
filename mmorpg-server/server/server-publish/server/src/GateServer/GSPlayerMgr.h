#ifndef _GS_PLAYER_MGR_H_
#define _GS_PLAYER_MGR_H_

#include "GSPlayer.h"
#include <CLObjectIndex.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>

struct PerfInfo
{
	UInt32 count;
	UInt32 times;
};

/**
 *@brief 玩家管理器
 */
class GSPlayerMgr : public Avalon::Singleton<GSPlayerMgr>
{
	typedef HashMap<UInt32,GSPlayer*>	AccPlayerMap;

public:
	GSPlayerMgr();
	~GSPlayerMgr();

public:
	/**
	 *@brief 初始化
	 */
	bool Init(Avalon::PropertySet* properties);

	/**
	 *@brief 终止
	 */
	void Final();


public:
	/**
	 *@brief 获取初始角色数据
	 */
	Avalon::PropertySet* GetPlayerInitAttr(){ return &m_InitAttrs; }

	/**
	 *@brief 添加一个玩家
	 */
	bool AddPlayer(GSPlayer* player);

	/**
	 *@brief 添加一个选中角色的玩家
	 */
	bool AddGuidPlayer(GSPlayer* player);

    /**
    *@brief 删除角色Map
    */
    bool RemoveGuidPlayer(GSPlayer* player);

	/**
	 *@brief 删除一个玩家
	 */
	void RemovePlayer(GSPlayer* player);

	/**
	 *@brief 根据账号查找一个玩家
	 */
	GSPlayer* FindPlayerByAccID(UInt32 id);

	/**
	 *@brief 根据角色guid查找一个玩家
	 */
	GSPlayer* FindPlayerByID(ObjID_t id);

	/**
	*@brief 获取玩家数量
	*/
	UInt32 GetPlayerNum() const { return m_accPlayers.size(); };

	/**
	*@brief 获取连接上的玩家数量
	*/
	UInt32 GetConnPlayerNum() const { return m_ConnPlayerNum; }

	/**
	*@brief 系统开放的可扩展角色数量
	*/
	UInt32 GetSystemOpenedExtensibleRoleFields() const { return m_SystemOpenedExtensibleRoleFields; }

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 网关连接建立   Noted by aprilliu , id是account id
	 */
	void OnGateConnected(UInt32 id);

	/**
	 *@brief 网关连接断开
	 */
	void OnGateDisconnected(UInt32 id);

public://访问
	/**
	 *@brief 遍历
	 */
	void VisitPlayers(GSPlayerVisitor& visitor);

public://消息
	/**
	 *@brief 广播
	 */
	void Broadcast(UInt8 bZone, UInt16 level, Avalon::Packet* packet);

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

	UInt32 GetLoadingPlayerNum() const { return m_loadingPlayerNum; }
	UInt32 GetNormalPlayerNum() const { return m_normalPlayerNum; }
	UInt32 GetRoleSelectPlayerNum() const { return m_roleSelectPlayerNum; }

public:
	// 性能统计相关

	/**
	*@brief 获取当前时间(us)
	*/
	UInt64 GetUSecsNow();

	/**
	*@brief 记录消耗时间(us)
	*/
	void RecordUsedTime(UInt32 id, UInt32 usec);

	/**
	*@brief 展示一帧内各模块使用的时间
	*/
	void ShowUsedTime();

public:
	//账号-玩家索引
	AccPlayerMap				m_accPlayers;
	//角色-玩家索引
	CLIDObjIndex<GSPlayer>		m_Players;

	//角色初始属性
	Avalon::PropertySet			m_InitAttrs;

	//连接上的玩家数
	UInt32						m_ConnPlayerNum;

	// 排队同步定时器
	Avalon::SimpleTimer			m_waitQueueTimer;
	// 正在排队的玩家
	std::list<UInt32>			m_waitPlayers;
	// 是否需要同步排队信息
	bool						m_needSyncWaitQueue;

	UInt32						m_loadingPlayerNum;
	UInt32						m_normalPlayerNum;
	UInt32						m_roleSelectPlayerNum;

	// 上报排队信息定时器
	Avalon::SimpleTimer			m_reportWaitQueueTimer;
	//系统开放的可扩展角色栏位
	UInt32		m_SystemOpenedExtensibleRoleFields;

	// 性能统计
	// 性能统计定时器
	Avalon::SimpleTimer			m_PerfTimer;
	// 一个tick内各个模块消耗的时间
	HashMap<UInt32, PerfInfo>	m_UsedTimeInOneTick;
};

#endif
