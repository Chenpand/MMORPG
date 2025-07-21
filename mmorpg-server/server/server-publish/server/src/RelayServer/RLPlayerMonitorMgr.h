#ifndef __RL_PLAYER_MONITOR_MGR_H__
#define __RL_PLAYER_MONITOR_MGR_H__

#include <CLPlayerMonitorMgr.h>
#include <AvalonSingleton.h>

class RLPlayerMonitorMgr : public ClPlayerMonitorMgr, public Avalon::Singleton<RLPlayerMonitorMgr>
{
public:
	void LoadFromString(const std::string& str);

public:
	/**
	*@brief ��ұ��������б�
	*/
	virtual void OnPlayerAddToMonitor(UInt32 accid);

	/**
	*@brief ��ҴӼ���б���ɾ��
	*/
	virtual void OnPlayerDelFromMonitor(UInt32 accid);
};

#endif