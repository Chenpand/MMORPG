#ifndef _CL_OFFLINE_EVENT_DEFINE_H_
#define _CL_OFFLINE_EVENT_DEFINE_H_

#include <CLDefine.h>

/**
*@brief ȫ�������¼���ѯ��������
*/
enum GlobalOfflineEventQueryType
{
	GOEQT_NO_CACHE = 0,		// û�л���
	GOEQT_NO_EVENT,			// �л��浫û�������¼�	
	GOEQT_HAS_EVENT,		// �������¼�
};

/**
 *@brief �����¼�����
 */
enum OfflineEventType
{
	OFFLINE_EVENT_INVALID = 0,
	OFFLINE_EVENT_IMPEACHFAILED_FORLEAVE = 1,		//�뿪���䵯��ʧ��
	OFFLINE_EVENT_IMPEACHFAILED_FORONLINE = 2,		//�������޵���ʧ��
	OFFLINE_EVENT_BEIMPEACHED = 3,					//������
	OFFLINE_EVENT_BETRIBELEADER = 4,				//��Ϊ��������
	OFFLINE_EVENT_BACKYARDOP = 5,					//��Ժ����  ---����
	OFFLINE_EVENT_FIRETRIBE = 6,					//����������
	OFFLINE_EVENT_INC_INTIMACY = 7,					//���Ӻøж�
	OFFLINE_EVENT_ESCORTTARGET_KILLED = 8,			//����Ŀ�걻ɱ
	OFFLINE_EVENT_ESCORTTARGET_DISAPPEAR = 9,		//Զ��
	OFFLINE_EVENT_GIVETITLE = 10,					//����һ���ƺ�
	OFFLINE_EVENT_GIVETRANSAWARD = 11,				//�����;���͹���
	OFFLINE_EVENT_ADD_MALLITEM_BUYCOUNT = 12,		//�̳�������Ʒ�������
	OFFLINE_EVENT_DIVORCE = 13,						//��Ե

	OFFLINE_INVITE_FRIEND = 14,						//�������
	OFFLINE_SELL_AUCTION = 15,						//������ �۳�
	OFFLINE_REPLY_FRIEND = 16,						//���ܺ���

	OFFLINE_PRIVATE_CHAT = 17,						//����˽��
	OFFLINE_REQUEST_MASTER = 18,					//���������ʦ
	OFFLINE_REQUEST_DISCILPE = 19,					//����������ͽ

	OFFLINE_JOIN_GUILD = 20,						//���빤��
	OFFLINE_GET_TITLE = 21,							//���ͷ��
	OFFLINE_GOLD_CONSIGNMENT_POINT = 22,			//��Ҽ��ۺ��
};

#endif // !_CL_OFFLINE_EVENT_DEFINE_H_

