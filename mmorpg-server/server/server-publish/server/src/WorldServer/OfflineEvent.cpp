#include "OfflineEvent.h"

#include <iomanip>
#include <AvalonDate.h>
#include <AvalonTime.h>

#include <CLRecordClient.h>
#include <CLSysNotify.h>
#include <CLGoldConsignmentProtocol.h>

#include "WSPlayer.h"
#include "CLFunctionDataEntry.h"

void ImpeachFailedForLeaveEvent::OnTriggered(WSPlayer* player)
{
	player->SendNotify(873,	m_TribeName.c_str());
}


void ::ImpeachFailedForOnlineEvent::OnTriggered(WSPlayer* player)
{
	Avalon::Time time(UInt64(m_OnlineTime) * 1000);
	Avalon::Date date(time);
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(4) << date.Year() << "/"
		<< std::setfill('0') << std::setw(2) << date.Month() << "/"
		<< std::setfill('0') << std::setw(2) << date.Day() << " "
		<< std::setfill('0') << std::setw(2) << date.Hour() << ":"
		<< std::setfill('0') << std::setw(2) << date.Min();  
	player->SendNotify(875, oss.str().c_str());
}

std::string ImpeachFailedForOnlineEvent::Encode() const
{
	std::ostringstream oss;
	oss << m_OnlineTime;
	return oss.str();
}

void ImpeachFailedForOnlineEvent::Decode(const std::string& str)
{
	std::istringstream iss(str);
	iss >> m_OnlineTime;
}

void BeImpeachedEvent::OnTriggered(WSPlayer* player)
{
	player->SendNotify(876);
}

void BeTribeLeaderEvent::OnTriggered(WSPlayer* player)
{
	player->SendNotify(877, m_TribeName.c_str());
}

void FireTribeEvent::OnTriggered(WSPlayer* player)
{
	player->SendNotify(832, m_TribeName.c_str());
}

void IncIntimacyEvent::OnTriggered(WSPlayer *pPlayer)
{
	pPlayer->AddIntimacy(m_uFriendId, m_uIntimacy);
}

std::string IncIntimacyEvent::Encode() const
{
	std::ostringstream	oss;
	oss << m_uFriendId << ',' << m_uIntimacy;
	return oss.str();
}

void IncIntimacyEvent::Decode(const std::string &str)
{
	std::istringstream	iss(str);
	char	split = 0;
	iss >> m_uFriendId >> split >> m_uIntimacy;
}

void AddMallItemBuyCount::OnTriggered(WSPlayer *pPlayer)
{
	UInt16	uBuyedNum = pPlayer->GetMallItemBuyedNum(m_uItemId);
	pPlayer->SetMallItemBuyedNum(m_uItemId, uBuyedNum + m_uNum);
}

std::string AddMallItemBuyCount::Encode() const
{
	std::ostringstream os;
	os << m_uItemId << ',' << m_uNum;
	return os.str();
}

void AddMallItemBuyCount::Decode(const std::string &str)
{
	std::istringstream is(str);
	char split = 0;
	is >> m_uItemId >> split >> m_uNum;
}

void TitleEvent::OnTriggered(WSPlayer *pPlayer)
{
	// 过期了就不用管
	if ((UInt32)CURRENT_TIME.Sec() > m_ExpireTime)
	{
		return;
	}

	pPlayer->GetTitleMgr().AddOneTitleByTitleId(m_TitleId, m_ExpireTime);
}

std::string TitleEvent::Encode() const
{
	std::ostringstream os;
	os << m_TitleId << ',' << m_ExpireTime;
	return os.str();
}

void TitleEvent::Decode(const std::string &str)
{
	std::istringstream is(str);
	char split = 0;
	is >> m_TitleId >> split >> m_ExpireTime;
}

void GoldConsignmentPointEvent::OnTriggered(WSPlayer *pPlayer)
{
	if ((UInt32)CURRENT_TIME.Sec() > m_redPointTime)
	{
		return;
	}

	// 发个消息通知玩家
	CLProtocol::SceneGoldConsignmentDealNotify notify;
	notify.roleId = pPlayer->GetID();
	notify.redPointTime = m_redPointTime;
	pPlayer->SendProtocol(notify);
}

std::string GoldConsignmentPointEvent::Encode() const
{
	std::ostringstream os;
	os << m_redPointTime;
	return os.str();
}

void GoldConsignmentPointEvent::Decode(const std::string &str)
{
	std::istringstream is(str);
	is >> m_redPointTime;
}

OfflineEvent::OfflineEvent()
{
	m_Time = UInt32(CURRENT_TIME.Sec());
	m_tarUid = 0;
}

OfflineEvent::~OfflineEvent()
{
}

