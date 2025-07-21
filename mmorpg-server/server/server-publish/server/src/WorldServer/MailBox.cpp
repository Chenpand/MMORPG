#include "MailBox.h"

#include <AvalonTimer.h>
#include <AvalonDate.h>
#include <CLMailProtocol.h>
#include <CLCenterProtocol.h>
#include <CLRecordClient.h>
#include <CLDefine.h>
#include <CLErrorCode.h>
#include <CLForbitWord.h>
#include <CLSysNotify.h>
#include <CLDBLogger.h>
#include <CLItemDataEntry.h>
#include <set>


#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "WSRouter.h"
#include "NewAuctionMgr.h"
#include "WSServiceSwitchMgr.h"

Mail::Mail()
	:senderId(0), senderAccid(0), silver(0)
{
}
Mail::~Mail()
{
	for(std::vector<WSItem*>::iterator iter = detailItems.begin();
		iter != detailItems.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	detailItems.clear();
}

void SendMailCallback::SetResult(ObjID_t receiverId, int errcode)
{
	if(errcode == 0) OnSucceed(receiverId);
	else OnFailed(receiverId, errcode);
	if(--m_refCount == 0)
	{
		OnFinished();
		delete this;
	}
}

/**
 *@brief 每日定时处理
 */
class DailyClearCounterHandler : public Avalon::TimerHandler
{
public:
	void OnTimeout(const Avalon::Time& now)
	{
		MailBox::m_SendCounts.clear();
	}
};

/**
 *@brief 查询邮件回调
 */
class SelectMailsCallback : public SelectRoleInfoCallback
{
public:
	SelectMailsCallback(WSPlayer* player):SelectRoleInfoCallback(player){}
	~SelectMailsCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetMailBox().OnLoadMailsRet(this);
	}

	void OnFailed(WSPlayer* player)
	{
		player->GetMailBox().OnLoadMailsRet(this);
	}
};

/**
 *@brief 查询所有邮件道具返回
 */
class SelectMailItemsCallback : public SelectRoleInfoCallback
{
public:
	SelectMailItemsCallback(WSPlayer* player):SelectRoleInfoCallback(player){}
	~SelectMailItemsCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetMailBox().OnLoadMailItemsRet(this);
	}

	void OnFailed(WSPlayer* player){
		player->GetMailBox().OnLoadMailItemsRet(this);
	}

};

/**
 *@brief 查询玩家返回
 */
class SelectRoleCallback : public CLRecordCallback
{
public:
	SelectRoleCallback(ObjID_t playerId, UInt8 replyTimes ,const std::string& title, const std::string& content
		, UInt32 silver, const std::map<ObjID_t,UInt16>& items)
		:m_PlayerId(playerId), m_ReplyTimes(replyTimes), m_Title(title), m_Content(content), m_Silver(silver), m_Items(items){}
	~SelectRoleCallback(){}

	void OnFinished()
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(m_PlayerId);
		if(player == NULL) return;

		if(!NextRow())
		{
			player->SendNotify(403);
			return;
		}

		ObjID_t id = GetKey();
		UInt32 accid = GetData("accid");
		std::string receiverName = GetData("name").GetString();
		player->GetMailBox().CheckMailCost(id, accid, receiverName, m_ReplyTimes, m_Title, m_Content, m_Silver,m_Items);
	}

private:
	ObjID_t		m_PlayerId;
	UInt8		m_ReplyTimes;
	std::string m_Title;
	std::string m_Content;
	UInt32		m_Silver;
	std::map<ObjID_t,UInt16> m_Items;
};

/**
 *@brief 系统邮件查询玩家返回
 */
class SysSelectRoleCallback : public CLRecordCallback
{
public:
	SysSelectRoleCallback(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime, SendMailCallback* callback = NULL)
		:m_ReceiverId(receiverId), m_SenderName(senderName), m_Title(title), m_Content(content),m_Reason(reason), m_Items(items)
		, m_ValidTime(validDay), m_SendTime(sendtime), m_pCallback(callback)
	{
		m_ValidTime = m_ValidTime > MAIL_VALID_DAY ? MAIL_VALID_DAY : m_ValidTime;
		m_Deadline = sendtime + m_ValidTime * DAY_TO_SEC;
	}

	SysSelectRoleCallback(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<WSItem*>& items, UInt32 validDay, UInt32 sendtime, SendMailCallback* callback = NULL)
		:m_ReceiverId(receiverId), m_SenderName(senderName), m_Title(title), m_Content(content), m_Reason(reason)
		, m_DetailItems(items), m_ValidTime(validDay), m_SendTime(sendtime), m_pCallback(callback)
	{
		m_ValidTime = m_ValidTime > MAIL_VALID_DAY ? MAIL_VALID_DAY : m_ValidTime;
		m_Deadline = sendtime + m_ValidTime * DAY_TO_SEC;
	}

	SysSelectRoleCallback(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
		, const std::string& reason, const std::vector<ItemReward>& items, const std::vector<WSItem*>& detailItems, UInt32 validDay, UInt32 sendtime, SendMailCallback* callback = NULL)
		:m_ReceiverId(receiverId), m_SenderName(senderName), m_Title(title), m_Content(content), m_Reason(reason), m_Items(items)
		, m_DetailItems(detailItems), m_ValidTime(validDay), m_SendTime(sendtime), m_pCallback(callback)
	{
		m_ValidTime = m_ValidTime > MAIL_VALID_DAY ? MAIL_VALID_DAY : m_ValidTime;
		m_Deadline = sendtime + m_ValidTime * DAY_TO_SEC;
	}


	~SysSelectRoleCallback()
	{
		for(std::vector<WSItem*>::iterator iter = m_DetailItems.begin();
			iter != m_DetailItems.end(); iter++)
		{
			CL_SAFE_DELETE(*iter);
		}
		m_DetailItems.clear();
	}

	void OnFinished()
	{
		if(!NextRow())
		{
			if(m_pCallback != NULL) m_pCallback->SetResult(m_ReceiverId, SendMailCallback::MAIL_ERROR_NOPLAYER);
			return;
		}

		do 
		{
			ObjID_t receiver = GetKey();
			//UInt32 accId = GetData("accid");
			std::string receiverName = GetData("name").GetString();

			ObjID_t mailid = CLRecordClient::Instance()->GenGuid();
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiver);
			if(player != NULL)
			{
				player->GetMailBox().PutSysMail(mailid, m_SenderName, m_Title, m_Content, m_Reason, m_Items, m_DetailItems, m_ValidTime, m_SendTime);
			}
			else
			{
				std::ostringstream itemstream;

				std::vector<ItemReward> items;
				for (std::vector<ItemReward>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
				{
					if (iter->num != 0 && ItemDataEntryMgr::Instance()->FindEntry(iter->id) != NULL) items.push_back(*iter);
				}

				MailBox::SaveSysMail(mailid, 0, receiver, m_SenderName, m_Title, m_Content, m_Reason, items, m_DetailItems, m_SendTime, m_Deadline);

				for (std::vector<ItemReward>::iterator iter = items.begin(); iter != items.end(); ++iter)
				{
					if (!itemstream.str().empty()) itemstream << '+';
					iter->EncodeString(itemstream);
				}
				
				for (std::vector<WSItem*>::iterator iter = m_DetailItems.begin(); iter != m_DetailItems.end(); ++iter)
				{
					if (!itemstream.str().empty()) itemstream << '+';
					WSItem* wsItem = *iter;
					itemstream << wsItem->m_DataId << '_' << int(wsItem->m_Bind) << '_' << int(wsItem->m_Quality) << '_' << int(wsItem->m_Num);
				}
			}
		}while(NextRow());

		if(m_pCallback != NULL) m_pCallback->SetResult(m_ReceiverId, 0);
	}

