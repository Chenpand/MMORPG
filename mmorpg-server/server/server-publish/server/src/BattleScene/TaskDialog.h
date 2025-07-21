#ifndef _TASK_DIALOG_H_
#define _TASK_DIALOG_H_

#include <CLTaskDefine.h>

class Player;

/**
 *@brief ����Ի���
 */
class TaskDialog
{
public:
	TaskDialog(Player* visitor, UInt8 type, ObjID_t id, UInt32 npcId, UInt32 taskId, UInt8 quality, UInt8 status = TASK_INIT);
	~TaskDialog();

public:
	/**
	 *@brief ���̸������
	 */
	void AddWord(const char* word);

	/**
	 *@brief ����ѡ��
	 */
	void AddReply(const char* word);

	/**
	 *@brief ���������
	 */
	void AddReward(){ m_bShowReward = true; }

	/**
	 *@brief ��ʾ
	 */
	void Show();

	/**
	 *@brief ���
	 */
	void Clear();

private:
	//��ǰ������
	Player* m_pVisitor;
	//��������
	UInt8	m_PublishType;
	//������id
	ObjID_t	m_PublishId;
	//npc
	UInt32	m_NpcId;
	//�������
	UInt32	m_Task;
	//����Ʒ��
	UInt8	m_Quality;
	//����״̬
	UInt8	m_Status;
	//����
	std::vector<std::string> m_Words;
	//�ظ�
	std::vector<std::string> m_Replies;
	//�Ƿ���ʾ����
	bool	m_bShowReward;
};

#endif
