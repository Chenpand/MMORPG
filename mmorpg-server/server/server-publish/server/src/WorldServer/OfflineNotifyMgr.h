#ifndef _OFFLINE_NOTIFY_MGR_H_
#define _OFFLINE_NOTIFY_MGR_H_

#include <map>
#include <set>
#include <AvalonSingleton.h>
#include <CLDefine.h>

class WSPlayer;

struct OfflineNotify
{
	std::set<WSPlayer*>		players;
	std::vector<std::string>	notifys;
};

class OfflineNotifyMgr : public Avalon::Singleton<OfflineNotifyMgr>
{
	//最多显示公告数
	const static UInt32	MAX_OFFLINENOTIFY_NUM = 10;

	typedef std::map<std::string, OfflineNotify>	OfflineNotifyMap;
public:
	OfflineNotifyMgr();
	~OfflineNotifyMgr();

public:
	void Init();
	void Final();

public:
	void PutOfflineNotify(const std::string& type, const std::string& word);

	void SendOfflineNotifys(WSPlayer* player, const std::string& type);

public:
	/**
	 *@brief 离线                                                                     
	 */
	void OnPlayerOffline(WSPlayer* player);

private:
	//离线通知列表
	OfflineNotifyMap	m_Notifys;
};

#endif
