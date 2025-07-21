#include "SceneScript.h"

#include "Scene.h"
#include "Player.h"

SceneScript::SceneScript(UInt32 id, Avalon::ScriptState* state)
	:Avalon::ScriptFile(state)
{
	SetId(id);
}

SceneScript::~SceneScript()
{
}

void SceneScript::OnSceneInit(Scene* scene)
{
	Call<void>("OnSceneInit", scene);
}

void SceneScript::OnSceneQuit(Scene* scene)
{
	Call<void>("OnSceneQuit", scene);
}

void SceneScript::OnSceneUnload(Scene* scene)
{
	Call<void>("OnSceneUnload", scene);
}

void SceneScript::OnBeforePlayerEnter(Player* player)
{
	Call<void>("OnBeforePlayerEnter", player);
}

void SceneScript::OnBeforeNpcEnter(Npc* pNpc)
{
	Call<void>("OnBeforeNpcEnter", pNpc);
}

void SceneScript::OnPlayerEnter(Player* player)
{
	Call<void>("OnPlayerEnter", player);
}

void SceneScript::OnPlayerLeave(Player* player)
{
	Call<void>("OnPlayerLeave", player);
}

void SceneScript::OnResetCustomsPass(Player* player)
{
	Call<void>("OnResetCustomsPass", player);
}

void SceneScript::OnTick(Scene* scene, UInt64 now)
{
	Call<void>("OnTick", scene, now);
}

void SceneScript::OnNpcKilled(Npc* npc, Creature* attacker)
{
	Call<void>("OnNpcKilled", npc, attacker);
}

void SceneScript::OnNpcDied(Npc* npc)
{
	Call<void>("OnNpcDied", npc);
}

void SceneScript::OnPlayerKilledByPlayer(Player* dead, Player* attacker)
{
	Call<void>("OnPlayerKilledByPlayer", dead, attacker);
}

void SceneScript::OnNpcKilledByPlayer(Npc* npc, Player* attacker)
{
	Call<void>("OnNpcKilledByPlayer", npc, attacker);
}

void SceneScript::OnPlayerKilledByNpc(Player *dead, Npc *attacker)
{
	Call<void>("OnPlayerKilledByNpc", dead, attacker);
}

void SceneScript::OnFixedDmgHpChanged(Creature* creature)
{
	Call<void>("OnFixedDmgHpChanged", creature);
}

bool SceneScript::OnEnterEventTrap(Player* player, UInt32 trapid)
{
	return Call<bool>("OnEnterEventTrap", player, trapid);
}

bool SceneScript::OnNpcEnterEventTrap(Npc* npc, UInt32 trapid)
{
	return Call<bool>("OnNpcEnterEventTrap", npc, trapid);
}

UInt32 SceneScript::OnEnterTransZone(Player *pPlayer, UInt32 uIndex)
{
	return Call<UInt32>("OnEnterTransZone", pPlayer, uIndex);
}

void SceneScript::VisitPlayer(const char* func, Player* player)
{
	Call<void>(func, player);
}

void SceneScript::VisitNpc(const char* func, Npc* npc)
{
	Call<void>(func, npc);
}

void SceneScript::VisitBattlePlayer(const char* func, Scene* scene, ObjID_t playerId)
{
	Call<void>(func, scene, playerId);
}

void SceneScript::VisitBattlePlayerInOrder(const char* func, Scene* scene, ObjID_t playerId, UInt32 totalNum, UInt32 order)
{
	Call<void>(func, scene, playerId, totalNum, order); 
}

bool SceneScript::CheckPlayerEnter(Scene* scene, ObjID_t id, ObjID_t tribeid, UInt16 level)
{
	return Call<bool>("CheckPlayerEnter", scene, id, tribeid, level);
}

bool SceneScript::CheckPlayerEnterCopy(UInt16 level, Player* player)
{
	return Call<bool>("CheckPlayerEnterCopy", level, player);
}

void SceneScript::ReqSummonBattleCreature(Player *pPlayer, Npc *pBuilder, UInt32 uDataId, UInt32 uParam)
{
	Call<void>("ReqSummonBattleCreature", pPlayer, pBuilder, uDataId, uParam);
}

ObjID_t SceneScript::ReqUpgradeBattleCreature(Player *pPlayer, Npc *pNpc)
{
	return Call<UInt64>("ReqUpgradeBattleCreature", pPlayer, pNpc);
}

void SceneScript::DoOperate(Player* player, const char* name, UInt32 param)
{
	Call<void>("DoOperate", player, name, param);
}

void SceneScript::VisitBattleTribe(const char *func, Scene *scene, ObjID_t tribeId)
{
	Call<void>(func, scene, tribeId);
}

void SceneScript::VisitBattleTribeInOrder(const char *func, Scene *scene, ObjID_t tribeId, UInt32 order)
{
	Call<void>(func, scene, tribeId, order);
}

void SceneScript::OnStoryEnd(Player* player, UInt32 storyId)
{
	Call<void>("OnStoryEnd", player, storyId);
}

void SceneScript::OnTimerEventTrigger(Scene* scene, const char* handler)
{
	Call<void>(handler, scene);
}
