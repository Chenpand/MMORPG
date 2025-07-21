#include "InviteMgr.h"

#include <CLInviteGameProtocol.h>
#include <CLGiftBagDataEntry.h>
#include <CLSysNotify.h>

#include "SSRouter.h"
#include "Player.h"
#include "ActivityMgr.h"

InviteMgr::InviteMgr()
	:m_pOwner(NULL)
{
	m_GetGoldAwardTime = 0;
}

InviteMgr::~InviteMgr()
{
}

void InviteMgr::SendDailyInfo()
{
	CLProtocol::SceneInviteDailyInfoRet ret;
	ret.inviteTimes = GetOwner()->GetCounter("invite_times");
	ret.inviteGold = GetOwner()->GetCounter("invite_gold");
	ret.giftbagFlag = 0;
	for(int i = 0; i <= 30; ++i)
	{
		if(CheckGiftbagGot(i)) ret.giftbagFlag |= ( 1 << i );
	}
	GetOwner()->SendProtocol(ret);
}

void InviteMgr::GetGiftbag(UInt8 type)
{
	
}

void InviteMgr::GetRecallAward(UInt32 uid)
{
	CLProtocol::SocialInviteCheckRecallAwardReq req;
	req.nodeId = NODE_ID;
	req.inviterId = GetOwner()->GetAccID();
	req.playerId = GetOwner()->GetID();
	req.inviteeId = uid;
	Router::SendToSocial(req);
}

void InviteMgr::GetConsumeAward()
{
	UInt32 todayGoldAward = GetOwner()->GetCounter("invite_gold");
	if(todayGoldAward >= MAX_DAILY_GOLDAWARD) return;

	if(CURRENT_TIME.Sec() < m_GetGoldAwardTime + 3) return;
	m_GetGoldAwardTime = UInt32(CURRENT_TIME.Sec());

	CLProtocol::SocialInviteCheckConsumeAwardReq req;
	req.nodeId = NODE_ID;
	req.inviterId = GetOwner()->GetAccID();
	req.playerId = GetOwner()->GetID();
	req.maxGold = MAX_DAILY_GOLDAWARD - todayGoldAward;
	Router::SendToSocial(req);
}

void InviteMgr::RequestInviteeList()
{
	CLProtocol::SocialInviteInviteeListReq req;
	req.nodeId  = NODE_ID;
	req.inviterId = GetOwner()->GetAccID();
	req.playerId = GetOwner()->GetID();
	Router::SendToSocial(req);
}

void InviteMgr::OnOnline()
{
	SendDailyInfo();

	if(ActivityMgr::Instance()->IsInActivity(GetOwner(), "kl_activity"))
	{
		CLProtocol::SocialInviteCheckKLActivityReq req;
		req.nodeId = ZONE_ID;
		req.accId = GetOwner()->GetAccID();
		req.playerId = GetOwner()->GetID();
		Router::SendToSocial(req);
	}
}

void InviteMgr::OnInvitFriend()
{
	GetOwner()->IncCounter("invite_times", 1);
	SendDailyInfo();
}

void InviteMgr::OnCheckGiftbagCondRet(UInt8 type)
{
	
}

void InviteMgr::OnCheckRecallAwardRet(UInt32 num)
{
	GetOwner()->AddGold("invite_game", INVITEE_RECALL_SILVER * num);
}

void InviteMgr::OnCheckConsumeAwardRet(UInt32 gold, UInt32 totalGold)
{
	if(gold == 0) return;

	UInt32 todayGold = GetOwner()->GetCounter("invite_gold");
	if(todayGold >= MAX_DAILY_GOLDAWARD) return;

	if(todayGold + gold > MAX_DAILY_GOLDAWARD)
	{
		todayGold = MAX_DAILY_GOLDAWARD;
		gold = MAX_DAILY_GOLDAWARD - todayGold;
	}
	else
	{
		todayGold += gold;
	}

	GetOwner()->SetCounter("invite_gold", todayGold);

	GetOwner()->AddBindGold("invite_game", gold);

	CLProtocol::SceneInviteGetConsumeAwardRet ret;
	ret.todayGoldAward = todayGold;
	ret.totalGold = totalGold;
	GetOwner()->SendProtocol(ret);
}

bool InviteMgr::CheckGiftbagGot(UInt8 type)
{
	if(type <= 5)
	{
		UInt32 dailyBoxFlag = GetOwner()->GetCounter("dailybox_flag");
		if(dailyBoxFlag & (1 << (type + DAILY_GIFTBAG_INVITE))) return true;
	}
	else if((type >= 11 && type <= 21) || (type >= 25 && type <= 26))
	{
		UInt32 onceBoxFlag = GetOwner()->GetCounter("oncebox_flag");
		if(onceBoxFlag & ( 1 << (type - 10))) return true;
	}

	return false;
}

void InviteMgr::SetGiftbagGot(UInt8 type)
{
	if(type <= 5)
	{
		UInt32 dailyBoxFlag = GetOwner()->GetCounter("dailybox_flag");
		dailyBoxFlag = dailyBoxFlag | (1 << (type + DAILY_GIFTBAG_INVITE));
		GetOwner()->SetCounter("dailybox_flag", dailyBoxFlag);
	}
	else if((type >= 11 && type <= 21) || (type >= 25 && type <= 26))
	{
		UInt32 onceBoxFlag = GetOwner()->GetCounter("oncebox_flag");
		onceBoxFlag = onceBoxFlag | (1 << (type - 10));
		GetOwner()->SetCounter("oncebox_flag", onceBoxFlag);
	}
}

UInt32 InviteMgr::GetGiftbagID(UInt8 type)
{
	if(type == 0) return 200;
	else if(type >= 1 && type <= 5) return 200 + type;
	else if(type >= 11 && type <= 21) return 200 + type;
	else if(type >= 25 && type <= 26) return 30 + type;
	return 0;
}

void InviteMgr::GiveMailGiftbag(const char* name, RewardGroup* rewardGroup)
{
	if(rewardGroup == NULL) return;

	if(ErrorCode::SUCCESS != GetOwner()->CheckAddRewards(rewardGroup))
	{
		char content[512];
		SysNotifyMgr::Instance()->MakeNotify(409, content, 512);
		//GetOwner()->SendSysMail(name, content, 0, rewardGroup);
	}
	else
	{
		//GetOwner()->AddRewards(GetReason(SOURCE_TYPE_INVALID).c_str(), rewardGroup);
		//GetOwner()->AddRewards("giftbag", rewardGroup);
	}
} 
