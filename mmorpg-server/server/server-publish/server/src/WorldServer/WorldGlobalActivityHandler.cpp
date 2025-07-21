#include "WorldGlobalActivityHandler.h"

#include <CLSystemValueDataEntry.h>
#include <CLRecordClient.h>
#include <CLOpActiveProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <udplog_typedef.h>

#include "AnnouncementMgr.h"
#include "MailBox.h"
#include "WSPlayerMgr.h"
#include "WSActivityMgr.h"
#include "GameParamMgr.h"

void HandleProtocol(CLProtocol::GASSendAnnounceReq& protocol)
{
	for (auto announceData : protocol.announcements)
	{
		AnnouncementMgr::Instance()->SaveAnnouncement(announceData.id, announceData.begintime, announceData.interval, announceData.times, announceData.content, announceData.isSave > 0 ? true : false, announceData.gmId);
	}
}

void HandleProtocol(CLProtocol::GASSendMailReq& protocol)
{
	UInt32 ret = MailBox::SendSysMail(DecodeRoleIdFromGASPlayerUId(protocol.receiverUId), protocol.senderName, protocol.title, protocol.content, protocol.reason, protocol.rewards);

	std::vector<std::string> params;
	Avalon::StringUtil::Split(protocol.reason, params, ";");
	UInt32 reason_id = 0;
	try
	{
		reason_id = std::atoi(params[0].c_str());
	}
	catch (...)
	{
		ErrorLogStream << "world mailbox parse reason failed!!!" << LogStream::eos;
	}

	// 如果是赌马奖励，需要给个globalActivity回复成功
	if (SOURCE_TYPE_BET_HORSE_REWARD == reason_id && ret == ErrorCode::SUCCESS)
	{
		CLProtocol::GASBetHorseMailRewardRes BetHorseMailRes;
		BetHorseMailRes.playerUid = protocol.receiverUId;
		Router::SendToGlobalActivity(BetHorseMailRes);
	}
}

void HandleProtocol(CLProtocol::GASActivitySync& protocol)
{
	DebugLogStream << "Sync activities from global activity..." << LogStream::eos;

	WSActivityMgr::Instance()->GlobalActivitySync(protocol.opActs);
}

void HandleProtocol(CLProtocol::GASWholeBargainDiscountSync& protocol)
{
	InfoLogStream << "whole bargain discount sync:" << protocol.discount << LogStream::eos;

	GameParamMgr::Instance()->SetValue(WHOLE_BARGAIN_DISCOUNT, protocol.discount);
	CLProtocol::SceneGameParamSync sync;
	sync.gameParamMap[WHOLE_BARGAIN_DISCOUNT] = protocol.discount;
	Router::BroadcastToScene(sync);
}