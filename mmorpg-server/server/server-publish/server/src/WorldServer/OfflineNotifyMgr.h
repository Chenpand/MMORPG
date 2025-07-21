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
	//�����ʾ������
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
	 *@brief ����                                                                     
	 */
	void OnPlayerOffline(WSPlayer* player);

private:
	//����֪ͨ�б�
	OfflineNotifyMap	m_Notifys;
};

#endif
