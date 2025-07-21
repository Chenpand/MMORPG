#include "SceneInviteHandler.h"

#include <CLItemDataEntry.h>
#include <CLSysNotify.h>
#include "PlayerMgr.h"
#include "Player.h"

#include "SSRouter.h"

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteDailyInfoReq &protocol)
{
	pPlayer->GetInviteMgr().SendDailyInfo();
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteInviteeListReq& protocol)
{
	pPlayer->GetInviteMgr().RequestInviteeList();
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteNotifyInvited& protocol)
{
	pPlayer->GetInviteMgr().OnInvitFriend();
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetGiftbag& protocol)
{
	pPlayer->GetInviteMgr().GetGiftbag(protocol.type);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetRecallAwardReq& protocol)
{
	pPlayer->GetInviteMgr().GetRecallAward(protocol.uid);
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteGetConsumeAwardReq& protocol)
{
	pPlayer->GetInviteMgr().GetConsumeAward();
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneInviteBindKLPlayer& protocol)
{
	CLProtocol::SocialInviteBindKLActivity req;
	req.nodeId = NODE_ID;
	req.accId = pPlayer->GetAccID();
	req.playerId = pPlayer->GetID();
	req.level = pPlayer->GetLevel();
	req.openid = pPlayer->GetOpenId();
	req.klopenid = protocol.klopenid;
	Router::SendToSocial(req);
}

void HandleProtocol(Player* pPlayer, CLProtocol::SceneInviteExchangeCDKey& protocol)
{
	if(protocol.cdkey.length() > 32) return;
	if(pPlayer->GetOpenId().size() < 5) return;

	CLProtocol::SceneInviteBeginExchangeCDKeyReq req;
	req.nodeId = NODE_ID;
	req.playerId = pPlayer->GetID();
	req.cdkey = protocol.cdkey;
	Router::SendToSocial(req);
}

void HandleProtocol(CLProtocol::SocialInviteInviteeListRet& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		CLProtocol::SceneInviteInviteeListRet ret;
		ret.totalGold = protocol.totalGold;
		ret.invitees = protocol.invitees;
		player->SendProtocol(ret);
	}
}

void HandleProtocol(CLProtocol::SocialInviteCheckGiftbagRet& protocol)
{
	if(protocol.result == 0) return;
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		player->GetInviteMgr().OnCheckGiftbagCondRet(protocol.type);
	}
}

void HandleProtocol(CLProtocol::SocialInviteCheckRecallAwardRet& protocol)
{
	if(protocol.invitees.size() == 0) return;
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		player->GetInviteMgr().OnCheckRecallAwardRet(protocol.invitees.size());

		for(std::vector<CLProtocol::InviteeInfo>::iterator iter = protocol.invitees.begin();
			iter != protocol.invitees.end(); ++iter)
		{
			CLProtocol::SceneInviteSyncInviteeData sync;
			sync.info = *iter;
			player->SendProtocol(sync);
		}
	}
}

void HandleProtocol(CLProtocol::SocialInviteCheckConsumeAwardRet& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		player->GetInviteMgr().OnCheckConsumeAwardRet(protocol.gold, protocol.totalGold);
	}
}

void HandleProtocol(CLProtocol::SocialInviteCheckKLActivityRet& protocol)
{
	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player != NULL)
	{
		CLProtocol::SceneInviteSyncKLActivity sync;
		sync.klopenid = protocol.klopenid;
		sync.finishFlag = protocol.finishFlag;
		player->SendProtocol(sync);
	}
}

void HandleProtocol(CLProtocol::SceneInviteBeginExchangeCDKeyRet& protocol)
{
// 	Player* player = PlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
// 	if(player == NULL) return;
// 
// 	ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(protocol.itemId);
// 	if(itemDataEntry == NULL || itemDataEntry->effectParam[2] > 30)
// 	{
// 		player->SendNotify(217);
// 		return;
// 	}
// 
// 	UInt32 type = itemDataEntry->effectParam[1];
// 	UInt32 serialNo = itemDataEntry->effectParam[2];
// 	UInt32 extraItemId = itemDataEntry->effectParam[3];
// 
// 	if(type == 1)
// 	{
// 		UInt32 flag = player->GetCounter("cdkey_daily");
// 		if((flag & (1 << serialNo)) != 0)
// 		{
// 			player->SendNotify(221);
// 			return;
// 		}
// 		player->SetCounter("cdkey_daily", flag | (1 << serialNo));
// 	}
// 	else if(type == 2)
// 	{
// 		UInt32 flag = player->GetCounter("cdkey_monthly");
// 		if((flag & (1 << serialNo)) != 0)
// 		{
// 			player->SendNotify(222);
// 			return;
// 		}
// 		player->SetCounter("cdkey_monthly", flag | (1 << serialNo));
// 	}
// 	else if(type == 3)
// 	{
// 		UInt32 flag = player->GetCounter("cdkey_forever");
// 		if((flag & (1 << serialNo)) != 0)
// 		{
// 			player->SendNotify(220);
// 			return;
// 		}
// 		player->SetCounter("cdkey_forever", flag | (1 << serialNo));
// 	}
// 	else
// 	{
// 		if(protocol.itemId == 322)
// 		{
// 			if(player->GetCounter("3366niudanji") >= 5)
// 			{
// 				player->SendNotify(218, 5);
// 				return;
// 			}
// 
// 			player->IncCounter("3366niudanji", 1);
// 		}
// 		else if(protocol.itemId == 323)
// 		{
// 			if(player->GetCounter("3366lottery") >= 5)
// 			{
// 				player->SendNotify(218, 5);
// 				return;
// 			}
// 
// 			player->IncCounter("3366lottery", 1);
// 		}
// 		else return;
// 	}
// 
// 	char content[512];
// 	memset(content, 0, sizeof(content));
// 	SysNotifyMgr::Instance()->MakeNotify(219, content, 512, itemDataEntry->name.c_str());
// 
// 	RewardGroup* pGroup = player->GetRewardGroup();
// 	pGroup->AddReward(protocol.itemId, 1, 1, 0);
// 	player->SendSysMail(itemDataEntry->name.c_str(), content, 0, pGroup);
// 
// 	if(extraItemId != 0 && (player->GetQQVipInfo().IsVip() || player->GetQQVipInfo().IsSuperYellow()))
// 	{
// 		ItemDataEntry* extraItemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(extraItemId);
// 		if(extraItemDataEntry != NULL)
// 		{
// 			SysNotifyMgr::Instance()->MakeNotify(219, content, 512, extraItemDataEntry->name.c_str());
// 
// 			RewardGroup* pGroup = player->GetRewardGroup();
// 			pGroup->AddReward(extraItemId, 1, 1, 0);
// 			player->SendSysMail(extraItemDataEntry->name.c_str(), content, 0, pGroup);
// 		}
// 	}
// 
// 	CLProtocol::SceneInviteEndExchangeCDKeyReq	req;
// 	req.cdkey = protocol.cdkey;
// 	req.openId = player->GetOpenId();
// 	Router::SendToSocial(req);
// 
// 	player->SendNotify(223);
// 
// 	GameInfoLogStream("Property") << "player(" << player->GetAccID() << "," << player->GetID() << "," << player->GetName() << ")"
// 		<< " use cdkey:" << req.cdkey << "|" << protocol.itemId << LogStream::eos;
}

