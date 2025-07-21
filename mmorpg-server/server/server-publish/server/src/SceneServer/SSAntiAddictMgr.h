#ifndef _SS_ANTI_ADDICT_MGR_H_
#define _SS_ANTI_ADDICT_MGR_H_

#include <CLAntiAddictMgr.h>
#include <AvalonSimpleTimer.h>

class Player;

/**
 *@brief �����Թ���
 */
class SSAntiAddictMgr : public CLAntiAddictMgr
{
public:
	SSAntiAddictMgr();
	~SSAntiAddictMgr();

	Player* GetOwner() { return m_Owner; }
	void SetOwner(Player* owner) { m_Owner = owner; }

	/**
	 *@brief tick
	 */
	virtual void  OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	virtual void OnDayChanged();

	/**
	 *@brief ���ͷ�����֪ͨ
	 */
	void SendAntiAddictNotice(UInt32 id);

	/**
	 *@brief ͬ������ʱ��
	 */
	void SyncAddictTimeToCenter();

private:

	Player* m_Owner;

	//ͬ���˺�����ʱ��Ķ�ʱ��
	Avalon::SimpleTimer m_SyncAddictTimer;
};

#endif