	void OnFailed(UInt32 errorcode)
	{
		if(m_pCallback != NULL) m_pCallback->SetResult(m_ReceiverId, SendMailCallback::MAIL_ERROR_NOPLAYER);
	}

private:
	ObjID_t		m_ReceiverId;
	std::string m_SenderName;
	std::string m_Title;
	std::string m_Content;
	std::string m_Reason;
	std::vector<ItemReward> m_Items;
	std::vector<WSItem*>	m_DetailItems;
	UInt32	m_ValidTime;
	UInt32	m_SendTime;
	UInt32	m_Deadline;
	SendMailCallback*	m_pCallback;
};



MailBox::CountMap MailBox::m_SendCounts;

MailBox::MailBox()
{
	m_pOwner = NULL;
	//m_Timer.SetInterval(5000);
	m_Timer.SetInterval(1000);
	//by huchenhui 因为一开始就有邮件列表的请求,所以需要减去interval
	m_Timer.SetLastTime(CURRENT_TIME.MSec() - m_Timer.GetInterval());
}

MailBox::~MailBox()
{
	for(MailList::iterator iter = m_Mails.begin();
		iter != m_Mails.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_Mails.clear();
	m_MailMap.clear();
}

void MailBox::Init()
{
	Avalon::Date date(CURRENT_TIME);
	date.Hour(0);
	date.Min(0);
	date.Sec(0);
	Avalon::Time time = date.ToTime();
	Avalon::TimerReactor::Instance()->ScheduleAt(new DailyClearCounterHandler(), time, (DAY_TO_SEC * 1000));
}

void MailBox::LoadMails()
{
	CLSelectCommand* selectmailcmd = CLRecordClient::Instance()->PrepareSelectCommand(GetOwner()->GetSrcZoneId(), "t_mail", GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
	selectmailcmd->PutCond("owner", GetOwner()->GetID());
	//只需要排序不需要限定数量 modified by huchenhui
	//selectmailcmd->SetLimit("ORDER BY `sendtime` DESC LIMIT 300");
	selectmailcmd->SetLimit("ORDER BY `sendtime` DESC");
	CLRecordClient::Instance()->SendCommand(selectmailcmd, new SelectMailsCallback(GetOwner()), GetOwner()->GetID());

	CLSelectCommand* selectitemcmd = CLRecordClient::Instance()->PrepareSelectCommand(GetOwner()->GetSrcZoneId(), "t_mailitem", GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
	selectitemcmd->PutCond("owner", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectitemcmd, new SelectMailItemsCallback(GetOwner()), GetOwner()->GetID());
}

void MailBox::VisitMails(const std::function<bool(Mail*)>& F)
{
	for (auto elem : m_MailMap)
	{
		Mail* pMail = elem.second;
		if (!pMail) continue;

		if (!F(pMail)) break;
	}
}

void MailBox::ListMails()
{
	//GameInfoLogStream("MailBox") << "player(" << GetOwner()->GetAccId() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") request mail list"  << LogStream::eos;
	if (!m_Timer.IsTimeout(CURRENT_TIME))
	{
		//太频繁
		return;
	}
	
	UInt32 maxCount = 300;
	UInt32 index = 0;

	CLProtocol::WorldMailListRet ret;
	for (MailList::reverse_iterator iter = m_Mails.rbegin(); iter != m_Mails.rend(); ++iter)
	{
		Mail* mail = *iter;
		//黑名单屏蔽
		if (GetOwner()->GetRelationMgr().FindRelation(RELATION_BLACKLIST, mail->senderId) != NULL)
			continue;
		ret.mails.push_back(mail->titleInfo);
		index++;
		if (index >= maxCount)
		{
			GetOwner()->SendProtocol(ret);
			ret.mails.clear();
			index = 0;
		}
	}
	if (index > 0)
	{
		GetOwner()->SendProtocol(ret);
	}
	
	/*
	CLProtocol::WorldMailListRet ret;
	for (MailList::reverse_iterator iter = m_Mails.rbegin(); iter != m_Mails.rend(); ++iter)
	{
		Mail* mail = *iter;
		ret.mails.push_back(mail->titleInfo);
	}
	GetOwner()->SendProtocol(ret);
	*/
}

/*
// modified by huchenhui
void MailBox::ListMails(UInt8 type, UInt16 start, UInt16 num)
{
	if(num > 30) num = 30;	//最大30条
	if(start > m_Mails.size()) start = m_Mails.size();

	CLProtocol::WorldMailListRet ret;
	ret.type = type;
	ret.start = start;
	ret.totalnum = 0;
	
	for(MailList::reverse_iterator iter = m_Mails.rbegin();
		iter != m_Mails.rend(); ++iter)
	{
		Mail* mail = *iter;

		//黑名单屏蔽
		if(GetOwner()->GetRelationMgr().FindRelation(RELATION_BLACKLIST, mail->senderId) != NULL)
			continue;

		switch(type)
		{
		case 1:	//系统
			if(mail->titleInfo.type != MAIL_TYPE_SYSTEM) continue;
			break;
		case 2:	//个人
			if(mail->titleInfo.type != MAIL_TYPE_NORMAL) continue;
			break;
		case 3: //已读
			if(mail->titleInfo.status != 1) continue;
			break;
		case 4:	//未读
			if(mail->titleInfo.status != 0) continue;
			break;
		default: //全部
			break;
		}
		
		if(ret.totalnum < 60000) ++ret.totalnum;
		if(ret.totalnum > start && ret.totalnum <= start + num)
		{
			ret.mails.push_back(mail->titleInfo);
		}
	}
	GetOwner()->SendProtocol(ret);
}
*/

void MailBox::ReadMail(ObjID_t id)
{
	Mail* mail = FindMail(id);
	if(mail == NULL) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_READMAIL_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		//modified by huchenhui
		//stream & id & mail->content & mail->silver & mail->items;
		stream & id & mail->content & mail->items;
		for(std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
		{
			WSItem* item = *iter;
			stream & item->GetID() & item->m_DataId;
			item->Encode(stream, SST_SELF, false);
		}
		//modified by huchenhui
		//stream & UInt32(0);
		stream & UInt64(0);
	}
	catch(const Avalon::NetException&)
	{
		ErrorLogStream << "MailBox::ReadMail encode mail:" << id << " failed!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	GetOwner()->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	if(mail->titleInfo.status == 0)
	{
		mail->titleInfo.status = 1;

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_mail", id, true, GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
		updateCmd->PutData("status",1);
		CLRecordClient::Instance()->SendCommand(updateCmd, NULL, GetOwner()->GetID());

		SyncMailStatus(mail);
		//不需要通知新邮件数量 modified by huchenhui
		//SyncUnreadMailNum();
	}
}

void MailBox::GetMailItems(ObjID_t id)
{
	Mail* mail = FindMail(id);
	if(mail == NULL) return;
	if (mail->silver == 0 && mail->items.empty() && mail->detailItems.empty())
	{
		ReasonInfo reasonInfo1;
		if (ParseReason(mail->reason.c_str(), reasonInfo1) && reasonInfo1.type == SOURCE_TYPE_ACTION_BUY)
		{
			GetOwner()->SendNotify(9963);
		}
		return; //没道具
	}
	
	//检查包裹
	CLProtocol::SceneMailCheckPackageReq req;
	req.playerId = GetOwner()->GetID();
	//req.items[id] = mail->items;

	for (size_t i = 0; i < mail->items.size(); ++i)
	{
		req.items[id].push_back(mail->items[i]);
	}
	
	for(std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
	{
		WSItem* item = *iter;
		req.items[id].push_back(ItemReward(item->m_DataId, item->m_Num, item->m_QualityLv, item->m_Strengthen, 0, 0, item->m_AuctionTransNum));
	}

	ReasonInfo reasonInfo;
	if (ParseReason(mail->reason.c_str(), reasonInfo) && reasonInfo.type == SOURCE_TYPE_ACTION_BUY && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_COOLTIME))
	{
		for (std::vector<ItemReward>::iterator iter = req.items[id].begin(); iter != req.items[id].end(); ++iter)
		{
			iter->auctionCoolTimeStamp = zjy_auction::AuctionMgr::CalcAuctionItemCoolTime(GetOwner()->GetPf(),iter->id, mail->titleInfo.date);
		}
	}

	GetOwner()->SendToScene(req);
}

void MailBox::GetMailItems(UInt32 mailType)
{	
	//检查包裹
	CLProtocol::SceneMailCheckPackageReq req;
	req.playerId = GetOwner()->GetID();

	UInt32 count = 0;
	std::vector<ObjID_t> ids;

	for (MailList::iterator iter = m_Mails.begin(); iter != m_Mails.end(); ++iter)
	{
		if (count >= MAX_ONE_KEY_NUM)
		{
			break;
		}
		count += 1;

		Mail* mail = *iter;

		if (mail->titleInfo.type != mailType)
		{
			continue;
		}

		//如果物品有数量限制,不能一键领取
		bool isContinue = false;
		for (UInt32 i = 0; i < mail->items.size(); ++i)
		{
			ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(mail->items[i].id);
			if (!data)
			{
				isContinue = true;
				break;
			}
			if (data->getLimitNum > 0)
			{
				isContinue = true;
				break;
			}
		}
		if (isContinue)
		{
			continue;
		}

		for (std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
		{
			WSItem* item = *iter;
			ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
			if (!data)
			{
				isContinue = true;
				break;
			}
			if (data->getLimitNum > 0)
			{
				isContinue = true;
				break;
			}
		}
		if (isContinue)
		{
			continue;
		}

		if (mail->items.size() == 0 && mail->detailItems.size() == 0)
		{
			ids.push_back(mail->titleInfo.id);
			continue;
		}

		/*req.items[mail->titleInfo.id] = mail->items;
		for(std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
		{
			WSItem* item = *iter;
			req.items[mail->titleInfo.id].push_back(ItemReward(item->m_DataId, item->m_Num, item->m_Bind, item->m_Quality));
		}*/

		for (size_t i = 0; i < mail->items.size(); ++i)
		{
			req.items[mail->titleInfo.id].push_back(mail->items[i]);
		}
		for (std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
		{
			WSItem* item = *iter;
			req.items[mail->titleInfo.id].push_back(ItemReward(item->m_DataId, item->m_Num, item->m_Quality, item->m_Strengthen, 0, 0, item->m_AuctionTransNum));
		}

		ReasonInfo reasonInfo;
		if (ParseReason(mail->reason.c_str(), reasonInfo) && reasonInfo.type == SOURCE_TYPE_ACTION_BUY && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_COOLTIME))
		{
			for (std::vector<ItemReward>::iterator iter = req.items[mail->titleInfo.id].begin(); iter != req.items[mail->titleInfo.id].end(); ++iter)
			{
				iter->auctionCoolTimeStamp = zjy_auction::AuctionMgr::CalcAuctionItemCoolTime(GetOwner()->GetPf(), iter->id, mail->titleInfo.date);
			}
		}
	}
	GetOwner()->SendToScene(req);

	//一键领取会删除邮件.
	DeleteMails(ids);
}

void MailBox::SendMail(const std::string& receiver, const std::string& title, const std::string& content, UInt32 silver
	, const std::map<ObjID_t,UInt16>& items)
{
	//没有玩家回复
	return;

	if(receiver.empty() || receiver.size() >= NAME_LENGTH) return;	//长度不对
	if(receiver == GetOwner()->GetName()) return;	//发送给自身
	if(GetOwner()->GetLevel() < MIN_CHAT_LEVEL)
	{
		GetOwner()->SendNotify(410, MIN_CHAT_LEVEL);
		return;
	}

	if(GetOwner()->GetSrcZoneId() != 0)
	{
		GetOwner()->SendNotify(411);
		return;
	}

	if(!CheckWord(title, content)) return;

	if(items.size() > MAX_MAILITEM_NUM) return;
	if(m_SendCounts[GetOwner()->GetID()] > 100)
	{
		GetOwner()->SendNotify(405);
		return;
	}
	if(!m_Timer.IsTimeout(CURRENT_TIME)) return;	//太频繁

	char titlebuf[MAX_TITLE_SHOW_LEN+1];
	avalon_strncpy(titlebuf, title.c_str(), MAX_TITLE_SHOW_LEN);
	if(ForbitWordParser::Instance()->Parse(titlebuf) == FORBIT_WORD_NOUTF8) return;

	char contentbuf[MAX_CONTENT_SHOW_LEN+1];
	avalon_strncpy(contentbuf, content.c_str(), MAX_CONTENT_SHOW_LEN);
	if(ForbitWordParser::Instance()->Parse(contentbuf) == FORBIT_WORD_NOUTF8) return;

// 	if(GetOwner()->CheckPunishFlag(PUNISH_FORBIDTALK) && GetOwner()->GetForbitTalkDueTime() > UInt32(CURRENT_TIME.Sec()))
// 	{
// 		std::string timestr = SysNotifyMgr::Instance()->TimeToStr(GetOwner()->GetForbitTalkDueTime() - UInt32(CURRENT_TIME.Sec()));
// 		GetOwner()->SendNotify(251,	timestr.c_str());
// 		return;
// 	}

	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectcmd->PutColumn("accid");
	selectcmd->PutColumn("name");
	selectcmd->PutCond("name",receiver);
	CLRecordClient::Instance()->SendCommand(selectcmd,
		new SelectRoleCallback(GetOwner()->GetID(), 0, titlebuf, contentbuf, silver, items));
}

void MailBox::ReplyMail(ObjID_t id, const std::string& title, const std::string& content, UInt32 silver
	, const std::map<ObjID_t,UInt16>& items)
{
	//没有玩家回复
	return;

	Mail* mail = FindMail(id);
	if(mail == NULL || mail->titleInfo.type == MAIL_TYPE_SYSTEM) return; //不能回复系统邮件
	if(GetOwner()->GetLevel() < MIN_CHAT_LEVEL)
	{
		GetOwner()->SendNotify(410, MIN_CHAT_LEVEL);
		return;
	}

	if(GetOwner()->GetSrcZoneId() != 0)
	{
		GetOwner()->SendNotify(412);
		return;
	}

	if(!CheckWord(title, content)) return;

	if(items.size() > MAX_MAILITEM_NUM) return;
	if(m_SendCounts[GetOwner()->GetID()] > 100)
	{
		GetOwner()->SendNotify(405);
		return;
	}
	if(!m_Timer.IsTimeout(CURRENT_TIME)) return;

	char titlebuf[MAX_TITLE_SHOW_LEN+1];
	avalon_strncpy(titlebuf, title.c_str(), MAX_TITLE_SHOW_LEN);
	if(ForbitWordParser::Instance()->Parse(titlebuf) == FORBIT_WORD_NOUTF8) return;
	char contentbuf[MAX_CONTENT_SHOW_LEN+1];
	avalon_strncpy(contentbuf, content.c_str(), MAX_CONTENT_SHOW_LEN);
	if(ForbitWordParser::Instance()->Parse(contentbuf) == FORBIT_WORD_NOUTF8) return;

// 	if(GetOwner()->CheckPunishFlag(PUNISH_FORBIDTALK) && GetOwner()->GetForbitTalkDueTime() > UInt32(CURRENT_TIME.Sec()))
// 	{
// 		std::string timestr = SysNotifyMgr::Instance()->TimeToStr(GetOwner()->GetForbitTalkDueTime() - UInt32(CURRENT_TIME.Sec()));
// 		GetOwner()->SendNotify(251,	timestr.c_str());
// 		return;
// 	}

	UInt8 replyTimes = mail->titleInfo.replyTimes;
	if(replyTimes < 100) ++replyTimes;
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectcmd->PutCond("guid",mail->senderId);
	CLRecordClient::Instance()->SendCommand(selectcmd,
		new SelectRoleCallback(GetOwner()->GetID(), replyTimes, titlebuf, contentbuf, silver, items));
}

bool MailBox::CheckWord(const std::string& title, const std::string& content)
{
	if(title.empty() || title.length() >= 100) return false;
	if(content.length() >= 1000) return false;
	int width = GetUtf8ShowWidth(title.c_str());
	if(width <= 0 || width > MAX_TITLE_SHOW_LEN) return false;
	width = GetUtf8ShowWidth(content.c_str());
	if(width < 0 || width > MAX_CONTENT_SHOW_LEN) return false;
	return true;
}

void MailBox::CheckMailCost(ObjID_t receiver, UInt32 accid, const std::string& receiverName, UInt8 replyTimes, const std::string& title, const std::string& content, UInt32 silver
	, const std::map<ObjID_t,UInt16>& items)
{
	//发到场景检查
	CLProtocol::SceneMailRemoveCostReq req;
	req.playerId = GetOwner()->GetID();
	req.receiveId = receiver;
	req.accid = accid;
	req.receiveName = receiverName;
	req.replyTimes = replyTimes;
	req.title = title;
	req.content = content;
	req.cost = MAIL_COST;
	req.silver = silver;
	for(std::map<ObjID_t, UInt16>::const_iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		if(iter->second != 0) req.items.insert(*iter);
	}
	GetOwner()->SendToScene(req);
}

UInt32 MailBox::SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime)
{
	UInt32 res = CheckMail(receiverId, title, content, items.size(), senderName);
	if (res != ErrorCode::SUCCESS)
	{
		return res;
	}
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiverId);
	if(player != NULL)
	{
		ObjID_t mailid = CLRecordClient::Instance()->GenGuid();
		player->GetMailBox().PutSysMail(mailid, senderName, title, content, reason, items, validDay, sendtime);
	}
	else
	{
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");
		selectcmd->PutColumn("name");
		selectcmd->PutCond("guid", receiverId);
		CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(receiverId, senderName, title, content, reason, items, validDay, sendtime));
	}
	return res;
}

UInt32 MailBox::SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>& items, SendMailCallback* callback, UInt32 validDay, UInt32 sendtime)
{
	UInt32 res = CheckMail(receiverId, title, content, items.size(), senderName);
	if (res != ErrorCode::SUCCESS)
	{
		return res;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiverId);
	if(player != NULL)
	{
		ObjID_t mailid = CLRecordClient::Instance()->GenGuid();
		player->GetMailBox().PutSysMail(mailid, senderName, title, content, reason, items, validDay, sendtime);
		if(callback != NULL) callback->SetResult(receiverId, 0);
	}
	else
	{
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");
		selectcmd->PutColumn("name");
		selectcmd->PutCond("guid", receiverId);
		CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(receiverId, senderName, title, content, reason, items, validDay, sendtime, callback));
	}
	return res;
}

