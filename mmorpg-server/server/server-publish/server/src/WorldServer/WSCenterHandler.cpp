#include "WSCenterHandler.h"
#include "WSPlayerMgr.h"
#include "PunishmentMgr.h"
#include "CLRecordClient.h"
#include "WSSortListMgr.h"
#include "WSRouter.h"
#include "AccountActivityTaskMgr.h"
#include "WSApplication.h"

void HandleProtocol(CLProtocol::CenterForbidTalkListRet& protocol)
{
	PunishmentMgr::Instance()->SetAllForbidTalkList(protocol.players);
}

void HandleProtocol(CLProtocol::CenterNotifyNewForbidTalk& protocol)
{
	PunishmentMgr::Instance()->AddAllForbidTalkPlayer(protocol.accid);
}

void HandleProtocol(CLProtocol::CenterNotifyDelForbidTalk& protocol)
{
	PunishmentMgr::Instance()->DelAllForbidTalkPlayer(protocol.accid);
}


class DBCallback : public CLRecordCallback
{
public:
	DBCallback(){}
	~DBCallback(){}

public:
	void OnFinished()
	{
		while (NextRow())
		{
			WSSortListMgr::Instance()->DeleteAllEntry(GetKey());
		}
	}
};

void HandleProtocol(CLProtocol::CenterForbidAccountNotify& protocol)
{
	for (size_t i = 0; i < protocol.accidList.size(); ++i)
	{
		if (protocol.time != 0)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(protocol.accidList[i]);
			if (player && player->GetGameStatus() == PS_NORMAL)
			{
				player->KickOff(ErrorCode::SYS_FORBIT_LOGIN);
			}
			PunishmentMgr::Instance()->Punish(protocol.accidList[i], PUNISH_FORBIDLOGIN, protocol.time);

			CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			selectCmd->PutCond("accid", protocol.accidList[i]);
			CLRecordClient::Instance()->SendCommand(selectCmd, new DBCallback());
		}
		else
		{
			PunishmentMgr::Instance()->UnPunish(protocol.accidList[i], PUNISH_FORBIDLOGIN);
		}

		
	}
}

void HandleProtocol(CLProtocol::CenterCustomServiceSignRes& res)
{
	InfoLogStream << "recv custom service sign res, role(" << res.roleId << ") sign(" << res.sign << ")." << LogStream::eos;

	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(res.roleId);
	if (!player)
	{
		return;
	}

	CLProtocol::WorldCustomServiceSignRes notify;
	notify.result = res.result;
	notify.sign = res.sign;
	player->SendProtocol(notify);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldCustomServiceSignReq& req)
{
	std::stringstream ss;
	//ss << "user_id=" << player->GetID() << "&user_name=" << player->GetName() + "&";

	CLProtocol::CenterCustomServiceSignReq protocol;
	protocol.platform = player->GetPf();
	protocol.roleId = player->GetID();
	protocol.info = /*ss.str() + */req.info;
	Router::SendToCenter(protocol);
}

void HandleProtocol(CLProtocol::CenterGlobalServerInfoRet& protocol)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (!player)
	{
		ErrorLogStream << "Can not find player(" << protocol.roleId << ")!" << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(player) << "success get global server info(" << protocol.platform << ", " << protocol.openid << ", "
		<< protocol.roleId << ", " << protocol.vipLevel << ")." << LogStream::eos;

	if (AccountActivityTaskMgr::Instance()->IsOppoPlayer(player))
	{
		UInt8 vipLevel = Avalon::StringUtil::ConvertToValue<UInt8>(protocol.vipLevel);

		if (player->GetOppoAmberLevel() == vipLevel)
		{
			return;
		}

		InfoLogStream << PLAYERINFO(player) << "oppo vip level(" << vipLevel << ")." << LogStream::eos;

		player->SetOppoAmberLevel(vipLevel);
		player->SyncEventToScene(SET_OPPO_VIP_LEVEL, vipLevel);
		AccountActivityTaskMgr::Instance()->UpdateAmberPrivilegeTask(player);
		AccountActivityTaskMgr::Instance()->UpdateOppoVipLevelTask(player);
	}
}

void HandleProtocol(CLProtocol::CenterQueryConvertAccountRes& protocol)
{
	if (protocol.fromGate)
	{
		Router::BroadcastToGate(protocol);
		return;
	}

	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.roleid);
	if (!player)
	{
		return;
	}

	if (protocol.account == "" || protocol.password == "")
	{
		return;
	}

	std::string sender = SysNotifyMgr::Instance()->MakeString(9400);
	std::string title = SysNotifyMgr::Instance()->MakeString(9401);
	std::string content = SysNotifyMgr::Instance()->MakeString(9402, player->GetName(), WSApplication::Instance()->GetDownloadUrl(), protocol.account, protocol.password);
	std::vector<ItemReward> reward;

	MailBox::SendSysMail(protocol.roleid, sender, title, content, GetReason(SOURCE_TYPE_GM), reward);
}