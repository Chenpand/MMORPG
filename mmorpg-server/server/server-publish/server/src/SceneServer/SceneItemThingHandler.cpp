#include "SceneItemThingHandler.h"

#include <CLErrorCode.h>

#include "PlayerMgr.h"
#include "ItemThingMgr.h"

void HandleProtocol(CLProtocol::SceneItemThingStartReq& req)
{
	CLProtocol::SceneItemThingStartRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.serialId = req.serialId;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	do 
	{
		if (NULL == player)
		{
			ErrorLogStream << "not find player" << req.roleId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			return;
		}

		InfoLogStream << PLAYERINFO(player) << " recv serial id:" << req.serialId << LogStream::eos;

		res.retCode = ItemThingMgr::Instance()->StartThing(player, req.serialId, req.itemCost, req.reason, res.itemThingGuid);
	} while (0);

	if (req.servType == ST_UNION)
	{
		 player->SendToUnion(UnionServerFunctionType(req.param), res);
	}
	else
	{
		req.SendBack(res);
	}
}

void HandleProtocol(CLProtocol::SceneItemThingStartCancel& req)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	if (NULL == player)
	{
		ErrorLogStream << "not find player" << req.roleId << LogStream::eos;
		return;
	}

	InfoLogStream << PLAYERINFO(player) << " cancel item thing serial id:" << req.serialId << LogStream::eos;

	ItemThingMgr::Instance()->CancelThing(player, req.serialId);
}

void HandleProtocol(CLProtocol::SceneItemThingCommitReq& req)
{
	CLProtocol::SceneItemThingCommitRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.itemThingGuid = req.itemThingGuid;

	Player* player = PlayerMgr::Instance()->FindNormalPlayer(req.roleId);

	do
	{
		if (NULL == player)
		{
			ErrorLogStream << "not find player" << req.roleId << " itemthingId:" << req.itemThingGuid
				<< " isCommit:" << req.isCommit << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			return;
		}

		// 确认提交事务
		if (req.isCommit != 0)
		{
			res.retCode = ItemThingMgr::Instance()->CommitThing(player, req.itemThingGuid);
		}
		else
		{	// 回滚事务，返还扣除的消耗
			res.retCode = ItemThingMgr::Instance()->RollBackThing(player, req.itemThingGuid);
		}

	} while (0);

	if (req.servType == ST_UNION)
	{
		player->SendToUnion(UnionServerFunctionType(req.param), res);
	}
	else
	{
		req.SendBack(res);
	}
}