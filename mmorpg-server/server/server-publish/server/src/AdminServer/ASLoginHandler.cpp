#include "ASLoginHandler.h"
#include <CLErrorCode.h>
#include <AvalonMd5.h>

#include "ASApplication.h"
#include "GateSessionMgr.h"
#include "ASInternalNetwork.h"
#include "ASExternalNetwork.h"
#include "ASRouter.h"

void HandleProtocol(CLProtocol::AdminLoginNewPlayer& req)
{
	/*GateSession* session = GateSessionMgr::Instance()->SelectGate();
	if(session == NULL)
	{
		Avalon::NetConnectionPtr conn = ASExternalNetwork::Instance()->FindConnection(req.accid);
		if(conn != NULL && conn->IsVerified())
		{
			CLProtocol::AdminLoginVerifyRet ret;
			ret.result = ErrorCode::LOGIN_SERVER_UNREADY;
			CL_SEND_PROTOCOL(conn,ret);
			conn->Close();
		}
		return;
	}
    
	CLProtocol::GateSendAccountInfo protocol;
	protocol.accid = req.accid;
	protocol.gmauthority = req.gmauthority;
	memmove(protocol.hash, req.hash, 20);
	protocol.openid = req.openid;
	protocol.qqVipInfo = req.qqVipInfo;
	protocol.source1 = req.source1;
	protocol.source2 = req.source2;
	protocol.pf = req.pf;
	session->SendProtocol(protocol);*/

	DebugLogStream << "account(" << req.accid << ") request to login." << LogStream::eos;

	// 向world server查询有没有已经为这个玩家分配gate server
	CLProtocol::AdminPlayerGateInfoReq protocol;
	protocol.info = req;
	Router::SendToWorld(protocol);
}

void HandleProtocol(CLProtocol::AdminPlayerGateInfoRet& ret)
{
	if (ret.result != ErrorCode::SUCCESS)
	{
		DebugLogStream << "account(" << ret.info.accid << ") get gate info failed, error(" << ret.result << ")." << LogStream::eos;

		Avalon::NetConnectionPtr conn = ASExternalNetwork::Instance()->FindConnection(ret.info.accid);
		if (conn != NULL && conn->IsVerified())
		{
			CLProtocol::AdminLoginVerifyRet cRet;
			cRet.result = ret.result;
			cRet.errMsg = ret.errMsg;
			CL_SEND_PROTOCOL(conn, cRet);
			conn->Close();
		}
		return;
	}

	GateSession* gate = NULL;
	if (ret.gateId > 0)
	{
		gate = GateSessionMgr::Instance()->FindSession(ret.gateId);
	}

	// 说明没有分配gateserver
	if (!gate)
	{
		gate = GateSessionMgr::Instance()->SelectGate();
	}

	// 没有可用的gateserver
	if (!gate)
	{
		DebugLogStream << "account(" << ret.info.accid << ") no useful gate." << LogStream::eos;
		Avalon::NetConnectionPtr conn = ASExternalNetwork::Instance()->FindConnection(ret.info.accid);
		if (conn != NULL && conn->IsVerified())
		{
			CLProtocol::AdminLoginVerifyRet cRet;
			cRet.result = ErrorCode::LOGIN_SERVER_UNREADY;
			CL_SEND_PROTOCOL(conn, cRet);
			conn->Close();
		}
		return;
	}
	
	DebugLogStream << "account(" << ret.info.accid << ") alloc gate(" << gate->GetID() << ") phonebind(" << ret.info.phoneBindRoleId << ")." << LogStream::eos;

	CLProtocol::GateSendAccountInfo protocol;
	protocol.accid = ret.info.accid;
	protocol.gmauthority = ret.info.gmauthority;
	memmove(protocol.hash, ret.info.hash, 20);
	protocol.openid = ret.info.openid;
	protocol.qqVipInfo = ret.info.qqVipInfo;
	protocol.source1 = ret.info.source1;
	protocol.source2 = ret.info.source2;
	protocol.pf = ret.info.pf;
	protocol.did = ret.info.did;
	protocol.phoneBindRoleId = ret.info.phoneBindRoleId;
	protocol.antiAddictInfo = ret.info.antiAddictInfo;
	protocol.model = ret.info.model;
	protocol.deviceVersion = ret.info.deviceVersion;
	gate->SendProtocol(protocol);
}

