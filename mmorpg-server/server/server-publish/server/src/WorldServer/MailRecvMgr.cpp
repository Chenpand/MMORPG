#include "MailRecvMgr.h"

MailRecvMgr::MailRecvMgr()
{

}

MailRecvMgr::~MailRecvMgr()
{

}

bool MailRecvMgr::RecvMail(UInt32 serverType, UInt64 mailGuid)
{
	auto iter = m_RecvMailMap.find(serverType);
	if (iter == m_RecvMailMap.end())
	{
		std::set<UInt64> mailGuidSet;
		mailGuidSet.insert(mailGuid);
		m_RecvMailMap[serverType] = mailGuidSet;
		return true;
	}

	auto it = iter->second.find(mailGuid);
	if (it == iter->second.end())
	{
		iter->second.insert(mailGuid);
		return true;
	}

	return false;
}