UInt32 MailBox::SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, std::vector<WSItem*>& items, UInt32 validDay, UInt32 sendtime)
{
	UInt32 res = CheckMail(receiverId, title, content, items.size(), senderName);
	if (res != ErrorCode::SUCCESS)
	{
		return res;
	}
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiverId);
	if(player != NULL)
	{
		ObjID_t mailid = CLRecordClient::Instance()->GenGuid();
		player->GetMailBox().PutSysMail(mailid, senderName, title, content, reason, items, validDay, sendtime);
	}
	else
	{
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");
		selectcmd->PutColumn("name");
		selectcmd->PutCond("guid", receiverId);
		CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(receiverId, senderName, title, content, reason, items, validDay, sendtime));
	}
	return res;
}

UInt32 MailBox::SendSysMail(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt32 validDay, UInt32 sendtime)
{
	UInt32 res = CheckMail(receiverId, title, content, items.size() + detailitems.size(), senderName);
	if (res != ErrorCode::SUCCESS)
	{
		return res;
	}
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiverId);
	if (player != NULL)
	{
		ObjID_t mailId = CLRecordClient::Instance()->GenGuid();
		player->GetMailBox().PutSysMail(mailId, senderName, title, content, reason, items, detailitems, validDay, sendtime);
	}
	else
	{
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");
		selectcmd->PutColumn("name");
		selectcmd->PutCond("guid", receiverId);
		CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(receiverId, senderName, title, content, reason, items, detailitems, validDay, sendtime));
	}
	return res;
}

