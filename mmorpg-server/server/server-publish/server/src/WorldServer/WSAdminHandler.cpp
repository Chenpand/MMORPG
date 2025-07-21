#include "WSAdminHandler.h"
#include "WSPlayerMgr.h"
#include "WSRouter.h"
#include "PunishmentMgr.h"
#include "WSApplication.h"
#include "WSActivityMgr.h"

void HandleProtocol(CLProtocol::AdminPlayerGateInfoReq& req)
{
	DebugLogStream << "account(" << req.info.accid << ") request to get gate info." << LogStream::eos;

	CLProtocol::AdminPlayerGateInfoRet ret;
	ret.info = req.info;
	auto player = WSPlayerMgr::Instance()->FindPlayerByAccid(req.info.accid);
	if (player)
	{
		ret.gateId = player->GetGateID();
	}

	if (WSApplication::Instance()->IsCheckGmDataLoadSituation())
	{
		if (!WSActivityMgr::Instance()->IsGmDataLoadOver())
		{
			ret.result = ErrorCode::LOGIN_GM_DATA_NOT_LOADED;
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(100001);
			Router::SendToAdmin(ret);
			return;
		}
	}

	UInt32 forbidLoginRemainTime = PunishmentMgr::Instance()->GetPunishRemainTime(req.info.accid, PUNISH_FORBIDLOGIN);
	if (forbidLoginRemainTime > 0)
	{
		ret.result = ErrorCode::LOGIN_FORBID_LOGIN;
		auto reason = PunishmentMgr::Instance()->GetForbidLoginReason(req.info.accid);
		if (reason == FORBID_LOGIN_CHEAT_FIRST_TIME)
		{
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(8305, SysNotifyMgr::Instance()->TimeToStr(forbidLoginRemainTime).c_str());
		}
		else if (reason == FORBID_LOGIN_CHEAT_FOREVER)
		{
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(8306, SysNotifyMgr::Instance()->TimeToStr(forbidLoginRemainTime).c_str());
		}
		else if (reason == FORBID_LOGIN_CHEAT_TEAM_COPY)
		{
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(8307, SysNotifyMgr::Instance()->TimeToStr(forbidLoginRemainTime).c_str());
		}
		else
		{
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(ErrorCode::LOGIN_FORBID_LOGIN, SysNotifyMgr::Instance()->TimeToStr(forbidLoginRemainTime).c_str());
		}
	}
	else
	{
		ret.result = ErrorCode::SUCCESS;
	}

	Router::SendToAdmin(ret);
}