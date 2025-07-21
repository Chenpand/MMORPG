#include "SceneSysNotify.h"

#include <CLUtf8Define.h>
#include <CLErrorCode.h>
#include <AvalonPacket.h>

#include "Item.h"

namespace Sys
{
	//by huchenhui
	/*
	void SendNotify(ObjID_t playerId, UInt32 id, ...)
	{
		va_list args;
		va_start(args, id);
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN+1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
		va_end(args);

		if(!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		Router::TransmitToPlayer(playerId, notify);
	}
	*/

	/*
	void BroadcastNotify(UInt32 id, ...)
	{
		va_list args;
		va_start(args, id);
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN+1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args);
		va_end(args);

		if(!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.color = color;
		notify.word = buffer;
		Router::BroadcastToPlayer(notify);
	}
	*/

	//UInt32 SendSysMail(ObjID_t tReceiverId, const char* title, const char* content, UInt32 silver, RewardGroup* rewards, const char* senderName)
	UInt32 SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, UInt32 validDay, RewardGroup* rewards, std::vector<Item*> detailItems)
	{
		UInt32 itemSize = 0;
		if (rewards != NULL) itemSize = rewards->GetRewards().size();
		itemSize += detailItems.size();
		if (validDay > MAIL_VALID_DAY) validDay = MAIL_VALID_DAY;
		UInt32 res = CheckMail(tReceiverId, title, content, itemSize, senderName);
		if (res != ErrorCode::SUCCESS)
		{
			return res;
		}

		std::vector<ItemReward> itemRewards = std::vector<ItemReward>();
		if (rewards != NULL)
		{
			itemRewards = rewards->GetRewards();
		}
		std::string strSenderName = senderName;
		std::string strTitle = title;
		std::string strContent = content;
		std::string strReason = reason;

		Avalon::Packet* pPacket = Avalon::Packet::Create();
		pPacket->SetID(CLProtocol::WORLD_SEND_SYSMAIL);
		Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
		try
		{
			stream & tReceiverId & strSenderName & strTitle & strContent & strReason & validDay & itemRewards;
			for (UInt32 i = 0; i < detailItems.size(); ++i)
			{
				stream & detailItems[i]->GetID() & detailItems[i]->GetDataID();
				detailItems[i]->Encode(stream, SST_WORLD, false);
			}
			stream & ObjID_t(0);
			pPacket->SetSize(stream.GetPos());
			Router::SendToWorld(pPacket);
			Avalon::Packet::Destroy(pPacket);
		}
		catch (Avalon::NetException& exception)
		{
			Avalon::Packet::Destroy(pPacket);
			ErrorLogStream << "SendSysMail encode error failed:" << exception.what() << "!" << LogStream::eos;
			return ErrorCode::MAIL_ERROR;
		}

		InfoLogStream << "Scene --> World SendSysMail : success " << LogStream::eos;

		return ErrorCode::SUCCESS;

		/*
		CLProtocol::WorldSendSysMail req;
		req.title = title;
		if (content != NULL)
		{
			req.content = content;
		}
		req.playerId = tReceiverId;
		req.senderName = senderName;
		req.reason = reason;
		req.validDay = validDay;
		//req.silver = silver;
		if(rewards != NULL)
		{
			req.items = rewards->GetRewards();
		}
		Router::SendToWorld(req);

		return ErrorCode::SUCCESS;
		*/
	}

	UInt32 SendSysMail(ObjID_t tReceiverId, const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards, UInt32 validDay)
	{
		return SendSysMail(tReceiverId, senderName, title, content, reason, validDay, rewards);
	}

	/*
	void SendOfflineNotify(const std::string& type, UInt32 id, ...)
	{
		va_list args;
		va_start(args, id);
		UInt16 itype = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN+1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, itype, color, buffer, CL_MAX_CHAT_LEN, args);
		va_end(args);

		if(!ret) return;

		CLProtocol::WorldSendOfflineNotify req;
		req.type = type;
		req.word = buffer;
		Router::SendToWorld(req);
	}
	*/

	void SendOfflineNotify(const std::string& type, const std::string& word)
	{
		CLProtocol::WorldSendOfflineNotify req;
		req.type = type;
		req.word = word;
		Router::SendToWorld(req);
	}
};