UInt32 MailBox::SendSysMailGetMailID(ObjID_t receiverId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>&items, std::vector<WSItem*>& detailitems, UInt64& mailId, UInt32 validDay, UInt32 sendtime)
{
	mailId = 0;
	UInt32 res = CheckMail(receiverId, title, content, items.size() + detailitems.size(), senderName);
	if (res != ErrorCode::SUCCESS)
	{
		return res;
	}
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiverId);
	if (player != NULL)
	{
		mailId = CLRecordClient::Instance()->GenGuid();
		player->GetMailBox().PutSysMail(mailId, senderName, title, content, reason, items, detailitems, validDay, sendtime);
	}
	else
	{
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");
		selectcmd->PutColumn("name");
		selectcmd->PutCond("guid", receiverId);
		CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(receiverId, senderName, title, content, reason, items, detailitems, validDay, sendtime));
	}
	return res;
}

void MailBox::SendSysMailByAccId(UInt32 accId, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime)
{
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectcmd->PutColumn("accid");
	selectcmd->PutColumn("name");
	selectcmd->PutCond("accid", accId);
	selectcmd->PutCond("deletetime", 0);
	CLRecordClient::Instance()->SendCommand(selectcmd, new SysSelectRoleCallback(0, senderName, title, content, reason, items, validDay, sendtime));
}

