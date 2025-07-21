#include "WorldRedPacketHandler.h"
#include "WSPlayerMgr.h"
#include "RedPacketMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendRedPacketReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldSendRedPacketRes ret;
	ret.result = RedPacketMgr::Instance()->OnPlayerSendRedPacket(player->GetID(), req.id, req.num);
	player->SendProtocol(ret);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOpenRedPacketReq& req)
{
	if (!player)
	{
		return;
	}
	RedPacketMgr::Instance()->OnPlayerOpenRedPacket(player, req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendCustomRedPacketReq& req)
{
	auto result = RedPacketMgr::Instance()->OnPlayerSendCustomRedPacket(player, req.reason, req.name, req.num);
	if (result != ErrorCode::SUCCESS)
	{
		player->OnSendRedPacketFailure(result);
	}
}

void HandleProtocol(CLProtocol::SceneAddRedPacketReq& req)
{
	auto player = WSPlayerMgr::Instance()->FindPlayer(req.id);
	if (!player)
	{
		ErrorLogStream << "player(" << req.id << ") add red packet(" << req.reason << ") but offline." << LogStream::eos;
		return;
	}

	ObjID_t redPacketId = 0;
	if (RedPacketMgr::Instance()->OnPlayerAddRedPacket(player, req.reason, "", redPacketId) != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << req.id << ") add red packet(" << req.reason << ") failed." << LogStream::eos;
		return;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetGuildRedPacketInfoReq& req)
{
	CLProtocol::WorldGetGuildRedPacketInfoRes res;
	res.code = RedPacketMgr::Instance()->GetGuildRedPacketInfo(player, res.infos);
	player->SendProtocol(res);
}