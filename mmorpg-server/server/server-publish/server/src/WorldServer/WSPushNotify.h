#ifndef _WS_PUSH_NOTIFY_H_
#define _WS_PUSH_NOTIFY_H_

#include "WSRouter.h"
#include "WSApplication.h"

#include <CLCenterProtocol.h>
#include <CLDefine.h>
#include <CLRecordClient.h>
#include "AccountMgr.h"

inline void pushNotifyToCenter(UInt32 accid, std::string title, std::string content)
{
	CLProtocol::CenterPushNotify protocol;

	protocol.platform = PLATFORM;
	protocol.accid = accid;
	protocol.title = title;
	protocol.content = content;

	Router::SendToCenter(protocol);
}

class SelectAccIDCallback : public CLRecordCallback
{
public:
	SelectAccIDCallback(ObjID_t roleId, const std::string& title, const std::string& content)
		: m_RoleId(roleId), m_Title(title), m_Content(content) {}

	void OnFinished()
	{
		if (!NextRow())
		{
			ErrorLogStream << "title(" << m_Title << ") content(" << m_Content << ")can't find player(" << m_RoleId << ")." << LogStream::eos;
			return;
		}

		UInt32 accid = GetData("accid");
		AccountMgr::Instance()->SetRoleID2AccID(m_RoleId, accid);

		pushNotifyToCenter(accid, m_Title, m_Content);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "title(" << m_Title << ") content(" << m_Content << ") role(" << m_RoleId << ") db error(" << errorcode << ")." << LogStream::eos;
	}

private:
	ObjID_t			m_RoleId;
	std::string		m_Title;
	std::string		m_Content;
};

inline void pushNotifyToCenterByRoleID(ObjID_t roleId, std::string title, std::string content)
{

	auto accid = AccountMgr::Instance()->GetAccIDByRoleID(roleId);
	if (accid > 0)
	{
		CLProtocol::CenterPushNotify protocol;

		protocol.platform = PLATFORM;
		protocol.accid = accid;
		protocol.title = title;
		protocol.content = content;

		Router::SendToCenter(protocol);
	}
	else
	{
		// 去数据库查询
		auto cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		cmd->PutCond("guid", roleId);
		CLRecordClient::Instance()->SendCommand(cmd, new SelectAccIDCallback(roleId, title, content));
	}
	
}


#endif