void MailBox::BroadcastSysMailOnline(const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime)
{
	class BroadcastMailVisitor : public WSPlayerVisitor
	{
	public:
		BroadcastMailVisitor(const std::string& senderName, const std::string& title, const std::string& content,
			const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime)
			:m_SenderName(senderName), m_Title(title), m_Content(content),m_Reason(reason), m_Items(items), m_ValidDay(validDay), m_SendTime(sendtime){}

		bool operator()(WSPlayer* player)
		{
			MailBox::SendSysMail(player->GetID(), m_SenderName, m_Title, m_Content, m_Reason, m_Items, m_ValidDay, m_SendTime);
			return true;
		}

	private:
		std::string m_SenderName;
		std::string	m_Title;
		std::string	m_Content;
		std::string m_Reason;
		std::vector<ItemReward>	m_Items;
		UInt32		m_ValidDay;
		UInt32		m_SendTime;
	};
	BroadcastMailVisitor visitor(senderName, title, content, reason, items, validDay, sendtime);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void MailBox::DeleteMails(const std::vector<ObjID_t>& mailids)
{
	std::set<ObjID_t> idset;
	idset.insert(mailids.begin(), mailids.end());

	//不需要同步新邮件数量 modified by huchenhui
	//bool bSyncUnread = false;
	CLProtocol::WorldNotifyDeleteMail notify;
	for(MailList::iterator iter = m_Mails.begin(); iter != m_Mails.end();)
	{
		Mail* mail = *iter;
		if(idset.find(mail->titleInfo.id) != idset.end())
		{
			//有道具不能删除. huchenhui
			if (!mail->detailItems.empty() || !mail->items.empty())
			{
				break;
			}

			//只删除邮件，道具由触发器删除
			CLDeleteCommand *deletecmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_mail" ,mail->titleInfo.id, GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
			CLRecordClient::Instance()->SendCommand(deletecmd, NULL, GetOwner()->GetID());

			GameInfoLogStream("MailBox") << "player(" << GetOwner()->GetAccId() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") deletemail:" << mail->titleInfo.id << LogStream::eos;

			notify.ids.push_back(mail->titleInfo.id);
			m_MailMap.erase(mail->titleInfo.id);
			CL_SAFE_DELETE(mail);
			iter = m_Mails.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	if(!notify.ids.empty())
	{
		GetOwner()->SendProtocol(notify);
	}
}

void MailBox::PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title
	, const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, UInt32 validDay, UInt32 sendtime)
{
	auto wsItems = std::vector<WSItem*>();
	PutSysMail(mailid, senderName, title, content, reason, items, wsItems, validDay, sendtime);
}

void MailBox::PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title
	, const std::string& content, const std::string& reason, std::vector<WSItem*>& items, UInt32 validDay, UInt32 sendtime)
{
	auto itemRewards = std::vector<ItemReward>();
	PutSysMail(mailid, senderName, title, content, reason, itemRewards, items, validDay, sendtime);
}

void MailBox::PutSysMail(ObjID_t mailid, const std::string& senderName, const std::string& title, const std::string& content
	, const std::string& reason, const std::vector<ItemReward>& items, std::vector<WSItem*>& detailItems, UInt32 validDay, UInt32 sendtime)
{
	if (validDay > MAIL_VALID_DAY) validDay = MAIL_VALID_DAY;
	Mail* mail = new Mail();
	mail->titleInfo.id = mailid;
	mail->titleInfo.date = sendtime;
	mail->titleInfo.deadline = sendtime + (validDay * DAY_TO_SEC);

	ReasonInfo reasonInfo;
	ParseReason(reason.c_str(), reasonInfo);
	if (reasonInfo.type == LogSourceType::SOURCE_TYPE_GM)
	{
		mail->titleInfo.type = MAIL_TYPE_GM;
	}
	else if (reasonInfo.type >= LogSourceType::SOURCE_TYPE_GUILD && reasonInfo.type < LogSourceType::SOURCE_TYPE_GUILD_MAX)
	{
		mail->titleInfo.type = MAIL_TYPE_GUILD;
	}
	else if (reasonInfo.type == LogSourceType::SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME || 
		reasonInfo.type == LogSourceType::SOURCE_TYPE_GUILD_DUNGEON_VICTORY ||
		reasonInfo.type == LogSourceType::SOURCE_TYPE_GUILD_DUNGEON_ATTEND)
	{
		mail->titleInfo.type = MAIL_TYPE_GUILD;
	}
	else
	{
		mail->titleInfo.type = MAIL_TYPE_SYSTEM;
	}

	mail->titleInfo.replyTimes = 0;
	mail->titleInfo.status = 0;
	if (!items.empty() || !detailItems.empty())
	{
		mail->titleInfo.hasitem = 1;
	}
	mail->titleInfo.title = title;
	mail->titleInfo.sender = senderName;
	mail->senderId = 0;
	mail->senderAccid = 0;
	mail->content = content;
	mail->reason = reason;
	mail->silver = 0;
	mail->detailItems = detailItems;

	std::ostringstream itemstream;
	bool bFirst = true;
	for (std::vector<ItemReward>::const_iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		if (iter->num != 0 && ItemDataEntryMgr::Instance()->FindEntry(iter->id) != NULL)
		{
			mail->items.push_back(*iter);

			if (bFirst) bFirst = false;
			else itemstream << '+';
			iter->EncodeString(itemstream);
		}
	}

	for (std::vector<WSItem*>::iterator iter = detailItems.begin(); iter != detailItems.end(); ++iter)
	{
		if (bFirst) bFirst = false;
		else itemstream << '+';
		WSItem* item = *iter;
		itemstream << item->m_DataId << '_' << int(item->m_Bind) << '_' << int(item->m_Quality) << '_' << int(item->m_Num);
	}

	if (!PutMail(mail))
	{
		CL_SAFE_DELETE(mail);
		detailItems.clear();
		return;
	}

	SaveSysMail(mailid, GetOwner()->GetSrcZoneId(), GetOwner()->GetID(), senderName, title, content, reason, items, detailItems, sendtime, mail->titleInfo.deadline);
	detailItems.clear();
}

void MailBox::SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
	const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, UInt32 sendtime, UInt32 deadline)
{
	SaveSysMail(mailid, zoneId, owner, senderName, title, content, reason, items, std::vector<WSItem*>(), sendtime, deadline);
}

