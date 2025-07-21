#ifndef _UC_PLAYERMGR_H_
#define _UC_PLAYERMGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <AvalonSimpleTimer.h>

class UCPlayer;

//��������UCPlayer���󣬵�������UCPlayer�Ĵ�����ɾ���� UCPlayer����ײ�����ObjectPool<T,MutexT>�������
class UCPlayerMgr : public Avalon::Singleton<UCPlayerMgr>
{
public:
	typedef HashMap<UInt32, UCPlayer*> UCPlayerMap;   //accId Ϊkey

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

	// �ϱ�ping��ʱ��
	Avalon::SimpleTimer		m_ReportPingTimer;

	// �ϱ��ӳ�
	Avalon::SimpleTimer		m_ReportDelayTimer;
};

#endif
