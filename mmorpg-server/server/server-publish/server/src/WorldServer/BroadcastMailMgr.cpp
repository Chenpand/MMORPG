#include "BroadcastMailMgr.h"

#include <CLRecordClient.h>
#include "WSSelectInitDataCallback.h"
#include "WSPlayerMgr.h"

class SelectBroadcastMailCallback : public SelectInitDataCallback
{
public:
	bool OnLoadFinished()
	{
		return BroadcastMailMgr::Instance()->OnSelectBroadcastMailRet(this);
	}
};

BroadcastMailMgr::BroadcastMailMgr()
{
}

BroadcastMailMgr::~BroadcastMailMgr()
{
}

void BroadcastMailMgr::Init()
{
	CLSelectCommand* selectcmd1 = CLRecordClient::Instance()->PrepareSelectCommand("t_broadcastmail");
	CLRecordClient::Instance()->SendCommand(selectcmd1, new SelectBroadcastMailCallback());
}

void BroadcastMailMgr::Final()
{
	m_BroadcastMails.clear();
}

bool BroadcastMailMgr::PutMail(const std::string& senderName, const std::string& title, const std::string& content, 
	const std::string& reason, const std::vector<ItemReward>& items, UInt16 level, std::string pf, UInt8 occu, UInt32 playerChargeNum)
{
	if(ErrorCode::SUCCESS != CheckMail(0,title, content, items.size(), senderName)) return false;

	BroadcastMail entry;
	entry.id = CLRecordClient::Instance()->GenGuid();
	entry.sendtime = UInt32(CURRENT_TIME.Sec());
	entry.senderName = senderName;
	entry.title = title;
	entry.content = content;
	entry.items = items;
	entry.level = level;
	entry.reason = reason;
	entry.pf = pf;
	entry.occu = occu;
	entry.playerChargeNum = playerChargeNum;
	m_BroadcastMails.push_back(entry);

	CLInsertCommand* insertcmd = CLRecordClient::Instance()->PrepareInsertCommand("t_broadcastmail", entry.id);
	insertcmd->PutData("sendtime", entry.sendtime);
	insertcmd->PutData("sendername", entry.senderName);
	insertcmd->PutData("title", entry.title);
	insertcmd->PutData("content", entry.content);
	std::ostringstream stream;
	for(std::vector<ItemReward>::const_iterator iter = items.begin();
		iter != items.end(); ++iter)
	{
		iter->EncodeString(stream);
		stream << ';';
	}
	insertcmd->PutData("items", stream.str());
	insertcmd->PutData("level", level);
	insertcmd->PutData("reason", entry.reason);
	insertcmd->PutData("platform", pf);
	insertcmd->PutData("occu", occu);
	insertcmd->PutData("playerChargeNum", playerChargeNum);
	CLRecordClient::Instance()->SendCommand(insertcmd);

	class SendSysMailVisitor : public WSPlayerVisitor
	{
	public:
		SendSysMailVisitor(const BroadcastMail& mail) : m_mail(mail){}

		bool operator()(WSPlayer* player)
		{
			if (BroadcastMailMgr::Instance()->CheckMailCanSend(player, &m_mail, false))
			{
				UInt32 nRes = MailBox::SendSysMail(player->GetID(), m_mail.senderName, m_mail.title, m_mail.content, m_mail.reason, m_mail.items);
				if (nRes == ErrorCode::SUCCESS)
				{
					player->SaveOfflineMailTime(CURRENT_TIME.MSec());
				}
			}
			return true;
		}
	private:
		BroadcastMail m_mail;
		
	};
	SendSysMailVisitor visitor(entry);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
	return true;
}

bool BroadcastMailMgr::OnSelectBroadcastMailRet(CLRecordCallback* callback)
{
	while(callback->NextRow())
	{
		BroadcastMail entry;
		entry.id = callback->GetKey();
		entry.sendtime = callback->GetData("sendtime");
		if(CURRENT_TIME.Sec() > entry.sendtime + MAIL_VALID_TIME) continue;
		entry.senderName = callback->GetData("sendername").GetString();
		entry.title = callback->GetData("title").GetString();
		entry.content = callback->GetData("content").GetString();
		entry.reason = callback->GetData("reason").GetString();
		std::string itemstr =	callback->GetData("items").GetString();
		//std::istringstream stream(itemstr);
		//char split = 0;
		//while(stream)
		//{
		//	ItemReward item;
		//	item.DecodeString(stream);
		//	if(item.id == 0) break;
		//	entry.items.push_back(item);
		//	stream >> split;
		//}
		ItemReward::DecodeStringToItemRewardsForMail(itemstr, entry.items);

		entry.level = callback->GetData("level");
		entry.pf = callback->GetData("platform").GetString();
		entry.occu = callback->GetData("occu");
		entry.playerChargeNum = callback->GetData("playerChargeNum");
		m_BroadcastMails.push_back(entry);
	}

	return true;
}

void BroadcastMailMgr::OnTick(const Avalon::Time& now)
{
	for(BroadcastMailVec::iterator iter = m_BroadcastMails.begin();
		iter != m_BroadcastMails.end();)
	{
		if(now.Sec() > iter->sendtime + MAIL_VALID_TIME)
		{
			iter = m_BroadcastMails.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void BroadcastMailMgr::OnPlayerOnline(WSPlayer* player)
{
	if(player->GetSrcZoneId() != 0) return;

	for(BroadcastMailVec::iterator iter = m_BroadcastMails.begin(); iter != m_BroadcastMails.end(); ++iter)
	{
		//上次离线时间在广播邮件发送之前
		if (CheckMailCanSend(player, &(*iter)))
		{
			MailBox::SendSysMail(player->GetID(), iter->senderName, iter->title, iter->content, iter->reason, iter->items, MAIL_VALID_DAY, iter->sendtime);
		}
	}

	player->SaveOfflineMailTime(CURRENT_TIME.MSec());
}

bool BroadcastMailMgr::FitPlatform(std::string playerPf, std::string pfs)
{
	if (pfs == "")
	{
		return true;
	}

	std::vector<std::string> pf_vec;
	Avalon::StringUtil::Split(pfs, pf_vec, ",");

	for (size_t i = 0; i < pf_vec.size(); ++i)
	{
		if (playerPf == pf_vec[i])
		{
			return true;
		}
	}

	return false;
}

bool BroadcastMailMgr::CheckMailCanSend(WSPlayer* player, BroadcastMail* mail, bool checkOfflineMailTime)
{
	if (!player || !mail)	return false;
	if ( (checkOfflineMailTime == false || (player->GetOfflineMailTime() != 0 && player->GetOfflineMailTime() < UInt64(mail->sendtime) * 1000))
		&& player->GetLevel() >= mail->level
		&& player->GetSrcZoneId() == 0
		&& FitPlatform(player->GetPf(), mail->pf)
		&& (mail->occu == 0 || ((player->GetOccu() - player->GetOccu() % 10) == mail->occu))
		&& (mail->playerChargeNum == 0 || player->GetTotalPlayerChargeNum() >= mail->playerChargeNum))
	{
		return true;
	}
	return false;
}