void MailBox::SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
	const std::string& content, const std::string& reason, const std::vector<WSItem*>& items, UInt32 sendtime, UInt32 deadline)
{
	SaveSysMail(mailid, zoneId, owner, senderName, title, content, reason, std::vector<ItemReward>(), items, sendtime, deadline);
}

void MailBox::SaveSysMail(ObjID_t mailid, UInt32 zoneId, ObjID_t owner, const std::string& senderName, const std::string& title,
	const std::string& content, const std::string& reason, const std::vector<ItemReward>& items, const std::vector<WSItem*>& detailItems, UInt32 sendtime, UInt32 deadline)
{
	

	CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand(zoneId, "t_mail", mailid, owner, WSApplication::Instance()->GetTableCount());
	insertCommand->PutData("owner", owner);
	insertCommand->PutData("senderid", ObjID_t(0));
	insertCommand->PutData("senderaccid", UInt32(0));
	insertCommand->PutData("sendername", senderName);
	insertCommand->PutData("sendtime", sendtime);
	insertCommand->PutData("deadline", deadline);
	insertCommand->PutData("title", title);
	insertCommand->PutData("content", content);
	ReasonInfo reasonInfo;
	ParseReason(reason.c_str(), reasonInfo);
	if (reasonInfo.type == LogSourceType::SOURCE_TYPE_GM)
	{
		insertCommand->PutData("mailtype", UInt8(MAIL_TYPE_GM));
	}
	else if (reasonInfo.type >= LogSourceType::SOURCE_TYPE_GUILD && reasonInfo.type < LogSourceType::SOURCE_TYPE_GUILD_MAX)
	{
		insertCommand->PutData("mailtype", UInt8(MAIL_TYPE_GUILD));
	}
	else if (reasonInfo.type == LogSourceType::SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME || 
		reasonInfo.type == LogSourceType::SOURCE_TYPE_GUILD_DUNGEON_VICTORY ||
		reasonInfo.type == LogSourceType::SOURCE_TYPE_GUILD_DUNGEON_ATTEND)
	{
		insertCommand->PutData("mailtype", UInt8(MAIL_TYPE_GUILD));
	}
	else
	{
		insertCommand->PutData("mailtype", UInt8(MAIL_TYPE_SYSTEM));
	}
	insertCommand->PutData("status", UInt8(0));
	insertCommand->PutData("replytimes", UInt8(0));
	insertCommand->PutData("silver", 0);
	insertCommand->PutData("reason", reason);
	//简单物品
	std::ostringstream stream;
	UInt32 itemnum = 0;
	for (std::vector<ItemReward>::const_iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		if (iter->num == 0) continue;
		if (++itemnum > MAX_MAILITEM_NUM) break;
		iter->EncodeString(stream);
		stream << ';';
	}
	insertCommand->PutData("items", stream.str());
	CLRecordClient::Instance()->SendCommand(insertCommand, NULL, owner);

	//详细物品
	for (std::vector<WSItem*>::const_iterator iter = detailItems.begin(); iter != detailItems.end(); ++iter)
	{
		WSItem* item = *iter;

		CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand(zoneId, "t_mailitem", item->GetID(), owner, WSApplication::Instance()->GetTableCount());
		insertCommand->PutData("mailid", mailid);
		insertCommand->PutData("owner", owner);
		// modified by huchenhui  在GetDBData中已经PutData
		//insertCommand->PutData("itemid",item->m_DataId);
		item->GetDBData(insertCommand, false);
		CLRecordClient::Instance()->SendCommand(insertCommand, NULL, owner);
	}
}

bool MailBox::PutMail(Mail* mail)
{
	if(mail == NULL) return false;
	if(m_MailMap.find(mail->titleInfo.id) != m_MailMap.end()) return false;

	MailList::iterator iter = m_Mails.begin();
	for(;iter != m_Mails.end(); ++iter)
	{
		if(mail->titleInfo.date < (*iter)->titleInfo.date) break;
	}
	m_Mails.insert(iter, mail);
	m_MailMap.insert(std::make_pair(mail->titleInfo.id, mail));
	//modified by huchenhui 没有数量限制.
	/*
	CLProtocol::WorldNotifyDeleteMail delNotify;
	while(m_Mails.size() > 300)
	{
		MailList::iterator begin = m_Mails.begin();
		Mail* mail = *begin;
		delNotify.ids.push_back(mail->titleInfo.id);
		m_MailMap.erase(mail->titleInfo.id);
		CL_SAFE_DELETE(mail);
		m_Mails.erase(begin);
	}
	if(!delNotify.ids.empty())
	{
		GetOwner()->SendProtocol(delNotify);
	}
	*/

	//黑名单屏蔽
	if(GetOwner()->GetRelationMgr().FindRelation(RELATION_BLACKLIST, mail->senderId) == NULL)
	{
		CLProtocol::WorldNotifyNewMail notify;
		notify.info = mail->titleInfo;
		GetOwner()->SendProtocol(notify);

		//InfoLogStream << "MailBox::PutMail : send protocol WorldNotifyNewMail  title=" << mail->titleInfo.title << LogStream::eos;

		//by huchenhui 不需要同步新邮件数量
		//SyncUnreadMailNum();
	}

	return true;
}

