#ifndef _MAIL_RECV_MGR_H_
#define _MAIL_RECV_MGR_H_

#include <set>
#include <CLDefine.h>

/**
 *@brief 邮件接收管理（主要作用是用来防止重复收到邮件）
 */
class MailRecvMgr : public Avalon::Singleton<MailRecvMgr>
{
public:
	MailRecvMgr();
	~MailRecvMgr();

	/**
	 *@brief 收到邮件
	 */
	bool RecvMail(UInt32 serverType, UInt64 mailGuid);

public:

	// 已经收到的邮件(服务类型--->邮件唯一id)
	std::map<UInt32, std::set<UInt64>> m_RecvMailMap;
};


#endif // !_MAIL_RECV_MGR_H_
