#include "OfflineNotifyMgr.h"

#include <CLChatProtocol.h>
#include "WSPlayer.h"
#include "ScriptMgr.h"
#include "WorldPlayerScript.h"

OfflineNotifyMgr::OfflineNotifyMgr()
{

}

OfflineNotifyMgr::~OfflineNotifyMgr()
{

}

void OfflineNotifyMgr::Init()
{

}

void OfflineNotifyMgr::Final()
{

}

void OfflineNotifyMgr::PutOfflineNotify(const std::string& type, const std::string& word)
{
	OfflineNotify& notify = m_Notifys[type];
	notify.notifys.push_back(word);
	if(notify.notifys.size() > MAX_OFFLINENOTIFY_NUM)
	{
		notify.notifys.erase(notify.notifys.begin());
	}

	CLProtocol::WorldSyncOfflineNotify sync;
	sync.type = type;
	sync.word = word;
	for(std::set<WSPlayer*>::iterator iter = notify.players.begin();
		iter != notify.players.end(); ++iter)
	{
		WSPlayer* player = *iter;
		player->SendProtocol(sync);
	}
}

void OfflineNotifyMgr::SendOfflineNotifys(WSPlayer* player, const std::string& type)
{
	//if(!ScriptMgr::Instance()->GetPlayerScript()->IsOfflineNotify(type)) return;

	CLProtocol::WorldfflineNotifyRet ret;
	ret.type = type;
	ret.notifys = m_Notifys[type].notifys;
	player->SendProtocol(ret);

	m_Notifys[type].players.insert(player);
}

void OfflineNotifyMgr::OnPlayerOffline(WSPlayer* player)
{
	for(OfflineNotifyMap::iterator iter = m_Notifys.begin();
		iter != m_Notifys.end(); ++iter)
	{
		iter->second.players.erase(player);
	}
}


