#ifndef _ONLINE_STATE_MGR_H_
#define _ONLINE_STATE_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief 在线状态管理器
 */
class OnlineStateMgr : public Avalon::Singleton<OnlineStateMgr>
{
	//上报在线间隔
	const static UInt32 REPORT_ONLINE_STATE_INTERVAL = 3 * 60;

	typedef HashMap<UInt32, UInt32>	OnlineStateMap;

public:
	OnlineStateMgr();
	~OnlineStateMgr();

public:
	/**
	 *@brief 上报在线
	 */
	void ReportOnline(UInt32 id);

	/**
	 *@brief 获取登陆时间
	 */
	UInt32 GetOnlineTime(UInt32 id) const;

public://事件
	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 上线
	 */
	void OnOnline(UInt32 id);

	/**
	 *@brief 离线
	 */
	void OnOffline(UInt32 id);

private:
	//在线状态列表
	OnlineStateMap		m_OnlineStates;
	//定时器
	Avalon::SimpleTimer	m_MinTimer;
};

#endif