void MailBox::OnHeartbeat(const Avalon::Time& now)
{
	CLProtocol::WorldNotifyDeleteMail notify;
	for(MailList::iterator iter = m_Mails.begin(); iter != m_Mails.end();)
	{
		Mail* mail = *iter;
		//if(now.Sec() <= mail->titleInfo.date + MAIL_VALID_TIME) return;
		if (now.Sec() <= mail->titleInfo.deadline) return;

		notify.ids.push_back(mail->titleInfo.id);
		m_MailMap.erase(mail->titleInfo.id);
		CL_SAFE_DELETE(mail);
		iter = m_Mails.erase(iter);
	}
	if(!notify.ids.empty())
	{
		GetOwner()->SendProtocol(notify);
	}
}

void MailBox::OnOnline()
{
	SyncUnreadMailNum();
}

void MailBox::OnLoadMailsRet(SelectMailsCallback* callback)
{
	if(callback == NULL) return;

	while(callback->NextRow())
	{
		ObjID_t id = callback->GetKey();
		if(FindMail(id) != NULL) continue;	//重复
		UInt32 date = callback->GetData("sendtime");
		if (CURRENT_TIME.Sec() > date + MAIL_VALID_TIME) continue;
		UInt32 deadline = callback->GetData("deadline");
		if (CURRENT_TIME.Sec() > deadline) continue;

		Mail* mail = new Mail();
		mail->titleInfo.id =	id;
		mail->senderId =		callback->GetData("senderid");
		mail->senderAccid =		callback->GetData("senderaccid");
		mail->titleInfo.sender = callback->GetData("sendername").GetString();
		mail->titleInfo.date =	date;
		mail->titleInfo.deadline = deadline;
		mail->titleInfo.title =	callback->GetData("title").GetString();
		mail->content =			callback->GetData("content").GetString();
		mail->titleInfo.type =	callback->GetData("mailtype");
		mail->titleInfo.status =	callback->GetData("status");

		mail->titleInfo.replyTimes =	callback->GetData("replytimes");
		mail->silver =			callback->GetData("silver");
		mail->reason =			callback->GetData("reason").GetString();
		
		std::string itemstr =	callback->GetData("items").GetString();
		//std::istringstream stream(itemstr);
		//char split = 0;
		//while(stream)
		//{
		//	ItemReward item;
		//	item.DecodeString(stream);
		//	if(item.id == 0) break;
		//	if (item.num != 0 )
		//	{
		//		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(item.id);
		//		if (dataEntry)
		//		{
		//			/*ReasonInfo param;
		//			if (ParseReason(mail->reason.c_str(), param) && param.type == SOURCE_TYPE_ACTION_BUY) //拍卖行购买邮件需计算交易购买冻结时间戳
		//			{
		//				item.auctionCoolTimeStamp = zjy_auction::AuctionMgr::CalcAuctionItemCoolTime(dataEntry, mail->titleInfo.date);
		//			}*/
		//			mail->items.push_back(item);
		//		}
		//	}
		//	stream >> split;
		//}

		ItemReward::DecodeStringToItemRewardsForMail(itemstr, mail->items);

		// modified by huchenhui
		//if(mail->silver != 0 || !mail->items.empty())
		if(mail->items.size() != 0)
		{
			mail->titleInfo.itemId = mail->items[0].id;
			mail->titleInfo.hasitem = 1;
		}

		m_Mails.insert(m_Mails.begin(),mail);
		m_MailMap.insert(std::make_pair(mail->titleInfo.id, mail));
	}
}

void MailBox::OnLoadMailItemsRet(SelectMailItemsCallback* callback)
{
	if(callback == NULL) return;

	while(callback->NextRow())
	{
		ObjID_t mailid = callback->GetData("mailid");
		Mail* mail = FindMail(mailid);
		if (mail == NULL)
		{
			InfoLogStream << "SelectMailItemsCallback mailId("<< mailid <<") is not exist !" << LogStream::eos;
			continue;
		}

		WSItem* item = new WSItem();
		item->SetID(callback->GetKey());
		item->m_DataId = callback->GetData("itemid");
		item->SetDBData(callback);
		ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
		if (dataEntry)
		{
			item->SetDataEntry(dataEntry);
			item->SetAttribute();
		}
		if(item->m_Num == 0)
		{
			CL_SAFE_DELETE(item);
		}
		else
		{
			mail->detailItems.push_back(item);
			mail->titleInfo.hasitem = 1;
			if (mail->titleInfo.itemId == 0)
			{
				mail->titleInfo.itemId = item->m_DataId;
			}
		}
	}
}

void MailBox::OnCheckPackageRet(const std::vector<ObjID_t>& mailids)
{
	std::vector<MailItemReward> rewards;
	std::vector<ObjID_t> deleteIds;

	UInt32 silver = 0;
	std::vector<Mail*> mails;
	for(std::vector<ObjID_t>::const_iterator iter = mailids.begin(); iter != mailids.end(); ++iter)
	{
		Mail* mail = FindMail(*iter);
		if(mail == NULL) continue;

		// modified by huchenhui
		//silver += mail->silver;
		//rewards.insert(rewards.end(), mail->items.begin(), mail->items.end());
		bool calcAuctionCooltime = false;
		ReasonInfo param;
		if (ParseReason(mail->reason.c_str(), param) && param.type == SOURCE_TYPE_ACTION_BUY
			&& WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_COOLTIME))
		{
			calcAuctionCooltime = true;
		}


		for (UInt32 i = 0; i < mail->items.size(); ++i)
		{
			UInt32 auctionCooltime = 0;
			if (calcAuctionCooltime)
			{
				auctionCooltime = zjy_auction::AuctionMgr::CalcAuctionItemCoolTime(GetOwner()->GetPf(), mail->items[i].id, mail->titleInfo.date);
			}
			MailItemReward reward(mail->items[i].id, mail->items[i].num, mail->items[i].strenth, mail->reason, auctionCooltime, mail->items[i].auctionTransTimes);
			rewards.push_back(reward);
		}

		mails.push_back(mail);
	}

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_MAIL_GIVEITEMS);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	stream & GetOwner()->GetID() & silver & rewards;
	for(std::vector<Mail*>::iterator iter = mails.begin(); iter != mails.end(); ++iter)
	{
		Mail* mail = *iter;
		for(std::vector<WSItem*>::iterator iter = mail->detailItems.begin(); iter != mail->detailItems.end(); ++iter)
		{
			WSItem* item = *iter;

			try
			{
				stream & item->GetID() & item->m_DataId & mail->reason & item->m_SealState & item->m_SealCount;
				ReasonInfo param;
				if (ParseReason(mail->reason.c_str(), param) && param.type == SOURCE_TYPE_ACTION_BUY && WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_COOLTIME))
				{
					UInt32 auctionCooltime = zjy_auction::AuctionMgr::CalcAuctionItemCoolTime(GetOwner()->GetPf(), item->m_DataId, mail->titleInfo.date);
					item->SetAuctionCoolTimeStamp(auctionCooltime);
				}
				item->Encode(stream, SST_SCENE, false);
				//item->EncodeAllField(stream, SST_SCENE);
			}
			catch (const Avalon::NetException& exception)
			{
				mail->detailItems.erase(std::remove(mail->detailItems.begin(), mail->detailItems.end(), (WSItem*)NULL), mail->detailItems.end());
				ErrorLogStream << PLAYERINFO(GetOwner()) << " encode items failed:" << exception.what() << "!" << LogStream::eos;
				return;
			}

			CLDeleteCommand* deleteCmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_mailitem", item->GetID(), GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
			CLRecordClient::Instance()->SendCommand(deleteCmd, NULL, GetOwner()->GetID());

			CL_SAFE_DELETE(item);
			*iter = NULL;
		}

		// modified by huchenhui
		//bool bUpdate = (mail->silver != 0 || !mail->items.empty() || mail->titleInfo.status == 0);
		bool bUpdate = (mail->items.size() != 0 || mail->titleInfo.status == 0);
		
		mail->items.clear();
		mail->detailItems.clear();

		mail->titleInfo.status = 1;
		mail->titleInfo.hasitem = 0;
		mail->silver = 0;

		deleteIds.push_back(mail->titleInfo.id);

		//更新数据库, noted by aprilliu,此处没有等更新数据库成功后就发送了附件，存在道具复制的可能性！！
		if(bUpdate)
		{
			CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_mail", mail->titleInfo.id, true, GetOwner()->GetID(), WSApplication::Instance()->GetTableCount());
			updateCmd->PutData("status",UInt8(1));
			updateCmd->PutData("silver",UInt32(0));
			updateCmd->PutData("items","");
			CLRecordClient::Instance()->SendCommand(updateCmd, NULL, GetOwner()->GetID());
		}
	}
	stream & ObjID_t(0);
	packet->SetSize(stream.GetPos());
	GetOwner()->SendToScene(packet);
	Avalon::Packet::Destroy(packet);
	
	//一键领取以后删除邮件.
	if (!deleteIds.empty())
	{
		DeleteMails(deleteIds);
	}
}

