#include "SocialInviteHandler.h"

#include "OnlineStateMgr.h"
#include "InviteDataMgr.h"
#include "OSNetwork.h"
#include "CDKeyMgr.h"

void HandleProtocol(CLProtocol::SocialInviteReportOnline& protocol)
{
	for(std::vector<UInt32>::iterator iter = protocol.players.begin();
		iter != protocol.players.end(); ++iter)
	{
		OnlineStateMgr::Instance()->ReportOnline(*iter);
	}
}

void HandleProtocol(CLProtocol::SocialInviteNotifyLevelup& protocol)
{
	InviteDataMgr::Instance()->OnPlayerLevelup(protocol.accId, protocol.level);

	InviteDataMgr::Instance()->CheckKLActivity(protocol.GetConnID(), protocol.nodeId, protocol.accId, protocol.playerId);
}

void HandleProtocol(CLProtocol::SocialInviteNotifyConsume& protocol)
{
	InviteDataMgr::Instance()->OnPlayerConsume(protocol.uid, protocol.gold);
}

void HandleProtocol(CLProtocol::SocialNotifyInviteeEnterGame& protocol)
{
	InviteDataMgr::Instance()->OnInviteeEnterGame(protocol.invitee, protocol.openid, protocol.inviter);
}

void HandleProtocol(CLProtocol::SocialInviteInviteeListReq& protocol)
{
	CLProtocol::SocialInviteInviteeListRet ret;
	ret.playerId = protocol.playerId;
	if(!InviteDataMgr::Instance()->GetInviteeList(protocol.inviterId, ret.invitees, ret.totalGold)) return;

	OSNetwork::Instance()->TransmitProtocol(protocol.GetConnID(), protocol.nodeId, ret);
	InviteDataMgr::Instance()->CheckKLActivity(protocol.GetConnID(), protocol.nodeId, protocol.inviterId, protocol.playerId);
}

void HandleProtocol(CLProtocol::SocialInviteCheckGiftbagReq& protocol)
{
	CLProtocol::SocialInviteCheckGiftbagRet ret;
	ret.playerId = protocol.playerId;
	ret.type = protocol.type;
	ret.result = 0;
	if(InviteDataMgr::Instance()->CheckGiftbagCond(protocol.inviterId, protocol.type, protocol.minLevel, protocol.maxLevel, protocol.playerNum))
	{
		ret.result = 1;
	}

	OSNetwork::Instance()->TransmitProtocol(protocol.GetConnID(), protocol.nodeId, ret);
}

void HandleProtocol(CLProtocol::SocialInviteCheckRecallAwardReq& protocol)
{
	CLProtocol::SocialInviteCheckRecallAwardRet ret;
	ret.playerId = protocol.playerId;
	if(!InviteDataMgr::Instance()->CheckRecallAwardCond(protocol.inviterId, protocol.inviteeId, ret.invitees)) return;

	OSNetwork::Instance()->TransmitProtocol(protocol.GetConnID(), protocol.nodeId, ret);
}

void HandleProtocol(CLProtocol::SocialInviteCheckConsumeAwardReq& protocol)
{
	CLProtocol::SocialInviteCheckConsumeAwardRet ret;
	ret.playerId = protocol.playerId;

	if(!InviteDataMgr::Instance()->CheckConsumeAward(protocol.inviterId, protocol.maxGold, ret.gold, ret.totalGold)) return;

	OSNetwork::Instance()->TransmitProtocol(protocol.GetConnID(), protocol.nodeId, ret);
}

void HandleProtocol(CLProtocol::SocialInviteBindKLActivity& protocol)
{
	if(!InviteDataMgr::Instance()->BindKLPlayer(protocol.accId, protocol.openid, protocol.level, protocol.klopenid)) return;

	InviteDataMgr::Instance()->CheckKLActivity(protocol.GetConnID(), protocol.nodeId, protocol.accId, protocol.playerId);
}

void HandleProtocol(CLProtocol::SocialInviteCheckKLActivityReq& protocol)
{
	InviteDataMgr::Instance()->CheckKLActivity(protocol.GetConnID(), protocol.nodeId, protocol.accId, protocol.playerId);
}

void HandleProtocol(CLProtocol::SceneInviteBeginExchangeCDKeyReq& protocol)
{
	UInt32 itemId = CDKeyMgr::Instance()->BeginExchangeCDKey(protocol.cdkey);
	
	CLProtocol::SceneInviteBeginExchangeCDKeyRet ret;
	ret.cdkey = protocol.cdkey;
	ret.playerId = protocol.playerId;
	ret.itemId = itemId;
	OSNetwork::Instance()->TransmitProtocol(protocol.GetConnID(), protocol.nodeId, ret);
}

void HandleProtocol(CLProtocol::SceneInviteEndExchangeCDKeyReq& protocol)
{
	CDKeyMgr::Instance()->EndExchangeCDKey(protocol.cdkey, protocol.openId);
}
