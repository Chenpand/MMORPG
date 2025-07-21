#ifndef _ROUTINE_MGR_H_
#define _ROUTINE_MGR_H_

#include <CLDefine.h>
#include <CLItemDefine.h>
#include <CLObjectProperty.h>

class Player;

/**
 *@brief �ճ�������
 */
class RoutineMgr : public CLComplexProperty
{
	typedef std::map<UInt8, UInt8> ActiveEventMap;

public:
	RoutineMgr();
	~RoutineMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

public:

	/**
	 *@brief ���ͻ�Ծ����Ϣ
	 */
	void SendActiveDegreeInfo();

	/**
	 *@brief ���ͽ���ǩ������
	 */
	void SendTodaySigninInfo();

	/**
	 *@brief ��ȡ����
	 */
	bool GetAward(UInt8 index);

	/**
	 *@brief ��ȡ��������ʱ��
	 */
	UInt32 GetDailyOnlineTime() const;

	/**
	*@brief ��ȡ��Ծ��
	*/
	UInt32 GetActiveDegree() const;

private:

	/**
	 *@brief ��ȡ������������
	 */
	void  Signin();
	UInt32 GetSigninDays() const;

	/**
	 *@brief ���轱��
	 */
	bool GiveAward(const char* reason, const std::vector<ItemReward>& rewards);

	/**
	 *@brief �����ʼ�
	 */
	void GiveMailAward(const char* name, const std::vector<ItemReward>& rewards);

public: //�¼�
	/**
	 *@brief ����
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief ����
	 */
	void OnOffline();

	/**
	 *@brief ����ڶ���
	 */
	void OnDayChanged();

	/**
	 *@brief ��ɻ�Ծ���¼�
	 */
	void OnActiveEventFinished(UInt8 id);

public: //�����
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_ActiveEvents;
	}

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//ӵ����
	Player*	m_pOwner;

	//��Ծ����  noted by aprilliu,2016.05.03 ÿ���ճ���� ��������ɵĴ���
	ActiveEventMap	m_ActiveEvents;
};

#endif
