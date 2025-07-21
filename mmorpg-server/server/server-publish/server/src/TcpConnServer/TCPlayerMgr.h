#ifndef _UC_PLAYERMGR_H_
#define _UC_PLAYERMGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>

class TCPlayer;

//负责索引TCPlayer对象，但不负责TCPlayer的创建和删除， TCPlayer对象底层是由ObjectPool<T,MutexT>来管理的
class TCPlayerMgr : public Avalon::Singleton<TCPlayerMgr>
{
public:
	typedef HashMap<UInt32, TCPlayer*> TCPlayerMap;   //accId 为key

public:
	TCPlayerMgr();
	~TCPlayerMgr();

	void OnTick(const Avalon::Time& now);
	void OnQuit();


	TCPlayer* find_player(UInt32 accId);
	bool add_player(TCPlayer* player);
	bool remove_player(TCPlayer* player);
	bool remove_player(UInt32 accId);

	UInt32 get_player_count();

protected:
private:

	TCPlayerMap m_stPlayerMap;

};

#endif
