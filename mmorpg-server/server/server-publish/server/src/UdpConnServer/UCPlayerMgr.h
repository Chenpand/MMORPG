#ifndef _UC_PLAYERMGR_H_
#define _UC_PLAYERMGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class UCPlayer;

//负责索引UCPlayer对象，但不负责UCPlayer的创建和删除， UCPlayer对象底层是由ObjectPool<T,MutexT>来管理的
class UCPlayerMgr : public Avalon::Singleton<UCPlayerMgr>
{
public:
	typedef HashMap<UInt32, UCPlayer*> UCPlayerMap;   //accId 为key

public:
	UCPlayerMgr();
	~UCPlayerMgr();

	void OnTick(const Avalon::Time& now);
	void OnQuit();


	UCPlayer* find_player(UInt32 accId);
	bool add_player(UCPlayer* player);
	bool remove_player(UCPlayer* player);
	bool remove_player(UInt32 accId);

	UInt32 get_player_count();

protected:
private:

	UCPlayerMap m_stPlayerMap;

	// 上报ping定时器
	Avalon::SimpleTimer		m_ReportPingTimer;

	// 上报延迟
	Avalon::SimpleTimer		m_ReportDelayTimer;
};

#endif