#define BEGINE_REGISTER_EVENT() \
	switch(type) { \

#define REGISTER_EVENT(Event) \
	case Event::TYPE: return new Event();

#define END_REGISTER_EVENT() \
	default: \
		ErrorLogStream << "unknow offline event " << type << "!" << LogStream::eos; \
		break; \
	};
	

OfflineEvent* OfflineEvent::Create(UInt16 type)
{
	BEGINE_REGISTER_EVENT()
	REGISTER_EVENT(ImpeachFailedForLeaveEvent)
	REGISTER_EVENT(ImpeachFailedForOnlineEvent)
	REGISTER_EVENT(BeImpeachedEvent)
	REGISTER_EVENT(BeTribeLeaderEvent)
	REGISTER_EVENT(FireTribeEvent)
	REGISTER_EVENT(IncIntimacyEvent)
	REGISTER_EVENT(InviteFriendEvent)
	REGISTER_EVENT(SellAuctionEvent)
	REGISTER_EVENT(SendPrivateChatEvent)
	REGISTER_EVENT(TitleEvent)
	REGISTER_EVENT(GoldConsignmentPointEvent)
	END_REGISTER_EVENT()

	return NULL;
}

void OfflineEvent::SetDBData(CLRecordCallback* callback)
{
	m_Time = callback->GetData("time");
	std::string datastr = callback->GetData("data").GetString();
	Decode(datastr);
}

void OfflineEvent::InsertToDB(ObjID_t id)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_offline_event", CLRecordClient::Instance()->GenGuid());
	cmd->PutData("owner", id);
	cmd->PutData("type", GetType());
	cmd->PutData("time", m_Time);
	std::string datastr = Encode();
	cmd->PutData("data", datastr);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void SellAuctionEvent::OnTriggered(WSPlayer* player)
{
	player->OnSellAuction(m_Money);
}

std::string SellAuctionEvent::Encode() const
{
	std::ostringstream os;
	os << m_Money;
	return os.str();
}

void SellAuctionEvent::Decode(const std::string& str)
{
	std::istringstream is(str);
	is >> m_Money;
}

void JoinGuildEvent::OnTriggered(WSPlayer* player)
{
	player->OnJoinGuild();
}

std::string JoinGuildEvent::Encode() const
{
	return "";
}

void JoinGuildEvent::Decode(const std::string& str)
{

}

void InviteFriendEvent::OnTriggered(WSPlayer* player)
{
	FuncNotifyDataEntry* functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
	if (!functionData)
	{
		ErrorLogStream << "can't find friend system func data!" << LogStream::eos;
		return;
	}

	//自己还不满足加好友的等级
	if (player->GetLevel() < functionData->endLevel)
	{
		return;
	}

	CLProtocol::SceneSyncRequest sync;
	sync.type = REQUEST_FRIEND;
	sync.requester = m_uid;
	sync.requesterName = m_Name;
	sync.requesterLevel = m_Lv;
	sync.requesterOccu = m_Occu;
	sync.vipLv = m_vipLv;
	player->SendProtocol(sync);
}

std::string InviteFriendEvent::Encode() const
{
	std::ostringstream os;
	char split = ',';
	os << m_uid << split;
	os << (UInt16)m_Occu << split;
	os << m_Lv << split;
	os << (UInt16)m_vipLv << split;
	os << m_Name;
	
	return os.str();
}

void InviteFriendEvent::Decode(const std::string& str)
{
	std::istringstream is(str);
	char split;
	UInt16 tmp = 0;
	is >> m_uid >> split;
	is >> tmp >> split;
	m_Occu = (UInt8)tmp;
	is >> m_Lv >> split;
	is >> tmp >> split;
	m_vipLv = (UInt8)tmp;
	std::getline(is, m_Name);
}

void ReplyFriendEvent::OnTriggered(WSPlayer* player)
{

}

std::string ReplyFriendEvent::Encode() const
{
	return "";
}

void ReplyFriendEvent::Decode(const std::string& str)
{

}

void SendPrivateChatEvent::OnTriggered(WSPlayer* player)
{
	if (!m_ChatData || !player)
	{
		return;
	}

	CLProtocol::SceneSyncChat sync;
	sync.channel = m_ChatData->channel;
	sync.objid = m_ChatData->objid;
	sync.sex = m_ChatData->sex;
	sync.occu = m_ChatData->occu;
	sync.level = m_ChatData->level;
	sync.viplvl = m_ChatData->viplvl;
	sync.objname = m_ChatData->objname;
	sync.receiverId = m_ChatData->receiverId;
	sync.word = m_ChatData->word;
	sync.bLink = m_ChatData->bLink;
	sync.isGm = m_ChatData->isGm;
	sync.voiceKey = m_ChatData->voiceKey;
	sync.voiceDuration = m_ChatData->voiceDuration;
	sync.headFrame = m_ChatData->headFrame;
	player->SendProtocol(sync);
}

