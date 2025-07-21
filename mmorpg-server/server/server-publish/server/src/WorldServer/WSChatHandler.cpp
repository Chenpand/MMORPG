#include "WSChatHandler.h"

#include <CLAnnouncementDataEntry.h>
#include <CLTransferProtocol.h>
#include "ChatDataMgr.h"
#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "WSTeamMgr.h"
#include "WorldSysNotify.h"
#include "WSRouter.h"
#include "WorldPlayerScript.h"
#include "ScriptMgr.h"
#include "GameParamMgr.h"
#include "OfflineNotifyMgr.h"
#include "AnnouncementMgr.h"
#include "NewAuctionMgr.h"
#include "GuildMgr.h"
#include "WSPushNotify.h"
#include "RoomMgr.h"
#include "WSRouter.h"

void TransChatFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	ObjID_t targetId = 0;
	CLProtocol::SceneSyncChat sync;
	stream & targetId & sync;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(sync.objid);
	if(player == NULL) return;
	if(player->GetID() == targetId) return;

	switch(sync.channel)
	{
	case CHAT_CHANNEL_TEAM:
	case CHAT_CHANNEL_TEAM_SPECIAL:
		{
			auto team = player->GetTeam();
			if(team != NULL)
			{
				team->Broadcast(sync);
			}
		}
		break;
	case CHAT_CHANNEL_PRIVATE:
		{
			WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
			if(target == NULL)
			{
				ChatDataInfo* chatData = new ChatDataInfo();
				chatData->channel = sync.channel;
				chatData->objid = sync.objid;
				chatData->sex = sync.sex;
				chatData->occu = sync.occu;
				chatData->level = sync.level;
				chatData->viplvl = sync.viplvl;
				chatData->objname = sync.objname;
				chatData->receiverId = sync.receiverId;
				chatData->word = sync.word;
				chatData->bLink = sync.bLink;
				chatData->isGm = sync.isGm;
				chatData->voiceKey = sync.voiceKey;
				chatData->voiceDuration = sync.voiceDuration;
				chatData->headFrame = sync.headFrame;

				player->PutOfflineEvent(targetId, new SendPrivateChatEvent(chatData));

				// 玩家离线的时候发送推送消息
				{
					auto title = SysNotifyMgr::Instance()->MakeString(8516);
					auto content = SysNotifyMgr::Instance()->MakeString(8517);
					pushNotifyToCenterByRoleID(targetId, title, content);
				}
			}
			else
			{
				target->SendProtocol(sync);
			}
			
			player->SendProtocol(sync);
		}
		break;
	case CHAT_CHANNEL_TRIBE:
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
			if (!guild)
			{
				ErrorLogStream << PLAYERINFO(player) << " guild is null! id = " << player->GetGuildID() << LogStream::eos;
				return;
			}

			guild->Broadcast(sync);
		}
		break;
	case CHAT_CHANNEL_WORLD:
	case CHAT_CHANNEL_TEAM_INVITE:
		{
			Router::BroadcastToPlayer(sync, 0);
		}
		break;
	case CHAT_CHANNEL_ROOM:
		if (RoomMgr::Instance()->IsCross())
		{
			Router::SendToCross(sync);
		}
		else
		{
			Room* room = RoomMgr::Instance()->GetRoom(player->GetRoomId());
			if (room != NULL)
			{
				room->Broadcast(sync);
			}
		}
		break;
	}
}

void CacheChatDataFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
	UInt8	tagType = 0;
	stream & tagType;
	switch (tagType)
	{
	case 'I':
	case 'R':
		{
			ObjID_t id;
			stream & id;
			ChatDataMgr::Instance()->AddData(id, (const char*)packet->GetBuffer() + sizeof(UInt8), packet->GetSize() - sizeof(UInt8));
		}
		break;
	case 'A':
		{
			/*ObjID_t id;
			stream & id;
			WSItem*	pItem = zjy_auction::AuctionMgr::Instance()->(id);
			if (NULL == pItem)
				return;

			Avalon::Packet	*pPacket = Avalon::Packet::Create();
			Avalon::NetOutputStream	os(pPacket->GetBuffer(), pPacket->GetMaxSize());

			os & id & pItem->m_DataId;
			pItem->Encode(os, SST_WATCH, false);
			pPacket->SetSize(os.GetPos());

			ChatDataMgr::Instance()->AddData(id, (const char*)pPacket->GetBuffer(), pPacket->GetSize());
			Avalon::Packet::Destroy(pPacket);*/
		}
		break;
	default:
		break;
	}
	
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChatLinkDataReq& req)
{
	if (NULL == player) return;
	
	if (req.zoneId != 0 && req.zoneId != ZONE_ID)
	{
		CLProtocol::TransferQueryPlayerTipsOne query;
		query.type = req.type;
		query.id = req.id;
		TransferHeader head(req.zoneId, ST_WORLD, 0, player->GetID());
		Router::SendToTransfer(head, query);
		return;
	}

	ChatDataMgr::Instance()->SendLinkData(player, req.type, req.id);
}

