#include "WorldMailHandler.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "WSItem.h"
#include "MailRecvMgr.h"

void HandleProtocol(WSPlayer* player, CLProtocol::WorldMailListReq& req)
{
	//player->GetMailBox().ListMails(req.type, req.start, req.num);
	player->GetMailBox().ListMails();
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldReadMailReq& req)
{
	player->GetMailBox().ReadMail(req.id);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetMailItems& req)
{
	if(req.type == 0)
	{
		player->GetMailBox().GetMailItems(req.id);
	}
	else
	{
		player->GetMailBox().GetMailItems(req.mailType);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSendMail& req)
{
	if(req.replyId == 0)
	{
		player->GetMailBox().SendMail(req.receiver, req.title, req.content, req.silver, req.items);
	}
	else
	{
		player->GetMailBox().ReplyMail(req.replyId, req.title, req.content, req.silver, req.items);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDeleteMail& req)
{
	player->GetMailBox().DeleteMails(req.ids);
}

void HandleProtocol(CLProtocol::SceneMailCheckPackageRet& ret)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(ret.playerId);
	if(player == NULL) return;

	player->GetMailBox().OnCheckPackageRet(ret.mailids);
}

void MailRemoveCostRetFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	ObjID_t playerID = 0;
	stream & playerID;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerID);
	if(player == NULL) return;

	ObjID_t receiverId = 0;
	UInt32 accid = 0;
	std::string	receiverName;
	UInt8 replyTimes = 0;
	std::string title;
	std::string content;
	UInt32 silver = 0;
	std::vector<WSItem*> items;

	stream & receiverId & accid & receiverName & replyTimes & title & content & silver;

	UInt32 dataId = 0;
	stream & dataId;
	while(dataId != 0)
	{
		WSItem* item = new WSItem();
		item->m_DataId = dataId;
		item->Decode(stream);
		items.push_back(item);

		stream & dataId;
	}

	player->GetMailBox().OnRemoveCostRet(receiverId, accid, receiverName, replyTimes, title, content, silver, items);
}

/*
void HandleProtocol(CLProtocol::WorldSendSysMail& req)
{
	InfoLogStream << "HandleProtocol : WorldSendSysMail recv success " << LogStream::eos;
	MailBox::SendSysMail(req.playerId, req.senderName, req.title, req.content, req.reason, req.items, req.validDay );
}
*/

void WorldSendSysMailFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

	ObjID_t playerId;
	std::vector<WSItem*>	items;
	std::string senderName;
	std::string title;
	std::string content;
	std::vector<ItemReward> itemRewards;
	std::string reason;
	//有效天数
	UInt32 validDay;

	try
	{
		stream & playerId & senderName & title & content & reason & validDay & itemRewards;

		ObjID_t itemId = 0;
		stream & itemId;
		while (itemId != 0)
		{
			UInt32 itemDataId = 0;
			stream & itemDataId;
			WSItem* item = new WSItem();
			if (item == NULL) return;

			item->SetID(itemId);
			item->m_DataId = itemDataId;
			item->Decode(stream);
			items.push_back(item);

			stream & itemId;
		}
	}
	catch (Avalon::NetException& exception)
	{
		ErrorLogStream << "WorldSendSysMailFunction encode error failed:" << exception.what() << "!" << LogStream::eos;
		return;
	}
	MailBox::SendSysMail(playerId, senderName, title, content, reason, itemRewards, items, validDay);
}

void HandleProtocol(CLProtocol::WorldSendSysMailToTribeLeader& req)
{
}

void HandleProtocol(CLProtocol::WorldTeamCopySendMail& req)
{
	MailBox::SendSysMail(req.roleId, req.name, req.title, req.content, req.reason, req.rewards);
}

void HandleProtocol(CLProtocol::WorldUnionSendMail & req)
{
	InfoLogStream << "mailGuid:" << req.mailGuid << " player:" << req.roleId << " receive mail("
		<< req.name << "|" << req.title << "|" << req.content << "|" << req.reason << LogStream::eos;

	if (req.mailGuid != 0 && !MailRecvMgr::Instance()->RecvMail(ST_UNION, req.mailGuid))
	{
		ErrorLogStream << "repeated recv mailGuid:" << req.mailGuid << " player:" << req.roleId << " receive mail("
			<< req.name << "|" << req.title << "|" << req.content << "|" << req.reason << LogStream::eos;
		return;
	}

	MailBox::SendSysMail(req.roleId, req.name, req.title, req.content, req.reason, req.rewards);
}

void TestItemMailFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	ObjID_t playerId;

	std::vector<WSItem*>	items;
	std::string senderName;
	std::string title;
	std::string content;
	std::vector<ItemReward> itemRewards;
	std::string reason;

	try
	{
		stream & playerId & senderName & title & content & reason & itemRewards;

		ObjID_t itemId = 0;
		stream & itemId;
		while (itemId != 0)
		{
			UInt32 itemDataId = 0;
			stream & itemDataId;
			WSItem* item = new WSItem();
			if (item == NULL) return;

			item->SetID(itemId);
			item->m_DataId = itemDataId;
			item->Decode(stream);
			items.push_back(item);

			stream & itemId;
		}
	}
	catch (Avalon::NetException&)
	{
		return;
	}

	MailBox::SendSysMail(playerId, senderName, title, content, reason, itemRewards, items);
}