void HandleProtocol(CLProtocol::GateNetAddressRet& protocol)
{
	DebugLogStream << "account(" << protocol.accid << ") send gate address." << LogStream::eos;
	Avalon::NetConnectionPtr conn = ASExternalNetwork::Instance()->FindConnection(protocol.accid);
	if (conn == NULL || !conn->IsVerified())
	{
		ErrorLogStream << "account(" << protocol.accid << ") invalid connection." << LogStream::eos;
		return;
	}

	CLProtocol::AdminLoginVerifyRet ret;
	ret.accid = protocol.accid;
	ret.addr = protocol.addr;
	ret.result = protocol.result;
	ret.errMsg = protocol.errMsg;
	ret.encryptProtocol = protocol.encryptProtocol;
	ret.openBugly = ASApplication::Instance()->IsOpenBugly(protocol.accid) ? 1 : 0;
	ret.battleUseTcp = ASApplication::Instance()->BattleUseTcp();

	std::string keyseed = protocol.openid + ASApplication::Instance()->GetDirCryptKey();
	char key[33];
	Avalon::md5_encode32(key, (void*)(keyseed.c_str()), keyseed.length());
	ret.dirSig = key;
	ret.replayAgentAddr = ASApplication::Instance()->GetReplayAgentAddress();
	ret.phoneBindRoleId = protocol.phoneBindRoleId;
	ret.voiceFlag = ASApplication::Instance()->GetVoiceFlag();
	ret.activityYearSortListUrl = ASApplication::Instance()->GetActivityYearSortListUrl(protocol.pf);
	ret.webActivityUrl = ASApplication::Instance()->GetWebActivityUrl(protocol.pf);
	ret.openNewReconnectAlgo = ASApplication::Instance()->IsOpenNewReconnectAlgo();
	ret.openNewReportFrameAlgo = ASApplication::Instance()->IsOpenNewReportFrameAlgo();
	ret.commentServerAddr = ASApplication::Instance()->GetCommentServerUrl();
	ret.redPacketRankUrl = ASApplication::Instance()->GetRedPacketRankUrl();

	DebugLogStream << "account(" << protocol.accid << ") weburl: " + ret.webActivityUrl << LogStream::eos;


	if (ASApplication::Instance()->IsNeedConvertAccount(protocol.pf))
	{
		ret.convertUrl = ASApplication::Instance()->GetConvertUrl();
	}
	ret.reportUrl = ASApplication::Instance()->GetReportUrl();
	ret.writeQuestionnaireUrl = ASApplication::Instance()->GetWriteQuestionnaireUrl();

	InfoLogStream << "account(" << protocol.accid << ") use tcp(" << ret.battleUseTcp << ")." << LogStream::eos;

	CL_SEND_PROTOCOL(conn, ret);

	conn->Close();
}

void HandleProtocol(CLProtocol::AdminSyncGatePlayerNum& protocol)
{
	GateSession* session = GateSessionMgr::Instance()->FindSession(protocol.GetConnID());
	if(session == NULL) return;

	session->SetPlayerNum(protocol.playerNum);
}

void HandleProtocol(CLProtocol::AdminSyncWorldPlayerNum& protocol)
{
	ASApplication::Instance()->SetWorldPlayerNum(protocol.playerNum);
	ASApplication::Instance()->SetWorldMaxPlayerNum(protocol.maxPlayerNum);
}

void HandleProtocol(CLProtocol::AdminSyncLoginQueueInfo& protocol)
{
	ASApplication::Instance()->SetInQueuePlayerNum(protocol.inQueuePlayerNum);
}
