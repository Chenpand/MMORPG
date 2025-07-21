#ifndef _INVITE_MGR_H_
#define _INVITE_MGR_H_

#include <CLDefine.h>
class Player;
class RewardGroup;

/**
 *@brief ���������
 */
class InviteMgr
{
	//ÿ�����������ٻ����ҽ���
	const static UInt32 INVITEE_RECALL_SILVER = 100;
	//ÿ��������ȡ����
	const static UInt32	MAX_DAILY_GOLDAWARD = 360;

public:
	InviteMgr();
	~InviteMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

	/**
	 *����ÿ����Ϣ
	 */
	void SendDailyInfo();

	/**
	 *@brief ��ȡ���
	 */
	void GetGiftbag(UInt8 type);

	/**
	 *@brief ��ȡ�ٻؽ���
	 */
	void GetRecallAward(UInt32 uid);

	/**
	 *@brief ��ȡ���ѻ���
	 */
	void GetConsumeAward();

	/**
	 *@brief ������������б�
	 */
	void RequestInviteeList();

public://�¼�
	/**
	 *@brief ����                                                                     
	 */
	void OnOnline();

	/**
	 *@brief ��������һ��
	 */
	void OnInvitFriend();

	/**
	 *@brief ��������������
	 */
	void OnCheckGiftbagCondRet(UInt8 type);

	/**
	 *@brief �����ȡ�ٻؽ�������
	 */
	void OnCheckRecallAwardRet(UInt32 num);

	/**
	 *@brief �����ȡ���ѷ�������
	 */
	void OnCheckConsumeAwardRet(UInt32 gold, UInt32 totalGold);

private:
	/**
	 *@brief �ж�����Ƿ���ȡ
	 */
	bool CheckGiftbagGot(UInt8 type);
	void SetGiftbagGot(UInt8 type);

	/**
	 *@brief ��ȡ���id
	 */
	UInt32 GetGiftbagID(UInt8 type);

	/**
	 *@brief ���������                                                                     
	 */
	void GiveMailGiftbag(const char* name, RewardGroup* rewardGroup);

private:
	//ӵ����
	Player* m_pOwner;
	//�ϴ���ȡ��ʱ��
	UInt32	m_GetGoldAwardTime;
};

#endif
