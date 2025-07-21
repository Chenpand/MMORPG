#ifndef _MAIL_RECV_MGR_H_
#define _MAIL_RECV_MGR_H_

#include <set>
#include <CLDefine.h>

/**
 *@brief �ʼ����չ�����Ҫ������������ֹ�ظ��յ��ʼ���
 */
class MailRecvMgr : public Avalon::Singleton<MailRecvMgr>
{
public:
	MailRecvMgr();
	~MailRecvMgr();

	/**
	 *@brief �յ��ʼ�
	 */
	bool RecvMail(UInt32 serverType, UInt64 mailGuid);

public:

	// �Ѿ��յ����ʼ�(��������--->�ʼ�Ψһid)
	std::map<UInt32, std::set<UInt64>> m_RecvMailMap;
};


#endif // !_MAIL_RECV_MGR_H_
