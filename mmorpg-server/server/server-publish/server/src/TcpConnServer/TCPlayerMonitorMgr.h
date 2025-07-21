#ifndef __UC_PLAYER_MONITOR_MGR_H__
#define __UC_PLAYER_MONITOR_MGR_H__

#include <CLPlayerMonitorMgr.h>
#include <AvalonSingleton.h>

class TCPlayerMonitorMgr : public ClPlayerMonitorMgr, public Avalon::Singleton<TCPlayerMonitorMgr>
{
public:
	void LoadFromString(const std::string& str);

public:
	/**
	*@brief 玩家被加入监控列表
	*/
	virtual void OnPlayerAddToMonitor(UInt32 accid);

	/**
	*@brief 玩家从监控列表中删除
	*/
	virtual void OnPlayerDelFromMonitor(UInt32 accid);
};

#endif