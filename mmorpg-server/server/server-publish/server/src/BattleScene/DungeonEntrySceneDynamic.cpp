#include "DungeonEntrySceneDynamic.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "Creature.h"
#include "LostDungeonMgr.h"
#include "CLBattleProtocol.h"
#include "CLBattleDefine.h"
#include "SSRouter.h"
#include "SceneMgr.h"

DungeonEntrySceneDynamic::DungeonEntrySceneDynamic()
	:DungeonEntryScene()
{
}

DungeonEntrySceneDynamic::DungeonEntrySceneDynamic(const Scene& scene)
	: DungeonEntryScene(scene)
{
}

DungeonEntrySceneDynamic::~DungeonEntrySceneDynamic()
{
	InfoLogStream << "lost dungeon dynamic dungeon entry scene is destroyed, battle id : " << this->GetBattleId()
		<< ", scene id : " << this->GetID() << LogStream::eos;
}

void DungeonEntrySceneDynamic::Destroy()
{
	InfoLogStream << "lost dungeon dynamic dungeon entry scene is begin destroy, change status to SCENE_QUIT, battle id : " << this->GetBattleId()
		<< ", scene id : " << this->GetID() << LogStream::eos;

	m_KickTimer.SetInterval(1000);
	this->SetStatus(SCENE_QUIT);
}

void DungeonEntrySceneDynamic::OnSceneQuit()
{
	SceneMgr::Instance()->DelDungeonEntrySceneDynamic(GetMajorCity(), GetMajorDoor(), GetMapID());
}

bool DungeonEntrySceneDynamic::CheckPlayerEnter(ObjID_t id, UInt16 level)
{
	Player* player = PlayerMgr::Instance()->FindPlayer(id);
	if (!player)
	{
		return false;
	}
	if (!LostDungeonMgr::Instance()->IsSceneHaveEmptyTeam(GetID()))
	{
		player->SendNotify(1305);
		return false;
	}
	return true;
}

void DungeonEntrySceneDynamic::OnPlayerEnter(Player* player)
{
	if (!player)	return;

	LostDungeonMgr::Instance()->OnPlayerEnterDungeonEntry(player, this);
}

void DungeonEntrySceneDynamic::OnPlayerLeave(Player* player)
{
	if (!player) return;

	LostDungeonMgr::Instance()->OnPlayerLeaveDungeonEntry(player, this);
}

void DungeonEntrySceneDynamic::KickPlayersOut()
{
	class KickPlayersVisitor : public PlayerVisitor
	{
	public:
		KickPlayersVisitor(DungeonEntrySceneDynamic* scene)
			:m_pScene(scene) {}

		bool operator()(Player* player)
		{
			m_pScene->KickPlayerOut(player);
			return true;
		}
	private:
		DungeonEntrySceneDynamic*	m_pScene;
	};

	KickPlayersVisitor visitor(this);
	VisitPlayers(visitor);
}

void DungeonEntrySceneDynamic::KickPlayerOut(Player* player)
{
	if (player == NULL || player->GetScene() == NULL)
	{
		return;
	}

	if (player->GetScene() != this)
	{
		ErrorLogStream << "player(" << player->GetID() << ") kick from scene(" << GetID() << ") current scene(" << player->GetScene()->GetID() << ")." << LogStream::eos;
		return;
	}

	if (player->GetGameStatus() != PS_NORMAL)
	{
		ErrorLogStream << "player(" << player->GetID() << ") kick from scene(" << GetID() << ") invalid status(" << player->GetGameStatus() << ")." << LogStream::eos;
		return;
	}

	// 回到原来的城镇
	CLProtocol::BattleExitSceneReq exit;
	exit.roleId = player->GetID();
	exit.battleType = BTY_DILAO;
	exit.result = LDBR_OVER;
	exit.battleId = GetBattleId();
	player->SetLostDungLeaveTag(true);
	Router::SendToSelfScene(player->GetSrcSceneNodeID(), exit);

	InfoLogStream << "kick player(" << player->GetID() << ") back to self scene." << LogStream::eos;

	m_KickTimer.SetLastTime(0);
	m_KickTimer.SetInterval(5000);
}

void DungeonEntrySceneDynamic::OnKickPlayerTimeout()
{
}