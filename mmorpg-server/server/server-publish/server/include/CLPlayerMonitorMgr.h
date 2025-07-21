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
	*@brief ��������û�
	*/
	void AddPlayer(UInt32 accid);

	/**
	*@brief ɾ������û�
	*/
	void DelPlayer(UInt32 accid);

	/**
	*@brief ������Ҫ��ص��˺��б�
	*/
	void SetMonitorList(std::set<UInt32>& accList);
	const std::set<UInt32>& GetMonitorList() const { return m_AccountList; }

	/**
	*@brief ����Ƿ񱻼��
	*/
	bool IsPlayerMonitored(UInt32 accid);

public:
	// һЩ�¼��ص�

	/**
	*@brief ��ұ��������б�
	*/
	virtual void OnPlayerAddToMonitor(UInt32 accid) {}

	/**
	*@brief ��ҴӼ���б���ɾ��
	*/
	virtual void OnPlayerDelFromMonitor(UInt32 accid) {}

private:
	std::set<UInt32>		m_AccountList;
};


#endif