void MailBox::OnRemoveCostRet(ObjID_t receiveId, UInt32 accid, const std::string& receiveName, UInt8 replyTimes, const std::string& title
	, const std::string& content, UInt32 silver, std::vector<WSItem*>& items)
{
	ObjID_t mailid = CLRecordClient::Instance()->GenGuid();
	UInt32 date = UInt32(CURRENT_TIME.Sec());

	bool bFreeItems = true;
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(receiveId);
	if(player != NULL)
	{
		Mail* mail = new Mail();
		mail->titleInfo.id = mailid;
		mail->titleInfo.sender = GetOwner()->GetName();
		mail->titleInfo.date = date;
		mail->titleInfo.type = MAIL_TYPE_NORMAL;
		mail->titleInfo.replyTimes = replyTimes;
		mail->titleInfo.status = 0;
		if(silver != 0 || !items.empty())
		{
			mail->titleInfo.hasitem = 1;
		}
		mail->titleInfo.title = title;
		mail->senderId = GetOwner()->GetID();
		mail->senderAccid = GetOwner()->GetAccId();
		mail->content = content;
		mail->silver = silver;
		mail->detailItems = items;
		bFreeItems = false;
		if(!player->GetMailBox().PutMail(mail))
		{
			CL_SAFE_DELETE(mail);
			items.clear();
			return;
		}
	}

	bool bFirst = true;
	std::ostringstream itemstream;
	for(std::vector<WSItem*>::iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		if(bFirst) bFirst = false;
		else itemstream << '+';
		WSItem* item = *iter;
		itemstream << item->m_DataId << '_' << int(item->m_Bind) << '_' << int(item->m_Quality) << '_' << int(item->m_Num);
	}

	CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand("t_mail", mailid, receiveId, WSApplication::Instance()->GetTableCount());
	insertCommand->PutData("owner",receiveId);
	insertCommand->PutData("senderid",GetOwner()->GetID());
	insertCommand->PutData("senderaccid",GetOwner()->GetAccId());
	insertCommand->PutData("sendername",GetOwner()->GetName());
	insertCommand->PutData("sendtime",date);
	insertCommand->PutData("title",title);
	insertCommand->PutData("content",content);
	insertCommand->PutData("mailtype",UInt8(MAIL_TYPE_NORMAL));
	insertCommand->PutData("status",UInt8(0));
	insertCommand->PutData("replytimes",UInt8(replyTimes));
	insertCommand->PutData("silver",silver);
	insertCommand->PutData("items","");
	CLRecordClient::Instance()->SendCommand(insertCommand, NULL, GetOwner()->GetID());

	for(std::vector<WSItem*>::iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		WSItem* item = *iter;
		item->SetID(CLRecordClient::Instance()->GenGuid());

		CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand("t_mailitem",item->GetID(), receiveId, WSApplication::Instance()->GetTableCount());
		insertCommand->PutData("mailid", mailid);
		insertCommand->PutData("owner",receiveId);
		insertCommand->PutData("itemid",item->m_DataId);
		item->GetDBData(insertCommand, false);
		CLRecordClient::Instance()->SendCommand(insertCommand, NULL, receiveId);

		if(bFreeItems) CL_SAFE_DELETE(item);
	}
	items.clear();

	++m_SendCounts[GetOwner()->GetID()];

	if(GetOwner()->GetLevel() < 70)
	{
// 		std::ostringstream oss;
// 		oss << "[TITLE]" << title << "[WORD]" <<  content;
// 		CLProtocol::CenterSyncChatInfo protocol;
// 		protocol.serverId = ZONE_ID;
// 		protocol.accId = GetOwner()->GetAccId();
// 		protocol.channel = 100;
// 		protocol.level = GetOwner()->GetLevel();
// 		protocol.viplvl = GetOwner()->GetVipLvl();
// 		protocol.name = GetOwner()->GetName();
// 		protocol.word = oss.str();
// 		Router::SendToCenter(protocol);
	}

	GetOwner()->SendNotify(407);
}

Mail* MailBox::FindMail(ObjID_t id)
{
	MailMap::iterator iter = m_MailMap.find(id);
	if(iter != m_MailMap.end()) return iter->second;
	return NULL;
}

void MailBox::SyncMailStatus(Mail* mail)
{
	CLProtocol::WorldSyncMailStatus sync;
	sync.id = mail->titleInfo.id;
	sync.status = mail->titleInfo.status;
	sync.hasitem = mail->titleInfo.hasitem;
	GetOwner()->SendProtocol(sync);
}

void MailBox::SyncUnreadMailNum()
{
	CLProtocol::WorldSyncUnreadMailNum protocol;
	protocol.num = 0;
	for(MailList::iterator iter = m_Mails.begin();
		iter != m_Mails.end(); ++iter)
	{
		Mail* mail = *iter;
		if(mail->titleInfo.status == 0 && GetOwner()->GetRelationMgr().FindRelation(RELATION_BLACKLIST, mail->senderId) == NULL)
		{
			++protocol.num;
		}
	}
	GetOwner()->SendProtocol(protocol);
}

