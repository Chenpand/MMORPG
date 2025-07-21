#ifndef _ACTION_COUNTER_MGR_H_
#define _ACTION_COUNTER_MGR_H_

#include <CLObjectProperty.h>

class Player;

/**
 *@brief ��Ϊ��������                                                                     
 */
enum ActionCounterType
{
	ACTION_COUNTER_INVALID = 0,		//��Ч����
	ACTION_COUNTER_KILLNPC = 1,		//ɱ����
	ACTION_COUNTER_KILLPLAYER = 2,	//��ս��ɱ����
	ACTION_COUNTER_CHALLENGEWIN = 3,	//��̨��Ӯ
	ACTION_COUNTER_ROBTRANS = 4,	//��ȡ����ľ��
	ACTION_COUNTER_CAIDAN = 5,		//�ʵ���
	ACTION_COUNTER_KILLBOSS = 6,	//ɱboss��
	ACTION_COUNTER_KILLSUPERBOSS = 7,	//ɱ����boss��
	ACTION_COUNTER_MAKEFRIEND = 8,	//��������
	ACTION_COUNTER_TERRITORYWIN = 9, //����սʤ��
	ACTION_COUNTER_USETREASUREMAP = 10,
	
	ACTION_COUNTER_MAX,
};

/**
 *@brief                                                                      
 */
class ActionCounterMgr : public CLComplexProperty
{
public:
	ActionCounterMgr();
	~ActionCounterMgr();

public:
	void SetOwner(Player* player){ m_pPlayer = player; }
	Player* GetOwner() const{ return m_pPlayer; } 

	UInt32 IncCounter(int type);
	UInt32 GetCounter(int type) const;

public: //�����
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(m_Count, ACTION_COUNTER_MAX);
	}

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

public://�¼�
	/**
	 *@brief  ��������                                                                     
	 */
	void OnIncCount(int type, UInt32 count);

private:
	//���
	Player* m_pPlayer;

	//����
	UInt32	m_Count[ACTION_COUNTER_MAX];
};

#endif
