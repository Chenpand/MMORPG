#include "VerifyServerMgr.h"
#include "VerifyClientMgr.h"
#include <CLVerifyProtocol.h>

VerifyServerMgr::VerifyServerMgr()
{

}

VerifyServerMgr::~VerifyServerMgr()
{

}

void VerifyServerMgr::OnUpdateAbility(UInt32 id, UInt32 ability)
{
	CLProtocol::VerifyCenterNotifyAbility notify;
	notify.nodeId = id;
	notify.ability = ability;
	VerifyClientMgr::Instance()->Broadcast(notify);
}

void VerifyServerMgr::OnAddNode(CLVerifyNode* node)
{
	InfoLogStream << "verify node(" << node->GetID() << ") online." << LogStream::eos;

	// 通知给所有的客户端
	CLProtocol::VerifyCenterNotifyVSOnline notify;
	notify.node = *node;
	VerifyClientMgr::Instance()->Broadcast(notify);
}

void VerifyServerMgr::OnDelNode(UInt32 id)
{
	InfoLogStream << "verify node(" << id << ") offline." << LogStream::eos;

	// 通知给所有的客户端
	CLProtocol::VerifyCenterNotifyVSOffline notify;
	notify.id = id;
	VerifyClientMgr::Instance()->Broadcast(notify);
}

void VerifyServerMgr::ShowAbility()
{
	UInt32 totalAbility = 0;
	for (auto itr : m_Nodes)
	{
		auto node = itr.second;
		InfoLogStream << "node(" << node->GetID() << ") ability(" << node->GetAbality() << ")." << LogStream::eos;
		totalAbility += node->GetAbality();
	}

	InfoLogStream << "total ability(" << totalAbility << ")." << LogStream::eos;
}