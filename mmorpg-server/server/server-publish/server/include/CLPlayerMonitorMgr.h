#ifndef __CL_PLAYER_MONITOR_MGR_H__
#define __CL_PLAYER_MONITOR_MGR_H__

#include <AvalonDefine.h>
#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <set>

class ClPlayerMonitorMgr
{
public:
	ClPlayerMonitorMgr();
	~ClPlayerMonitorMgr();

	/**
	*@brief 新增监控用户
	*/
	void AddPlayer(UInt32 accid);

	/**
	*@brief 删除监控用户
	*/
	void DelPlayer(UInt32 accid);

	/**
	*@brief 设置需要监控的账号列表
	*/
	void SetMonitorList(std::set<UInt32>& accList);
	const std::set<UInt32>& GetMonitorList() const { return m_AccountList; }

	/**
	*@brief 玩家是否被监控
	*/
	bool IsPlayerMonitored(UInt32 accid);

public:
	// 一些事件回调

	/**
	*@brief 玩家被加入监控列表
	*/
	virtual void OnPlayerAddToMonitor(UInt32 accid) {}

	/**
	*@brief 玩家从监控列表中删除
	*/
	virtual void OnPlayerDelFromMonitor(UInt32 accid) {}

private:
	std::set<UInt32>		m_AccountList;
};


#endif
