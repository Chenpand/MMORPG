#ifndef _SS_ANTI_ADDICT_MGR_H_
#define _SS_ANTI_ADDICT_MGR_H_

#include <CLAntiAddictMgr.h>
#include <AvalonSimpleTimer.h>

class Player;

/**
 *@brief 防沉迷管理
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
	 *@brief 跨天
	 */
	virtual void OnDayChanged();

	/**
	 *@brief 发送防沉迷通知
	 */
	void SendAntiAddictNotice(UInt32 id);

	/**
	 *@brief 同步沉迷时间
	 */
	void SyncAddictTimeToCenter();

private:

	Player* m_Owner;

	//同步账号在线时间的定时器
	Avalon::SimpleTimer m_SyncAddictTimer;
};

#endif