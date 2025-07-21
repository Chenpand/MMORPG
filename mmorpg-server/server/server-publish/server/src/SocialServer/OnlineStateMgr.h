#ifndef _ONLINE_STATE_MGR_H_
#define _ONLINE_STATE_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief ����״̬������
 */
class OnlineStateMgr : public Avalon::Singleton<OnlineStateMgr>
{
	//�ϱ����߼��
	const static UInt32 REPORT_ONLINE_STATE_INTERVAL = 3 * 60;

	typedef HashMap<UInt32, UInt32>	OnlineStateMap;

public:
	OnlineStateMgr();
	~OnlineStateMgr();

public:
	/**
	 *@brief �ϱ�����
	 */
	void ReportOnline(UInt32 id);

	/**
	 *@brief ��ȡ��½ʱ��
	 */
	UInt32 GetOnlineTime(UInt32 id) const;

public://�¼�
	/**
	 *@brief ����
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	void OnOnline(UInt32 id);

	/**
	 *@brief ����
	 */
	void OnOffline(UInt32 id);

private:
	//����״̬�б�
	OnlineStateMap		m_OnlineStates;
	//��ʱ��
	Avalon::SimpleTimer	m_MinTimer;
};

#endif
