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
	*@brief ��ұ��������б�
	*/
	virtual void OnPlayerAddToMonitor(UInt32 accid);

	/**
	*@brief ��ҴӼ���б���ɾ��
	*/
	virtual void OnPlayerDelFromMonitor(UInt32 accid);
};

#endif