#include "GSLeaveGameHandler.h"
#include <AvalonLogger.h>
#include <CLErrorCode.h>
#include "GSPlayerMgr.h"
#include "GSApplication.h"
#include "GSRouter.h"
#include "GSAccountMgr.h"

void HandleProtocol(GSPlayer* player,CLProtocol::GateLeaveGameReq& protocol)
{
	InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName()
		<< ") require leave game." << LogStream::eos;
	player->NotifyLeaveGame(player->GetID());
	player->Disconnect();
}

void HandleProtocol(CLProtocol::GateKickOffPlayer& protocol)
{
	GSPlayer* player = NULL;
	if(protocol.accid != 0)
	{
		player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accid);
	}
	else
	{
 		player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	}
	if(player == NULL) return;

	switch (protocol.errorCode)
	{
	case ErrorCode::LOGIN_ANTIADDICT_NIGHT:
	case ErrorCode::LOGIN_ANTIADDICT_TIPS_COMMON:
	case ErrorCode::LOGIN_ANTIADDICT_TIPS_HOLIDAY:
	case ErrorCode::LOGIN_ANTIADDICT_TOURIST_LOGIN:
	case ErrorCode::LOGIN_ANTIADDICT_TOURIST_ONLINE:
		break;
	default:
	{
		CLProtocol::GateNotifyKickoff notify;
		notify.errorCode = protocol.errorCode;
		notify.msg = protocol.msg;
		player->SendProtocol(notify);
	}
	}

	InfoLogStream << "player(" << player->GetID() << "," << player->GetName() <<
		") is kicked off for error " << protocol.errorCode << "." << LogStream::eos;
	player->Disconnect();
	player->NotifyLeaveGame(player->GetID());

	// 踢人情况下把缓存的包都清掉
	if (GSApplication::Instance()->IsClearPacketWhenKick())
	{
		player->ClearPacketBuffer();
	}
}

void HandleProtocol(CLProtocol::CenterPlayerAddictTimeSync& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.roleid);
	if (player != NULL)
	{

		GSAccountMgr::Instance()->UpdateAntiAddictTime(protocol.accid, protocol.addictTime);

		InfoLogStream << "player(roleid: " << protocol.roleid << ", platform: " << protocol.platform << ", openid: "
			<< protocol.openid << ") set addict time(" << protocol.addictTime << ") on gate player." << LogStream::eos;
	}

	Router::SendToCenter(protocol);
}