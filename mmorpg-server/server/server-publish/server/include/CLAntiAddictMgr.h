#ifndef _CL_ANTI_ADDICT_MGR_H_
#define _CL_ANTI_ADDICT_MGR_H_

#include <CLAntiAddictDefine.h>

/**
 *@brief �����Թ���
 */
class CLAntiAddictMgr
{
public:
	CLAntiAddictMgr();
	virtual ~CLAntiAddictMgr();

	void SetAntiAddictTime(UInt32 time) { m_AntiAddictTime = time; }
	UInt32 GetAntiAddictTime() { return m_AntiAddictTime; }

	void SetIsIDAuth(bool idAuth) { m_IsIDAuth = idAuth; }
	bool GetIsIDAuth() { return m_IsIDAuth; }

	void SetAge(UInt32 age) { m_Age = age; }
	UInt32 GetAge() { return m_Age; }

	UInt32 GetTouristLoginInterval() { return m_TouristLoginInterval; }
	UInt32 GetTouristOnlineTime() { return m_TouristOnlineTime; }

	/**
	 *@brief init
	 */
	void OnInit(const AntiAddictInfo& info, UInt32 touristLoginInterval, UInt32 touristOnlineTime);

	/**
	 *@brief tick
	 */
	virtual void  OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	virtual void OnDayChanged();

	/**
	 *@brief ��¼���
	 */
	UInt32 OnLoginCheck();

	/**
	 *@brief ��ȡ������Ϣ
	 */
	AntiAddictInfo GetAntiAddictInfo();

	/**
	 *@brief �����Գ�ʱ
	 */
	UInt32 AddictTimeOut(UInt32& allowGameTime);

	/**
	 *@brief ������ҹ��
	 */
	bool IsNightAddict();

	/**
	 *@brief �Ƿ����
	 */
	bool IsAdult();

private:

	// ����ʱ��
	UInt32 m_AntiAddictTime;
	// �Ƿ������֤
	UInt32 m_IsIDAuth;
	// ����
	UInt32 m_Age;
	// �˺��ϴε�¼ʱ��
	UInt32 m_LastLoginTime;
	// �ο͵�¼���
	UInt32 m_TouristLoginInterval;
	// �ο�������Ϸʱ��
	UInt32 m_TouristOnlineTime;
};

#endif
