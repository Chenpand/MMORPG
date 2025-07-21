#include "Creature.h"

#include <algorithm>
#include <math.h>
#include <AvalonRandom.h>
#include <CLMoveProtocol.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>
#include <CLRoutineProtocol.h>

#include "SkillMgr.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "SSRouter.h"
#include "Player.h"
#include "Team.h"
#include "ScriptMgr.h"
#include "FinalEquipScript.h"
#include "ActivityMgr.h"
#include "PlayerMgr.h"

Creature::Creature()
{
	m_SkillMgr.SetOwner(this);
	m_BuffMgr.SetOwner(this);
	m_SavebackTime = 0;
    m_isHidden = false;
	m_MoveSpeed = PLAYER_INIT_MOVE_SPEED;
}

Creature::~Creature()
{
}

Player* Creature::ToPlayer() const
{
	return GetType() == SOT_PLAYER ? (Player*)this : NULL;
}

bool Creature::UseSkill(UInt16 id, const CLPosition& pos, ObjID_t targetid, bool bPrepared)
{
	if(GetStatus() != CREATURE_STATUS_NORMAL) return false;

	Skill* skill = m_SkillMgr.FindSkill(id);
	return UseSkill(skill, pos, targetid, bPrepared);
}

bool Creature::ScriptUseSkillDirect(UInt16 id, UInt8 level, ObjID_t targetid)
{
	Skill	tmpskill;
	tmpskill.id = id;
	tmpskill.level = level;
	if(!tmpskill.Init()) return false;

	Scene	*pScene = GetScene();
	SceneObject	*pObject = pScene->FindObject(targetid);
	if(NULL == pObject)
	{
		return UseSkill(&tmpskill, GetPos(), 0, true);
	}
	else
	{
		return UseSkill(&tmpskill, pObject->GetPos(), targetid, true);
	}
}


bool Creature::UseSkill(Skill* skill, const CLPosition& pos, ObjID_t targetid, bool bPrepared)
{
	if (skill == NULL) return false;
	if (GetStatus() != CREATURE_STATUS_NORMAL) return false;
	return true;
}

Skill* Creature::FindSkill(UInt16 skillid)
{
	return m_SkillMgr.FindSkill(skillid);
}

Buff* Creature::FindBuff(UInt16 buffid)
{
	return m_BuffMgr.FindBuff(buffid);
}

bool Creature::HasBuff(UInt16 buffid)
{
	return FindBuff(buffid) != NULL;
}

UInt32 Creature::GetMoveInterval() const
{
	return UInt32(400.f / (1.f + m_MoveSpeed) * 300);
}

void Creature::Speak(UInt8 channel, const std::string& word, std::string voiceKey, UInt8 voiceDuration, ObjID_t target, bool bLink, UInt32 headFrame)
{
	if(channel == 0 || channel >= CHAT_CHANNEL_MAX) return;

	CLProtocol::SceneSyncChat protocol;
	protocol.channel = channel;
	protocol.objid = GetID();
	protocol.occu = GetOccu();
	protocol.sex = GetSex();
	protocol.level = GetLevel();
	protocol.viplvl = GetVipLvl();
	if(GetType() == SOT_PLAYER)
	{
		//protocol.qqVipInfo = ((Player*)this)->GetQQVipInfo();
	}
	protocol.objname = GetName();
	protocol.receiverId = target;
	protocol.word = word;
	protocol.bLink = bLink;
	protocol.voiceKey = voiceKey;
	protocol.voiceDuration = voiceDuration;

	protocol.isGm = HasGmAuthority(GM_AUTHORITY_SUPER);
	protocol.headFrame = headFrame;

	switch(channel)
	{
	case CHAT_CHANNEL_AROUND:
		{
			class BroadAroundChatVisit : public PlayerVisitor
			{
			public:
				BroadAroundChatVisit(Creature* my, UInt32 sceneId, CLProtocol::SceneSyncChat* chatProtocol)
					:m_my(my), m_SceneId(sceneId), m_chatProtocol(chatProtocol), m_IsInGuildScene(false), m_GuildId(0)
				{
					if (m_my->GetScene() && m_my->GetScene()->GetDataEntry())
					{
						auto data = m_my->GetScene()->GetDataEntry();
						if (data->subType == SCENE_SUB_GUILD)
						{
							m_IsInGuildScene = true;
						}	
					}

					Player* player = (Player*)m_my;
					m_GuildId = player->GetGuildId();
				}

				bool operator()(Player* player)
				{
					if (player->GetScene()->GetType() != SCENE_TYPE_SINGLE &&
						m_SceneId == player->GetSceneID())
					{
						if (m_IsInGuildScene && m_GuildId != player->GetGuildId())
						{
							return true;
						}

						player->SendProtocol(*m_chatProtocol);
					}

					
					return true;
				}

			private:
				Creature* m_my;
				UInt32	m_SceneId;
				CLProtocol::SceneSyncChat* m_chatProtocol;
				bool m_IsInGuildScene;
				ObjID_t m_GuildId;
			};

			BroadAroundChatVisit visitor(this, GetSceneID(), &protocol);
			PlayerMgr::Instance()->VisitNormalPlayers(visitor);
		}
		break;
	case CHAT_CHANNEL_WORLD:
		{
			if (!ActivityMgr::Instance()->IsInActivity("chat_question"))
			{
				Router::BroadcastToPlayer(protocol, 0);
				break;
			}
		}
	case CHAT_CHANNEL_TEAM:
	case CHAT_CHANNEL_TEAM_SPECIAL:
	case CHAT_CHANNEL_PRIVATE:
	case CHAT_CHANNEL_TRIBE:
	case CHAT_CHANNEL_TEAM_INVITE:
		{
			Avalon::Packet* packet = Avalon::Packet::Create();
			packet->SetID(CLProtocol::WORLD_TRANSMIT_CHAT);
			Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
			try
			{
				stream & target & protocol;
			}
			catch(Avalon::NetException&)
			{
				Avalon::Packet::Destroy(packet);
				return;
			}
			packet->SetSize(stream.GetPos());
			Router::SendToWorld(packet);
			Avalon::Packet::Destroy(packet);
		}
		break;
	default:
		break;
	}
}

void Creature::ScriptSpeak(UInt8 channel, const char* word)
{
	if(word == NULL) return;

	Creature::Speak(channel, word);
}

void Creature::OnHeartbeat(const Avalon::Time& now)
{
	SceneObject::OnHeartbeat(now);

	m_BuffMgr.OnTick(now);
	m_SkillMgr.OnTick(now);
}

void Creature::OnOffline()
{
}

void Creature::OnEnterScene()
{
	SceneObject::OnEnterScene();
}

void Creature::OnLeaveScene()
{
	SceneObject::OnLeaveScene();

	/*m_SkillMgr.SetDirty(DB_SYNC_TYPE);
	m_BuffMgr.SetDirty(DB_SYNC_TYPE);*/
}

void Creature::OnSceneChanged()
{
	SceneObject::OnSceneChanged();
}

void Creature::OnMoved(const CLPosition& oldpos)
{
	SceneObject::OnMoved(oldpos);
}

void Creature::ClearInvalidData()
{
	//m_SkillMgr.ClearInvalidData();
}


void Creature::SyncProperty()
{
	SceneObject::SyncProperty();
}

void Creature::AddMoveSpeed(Int16 val)
{
	m_MoveSpeed = m_MoveSpeed + val;
}