std::string SendPrivateChatEvent::Encode() const
{
	if (!m_ChatData)
	{
		return std::string("");
	}

	//Avalon::StringUtil::ConvertToValue<UInt8>(*iter)
	char split = ',';
	std::ostringstream oss;
	oss << (UInt16)m_ChatData->channel << split;
	oss << m_ChatData->objid << split;
	oss << (UInt16)m_ChatData->sex << split;
	oss << (UInt16)m_ChatData->occu << split;
	oss << m_ChatData->level << split;
	oss << (UInt16)m_ChatData->viplvl << split;
	oss << m_ChatData->receiverId << split;
	oss << (UInt16)m_ChatData->bLink << split;
	oss << (UInt16)m_ChatData->isGm << split;
	oss << (UInt16)m_ChatData->voiceDuration << split;
	oss << m_ChatData->objname << split;
	oss << m_ChatData->word << split;
	oss << m_ChatData->voiceKey;
	

	return oss.str();
}

void SendPrivateChatEvent::Decode(const std::string& str)
{
	if (str.length() == 0)
	{
		return;
	}

	char split;
	UInt16 tmp;
	std::string strTmp;
	m_ChatData = new ChatDataInfo();
	std::istringstream iss(str);
	
	iss >> tmp >> split;
	m_ChatData->channel = (UInt8)tmp;
	iss >> m_ChatData->objid >> split;
	iss >> tmp >> split;
	m_ChatData->sex = (UInt8)tmp;
	iss >> tmp >> split;
	m_ChatData->occu = (UInt8)tmp;
	iss >> m_ChatData->level >> split;
	iss >> tmp >> split;
	m_ChatData->viplvl = (UInt8)tmp;
	iss >> m_ChatData->receiverId >> split;
	iss >> tmp >> split;
	m_ChatData->bLink = (UInt8)tmp;
	iss >> tmp >> split;
	m_ChatData->isGm = (UInt8)tmp;
	iss >> tmp >> split;
	m_ChatData->voiceDuration = (UInt8)tmp;

	std::getline(iss, strTmp);
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(strTmp, strs, ",");

	if (strs.size() != 3)
	{
		return;
	}

	m_ChatData->objname = strs[0];
	m_ChatData->word = strs[1];
	m_ChatData->voiceKey = strs[2];
}

void RequestMasterEvent::OnTriggered(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneSyncRequest sync;
	sync.type = REQUEST_MASTER;
	sync.requester = m_uid;
	sync.requesterName = m_Name;
	sync.requesterLevel = m_Lv;
	sync.requesterOccu = m_Occu;
	sync.vipLv = m_vipLv;
	player->SendProtocol(sync);
}

std::string RequestMasterEvent::Encode() const
{
	std::ostringstream os;
	char split = ',';
	os << m_uid << split;
	os << (UInt16)m_Occu << split;
	os << m_Lv << split;
	os << (UInt16)m_vipLv << split;
	os << m_Name;

	return os.str();
}

void RequestMasterEvent::Decode(const std::string& str)
{
	std::istringstream is(str);
	char split;
	UInt16 tmp = 0;
	is >> m_uid >> split;
	is >> tmp >> split;
	m_Occu = (UInt8)tmp;
	is >> m_Lv >> split;
	is >> tmp >> split;
	m_vipLv = (UInt8)tmp;
	std::getline(is, m_Name);
}

void RequestDiscipleEvent::OnTriggered(WSPlayer* player)
{
	if (!player)
	{
		return;
	}

	CLProtocol::SceneSyncRequest sync;
	sync.type = REQUEST_DISCIPLE;
	sync.requester = m_uid;
	sync.requesterName = m_Name;
	sync.requesterLevel = m_Lv;
	sync.requesterOccu = m_Occu;
	sync.vipLv = m_vipLv;
	player->SendProtocol(sync);
}

std::string RequestDiscipleEvent::Encode() const
{
	std::ostringstream os;
	char split = ',';
	os << m_uid << split;
	os << (UInt16)m_Occu << split;
	os << m_Lv << split;
	os << (UInt16)m_vipLv << split;
	os << m_Name;

	return os.str();
}

void RequestDiscipleEvent::Decode(const std::string& str)
{
	std::istringstream is(str);
	char split;
	UInt16 tmp = 0;
	is >> m_uid >> split;
	is >> tmp >> split;
	m_Occu = (UInt8)tmp;
	is >> m_Lv >> split;
	is >> tmp >> split;
	m_vipLv = (UInt8)tmp;
	std::getline(is, m_Name);
}
