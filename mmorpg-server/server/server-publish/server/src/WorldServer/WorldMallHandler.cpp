#include "WorldMallHandler.h"
#include "MallMgr.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallBuy &req)
{
	MallMgr::Instance()->Buy(pPlayer, req.m_uId, req.m_uNum);
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQueryItemReq &req)
{
	MallMgr::Instance()->OnQueryItems(pPlayer, req.m_tagType, req.m_type, req.m_subType, req.m_job, req.m_moneyType, req.m_IsPersonalTailor);
}


void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQueryItemDetailReq& protocol)
{
	CLProtocol::WorldMallQueryItemDetailRet ret;
	MallMgr::Instance()->OnQueryItemDetail(protocol.m_MallItemId, ret.details);
	pPlayer->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::WorldMallCheckBuyRet &ret)
{
	MallMgr::Instance()->OnCheckBuyRet(ret.m_uPlayerId, ret.m_uRet, ret.m_uMallItemId, ret.m_getItemUid, ret.m_uMallItemNum, ret.m_uReceiver, ret.m_restLimitNum);
}

void HandleProtocol(CLProtocol::SWRefreshLimiteBuyNum &protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerid);
	if (!player)
	{
		return;
	}

	if (protocol.isQuerySingleGoods == 0)
	{
		CLProtocol::WorldMallQueryItemRet ret;
		ret.mallType = protocol.mallType;

		MallMgr::Instance()->GetQueryItems(player, protocol, ret.m_items);
		player->SendProtocol(ret);
	}
	else
	{
		CLProtocol::WorldMallQuerySingleItemRes ret;
		ret.retCode = ErrorCode::MALL_CANNOT_FIND_MALL_GOODS;

		if (protocol.qgs.empty())
		{
			player->SendProtocol(ret);
			return;
		}

		std::vector<MallItemInfo> mallItems;
		MallMgr::Instance()->GetQueryItems(player, protocol, mallItems);

		if (mallItems.empty())
		{
			player->SendProtocol(ret);
			return;
		}

		ret.retCode = ErrorCode::SUCCESS;
		ret.mallItemInfo = mallItems.front();
		player->SendProtocol(ret);
	}
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::CWMallBatchBuyReq& protocol)
{
	MallMgr::Instance()->BatchBuy(pPlayer, protocol._buyItems);
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallGiftPackActivateReq& protocol)
{
	pPlayer->GetMallGiftPackMgr().ActivateLimitGiftPack(protocol.m_GiftPackActCond);
}

void HandleProtocol(CLProtocol::MallGiftPackActivateReq& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.m_PlayerId);
	if (!player)
	{
		return;
	}

	player->GetMallGiftPackMgr().ActivateLimitGiftPack(protocol.m_GiftPackActCond);
}

void HandleProtocol(CLProtocol::MallActivateCondReq &protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.m_RoleId);
	if (!player)
	{
		ErrorLogStream << "Failed to find normal player(roleId: " << protocol.m_RoleId << ")!" << LogStream::eos;
		return;
	}

	player->GetCondActivateMgr().ActivateCond(protocol.m_ActivateCond, protocol.m_ActivateType);
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldGetMallItemByItemIdReq &protocol)
{
	MallMgr::Instance()->GetMallItemByItemId(pPlayer, protocol.m_ItemId);
}

void HandleProtocol(WSPlayer *pPlayer, CLProtocol::WorldMallQuerySingleItemReq &protocol)
{
	MallMgr::Instance()->OnQueryItem(pPlayer, protocol.mallItemId);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldGetRechargePushItemsReq& protocol)
{
	MallMgr::Instance()->GetRechargePushItems(pPlayer);
}

void HandleProtocol(WSPlayer* pPlayer, CLProtocol::WorldBuyRechargePushItemReq& protocol)
{
	MallMgr::Instance()->BuyRechargePushItem(pPlayer, protocol.pushId);
}

void HandleProtocol(CLProtocol::WorldCheckRechargePushRes& protocol)
{
	WSPlayer* pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!pPlayer)
	{
		ErrorLogStream << "Failed to find normal player(roleId: " << protocol.roleId << ")!" << LogStream::eos;
		return;
	}

	MallMgr::Instance()->OnCheckRechargePushRes(pPlayer, protocol.retCode, protocol.pushId);
}

void HandleProtocol(CLProtocol::WorldSceneBuyRechargePushItemRes& protocol)
{
	WSPlayer* pPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.roleId);
	if (!pPlayer)
	{
		ErrorLogStream << "Failed to find normal player(roleId: " << protocol.roleId << ")!" << LogStream::eos;
		return;
	}

	MallMgr::Instance()->OnSceneBuyRechargePushItemRes(pPlayer, protocol.retCode, protocol.pushId);
}