void HandleProtocol(CLProtocol::WorldTransmitRequest& req)
{
	WSPlayer* requester = WSPlayerMgr::Instance()->FindNormalPlayer(req.requester);
	if(requester == NULL) return;

	switch(req.type)
	{
	case REQUEST_INVITETEAM:
		{
			CLProtocol::WorldTeamInviteRes res;
			res.targetId = req.target;
			res.result = requester->InviteTeam(req.target);
			requester->SendProtocol(res);
		}
		break;
	case REQUEST_JOINTEAM:
		{
			requester->RequestTeam(req.target, req.targetName);
		}
		break;
	case REQUEST_JOINTEAM_TEAMID:
		{
			requester->RequestJoinTeam((UInt32)req.target, req.targetName);
		}
		break;
	case REQUEST_FRIEND:
		{
			requester->InviteFriend(req.target,"");
		}
		break;
	case REQUEST_FRIEND_BY_NAME:
		{
			requester->InviteFriend(0, req.targetName);
		}
		break;
	case REQUEST_JOINCOPYTEAM:
		{
			WSTeam* team = WSTeamMgr::Instance()->FindTeam(req.id);
			if(team == NULL)
			{
				requester->SendNotify(522);
				return;
			}
			requester->RequestTeam(team->GetMaster(), req.targetName);
		}
		break;
    case REQUEST_CHALLENGE_PK:
        {
			requester->RequestPkChallenge(req.target, REQUEST_CHALLENGE_PK);
        }
        break;
	case REQUEST_INVITEGUILD:
		{
			requester->InviteJoinGuild(req.target);
		}
		break;
	case REQUEST_MASTER:
		{
			requester->ReqMaster(req.target, req.targetName);
		}
		break;
	case REQUEST_DISCIPLE:
		{
			requester->ReqDisciple(req.target, req.targetName);
		}
		break;
	case REQUEST_EQUAL_PK:
		{
			requester->RequestPkChallenge(req.target, REQUEST_EQUAL_PK);
		}
	}
}

void HandleProtocol(CLProtocol::WorldTransmitReply& req)
{
	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(req.target);
	if(target == NULL) return;

	switch(req.type)
	{
	case REQUEST_INVITETEAM:
		{
			UInt32 result = target->ReplyInviteTeam((UInt32)req.requester, req.result == 1);
			if (req.result == 1 && result != ErrorCode::SUCCESS)
			{
				target->SendNotify(result);
			}
		}
		break;
	case REQUEST_JOINTEAM:
		{
			if(req.result == 1)
			{
				target->ReplyRequestTeam(req.requester);
			}
		}
		break;
	case REQUEST_FRIEND:
		{
			if(req.result == 1)
			{
				target->ReplyInviteFriend(req.requester);
			}
		}
		break;
	case REQUEST_QUICKFRIEND:
		{
			if(req.result == 1)
			{
				target->ReplyInviteFriend(req.requester);
			}
		}
		break;
	case REQUEST_CHALLENGE_PK:
		{
			target->ReplyPkChallenge(req.requester, req.result != 0, REQUEST_CHALLENGE_PK);
		}
		break;
	case REQUEST_MASTER:
		{
			if (req.result == 1)
			{
				target->ReplyMaster(req.requester); //师傅同意拜师
			}
		}
		break;
	case REQUEST_DISCIPLE:
		{
			if (req.result == 1)
			{
				target->ReplyDisciple(req.requester); //徒弟同意收徒
			}
		}
		break;
	case REQUEST_EQUAL_PK:
		{
			target->ReplyPkChallenge(req.requester, req.result != 0, REQUEST_EQUAL_PK);
		}
		break;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldOfflineNotifyReq& req)
{
	OfflineNotifyMgr::Instance()->SendOfflineNotifys(player, req.type);
}

void HandleProtocol(CLProtocol::WorldSendOfflineNotify& req)
{
	OfflineNotifyMgr::Instance()->PutOfflineNotify(req.type, req.word);
}

void HandleProtocol(CLProtocol::WorldSendAnnouncement& req)
{
	AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(req.id);
	if (dataEntry == NULL) return;

	UInt32 startTime = UInt32(CURRENT_TIME.Sec());
	if (req.startTime != 0 && req.startTime > startTime)
		startTime = req.startTime;

	if (req.content.empty())
	{
		AnnouncementMgr::Instance()->PublicAnnouncement(req.guildId, req.id, startTime);
	}
	else
	{
		AnnouncementMgr::Instance()->SaveAnnouncement(req.id, startTime, dataEntry->interval, dataEntry->times, req.content);
		// 发送到公会日志
		if ((dataEntry->SyncEventBit & AnnouncementSyncEvent::SyncGuildLog) && req.guildId != 0)
		{
			Guild* guild = GuildMgr::Instance()->FindGuildByID(req.guildId);
			if (guild != NULL)
			{
				guild->AddGuildEvent(req.content);
			}
		}
	}
}

void HandleProtocol(CLProtocol::WorldChatHorn& horn)
{
	Router::BroadcastToPlayer(horn);
}
