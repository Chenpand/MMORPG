#include "MsgConfirmMgr.h"

#include <AvalonPacket.h>
#include <CLSysProtocol.h>
#include <CLRecordClient.h>

MsgConfirmMgr::MsgConfirmMgr()
{
	m_SendFun = NULL;
}

MsgConfirmMgr::~MsgConfirmMgr()
{

}

CLProtocol::SysMsgConfirmReq* MsgConfirmMgr::CreateNoConfirmMsg(Avalon::Packet* packet)
{


	return NULL;
}

bool MsgConfirmMgr::RegisterSendMsgFun(SendMsgToGameZone sendFun)
{
	if (NULL == sendFun)
	{
		ErrorLogStream << "register send msg fun null!" << LogStream::eos;
		return false;
	}

	m_SendFun = sendFun;
	return true;
}

void MsgConfirmMgr::OnTick(const Avalon::Time& now)
{
	for(auto& iter : m_NoConfirmMsgMap)
	{
		//if ()
		{

		}

	}
}

void MsgConfirmMgr::AddNoConfirmMsg(UInt64 roleId, UInt32 zoneId, Avalon::Packet* packet)
{
	MsgConfirm msgF;
	msgF.guid = CLRecordClient::Instance()->GenGuid();
	msgF.msgId = packet->GetID();
	msgF.roleId = roleId;
	msgF.zoneId = zoneId;
	msgF.sendTime = UInt32(CURRENT_TIME.Sec());
	msgF.packet = packet;

	m_NoConfirmMsgMap[msgF.guid] = msgF;

	//InfoLogStream << "add confirm msg guid:" << 0 << LogStream::eos;
}

void MsgConfirmMgr::ReplyConfirmMsg(UInt64 guid)
{
	InfoLogStream << "reply confirm msg guid:" << guid << LogStream::eos;

	m_NoConfirmMsgMap.erase(guid);
}

void MsgConfirmMgr::_AgainSendNoConfirmMsg()
{
	m_SendFun(0, NULL);
}
