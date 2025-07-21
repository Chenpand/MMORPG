#include "WorldOperateHandler.h"
#include "WSPlayer.h"
#include "WSRouter.h"
#include "WSPlayerMgr.h"
#include "WSConsoleAgent.h"
#include "MailBox.h"

void HandleProtocol(WSPlayer* player, CLProtocol::CWCDKVerifyReq& req)
{
	//去CenterServer请求验证cdk
	CLProtocol::WCCDKVerifyReq centerReq;
	centerReq.accId = player->GetAccId();
	centerReq.playerUid = player->GetID();
	centerReq.platform = player->GetPf();
	centerReq.serverId = ZONE_ID;
	centerReq.cdk = req.cdk;
	centerReq.reason = SOURCE_CDK_GIVE;
	centerReq.connId = 0;

	Router::SendToCenter(centerReq);
}

void HandleProtocol(CLProtocol::CSCDKVerifyRes& res)
{
	if (res.reason == SOURCE_OPPO_CDK)
	{
		auto usedTime = CURRENT_TIME.MSec() - res.queryTime;
		InfoLogStream << "oppo cdk used " << usedTime << "ms" << LogStream::eos;

		json::Object body;
		body["error"] = json::Boolean(false);
		body["result"] = json::Number(res.result);
		WSConsoleAgent::Instance()->SendResponse(res.connId, "activity", "cdkey", body);

		if (res.result != 1)
		{
			return;
		}

		// 发送成功
		// 发送到邮箱
		std::string sender = SysNotifyMgr::Instance()->MakeString(9800);
		std::string title = SysNotifyMgr::Instance()->MakeString(9801);
		std::string content = SysNotifyMgr::Instance()->MakeString(9802);
		MailBox::SendSysMail(res.playerUid, sender.c_str(), title.c_str(), content.c_str(), GetReason((LogSourceType)res.reason).c_str(), res.itemRewards);
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(res.playerUid);
	if (!player)
	{
		return;
	}

	player->SendToScene(res);
}
