#ifndef _UC_PLAYERMGR_H_
#define _UC_PLAYERMGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>

class TCPlayer;

//��������TCPlayer���󣬵�������TCPlayer�Ĵ�����ɾ���� TCPlayer����ײ�����ObjectPool<T,MutexT>�������
class TCPlayerMgr : public Avalon::Singleton<TCPlayerMgr>
{
public:
	typedef HashMap<UInt32, TCPlayer*> TCPlayerMap;   //accId Ϊkey

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
