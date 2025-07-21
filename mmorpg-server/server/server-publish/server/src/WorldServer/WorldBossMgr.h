#ifndef _WORLD_WORLDBOSS_H_
#define _WORLD_WORLDBOSS_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLGameDefine.h>
#include <CLSysNotify.h>
#include <CLChatProtocol.h>

class WSPlayer;

/**
*@brief 世界boss信息
*/
struct WorldBossInfo
{
	// boss id
	UInt64	uId;
	// 地图id
	UInt32	uMapId;
	// 重生间隔
	UInt32	uReviveInterval;
	// 重生时间
	UInt32	uReviveTime;
	// boss名字
	std::string	strName;
	// boss标记，0 世界boss,1 超级boss 2 阵营Boss
	UInt8	uFlag;
	// 上次击杀玩家名字
	std::string	strLastKiller;
	// 关注玩家列表
	std::map<ObjID_t, WSPlayer*>	mapPlayers;
};

/**
*@brief 世界boss信息管理器
*/
class WorldBossMgr : public Avalon::Singleton<WorldBossMgr>
{
	typedef std::map<UInt32, WorldBossInfo*>	WorldBossInfoMap;
	typedef std::list<UInt32> RefreshTimeList;
	typedef std::map<UInt32, RefreshTimeList> RefreshTimeMap;

public:
	WorldBossMgr();
	~WorldBossMgr();

	/**
	*@brief 注册boss信息
	*/
	void RegisterBossInfo(UInt32 uNpcId, ObjID_t uId, std::string strName, UInt32 uMapId, UInt32 uReviveInterval, UInt32 uBirthTime, UInt8 uFlag);

	/**
	*@brief 注册阵营Boss信息
	*/
	void RegisterCountryInfo(UInt32 npcId, std::string& name, UInt32 mapId, std::string& strRefreshTime);

	/**
	*@brief 刷新boss信息
	*/
	void RefreshBossInfo(UInt32 uNpcId, ObjID_t uId, UInt8 uStatus, std::string strKiller);

	/**
	*@brief 同步boss信息
	*/
	void SyncBossInfo(WSPlayer *pPlayer);

	/**
	*@brief 设置boss刷新提醒
	*/
	void TraceBoss(WSPlayer *pPlayer, UInt32 uNpcId, UInt8 uFlag);

	/**
	*@brief 取消所有追踪
	*/
	void CancelAllTrace(WSPlayer *pPlayer);

public:	//事件
	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time &now);

private:
	
	// by huchenhui
	//void Notify(WorldBossInfo *pBossInfo, UInt32 id, ...);
	void SendNotify(WorldBossInfo* pBossInfo, UInt16 type, UInt8 color, char *word);

	/**
	*@brief 通知boss刷新
	*/
	template <typename... Args>
	void Notify(WorldBossInfo *pBossInfo, UInt32 id, const Args & ... args)
	{
		if (pBossInfo->mapPlayers.empty())
			return;

		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		SendNotify(pBossInfo, type, color, buffer);
	}


	/**
	*@brief 计算阵营Boss复活时间点
	*/
	UInt32 CalcRebirthTime(UInt32 npcId);

private:
	// 世界boss信息表
	WorldBossInfoMap	m_mapWorldBossInfo;

	//阵营Boss复活时间点(每天多个复活时间点且分布不规则)
	RefreshTimeMap m_RefreshTimeMap;

	// 分钟定时器
	Avalon::SimpleTimer		m_minTimer;
};

#endif

